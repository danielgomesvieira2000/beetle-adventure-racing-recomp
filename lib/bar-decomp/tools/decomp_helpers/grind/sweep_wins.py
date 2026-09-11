#!/usr/bin/env python3
"""REPORT-ONLY sweep: find score-0 permuter outputs across nonmatchings/*, gate each
through the module DaisyBox hash (same swap/restore as _tryfunc.py), and report
BANKABLE / NOMATCH / BUILDERR. Never leaves changes. Usage: sweep_wins.py [fn ...]
"""
import os, re, glob, subprocess, sys, json
from collections import Counter

REPO = os.path.expanduser("~/projects/bar-decomp"); os.chdir(REPO)
GRIND = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind"

st = subprocess.run("git status --porcelain", shell=True, capture_output=True, text=True).stdout
print("git-dirty-before:", (st.strip()[:200] or "CLEAN"))

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

def seed_preamble(fn):
    for p in (f"{GRIND}/seeds/{fn}.c", f"progress/seeds/{fn}.c", f"progress/wins/{fn}.c"):
        if os.path.exists(p):
            lines = open(p).read().split("\n"); di = find_def(lines, fn)
            if di is not None:
                return "\n".join(lines[:di]).strip(), p
    return "", None

only = set(sys.argv[1:])
wins = []
for sd in sorted(glob.glob("nonmatchings/func_*")):
    fn = os.path.basename(sd)
    if "-" in fn: continue  # skip func_x-2 style dup imports
    if only and fn not in only: continue
    for o in sorted(glob.glob(f"{sd}/output*")):
        try: s = int(open(f"{o}/score.txt").read().strip())
        except Exception: continue
        if s == 0:
            wins.append((fn, o)); break
print(f"score-0 outputs found: {len(wins)}")

results = {}
for fn, o in wins:
    m = re.match(r'func_([a-z0-9_]+?)_[0-9A-Fa-f]{8}$', fn)
    if not m: results[fn] = "BAD-NAME"; continue
    mod = m.group(1)
    SRC = f"src/modules/{mod}.c"
    if not os.path.exists(SRC): results[fn] = "NO-MODULE"; continue
    orig = open(SRC).read()
    pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")'
    if pragma not in orig:
        results[fn] = "ALREADY-BANKED"; print(f"  {fn:42s} ALREADY-BANKED"); continue
    body = extract_def(open(f"{o}/source.c").read(), fn)
    if not body: results[fn] = "NO-DEF"; continue
    pre, src_used = seed_preamble(fn)
    # keep only preamble units whose key symbol the module doesn't already declare
    hay = re.sub(r'#pragma GLOBAL_ASM\([^)]*\)', '', orig)
    keep = []
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
    cand = ("\n".join(keep) + "\n\n" if keep else "") + body
    try:
        open(SRC, "w").write(orig.replace(pragma, cand))
        subprocess.run(["rm", "-f", f"build/bin/us/{mod}.o", f"build/src/modules/{mod}.o"],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        r = subprocess.run(f"make build/bin/us/{mod}.o 2>&1", shell=True, capture_output=True, text=True)
        out = r.stdout + r.stderr
    finally:
        open(SRC, "w").write(orig)
    res = "BANKABLE" if "Hash verification passed" in out else ("NOMATCH" if "Hash verification failed" in out else "BUILDERR")
    if res == "BUILDERR":
        err = next((l.strip() for l in out.split("\n") if "error" in l.lower()), "")[:100]
        print(f"  {fn:42s} {res}  {err}")
    else:
        print(f"  {fn:42s} {res}   (pre: {os.path.basename(src_used) if src_used else '-'})")
    results[fn] = res

print("\nSUMMARY:", dict(Counter(results.values())))
json.dump(results, open(f"{GRIND}/sweep_results.json", "w"), indent=1)
st = subprocess.run("git status --porcelain", shell=True, capture_output=True, text=True).stdout
print("git-dirty-after:", (st.strip()[:200] or "CLEAN"))
