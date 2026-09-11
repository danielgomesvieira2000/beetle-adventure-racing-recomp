"""Phase 1 prep: for each clean-seed Tier-A near-miss (bands P1/P2/P3), generate
the m2c seed, import it into decomp-permuter (creating nonmatchings/<fn>/), and
read the base score. Serial (import rewrites the shared module .c, then restores).
Updates progress/ledger.csv with base_score + status, and saves each seed to
progress/seeds/<fn>.c for later winner review.

Usage: _phase1_prep.py [P1 P2 P3 ...]   (default: P1 P2 P3)
"""
import os, re, sys, csv, json, glob, subprocess

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
WIN = "/mnt/c/Users/Bryan/Projects/BeetleAdventureDecomp"
BANDS = sys.argv[1:] or ["P1", "P2", "P3"]
LEDGER = "progress/ledger.csv"

rows = list(csv.DictReader(open(LEDGER)))
targets = [r for r in rows if r["band"] in BANDS and r["status"] in ("asm", "seed", "")]
by_mod = {}
for r in targets:
    by_mod.setdefault(r["mod"], []).append(r["fn"])
print(f"prep: {len(targets)} functions across {len(by_mod)} modules; bands={BANDS}")

os.makedirs("progress/seeds", exist_ok=True)

def prep_text(orig, externs, fn, seed):
    t = orig
    if '#include "common.h"' not in t:
        lines = t.split("\n"); i = 0
        while i < len(lines) and (lines[i].strip() == "" or lines[i].lstrip().startswith(("//", "/*", "*"))):
            i += 1
        lines.insert(i, '#include "common.h"'); t = "\n".join(lines)
    sig = seed.split("{", 1)[0].strip() + ";"
    pat = re.compile(r'^[^\n{}]*\b%s\s*\([^;{]*\)\s*;[ \t]*$' % re.escape(fn), re.M)
    if pat.search(t):
        t = pat.sub(sig, t, count=1)
    miss = []
    for e in externs:
        syms = re.findall(r'\bD_\w+', e)
        if syms and re.search(r'\b%s\b' % re.escape(syms[0]), t): continue
        if e in t: continue
        miss.append(e)
    if miss:
        lines = t.split("\n")
        li = max((i for i, l in enumerate(lines) if l.startswith("#include")), default=0)
        for j, e in enumerate(miss): lines.insert(li + 1 + j, e)
        t = "\n".join(lines)
    pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")'
    return t.replace(pragma, seed)

results = {(r["mod"], r["fn"]): r for r in rows}
for mod, fns in by_mod.items():
    spec_path = f"/tmp/spec_{mod}.json"
    subprocess.run(["python3", f"{WIN}/_m2cwave.py", mod, spec_path],
                   capture_output=True, text=True)
    if not os.path.exists(spec_path):
        print(f"  {mod}: no spec"); continue
    spec = json.load(open(spec_path))
    externs = spec.get("externs", [])
    SRC = f"src/modules/{mod}.c"
    orig = open(SRC).read()
    for fn in fns:
        if fn not in spec["funcs"]:
            results[(mod, fn)]["status"] = "no-seed"; print(f"  {fn}: no seed in spec"); continue
        seed = spec["funcs"][fn]
        open(f"progress/seeds/{fn}.c", "w").write(seed)
        subprocess.run(f"rm -rf nonmatchings/{fn} nonmatchings/{fn}-*", shell=True)
        try:
            open(SRC, "w").write(prep_text(orig, externs, fn, seed))
            r = subprocess.run(["python3", "tools/decomp-permuter/import.py", SRC,
                                f"asm/us/nonmatchings/modules/{mod}/{fn}.s"],
                               capture_output=True, text=True)
        finally:
            open(SRC, "w").write(orig)
        if "Done. Imported" not in (r.stdout + r.stderr):
            results[(mod, fn)]["status"] = "import-fail"; print(f"  {fn}: IMPORT FAIL"); continue
        dirs = sorted(glob.glob(f"nonmatchings/{fn}*"), key=os.path.getmtime)
        d = dirs[-1]
        rr = subprocess.run(f"timeout 12 python3 tools/decomp-permuter/permuter.py {d} -j2 2>&1",
                            shell=True, capture_output=True, text=True)
        m = re.search(r'base score = (\d+)', rr.stdout + rr.stderr)
        base = m.group(1) if m else ""
        results[(mod, fn)]["base_score"] = base
        results[(mod, fn)]["status"] = "ready" if base else "import-ok-nobase"
        print(f"  {fn:40s} base={base}")

# write ledger back
cols = list(rows[0].keys())
with open(LEDGER, "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=cols); w.writeheader()
    for r in rows: w.writerow(r)
ready = [r for r in rows if r["status"] == "ready"]
print(f"\nREADY to permute: {len(ready)}")
bs = sorted(int(r["base_score"]) for r in ready if r["base_score"])
if bs:
    print(f"  base score: min={bs[0]} median={bs[len(bs)//2]} max={bs[-1]}")
    print(f"  base<400:{sum(1 for x in bs if x<400)} 400-1000:{sum(1 for x in bs if 400<=x<1000)} >=1000:{sum(1 for x in bs if x>=1000)}")
