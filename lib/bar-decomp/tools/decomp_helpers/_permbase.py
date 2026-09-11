"""Permuter base-score triage: import a seed and read the permuter's initial
'base score' WITHOUT running a full search, so we can decide whether a function
is worth permuting before spending minutes. Lower base = closer = better odds.

Inserts the seed in place of the func's pragma, runs import.py, restores the
pragma, then runs the permuter ~10s to capture 'base score = N'.

Usage: _permbase.py <module> <func> <seed.c>   -> prints "BASE <n>" or "ERR ..."
"""
import os, re, sys, subprocess

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
MOD, FN, SEED = sys.argv[1], sys.argv[2], sys.argv[3]
SRC = f"src/modules/{MOD}.c"
ASM = f"asm/us/nonmatchings/modules/{MOD}/{FN}.s"
PRAGMA = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{MOD}/{FN}.s")'

orig = open(SRC).read()
seed = open(SEED).read().rstrip()
if PRAGMA not in orig:
    print("ERR no-pragma"); sys.exit(0)

# also fix any conflicting K&R prototype so the seed compiles
import re as _re
sig = seed.split("{", 1)[0].strip() + ";"
patched = orig
pat = _re.compile(r'^[^\n{}]*\b%s\s*\([^;{]*\)\s*;[ \t]*$' % _re.escape(FN), _re.M)
if pat.search(patched):
    patched = pat.sub(sig, patched, count=1)
patched = patched.replace(PRAGMA, seed)

try:
    open(SRC, "w").write(patched)
    r = subprocess.run(["python3", "tools/decomp-permuter/import.py", SRC, ASM],
                       capture_output=True, text=True)
    if "Done. Imported" not in (r.stdout + r.stderr):
        print("ERR import:" + " ".join((r.stdout + r.stderr).split())[-200:]);
        raise SystemExit
finally:
    open(SRC, "w").write(orig)

# the import may land in <fn> or <fn>-N; pick the newest
import glob
dirs = sorted(glob.glob(f"nonmatchings/{FN}*"), key=os.path.getmtime)
if not dirs:
    print("ERR no-dir"); sys.exit(0)
d = dirs[-1]
r = subprocess.run(f"timeout 12 python3 tools/decomp-permuter/permuter.py {d} -j2 2>&1",
                   shell=True, capture_output=True, text=True)
m = re.search(r'base score = (\d+)', r.stdout + r.stderr)
print(f"BASE {m.group(1)}" if m else "ERR no-base " + " ".join((r.stdout+r.stderr).split())[-150:])
