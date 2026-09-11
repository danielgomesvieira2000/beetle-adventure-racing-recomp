#!/usr/bin/env python3
"""Generate decomp task cards (asm + m2c seed) for HUD/menu unmatched functions.
Run in WSL: python3 /mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind/gen_cards.py <max_size> <mod1,mod2,...|ALL>
Writes cards + worklist.json to the .grind/cards dir on /mnt/c, stages headers.
"""
import csv, os, sys, subprocess, json, shutil

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
OUT = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind"
CARDS = f"{OUT}/cards"
HDRS = f"{OUT}/headers"
os.makedirs(CARDS, exist_ok=True)
os.makedirs(HDRS, exist_ok=True)

MAXSZ = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MINSZ = int(sys.argv[3]) if len(sys.argv) > 3 else 0
HUD = ['letter','cbars','scrn','menuslct','filmroll','flag','glare','gamegui','pause','results',
       'fileux','filerom','med','battle','selection','plyr','scene','cam','demo','intro','logo','splash']
if len(sys.argv) > 2 and sys.argv[2] != 'ALL':
    HUD = sys.argv[2].split(',')

rows = list(csv.DictReader(open('progress/ledger.csv')))
work = []
for r in rows:
    if r['mod'] not in HUD or r['status'] == 'banked':
        continue
    if int(r['size']) > MAXSZ or int(r['size']) <= MINSZ:
        continue
    p = f"asm/us/nonmatchings/modules/{r['mod']}/{r['fn']}.s"
    if not os.path.exists(p):
        continue
    work.append((int(r['size']), r['mod'], r['fn'], p, r))
work.sort()  # smallest first

worklist = []
for size, mod, fn, p, r in work:
    asm = open(p).read()
    try:
        m2c = subprocess.run(["python3", "tools/m2c/m2c.py", "-t", "mips-ido-c", p],
                             capture_output=True, text=True, timeout=60).stdout
    except Exception as e:
        m2c = f"/* m2c failed: {e} */"
    card = (f"FUNCTION: {fn}\nMODULE: {mod}\nSIZE: {size}  HAS_CALL: {r.get('has_call')}  "
            f"NFLT: {r.get('nflt')}  HAS_LOOP: {r.get('has_loop')}\n\n"
            f"=== TARGET ASM (this is what your C must compile to, byte-for-byte) ===\n{asm}\n\n"
            f"=== M2C FIRST-PASS SEED (types/signatures are GUESSES; fix them) ===\n{m2c}\n")
    open(f"{CARDS}/{mod}__{fn}.txt", "w").write(card)
    worklist.append({"mod": mod, "fn": fn, "size": size, "card": f"{mod}__{fn}.txt"})

json.dump(worklist, open(f"{OUT}/worklist.json", "w"), indent=1)

# stage headers agents should READ (types/prototypes to use instead of inventing)
for h in ['common.h','structs.h','functions.h','variables.h','global_exports.h','macros.h',
          'file_manager.h','mem_allocator.h','uv_memory.h','uv_filesystem.h','selection.h',
          'fileux.h','game.h','intro.h','snd.h']:
    src = f"include/{h}"
    if os.path.exists(src):
        shutil.copy(src, f"{HDRS}/{h}")

print(f"cards: {len(worklist)}  (max_size={MAXSZ})  modules={sorted(set(w['mod'] for w in worklist))}")
from collections import Counter
c = Counter(w['mod'] for w in worklist)
for m, n in c.most_common():
    print(f"  {m}: {n}")
