"""Import the agent hand-seeds that did NOT match directly, into decomp-permuter,
and read base scores. Serial (import rewrites the shared module .c). Reads each
seed from progress/seeds/<fn>.c (extern lines + function code).

Usage: _hs_import.py    (processes all handseed funcs not in /tmp/hs_matched.txt)
"""
import os, re, csv, glob, subprocess, sys

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
WIN = "/mnt/c/Users/Bryan/Projects/BeetleAdventureDecomp"

WORKLIST = sys.argv[1] if len(sys.argv) > 1 else f"{WIN}/handseed/_worklist.txt"
MATCHED = sys.argv[2] if len(sys.argv) > 2 else "/tmp/hs_matched.txt"
worklist = [l.split() for l in open(WORKLIST).read().split("\n") if l.strip()]
matched = set(l.split()[1] for l in open(MATCHED).read().split("\n") if l.strip())
mod_of = {fn: mod for mod, fn in worklist}
todo = [(mod, fn) for mod, fn in worklist if fn not in matched]
print(f"importing {len(todo)} non-matched hand-seeds")

def split_seed(text):
    externs = [l.strip() for l in text.split("\n") if l.strip().startswith("extern ")]
    code = "\n".join(l for l in text.split("\n") if not l.strip().startswith("extern ")).strip()
    return externs, code

def prep_text(orig, externs, fn, code, mod):
    t = orig
    if '#include "common.h"' not in t:
        lines = t.split("\n"); i = 0
        while i < len(lines) and (lines[i].strip() == "" or lines[i].lstrip().startswith(("//","/*","*"))):
            i += 1
        lines.insert(i, '#include "common.h"'); t = "\n".join(lines)
    sig = code.split("{", 1)[0].strip() + ";"
    pat = re.compile(r'^[^\n{}]*\b%s\s*\([^;{]*\)\s*;[ \t]*$' % re.escape(fn), re.M)
    if pat.search(t): t = pat.sub(sig, t, count=1)
    miss = []
    for e in externs:
        syms = re.findall(r'\bD_\w+|\bg\w+', e)
        if syms and re.search(r'\bextern\b[^;\n]*\b%s\b' % re.escape(syms[0]), t): continue
        if e in t: continue
        miss.append(e)
    if miss:
        lines = t.split("\n"); li = max((i for i,l in enumerate(lines) if l.startswith("#include")), default=0)
        for j,e in enumerate(miss): lines.insert(li+1+j, e)
        t = "\n".join(lines)
    return t.replace(f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")', code)

rows = list(csv.DictReader(open("progress/ledger.csv")))
led = {r["fn"]: r for r in rows}
for mod, fn in todo:
    sp = f"progress/seeds/{fn}.c"
    if not os.path.exists(sp): continue
    externs, code = split_seed(open(sp).read())
    SRC = f"src/modules/{mod}.c"; orig = open(SRC).read()
    subprocess.run(f"rm -rf nonmatchings/{fn} nonmatchings/{fn}-*", shell=True)
    try:
        open(SRC, "w").write(prep_text(orig, externs, fn, code, mod))
        r = subprocess.run(["python3", "tools/decomp-permuter/import.py", SRC,
                            f"asm/us/nonmatchings/modules/{mod}/{fn}.s"], capture_output=True, text=True)
    finally:
        open(SRC, "w").write(orig)
    if "Done. Imported" not in (r.stdout + r.stderr):
        if fn in led: led[fn]["status"] = "hs-import-fail"
        print(f"  {fn:42s} IMPORT-FAIL"); continue
    dirs = sorted(glob.glob(f"nonmatchings/{fn}*"), key=os.path.getmtime)
    rr = subprocess.run(f"timeout 12 python3 tools/decomp-permuter/permuter.py {dirs[-1]} -j2 2>&1",
                        shell=True, capture_output=True, text=True)
    m = re.search(r'base score = (\d+)', rr.stdout + rr.stderr)
    base = m.group(1) if m else ""
    if fn in led: led[fn]["base_score"] = base; led[fn]["status"] = "hs-ready" if base else "hs-import-ok"
    print(f"  {fn:42s} base={base}")

w = csv.DictWriter(open("progress/ledger.csv","w",newline=""), fieldnames=rows[0].keys()); w.writeheader(); w.writerows(rows)
bs = sorted(int(led[fn]["base_score"]) for _,fn in todo if led.get(fn,{}).get("base_score","").isdigit())
print(f"\nimported with base scores: {len(bs)}")
if bs: print(f"  base<400:{sum(1 for x in bs if x<400)} 400-1500:{sum(1 for x in bs if 400<=x<1500)} >=1500:{sum(1 for x in bs if x>=1500)}")
