"""Dump Phase-2 decomp task cards (asm + m2c) for the given modules' remaining
functions, to a Windows-readable dir so workflow agents can Read them. Emits a
worklist. Phase-2 functions may have calls, loops, and typed-global access.

Usage: _p2cards.py <mod1> <mod2> ...
"""
import os, csv, sys, subprocess, re

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
CARDS = "/mnt/c/Users/Bryan/Projects/BeetleAdventureDecomp/p2cards"
os.makedirs(CARDS, exist_ok=True)
mods = set(sys.argv[1:])

led = {r["fn"]: r for r in csv.DictReader(open("progress/ledger.csv"))}
worklist = []
for fn, r in led.items():
    if r["mod"] not in mods or r["status"] == "banked":
        continue
    p = f"asm/us/nonmatchings/modules/{r['mod']}/{fn}.s"
    if not os.path.exists(p):
        continue
    worklist.append((int(r["size"]), r["mod"], fn, p, r))

worklist.sort()  # smallest first
for size, mod, fn, p, r in worklist:
    asm = open(p).read()
    m2c = subprocess.run(["python3", "tools/m2c/m2c.py", "-t", "mips-ido-c", p],
                         capture_output=True, text=True).stdout
    card = (f"FUNCTION: {fn}\nMODULE: {mod}\n"
            f"SIZE: 0x{size:X}  INSTRS: {r['ninstr']}  FLOAT_OPS: {r['nflt']}  "
            f"COND_BRANCHES: {r['ncond']}  HAS_LOOP: {r['has_loop']}  HAS_CALL: {r['has_call']}\n"
            f"\n===== TARGET ASM (authoritative) =====\n{asm}\n"
            f"===== m2c OUTPUT (hint; often wrong on types/widths) =====\n{m2c}\n")
    open(f"{CARDS}/{fn}.txt", "w").write(card)

open(f"{CARDS}/_worklist.txt", "w").write("\n".join(f"{m} {fn}" for _, m, fn, _, _ in worklist))
print(f"p2 cards: {len(worklist)} functions across {len(mods)} modules -> {CARDS}")
from collections import Counter
c = Counter(m for _, m, _, _, _ in worklist)
for m, n in c.most_common():
    print(f"  {m:18s} {n}")
