#!/usr/bin/env python3
"""Greedy per-module integration of existing agent seeds (progress/seeds/func_<mod>_*.c).

For each seed (in address order), tentatively:
  - strip its leading `extern ...;` lines and keep the function definition VERBATIM
    (IDO is line-layout sensitive),
  - replace the func's GLOBAL_ASM pragma with the body,
  - add any not-yet-declared externs to a managed block (FIRST-declaration-wins, which
    structurally avoids cross-seed `extern u8 X;` vs `extern u8 X[];` type conflicts),
  - build build/bin/us/<mod>.o and check the per-module hash.
Keep the func iff the hash passes; otherwise revert it (pragma + its new externs).
Leaves the module .c in the maximal matching state. Safe: every change is byte-gated.

Usage: _seedconsolidate.py <module> [--list f1,f2,...]
"""
import os, re, sys, glob, subprocess

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)

MOD = sys.argv[1]
ONLY = None
if "--list" in sys.argv:
    ONLY = set(sys.argv[sys.argv.index("--list") + 1].split(","))
SRC = f"src/modules/{MOD}.c"
SENT = "/*__SEEDEXTERNS__*/"

def decl_key(line):
    s = line.strip()
    if s.startswith("typedef"):
        m = re.search(r'(\w+)\s*;\s*$', s)
        return ("typedef", m.group(1) if m else s)
    m = re.search(r'\b(D_\w+|g[A-Z]\w+)\b', line)
    return ("extern", m.group(1) if m else s)

def parse_seed(path, fn):
    lines = open(path).read().split("\n")
    di = None
    for i, l in enumerate(lines):
        if re.search(r'\b%s\s*\(' % re.escape(fn), l) and not l.rstrip().endswith(";"):
            di = i; break
    if di is None:
        return None, None
    decls = [l for l in lines[:di] if l.strip().startswith(("extern ", "typedef "))]
    depth = 0; started = False; end = None
    for i in range(di, len(lines)):
        depth += lines[i].count("{") - lines[i].count("}")
        if "{" in lines[i]:
            started = True
        if started and depth == 0:
            end = i; break
    if end is None:
        return None, None
    return decls, "\n".join(lines[di:end + 1])

def ensure_sentinel(text):
    if SENT in text:
        return text
    lines = text.split("\n")
    last_inc = -1
    for i, l in enumerate(lines):
        if l.strip().startswith("#include"):
            last_inc = i
    if last_inc >= 0:
        ins = last_inc + 1
    else:
        ins = 0
        for i, l in enumerate(lines):
            s = l.strip()
            if s == "" or s.startswith("//") or s.startswith("/*") or s.startswith("*") or s.endswith("*/"):
                ins = i + 1
            else:
                break
    lines.insert(ins, SENT)
    return "\n".join(lines)

def build_ok():
    subprocess.run(["rm", "-f", f"build/bin/us/{MOD}.o", f"build/src/modules/{MOD}.o"],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    r = subprocess.run(f"make build/bin/us/{MOD}.o 2>&1", shell=True,
                       capture_output=True, text=True)
    out = r.stdout + r.stderr
    if "Hash verification passed" in out:
        return "MATCH", ""
    if "Hash verification failed" in out:
        return "NOMATCH", ""
    err = next((l.strip() for l in out.split("\n") if "error:" in l), "")
    err = re.sub(r'\x1b\[[0-9;]*[mK]', '', err)
    return "BUILDERR", err[:140]

orig = open(SRC).read()
good = ensure_sentinel(orig)
open(SRC, "w").write(good)
declared = set(decl_key(l) for l in good.split("\n")
               if l.strip().startswith(("extern ", "typedef ")))

seeds = sorted(glob.glob(f"progress/seeds/func_{MOD}_*.c"))
matched, failed, skipped = [], [], []
for sp in seeds:
    fn = os.path.basename(sp)[:-2]
    if ONLY and fn not in ONLY:
        continue
    pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{MOD}/{fn}.s")'
    if pragma not in good:
        skipped.append(fn); continue
    decls, body = parse_seed(sp, fn)
    if body is None:
        skipped.append(fn); continue
    new_ext = [e for e in decls if decl_key(e) not in declared]
    cand = good.replace(pragma, body)
    # strip a pre-existing K&R forward proto for THIS fn (e.g. `void func_X();`)
    # that would conflict with the seed's real signature -- like _handwave does.
    cand = re.sub(r'(?m)^[ \t]*[A-Za-z_][\w \t\*]*\b%s\s*\([^;{]*\)\s*;[ \t]*\r?\n'
                  % re.escape(fn), '', cand)
    cand = cand.replace(SENT, SENT + ("\n" + "\n".join(new_ext) if new_ext else ""), 1)
    open(SRC, "w").write(cand)
    res, err = build_ok()
    if res == "MATCH":
        good = cand
        declared |= set(decl_key(e) for e in new_ext)
        matched.append(fn)
        print(f"  MATCH  {fn}", flush=True)
    else:
        failed.append((fn, res))
        print(f"  ----   {fn} [{res}] {err}", flush=True)

open(SRC, "w").write(good if matched else orig)
nb = sum(1 for _, r in failed if r == "BUILDERR")
nn = sum(1 for _, r in failed if r == "NOMATCH")
print(f"\n[{MOD}] matched {len(matched)} / tested {len(matched)+len(failed)} "
      f"(skipped {len(skipped)})  fails: {nb} BUILDERR, {nn} NOMATCH")
if matched:
    print("MATCHED:", " ".join(matched))
