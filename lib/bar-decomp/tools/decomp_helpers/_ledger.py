"""Build/refresh progress/ledger.csv — the primary per-function progress metric.
Reads /tmp/remaining.csv (regenerates via _categorize.py if absent), assigns each
function a permuter band, and a status. Preserves existing status/attempts/notes
from a prior ledger so re-runs are idempotent.

Bands (Phase 1 triage):
  P1  pure-int clean seed, no-call no-loop          (smallest search)
  P2  float clean seed, no struct access            (lwc1-reorder class)
  P3  struct-touching clean seed                    (split brute/typing later)
  S   no-call no-loop, m2c NOT clean (hand-seed)
  (blank) = not a Phase-1 target (has call or loop) -> Phase 2

Usage: _ledger.py            (build/refresh)
       _ledger.py --summary  (print band counts)
"""
import os, csv, sys, subprocess

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
WIN = "/mnt/c/Users/Bryan/Projects/BeetleAdventureDecomp"
LEDGER = "progress/ledger.csv"
REMAIN = "/tmp/remaining.csv"

if not os.path.exists(REMAIN):
    subprocess.run(["python3", f"{WIN}/_categorize.py", "--m2c"], check=True)

rows = list(csv.DictReader(open(REMAIN)))
def tb(x): return str(x) == "True"
def ti(x):
    try: return int(x)
    except: return 0

# load prior ledger to preserve status/attempts/notes
prior = {}
if os.path.exists(LEDGER):
    for r in csv.DictReader(open(LEDGER)):
        prior[(r["mod"], r["fn"])] = r

def band(r):
    has_call = tb(r["has_call"]); has_loop = tb(r["has_loop"])
    if has_call or has_loop:
        return ""  # Phase 2
    clean = ti(r["m2c_ok"]) == 1 and ti(r["m2c_calls"]) == 0 and ti(r["m2c_flow"]) == 0
    if not clean:
        return "S"
    if ti(r["nflt"]) == 0:
        return "P1"
    if ti(r["m2c_struct"]) == 1:
        return "P3"
    return "P2"

out = []
for r in rows:
    key = (r["mod"], r["fn"])
    p = prior.get(key, {})
    out.append({
        "mod": r["mod"], "fn": r["fn"], "size": r["size"], "ninstr": r["ninstr"],
        "has_call": r["has_call"], "has_loop": r["has_loop"], "nflt": r["nflt"],
        "ncond": r["ncond"], "m2c_ok": r["m2c_ok"], "m2c_struct": r["m2c_struct"],
        "m2c_flow": r["m2c_flow"], "m2c_calls": r["m2c_calls"],
        "band": band(r),
        "status": p.get("status", "asm"),
        "base_score": p.get("base_score", ""),
        "best_score": p.get("best_score", ""),
        "attempts": p.get("attempts", "0"),
        "notes": p.get("notes", ""),
    })

os.makedirs("progress", exist_ok=True)
cols = ["mod","fn","size","ninstr","has_call","has_loop","nflt","ncond","m2c_ok",
        "m2c_struct","m2c_flow","m2c_calls","band","status","base_score","best_score","attempts","notes"]
with open(LEDGER, "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=cols); w.writeheader()
    for r in out: w.writerow(r)

from collections import Counter
bands = Counter(r["band"] for r in out)
print(f"ledger: {len(out)} functions -> {LEDGER}")
print(f"  P1 (int clean)      : {bands['P1']}")
print(f"  P2 (float clean)    : {bands['P2']}")
print(f"  P3 (struct clean)   : {bands['P3']}")
print(f"  S  (hand-seed)      : {bands['S']}")
print(f"  Phase-1 total       : {bands['P1']+bands['P2']+bands['P3']+bands['S']}")
print(f"  Phase-2 (call/loop) : {bands['']}")
