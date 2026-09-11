#!/usr/bin/env python3
"""Robust byte-gated seed consolidator: like tools/decomp_helpers/_seedconsolidate.py but correctly
handles MULTI-LINE `typedef struct {...} Name;` blocks in seed preambles (dedup by type/symbol name).
Run in WSL:  python3 /mnt/c/.../.grind/consolidate2.py [mod1 mod2 ...]   (default: all mods with seeds)

Per seed (address order): parse preamble into decl-units (externs, forward protos, full typedef blocks)
+ the function body; add only NOT-yet-declared units to a managed block after the includes; swap the
GLOBAL_ASM pragma for the body; build build/bin/us/<mod>.o; keep iff DaisyBox hash passes, else revert.
Then full build + SHA verify.
"""
import os, re, sys, glob, shutil, subprocess
from collections import defaultdict

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
GRIND_SEEDS = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind/seeds"
SENT = "/*__SEEDEXTERNS__*/"

def unit_key(text):
    t = text.strip()
    m = re.search(r'}\s*(\w+)\s*;', t)            # typedef struct {...} Name;
    if m: return ("type", m.group(1))
    m = re.match(r'typedef\b.*\b(\w+)\s*;', t)      # typedef X Name;
    if m: return ("type", m.group(1))
    m = re.search(r'\b(D_\w+|g[A-Z]\w+)\b', t)      # extern data / global export ptr
    if m: return ("sym", m.group(1))
    m = re.search(r'\b(func_\w+)\s*\(', t)          # forward proto
    if m: return ("sym", m.group(1))
    return ("raw", t)

def parse_seed(path, fn):
    """Return (comment_block, [(key,text)] decl_units, body) or (None,None,None)."""
    lines = open(path).read().split("\n")
    di = None
    for i, l in enumerate(lines):
        if re.search(r'\b%s\s*\(' % re.escape(fn), l) and not l.rstrip().endswith(";"):
            di = i; break
    if di is None:
        return None, None, None
    # function body: di .. matching close brace
    depth = 0; started = False; end = None
    for i in range(di, len(lines)):
        depth += lines[i].count("{") - lines[i].count("}")
        if "{" in lines[i]:
            started = True
        if started and depth == 0:
            end = i; break
    if end is None:
        return None, None, None
    body = "\n".join(lines[di:end + 1])
    # preamble -> units + comments
    pre = lines[:di]
    comments, units = [], []
    i = 0
    while i < len(pre):
        s = pre[i].strip()
        if s == "":
            i += 1; continue
        if s.startswith("//") or (s.startswith("/*") and s.endswith("*/")):
            comments.append(pre[i]); i += 1; continue
        # multi-line typedef struct block
        if ("typedef" in s or s.startswith("struct")) and ("{" in s or s.endswith("{")):
            blk = []; d = 0; j = i
            while j < len(pre):
                blk.append(pre[j]); d += pre[j].count("{") - pre[j].count("}")
                if d <= 0 and "{" in "".join(blk) and pre[j].rstrip().endswith(";"):
                    break
                j += 1
            text = "\n".join(blk); units.append((unit_key(text), text)); i = j + 1; continue
        # single-line decl (extern / typedef X Y; / forward proto)
        if s.endswith(";"):
            units.append((unit_key(pre[i]), pre[i])); i += 1; continue
        i += 1  # skip anything else
    return "\n".join(comments), units, body

def ensure_sentinel(text):
    if SENT in text: return text
    lines = text.split("\n"); last_inc = -1
    for i, l in enumerate(lines):
        if l.strip().startswith("#include"): last_inc = i
    ins = last_inc + 1 if last_inc >= 0 else 0
    lines.insert(ins, SENT)
    return "\n".join(lines)

def header_closure(root="include/common.h"):
    """Transitive set of headers reachable from common.h (what a module actually sees)."""
    seen, stack = set(), [root]
    while stack:
        p = stack.pop()
        if p in seen or not os.path.exists(p):
            continue
        seen.add(p)
        try:
            t = open(p, errors="ignore").read()
        except Exception:
            continue
        for m in re.finditer(r'#include\s+"([^"]+)"', t):
            cand = os.path.normpath(os.path.join("include", m.group(1)))
            if os.path.exists(cand):
                stack.append(cand)
            alt = os.path.normpath(os.path.join(os.path.dirname(p), m.group(1)))
            if os.path.exists(alt):
                stack.append(alt)
    return seen

def header_known_symbols():
    """Symbols/types declared in common.h's closure ONLY (so a module already sees them). Used to drop
    seed decls that would CONFLICT (e.g. redeclaring gGameSettings). Export globals in global_exports.h
    are NOT in this set (modules don't include it), so seeds must still declare those themselves."""
    keys = set()
    for hp in header_closure():
        try:
            t = open(hp, errors="ignore").read()
        except Exception:
            continue
        for m in re.finditer(r'\bextern\s+[\w \t\*]+?\b(\w+)\s*(\[[^\]]*\])?\s*;', t):
            keys.add(("sym", m.group(1)))
        for m in re.finditer(r'}\s*(\w+)\s*;', t):
            keys.add(("type", m.group(1)))
        for m in re.finditer(r'typedef\b[^;{]*\b(\w+)\s*;', t):
            keys.add(("type", m.group(1)))
        for m in re.finditer(r'(?m)^[ \t]*[A-Za-z_][\w \t\*]+?\b(\w+)\s*\([^;{]*\)\s*;', t):
            keys.add(("sym", m.group(1)))
    return keys

HEADER_SYMS = header_known_symbols()

def module_declared_keys(text):
    keys = set()
    for l in text.split("\n"):
        s = l.strip()
        if s.startswith(("extern ", "typedef ")) and s.endswith(";"):
            keys.add(unit_key(l))
    # struct tags already defined in the module
    for m in re.finditer(r'}\s*(\w+)\s*;', text):
        keys.add(("type", m.group(1)))
    return keys

def build_ok(mod):
    subprocess.run(["rm", "-f", f"build/bin/us/{mod}.o", f"build/src/modules/{mod}.o"],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    r = subprocess.run(f"make build/bin/us/{mod}.o 2>&1", shell=True, capture_output=True, text=True)
    out = r.stdout + r.stderr
    if "Hash verification passed" in out: return "MATCH", ""
    if "Hash verification failed" in out: return "NOMATCH", ""
    err = next((l.strip() for l in out.split("\n") if "error:" in l), "")
    return "BUILDERR", re.sub(r'\x1b\[[0-9;]*[mK]', '', err)[:140]

def consolidate(mod):
    SRC = f"src/modules/{mod}.c"
    orig = open(SRC).read()
    good = ensure_sentinel(orig)
    open(SRC, "w").write(good)
    declared = module_declared_keys(good)
    seeds = sorted(glob.glob(f"progress/seeds/func_{mod}_*.c"))
    matched, failed = [], []
    for sp in seeds:
        fn = os.path.basename(sp)[:-2]
        pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")'
        if pragma not in good:
            continue
        comment, units, body = parse_seed(sp, fn)
        if body is None:
            failed.append((fn, "PARSE")); continue
        new_units = [t for (k, t) in units if k not in declared and k not in HEADER_SYMS]
        insert = "\n".join(new_units)
        cand = good.replace(pragma, (comment + "\n" if comment else "") + body)
        cand = re.sub(r'(?m)^[ \t]*[A-Za-z_][\w \t\*]*\b%s\s*\([^;{]*\)\s*;[ \t]*\r?\n'
                      % re.escape(fn), '', cand)  # strip pre-existing K&R proto for this fn
        if insert:
            cand = cand.replace(SENT, SENT + "\n" + insert, 1)
        open(SRC, "w").write(cand)
        res, err = build_ok(mod)
        if res == "MATCH":
            good = cand
            declared |= set(k for (k, t) in units)
            matched.append(fn)
            print(f"  MATCH  {fn}", flush=True)
        else:
            failed.append((fn, res))
            print(f"  ----   {fn} [{res}] {err}", flush=True)
    open(SRC, "w").write(good if matched else orig)
    nb = sum(1 for _, r in failed if r == "BUILDERR")
    nn = sum(1 for _, r in failed if r == "NOMATCH")
    print(f"[{mod}] matched {len(matched)}/{len(matched)+len(failed)}  fails: {nb} BUILDERR, {nn} NOMATCH", flush=True)
    return matched

# stage seeds
os.makedirs("progress/seeds", exist_ok=True)
by_mod = defaultdict(list)
for s in glob.glob(f"{GRIND_SEEDS}/func_*.c"):
    fn = os.path.basename(s)[:-2]
    m = re.match(r'func_([a-z0-9]+)_[0-9A-Fa-f]+$', fn)
    if not m: continue
    shutil.copy(s, f"progress/seeds/{os.path.basename(s)}")
    by_mod[m.group(1)].append(fn)

mods = sys.argv[1:] if len(sys.argv) > 1 else sorted(by_mod)
allm = []
for mod in mods:
    if mod not in by_mod: continue
    print(f"\n===== {mod} ({len(by_mod[mod])} seeds) =====", flush=True)
    allm += consolidate(mod)

print("\n===== full build + SHA verify =====", flush=True)
subprocess.run("make -j6 >/dev/null 2>&1", shell=True)
sha = subprocess.run("sha1sum build/beetleadventurerac.us.z64", shell=True, capture_output=True, text=True).stdout.strip()
ok = "e5ab4d226c08d22f68a2edcc48870203e67454b8" in sha
print(f"SHA: {sha}\n==== banked {len(allm)}; ROM {'OK' if ok else 'MISMATCH!!'} ====")
if allm: print("BANKED:", " ".join(allm))
