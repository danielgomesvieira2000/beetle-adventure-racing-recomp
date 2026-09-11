"""Dump per-function task cards for the Phase-1 hand-seed track: Tier-A
(no-call, no-loop) functions WITHOUT struct access that m2c couldn't cleanly
seed. Cards (asm + m2c output) are written to the Windows project dir so
workflow agents can Read them directly. Struct-touching functions are routed to
Phase 2 instead (printed separately).

Usage: _handseed_cards.py
"""
import os, csv, subprocess, re

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
CARDS = "/mnt/c/Users/Bryan/Projects/BeetleAdventureDecomp/handseed"
os.makedirs(CARDS, exist_ok=True)

rows = list(csv.DictReader(open("progress/ledger.csv")))
def f(x): return str(x) == "True"

handseed, struct = [], []
for r in rows:
    if f(r["has_call"]) or f(r["has_loop"]):
        continue
    if r["status"] in ("banked", "ready", "import-fail", "deferred-manual"):
        continue
    if r["band"] not in ("S", "P1", "P2", "P3"):
        continue
    if r["m2c_struct"] == "1" or r["band"] == "P3":
        struct.append(r)
    else:
        handseed.append(r)

# sort by size (smallest first)
handseed.sort(key=lambda r: int(r["size"]))
written = []
for r in handseed:
    mod, fn = r["mod"], r["fn"]
    p = f"asm/us/nonmatchings/modules/{mod}/{fn}.s"
    if not os.path.exists(p):
        continue
    asm = "".join(l for l in open(p))
    m2c = subprocess.run(["python3", "tools/m2c/m2c.py", "-t", "mips-ido-c", p],
                         capture_output=True, text=True).stdout
    card = (f"FUNCTION: {fn}\nMODULE: {mod}\nSIZE: 0x{int(r['size']):X}  "
            f"INSTRS: {r['ninstr']}  FLOAT_OPS: {r['nflt']}  COND_BRANCHES: {r['ncond']}\n"
            f"\n===== TARGET ASM =====\n{asm}\n===== m2c OUTPUT (starting point; may be wrong) =====\n{m2c}\n")
    open(f"{CARDS}/{fn}.txt", "w").write(card)
    written.append(fn)

print(f"hand-seed cards (Tier-A, no-struct): {len(written)} -> {CARDS}")
print(f"routed to Phase 2 (struct-typing): {len(struct)}")
# emit the worklist
open(f"{CARDS}/_worklist.txt", "w").write("\n".join(f"{r['mod']} {r['fn']}" for r in handseed if r['fn'] in written))
for r in handseed[:0]:
    pass
print("worklist:", f"{CARDS}/_worklist.txt")
