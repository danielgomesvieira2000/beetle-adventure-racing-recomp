#!/usr/bin/env python3
"""Byte-gate + bank the agent seeds, per module. Run in WSL.
  python3 /mnt/c/.../.grind/consolidate.py
Copies .grind/seeds/*.c -> progress/seeds/, runs _seedconsolidate.py for each module that has
seeds (which banks matching functions into src/modules/<mod>.c), then full-builds + SHA-verifies.
"""
import os, glob, shutil, subprocess, re, sys
from collections import defaultdict

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
GRIND_SEEDS = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind/seeds"
os.makedirs("progress/seeds", exist_ok=True)

# 1. stage seeds into the repo
seeds = glob.glob(f"{GRIND_SEEDS}/func_*.c")
by_mod = defaultdict(list)
for s in seeds:
    fn = os.path.basename(s)[:-2]
    m = re.match(r'func_([a-z0-9]+)_[0-9A-Fa-f]+$', fn)
    if not m:
        continue
    mod = m.group(1)
    shutil.copy(s, f"progress/seeds/{os.path.basename(s)}")
    by_mod[mod].append(fn)

print(f"staged {len(seeds)} seeds across {len(by_mod)} modules: {dict((k,len(v)) for k,v in by_mod.items())}")

# 2. consolidate per module (byte-gated bank)
total_matched = []
for mod in sorted(by_mod):
    print(f"\n===== consolidate {mod} ({len(by_mod[mod])} seeds) =====", flush=True)
    r = subprocess.run(["python3", "tools/decomp_helpers/_seedconsolidate.py", mod],
                       capture_output=True, text=True)
    out = r.stdout + r.stderr
    print(out, flush=True)
    for line in out.split("\n"):
        if line.strip().startswith("MATCH  "):
            total_matched.append(line.split()[1])

# 3. full build + verify still byte-exact
print("\n===== full build + SHA verify =====", flush=True)
b = subprocess.run("make -j6 2>&1 | tail -4", shell=True, capture_output=True, text=True)
print(b.stdout + b.stderr)
sha = subprocess.run("sha1sum build/beetleadventurerac.us.z64", shell=True, capture_output=True, text=True).stdout.strip()
print("SHA:", sha)
print("expected: e5ab4d226c08d22f68a2edcc48870203e67454b8")
ok = "e5ab4d226c08d22f68a2edcc48870203e67454b8" in sha
print(f"\n==== WAVE RESULT: banked {len(total_matched)} functions; ROM {'OK' if ok else 'MISMATCH!!'} ====")
if total_matched:
    print("BANKED:", " ".join(total_matched))
