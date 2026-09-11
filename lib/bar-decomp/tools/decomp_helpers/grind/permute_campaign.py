#!/usr/bin/env python3
"""Permuter campaign over the NOMATCH pool. Import each seed serially, then permute in parallel.
Saves score-0 (byte-match) winning C to .grind/wins/<fn>.c. Run in WSL.
  python3 .../.grind/permute_campaign.py [timeout_secs] [concurrent] [jobs_per]
"""
import json, os, re, glob, subprocess, sys, shutil
from concurrent.futures import ThreadPoolExecutor

REPO = os.path.expanduser("~/projects/bar-decomp"); os.chdir(REPO)
GRIND = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind"
WINS = f"{GRIND}/wins"; os.makedirs(WINS, exist_ok=True)
TIMEOUT = int(sys.argv[1]) if len(sys.argv) > 1 else 300
CONC = int(sys.argv[2]) if len(sys.argv) > 2 else 6
JOBS = int(sys.argv[3]) if len(sys.argv) > 3 else 4

pool = json.load(open(f"{GRIND}/nomatch.json"))
def mod_of(fn): return re.match(r'func_([a-z0-9]+)_', fn).group(1)

# ---- Phase A: import all (serial; each mutates+restores the module .c) ----
imported = []
for fn in pool:
    mod = mod_of(fn); SRC = f"src/modules/{mod}.c"; seed = f"{GRIND}/seeds/{fn}.c"
    if not os.path.exists(seed): continue
    orig = open(SRC).read()
    pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")'
    if pragma not in orig: continue
    cand = open(seed).read().rstrip()
    subprocess.run(f"rm -rf nonmatchings/{fn} nonmatchings/{fn}-*", shell=True)
    try:
        open(SRC, "w").write(orig.replace(pragma, cand))
        r = subprocess.run(["python3", "tools/decomp-permuter/import.py", SRC,
                            f"asm/us/nonmatchings/modules/{mod}/{fn}.s"], capture_output=True, text=True)
    finally:
        open(SRC, "w").write(orig)
    if "Done. Imported" in (r.stdout + r.stderr) and os.path.isdir(f"nonmatchings/{fn}"):
        imported.append((mod, fn))
    else:
        print(f"IMPORT-FAIL {fn}", flush=True)
print(f"imported {len(imported)}/{len(pool)}; permuting {CONC} at a time, -j{JOBS}, {TIMEOUT}s each", flush=True)

# ---- Phase B: permute in parallel ----
def permute(mf):
    mod, fn = mf; d = f"nonmatchings/{fn}"
    subprocess.run(f"rm -rf {d}/output*", shell=True)
    subprocess.run(f"timeout {TIMEOUT} python3 tools/decomp-permuter/permuter.py {d} -j{JOBS} "
                   f"--stop-on-zero --best-only >/dev/null 2>&1", shell=True)
    best = None
    for o in glob.glob(f"{d}/output*"):
        try: s = int(open(f"{o}/score.txt").read().strip())
        except: continue
        if best is None or s < best[0]: best = (s, o)
    if best and best[0] == 0:
        src = open(f"{best[1]}/source.c").read()
        open(f"{WINS}/{fn}.c", "w").write(src)  # full permuted source; integrate step extracts the fn
        print(f"  WIN  {fn} score=0", flush=True)
        return (fn, 0)
    print(f"  ---- {fn} best={best[0] if best else 'NA'}", flush=True)
    return (fn, best[0] if best else None)

with ThreadPoolExecutor(max_workers=CONC) as ex:
    res = list(ex.map(permute, imported))
wins = [fn for fn, s in res if s == 0]
print(f"\n==== PERMUTER CAMPAIGN: {len(wins)} score-0 wins ====", flush=True)
print("WINS:", " ".join(wins))
json.dump(wins, open(f"{GRIND}/perm_wins.json", "w"))
