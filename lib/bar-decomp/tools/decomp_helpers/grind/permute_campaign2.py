#!/usr/bin/env python3
"""Permuter campaign v2 — integration-gated wins + budget ladder + fresh imports.

Fixes the v1 losses: (1) score-0 "wins" that don't survive the module build are now
gated THE MOMENT they appear and the permuter keeps hunting if the gate fails;
(2) stale pools/imports are refreshed from the live pragmas every run; (3) budget is
allocated by a ladder (everyone gets a cheap round, only the closest survivors get
the long rounds) instead of a uniform timeout.

Usage (WSL):  python3 permute_campaign2.py [budgets_csv] [concurrent] [jobs_per] [pool.json]
  budgets_csv  per-round permuter seconds, e.g. "300,900,1800" (default). After each
               round the pool is ranked by best score and halved (floor 8).
Pool: .grind/nomatch.json (or the 4th-arg json list) filtered to live GLOBAL_ASM pragmas.
Wins: gated candidate saved to .grind/wins/<fn>.c and recorded as BANKABLE in
.grind/sweep_results.json — run bank_wins.py afterwards to bank+commit. This script
NEVER permanently modifies src/ (every swap is restored).
"""
import json, os, re, glob, subprocess, sys, threading
from concurrent.futures import ThreadPoolExecutor

REPO = os.path.expanduser("~/projects/bar-decomp"); os.chdir(REPO)
GRIND = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind"
WINS = f"{GRIND}/wins"; os.makedirs(WINS, exist_ok=True)
BUDGETS = [int(x) for x in (sys.argv[1] if len(sys.argv) > 1 else "300,900,1800").split(",")]
CONC = int(sys.argv[2]) if len(sys.argv) > 2 else 6
JOBS = int(sys.argv[3]) if len(sys.argv) > 3 else 4
POOL_PATH = sys.argv[4] if len(sys.argv) > 4 else f"{GRIND}/nomatch.json"
BIG = 10 ** 9
gate_lock = threading.Lock()

def mod_of(fn): return re.match(r'func_([a-z0-9_]+?)_[0-9A-Fa-f]{8}$', fn).group(1)

def find_def(lines, fn):
    for i, l in enumerate(lines):
        if re.search(r'\b%s\s*\(' % re.escape(fn), l) and not l.rstrip().endswith(";"):
            return i
    return None

def extract_def(text, fn):
    lines = text.split("\n"); di = find_def(lines, fn)
    if di is None: return None
    depth = 0; started = False
    for i in range(di, len(lines)):
        depth += lines[i].count("{") - lines[i].count("}")
        if "{" in lines[i]: started = True
        if started and depth == 0:
            return "\n".join(lines[di:i + 1])
    return None

def seed_path(fn):
    for p in (f"{GRIND}/seeds/{fn}.c", f"progress/seeds/{fn}.c"):
        if os.path.exists(p): return p
    return None

def seed_preamble(fn):
    p = seed_path(fn)
    if not p: return ""
    lines = open(p).read().split("\n"); di = find_def(lines, fn)
    return "\n".join(lines[:di]).strip() if di is not None else ""

def swap_in(orig, pragma, fn, cand):
    """Pragma -> candidate, stripping any stale standalone prototype for fn (conflicting
    signature = guaranteed compile error; same behavior as consolidate2)."""
    t = orig.replace(pragma, cand)
    return re.sub(r'(?m)^[ \t]*[A-Za-z_][\w \t\*]*\b%s\s*\([^;{]*\)\s*;[ \t]*\r?\n'
                  % re.escape(fn), '', t)

def make_candidate(fn, body, orig, pragma):
    """Seed preamble (units the module doesn't already declare) + permuted body."""
    hay = re.sub(r'#pragma GLOBAL_ASM\([^)]*\)', '', orig)
    keep = []
    pre = seed_preamble(fn)
    if pre:
        units, buf, depth = [], [], 0
        for l in pre.split("\n"):
            s = l.strip()
            if not s or s.startswith("//"): continue
            buf.append(l); depth += l.count("{") - l.count("}")
            if depth <= 0 and s.endswith(";"):
                units.append("\n".join(buf)); buf = []; depth = 0
        for u in units:
            mm = re.search(r'}\s*(\w+)\s*;', u) or re.search(r'\b(D_\w+|g[A-Z]\w+|func_\w+)\b', u)
            key = mm.group(1) if mm else None
            if key and re.search(r'\b%s\b' % re.escape(key), hay):
                continue
            keep.append(u)
    return ("\n".join(keep) + "\n\n" if keep else "") + body

def integration_gate(fn, out_dir):
    """Module-hash gate for a score-0 output. Returns True iff it banks. Restores src."""
    mod = mod_of(fn); SRC = f"src/modules/{mod}.c"
    pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")'
    body = extract_def(open(f"{out_dir}/source.c").read(), fn)
    if not body: return False
    with gate_lock:
        orig = open(SRC).read()
        if pragma not in orig: return False
        cand = make_candidate(fn, body, orig, pragma)
        try:
            open(SRC, "w").write(swap_in(orig, pragma, fn, cand))
            subprocess.run(["rm", "-f", f"build/bin/us/{mod}.o", f"build/src/modules/{mod}.o"],
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            r = subprocess.run(f"make build/bin/us/{mod}.o 2>&1", shell=True,
                               capture_output=True, text=True)
            ok = "Hash verification passed" in (r.stdout + r.stderr)
        finally:
            open(SRC, "w").write(orig)
    if ok:
        open(f"{WINS}/{fn}.c", "w").write(cand)
    return ok

# ---- Phase 0: refresh pool from live pragmas ----
pool0 = json.load(open(POOL_PATH))
pool = []
for fn in pool0:
    m = re.match(r'func_([a-z0-9_]+?)_[0-9A-Fa-f]{8}$', fn)
    if not m: continue
    src = f"src/modules/{m.group(1)}.c"
    if os.path.exists(src) and f'/{fn}.s"' in open(src).read() and seed_path(fn):
        pool.append(fn)
print(f"pool: {len(pool0)} -> {len(pool)} (live pragma + seed present)", flush=True)

# ---- Phase A: fresh serial imports (stale contexts poison wins) ----
imported = []
for fn in pool:
    mod = mod_of(fn); SRC = f"src/modules/{mod}.c"
    orig = open(SRC).read()
    pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")'
    sp = seed_path(fn)
    # rebuild the full seed file (preamble+body) into the module for import
    cand = open(sp).read().rstrip()
    subprocess.run(f"rm -rf nonmatchings/{fn} nonmatchings/{fn}-*", shell=True)
    try:
        open(SRC, "w").write(swap_in(orig, pragma, fn, cand))
        r = subprocess.run(["python3", "tools/decomp-permuter/import.py", SRC,
                            f"asm/us/nonmatchings/modules/{mod}/{fn}.s"],
                           capture_output=True, text=True)
    finally:
        open(SRC, "w").write(orig)
    if "Done. Imported" in (r.stdout + r.stderr) and os.path.isdir(f"nonmatchings/{fn}"):
        imported.append(fn)
    else:
        print(f"IMPORT-FAIL {fn}", flush=True)
print(f"imported {len(imported)}/{len(pool)}", flush=True)

# ---- Phase B: laddered rounds, gate-on-zero ----
state = {fn: {"best": BIG, "status": "pool"} for fn in imported}

def best_output_score(fn):
    best = BIG
    for o in glob.glob(f"nonmatchings/{fn}/output*"):
        try: s = int(open(f"{o}/score.txt").read().strip())
        except Exception: continue
        best = min(best, s)
    return best

def run_one(fn, budget):
    d = f"nonmatchings/{fn}"
    # transfer-fail fns: drop --stop-on-zero so the permuter keeps producing distinct zeros
    stop = "--stop-on-zero " if state[fn]["status"] != "transfer_fail" else ""
    subprocess.run(f"timeout {budget} python3 tools/decomp-permuter/permuter.py {d} -j{JOBS} "
                   f"{stop}--best-only >/dev/null 2>&1", shell=True)
    state[fn]["best"] = best_output_score(fn)
    # gate every distinct score-0 output not yet judged
    for o in sorted(glob.glob(f"{d}/output*")):
        try: s = int(open(f"{o}/score.txt").read().strip())
        except Exception: continue
        if s != 0 or os.path.exists(f"{o}/.gated"): continue
        open(f"{o}/.gated", "w").write("1")
        if integration_gate(fn, o):
            state[fn]["status"] = "BANKABLE"
            print(f"  WIN  {fn} (gated: banks in module)", flush=True)
            return
        state[fn]["status"] = "transfer_fail"
        print(f"  zero-but-NOMATCH {fn} (transfer fail; will keep hunting)", flush=True)

active = list(imported)
for rnd, budget in enumerate(BUDGETS):
    if not active: break
    print(f"\n== round {rnd + 1}: {len(active)} fns x {budget}s (conc {CONC} x -j{JOBS}) ==", flush=True)
    with ThreadPoolExecutor(max_workers=CONC) as ex:
        list(ex.map(lambda fn: run_one(fn, budget), active))
    active = [fn for fn in active if state[fn]["status"] != "BANKABLE"]
    active.sort(key=lambda fn: state[fn]["best"])
    for fn in active[:12]:
        print(f"   {fn:42s} best={state[fn]['best'] if state[fn]['best'] < BIG else '-'}"
              f" {state[fn]['status']}", flush=True)
    keep = max(8, min(len(active) // 2, 120))
    if rnd < len(BUDGETS) - 1:
        active = active[:keep]

# record BANKABLEs for bank_wins.py
res_path = f"{GRIND}/sweep_results.json"
res = json.load(open(res_path)) if os.path.exists(res_path) else {}
wins = [fn for fn in state if state[fn]["status"] == "BANKABLE"]
for fn in wins: res[fn] = "BANKABLE"
json.dump(res, open(res_path, "w"), indent=1)
print(f"\n==== CAMPAIGN v2: {len(wins)} gated wins (run bank_wins.py to bank+commit) ====")
print("WINS:", " ".join(sorted(wins)))
tf = [fn for fn in state if state[fn]["status"] == "transfer_fail"]
if tf: print("TRANSFER-FAIL (had a zero that didn't bank):", " ".join(sorted(tf)))
