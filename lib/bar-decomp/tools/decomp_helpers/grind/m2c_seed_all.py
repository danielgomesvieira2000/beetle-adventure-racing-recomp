#!/usr/bin/env python3
"""Token-free seed expansion across the whole incomplete module codebase.

For every unmatched GLOBAL_ASM function in src/modules that has NO staged seed yet,
generate a context-aware m2c candidate (module cpp context) and gate it against the
module DaisyBox hash:
  MATCH    -> candidate staged to .grind/wins/<fn>.c + marked BANKABLE in sweep_results.json
              (bank_wins.py banks + commits afterwards)
  NOMATCH  -> seed kept at .grind/seeds/<fn>.c  (joins the permuter pool)
  BUILDERR -> seed + exact error staged to .grind/m2c_builderr/<fn>.{c,err} for a later
              LLM fix-wave (no tokens spent now)
Finally writes .grind/pool_night.json = every live-pragma function that has a seed file
(new m2c seeds + existing agent/hand seeds) — the input for permute_campaign2.py.

CPU only, serial, always restores src/. Usage:
  m2c_seed_all.py [max_bytes=3000] [mod1,mod2|ALL]
"""
import csv, json, os, re, glob, shutil, subprocess, sys, tempfile
from collections import Counter

REPO = os.path.expanduser("~/projects/bar-decomp"); os.chdir(REPO)
GRIND = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind"
SEEDS = f"{GRIND}/seeds"; WINS = f"{GRIND}/wins"; BERR = f"{GRIND}/m2c_builderr"
CTXS = f"{GRIND}/ctx"
for d in (SEEDS, WINS, BERR, CTXS): os.makedirs(d, exist_ok=True)
MAXSZ = int(sys.argv[1]) if len(sys.argv) > 1 else 3000
MODS = sys.argv[2].split(",") if len(sys.argv) > 2 and sys.argv[2] != "ALL" else None

st = subprocess.run("git status --porcelain -- src asm progress", shell=True,
                    capture_output=True, text=True).stdout.strip()
if st:
    print("WORK PATHS NOT CLEAN (src/asm/progress) — aborting:\n" + st); sys.exit(1)

CPP_FLAGS = ("-P -undef -nostdinc -I include -I . -I include/libc -I include/2.0I -I include/2.0I/PR "
             "-I bin -I tools/ultralib/include -DVERSION_US=1 -D_MIPS_SZLONG=32 -DNDEBUG -DF3DEX_GBI_2 "
             "-D_FINALROM -DTARGET_N64 -DSSSV -DLANGUAGE_C -D_LANGUAGE_C -D_MIPS_ISA=2 -DMIPSEB")

def module_context(mod):
    ctx = f"{CTXS}/{mod}_ctx.c"; src = f"src/modules/{mod}.c"
    text = re.sub(r'(?m)^\s*#pragma GLOBAL_ASM\([^)]*\)\s*$', '', open(src).read())
    with tempfile.NamedTemporaryFile("w", suffix=".c", dir=".", delete=False) as tf:
        tf.write(text); tmp = tf.name
    try:
        r = subprocess.run(f"cpp {CPP_FLAGS} {tmp}", shell=True, capture_output=True, text=True)
        if r.returncode != 0 or not r.stdout.strip(): return None
        open(ctx, "w").write(r.stdout); return ctx
    finally:
        os.unlink(tmp)

# ---- header closure symbols (things a module already sees via common.h) ----
def header_closure(root="include/common.h"):
    seen, stack = set(), [root]
    while stack:
        p = stack.pop()
        if p in seen or not os.path.exists(p): continue
        seen.add(p)
        for m in re.finditer(r'#include\s+"([^"]+)"', open(p, errors="ignore").read()):
            for cand in (os.path.normpath(os.path.join("include", m.group(1))),
                         os.path.normpath(os.path.join(os.path.dirname(p), m.group(1)))):
                if os.path.exists(cand): stack.append(cand)
    return seen

HEADER_SYMS = set()
for hp in header_closure():
    t = open(hp, errors="ignore").read()
    for m in re.finditer(r'\bextern\s+[\w \t\*]+?\b(\w+)\s*(\[[^\]]*\])?\s*;', t): HEADER_SYMS.add(m.group(1))
    for m in re.finditer(r'}\s*(\w+)\s*;', t): HEADER_SYMS.add(m.group(1))
    for m in re.finditer(r'typedef\b[^;{]*\b(\w+)\s*;', t): HEADER_SYMS.add(m.group(1))
    for m in re.finditer(r'(?m)^[ \t]*[A-Za-z_][\w \t\*]+?\b(\w+)\s*\([^;{]*\)\s*;', t): HEADER_SYMS.add(m.group(1))

def find_def(lines, fn):
    for i, l in enumerate(lines):
        if re.search(r'\b%s\s*\(' % re.escape(fn), l) and not l.rstrip().endswith(";"):
            return i
    return None

def seed_exists(fn):
    return os.path.exists(f"{SEEDS}/{fn}.c") or os.path.exists(f"progress/seeds/{fn}.c")

def asm_size(mod, fn, ledger):
    if fn in ledger and ledger[fn].get("size", "").isdigit():
        return int(ledger[fn]["size"])
    p = f"asm/us/nonmatchings/modules/{mod}/{fn}.s"
    if not os.path.exists(p): return None
    return 4 * sum(1 for l in open(p) if re.match(r'\s*/\* [0-9A-F]', l))

def fix_qmarks(text):
    text = re.sub(r'(?m)^(\s*)\? ', r'\1s32 ', text)          # decls / return types at line start
    text = re.sub(r'\bextern \? ', 'extern s32 ', text)
    text = re.sub(r'\(\? \*\)', '(s32 *)', text)
    text = re.sub(r'\(\? \(\*\)', '(s32 (*)', text)
    return text

def make_candidate(fn, seed_text, module_text, pragma):
    """Split seed into preamble units + body; drop units the module/headers already declare."""
    lines = seed_text.split("\n"); di = find_def(lines, fn)
    if di is None: return None
    depth = 0; started = False; end = None
    for i in range(di, len(lines)):
        depth += lines[i].count("{") - lines[i].count("}")
        if "{" in lines[i]: started = True
        if started and depth == 0: end = i; break
    if end is None: return None
    body = "\n".join(lines[di:end + 1])
    hay = re.sub(r'#pragma GLOBAL_ASM\([^)]*\)', '', module_text)
    keep, buf, d = [], [], 0
    for l in lines[:di]:
        s = l.strip()
        if not s or s.startswith(("//", "/*")): continue
        buf.append(l); d += l.count("{") - l.count("}")
        if d <= 0 and s.endswith(";"):
            u = "\n".join(buf); buf = []; d = 0
            mm = re.search(r'}\s*(\w+)\s*;', u) or re.search(r'\b(D_\w+|g[A-Z]\w+|func_\w+)\b', u)
            key = mm.group(1) if mm else None
            if key and (key in HEADER_SYMS or re.search(r'\b%s\b' % re.escape(key), hay)):
                continue
            keep.append(u)
    return ("\n".join(keep) + "\n\n" if keep else "") + body, body

def gate(mod):
    subprocess.run(["rm", "-f", f"build/bin/us/{mod}.o", f"build/src/modules/{mod}.o"],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    r = subprocess.run(f"make build/bin/us/{mod}.o 2>&1", shell=True, capture_output=True, text=True)
    out = r.stdout + r.stderr
    if "Hash verification passed" in out: return "MATCH", ""
    if "Hash verification failed" in out: return "NOMATCH", ""
    err = "\n".join(l for l in out.split("\n") if "error" in l.lower())[:2000]
    return "BUILDERR", err

def swap_in(orig, pragma, fn, cand):
    """Replace the pragma with the candidate and strip any pre-existing standalone
    prototype for fn (same behavior as consolidate2 — a stale proto with a different
    signature is a guaranteed conflicting-types error)."""
    t = orig.replace(pragma, cand)
    return re.sub(r'(?m)^[ \t]*[A-Za-z_][\w \t\*]*\b%s\s*\([^;{]*\)\s*;[ \t]*\r?\n'
                  % re.escape(fn), '', t)

def synth_structs(cand, fn):
    """m2c emits member access on void* locals (valid m2c pseudo-C, invalid C). Synthesize
    a field-at-offset struct per such local, typing fields by crude usage inference — the
    permuter's type-randomization passes can correct near-miss types later."""
    f32_names = set(re.findall(r'\bf32 (\w+)', cand)) | set(re.findall(r'\bf32 \*?(\w+),', cand))
    changed = False
    for v in re.findall(r'(?m)^\s*void \*(\w+);', cand):
        accs = sorted(set(re.findall(r'\b%s->unk([0-9A-Fa-f]+)\b' % re.escape(v), cand)),
                      key=lambda a: int(a, 16))
        if not accs: continue
        fields, pos = [], 0
        for a in accs:
            off = int(a, 16)
            if off > pos:
                fields.append(f"    u8 pad{pos:X}[0x{off - pos:X}];")
            ty = "s32"
            m = re.search(r'%s->unk%s\s*=\s*([^;]+);' % (re.escape(v), a), cand)
            rhs = m.group(1).strip() if m else ""
            m2 = re.search(r'(\w[\w\[\]\.]*)\s*=\s*%s->unk%s\b' % (re.escape(v), a), cand)
            lhs = m2.group(1) if m2 else ""
            if rhs.startswith(("(void *)", "&")) or re.match(r'^\(\w+ \*\)', rhs):
                ty = "void *"
            elif rhs in f32_names or lhs in f32_names or re.match(r'^-?\d+\.\d+f?$', rhs):
                ty = "f32"
            fields.append(f"    {ty} unk{a};")
            pos = off + (4 if ty != "void *" else 4)
        tname = f"AutoS_{fn.split('_')[-1]}_{v}"
        cand = f"typedef struct {{\n" + "\n".join(fields) + f"\n}} {tname};\n\n" + cand
        cand = re.sub(r'(?m)^(\s*)void \*%s;' % re.escape(v), r'\1%s *%s;' % (tname, v), cand)
        changed = True
    return cand if changed else None

ledger = {}
if os.path.exists("progress/ledger.csv"):
    ledger = {r["fn"]: r for r in csv.DictReader(open("progress/ledger.csv"))}

# ---- walk every live pragma ----
todo = []
for src in sorted(glob.glob("src/modules/*.c")):
    mod = os.path.basename(src)[:-2]
    if MODS and mod not in MODS: continue
    text = open(src).read()
    for m in re.finditer(r'#pragma GLOBAL_ASM\("asm/us/nonmatchings/modules/%s/(func_\w+)\.s"\)' % re.escape(mod), text):
        fn = m.group(1)
        if seed_exists(fn): continue
        if os.path.exists(f"{WINS}/{fn}.c"): continue  # already gate-verified, awaiting bank
        sz = asm_size(mod, fn, ledger)
        if sz is None or sz > MAXSZ: continue
        todo.append((sz, mod, fn))
todo.sort()
print(f"m2c seed expansion: {len(todo)} unseeded functions <= {MAXSZ}B "
      f"(mods: {len(set(m for _, m, _ in todo))})", flush=True)

res = {}
ctx_cache = {}
for idx, (sz, mod, fn) in enumerate(todo):
    if mod not in ctx_cache: ctx_cache[mod] = module_context(mod)
    asm = f"asm/us/nonmatchings/modules/{mod}/{fn}.s"
    m2c = None
    for use_ctx in (True, False):
        cmd = ["python3", "tools/m2c/m2c.py", "-t", "mips-ido-c"]
        if use_ctx and ctx_cache[mod]: cmd += ["--context", ctx_cache[mod]]
        elif use_ctx: continue
        try:
            r = subprocess.run(cmd + [asm], capture_output=True, text=True, timeout=120)
            if r.returncode == 0 and r.stdout.strip():
                m2c = r.stdout; break
        except Exception:
            pass
    if m2c is None or "M2C_ERROR" in m2c or "MIPS2C_ERROR" in m2c:
        res[fn] = "M2C-FAIL"; continue
    m2c = fix_qmarks(m2c)
    SRC = f"src/modules/{mod}.c"; orig = open(SRC).read()
    pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")'
    made = make_candidate(fn, m2c, orig, pragma)
    if not made:
        res[fn] = "M2C-FAIL"; continue
    cand, body = made
    try:
        open(SRC, "w").write(swap_in(orig, pragma, fn, cand))
        verdict, err = gate(mod)
        if verdict == "BUILDERR":
            synth = synth_structs(cand, fn)
            if synth:
                open(SRC, "w").write(swap_in(orig, pragma, fn, synth))
                verdict, err2 = gate(mod)
                if verdict != "BUILDERR":
                    cand = synth
                    # keep the typed version as the seed too
                    m2c = synth
                else:
                    err = err2
    finally:
        open(SRC, "w").write(orig)
    res[fn] = verdict
    if verdict == "MATCH":
        open(f"{WINS}/{fn}.c", "w").write(cand)
        # record incrementally so an interrupted run loses nothing
        rp = f"{GRIND}/sweep_results.json"
        allres = json.load(open(rp)) if os.path.exists(rp) else {}
        allres[fn] = "BANKABLE"
        json.dump(allres, open(rp, "w"), indent=1)
    elif verdict == "NOMATCH":
        open(f"{SEEDS}/{fn}.c", "w").write(m2c)   # full seed (preamble+body) for import
    else:
        open(f"{BERR}/{fn}.c", "w").write(m2c)
        open(f"{BERR}/{fn}.err", "w").write(err)
    if verdict == "MATCH" or (idx + 1) % 25 == 0:
        print(f"  [{idx+1}/{len(todo)}] {fn:42s} {verdict}", flush=True)

# record MATCHes for bank_wins.py
rp = f"{GRIND}/sweep_results.json"
allres = json.load(open(rp)) if os.path.exists(rp) else {}
for fn, v in res.items():
    if v == "MATCH": allres[fn] = "BANKABLE"
json.dump(allres, open(rp, "w"), indent=1)

# pool_night.json = every live-pragma fn with a seed file
pool = []
for src in sorted(glob.glob("src/modules/*.c")):
    mod = os.path.basename(src)[:-2]
    text = open(src).read()
    for m in re.finditer(r'#pragma GLOBAL_ASM\("asm/us/nonmatchings/modules/%s/(func_\w+)\.s"\)' % re.escape(mod), text):
        fn = m.group(1)
        if seed_exists(fn): pool.append(fn)
json.dump(pool, open(f"{GRIND}/pool_night.json", "w"))

print("\nSUMMARY:", dict(Counter(res.values())))
print(f"pool_night.json: {len(pool)} permutable functions (live pragma + seed)")
st = subprocess.run("git status --porcelain", shell=True, capture_output=True, text=True).stdout.strip()
print("git-dirty-after:", (st[:200] or "CLEAN"))
