"""Integrate approved Phase-1 permuter/base-0 winners into their modules.
For each approved func: extract the function definition VERBATIM (preserving line
layout - IDO is line-sensitive) from progress/wins/<fn>.c, collect needed externs
(from the winner file + the module's m2cwave spec), build a per-module handwave
spec, and run _handwave.py (which build-verifies + bisects). Prints MATCHED/FAILED.

Usage: _wins_integrate.py <fn1> <fn2> ...
"""
import os, re, sys, json, csv, subprocess

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
WIN = "/mnt/c/Users/Bryan/Projects/BeetleAdventureDecomp"
approved = sys.argv[1:]

mod_of = {}
for r in csv.DictReader(open("progress/ledger.csv")):
    mod_of[r["fn"]] = r["mod"]

def extract_def(text, fn):
    lines = text.split("\n")
    di = None
    for i, l in enumerate(lines):
        if re.search(r'\b%s\s*\(' % re.escape(fn), l) and not l.rstrip().endswith(";"):
            di = i; break
    if di is None:
        return None
    depth = 0; started = False; end = None
    for i in range(di, len(lines)):
        depth += lines[i].count("{") - lines[i].count("}")
        if "{" in lines[i]: started = True
        if started and depth == 0:
            end = i; break
    return "\n".join(lines[di:end+1])

by_mod = {}
for fn in approved:
    mod = mod_of[fn]
    win = open(f"progress/wins/{fn}.c").read()
    code = extract_def(win, fn)
    if not code:
        print(f"FAILED: {fn} (no def extracted)"); continue
    externs = [l.strip() for l in win.split("\n") if l.strip().startswith("extern ")]
    by_mod.setdefault(mod, {"funcs": {}, "externs": set()})
    by_mod[mod]["funcs"][fn] = code
    for e in externs:
        by_mod[mod]["externs"].add(e)

for mod, d in by_mod.items():
    # augment externs from m2cwave spec
    sp = f"/tmp/spec_{mod}.json"
    if os.path.exists(sp):
        spec = json.load(open(sp))
        for e in spec.get("externs", []):
            syms = re.findall(r'\bD_\w+', e)
            if syms and any(syms[0] in c for c in d["funcs"].values()):
                d["externs"].add(e)
    out = {"module": mod, "externs": sorted(d["externs"]), "funcs": d["funcs"]}
    path = f"/tmp/winspec_{mod}.json"
    json.dump(out, open(path, "w"), indent=1)
    print(f"--- {mod}: {len(d['funcs'])} func(s) ---")
    r = subprocess.run(["python3", f"{WIN}/_handwave.py", path], capture_output=True, text=True)
    for line in r.stdout.split("\n"):
        if line.startswith(("MATCHED", "FAILED", "SUMMARY")):
            print(line)
