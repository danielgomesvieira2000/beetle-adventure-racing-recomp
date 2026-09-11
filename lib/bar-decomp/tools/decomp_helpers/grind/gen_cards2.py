#!/usr/bin/env python3
"""Generate decomp task cards (asm + CONTEXT-AWARE m2c seed) for unmatched functions.
v2 of gen_cards.py: builds a per-module preprocessed context (module .c with GLOBAL_ASM
pragmas stripped, run through cpp with the real build defines/includes) and passes it to
m2c via --context, so seed signatures/types come from the module's actual declarations
instead of guesses. Falls back to context-free m2c if cpp/m2c chokes.

Run in WSL: python3 gen_cards2.py <max_size> <mod1,mod2,...|ALL> [min_size]
"""
import csv, os, re, sys, subprocess, json, shutil, tempfile

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
OUT = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind"
CARDS = f"{OUT}/cards"; HDRS = f"{OUT}/headers"; CTXS = f"{OUT}/ctx"
for d in (CARDS, HDRS, CTXS): os.makedirs(d, exist_ok=True)

MAXSZ = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MINSZ = int(sys.argv[3]) if len(sys.argv) > 3 else 0
HUD = ['letter','cbars','scrn','menuslct','filmroll','flag','glare','gamegui','pause','results',
       'fileux','filerom','med','battle','selection','plyr','scene','cam','demo','intro','logo','splash']
MODS = sys.argv[2].split(',') if len(sys.argv) > 2 and sys.argv[2] != 'ALL' else HUD

# include/define flags lifted from the real module compile command (make --dry-run)
CPP_FLAGS = ("-P -undef -nostdinc -I include -I . -I include/libc -I include/2.0I -I include/2.0I/PR "
             "-I bin -I tools/ultralib/include -DVERSION_US=1 -D_MIPS_SZLONG=32 -DNDEBUG -DF3DEX_GBI_2 "
             "-D_FINALROM -DTARGET_N64 -DSSSV -DLANGUAGE_C -D_LANGUAGE_C -D_MIPS_ISA=2 -DMIPSEB")

def module_context(mod):
    """Preprocess the module (pragmas stripped) into an m2c context file. Cached per run."""
    ctx = f"{CTXS}/{mod}_ctx.c"
    src = f"src/modules/{mod}.c"
    if not os.path.exists(src): return None
    text = re.sub(r'(?m)^\s*#pragma GLOBAL_ASM\([^)]*\)\s*$', '', open(src).read())
    with tempfile.NamedTemporaryFile("w", suffix=".c", dir=".", delete=False) as tf:
        tf.write(text); tmp = tf.name
    try:
        r = subprocess.run(f"cpp {CPP_FLAGS} {tmp}", shell=True, capture_output=True, text=True)
        if r.returncode != 0 or not r.stdout.strip():
            return None
        open(ctx, "w").write(r.stdout)
        return ctx
    finally:
        os.unlink(tmp)

rows = list(csv.DictReader(open('progress/ledger.csv')))
work = []
for r in rows:
    if r['mod'] not in MODS or r['status'] == 'banked':
        continue
    if int(r['size']) > MAXSZ or int(r['size']) <= MINSZ:
        continue
    p = f"asm/us/nonmatchings/modules/{r['mod']}/{r['fn']}.s"
    # trust the live pragma over ledger status (ledger drifts)
    src = f"src/modules/{r['mod']}.c"
    if not os.path.exists(p) or not os.path.exists(src) or f"/{r['fn']}.s\"" not in open(src).read():
        continue
    work.append((int(r['size']), r['mod'], r['fn'], p, r))
work.sort()

ctx_cache = {}
worklist = []
for size, mod, fn, p, r in work:
    asm = open(p).read()
    if mod not in ctx_cache:
        ctx_cache[mod] = module_context(mod)
    ctx = ctx_cache[mod]
    m2c = None
    if ctx:
        try:
            rr = subprocess.run(["python3", "tools/m2c/m2c.py", "-t", "mips-ido-c",
                                 "--context", ctx, p], capture_output=True, text=True, timeout=90)
            if rr.returncode == 0 and rr.stdout.strip():
                m2c = rr.stdout
        except Exception:
            pass
    if m2c is None:
        try:
            m2c = subprocess.run(["python3", "tools/m2c/m2c.py", "-t", "mips-ido-c", p],
                                 capture_output=True, text=True, timeout=60).stdout
            m2c = "/* NOTE: context-free m2c (module context failed to build) */\n" + m2c
        except Exception as e:
            m2c = f"/* m2c failed: {e} */"
    card = (f"FUNCTION: {fn}\nMODULE: {mod}\nSIZE: {size}  HAS_CALL: {r.get('has_call')}  "
            f"NFLT: {r.get('nflt')}  HAS_LOOP: {r.get('has_loop')}\n\n"
            f"=== TARGET ASM (this is what your C must compile to, byte-for-byte) ===\n{asm}\n\n"
            f"=== M2C SEED (module-context-aware; still verify every type) ===\n{m2c}\n")
    open(f"{CARDS}/{mod}__{fn}.txt", "w").write(card)
    worklist.append({"mod": mod, "fn": fn, "size": size, "card": f"{mod}__{fn}.txt"})

json.dump(worklist, open(f"{OUT}/worklist.json", "w"), indent=1)
for h in ['common.h','structs.h','functions.h','variables.h','global_exports.h','macros.h',
          'file_manager.h','mem_allocator.h','uv_memory.h','uv_filesystem.h','selection.h',
          'fileux.h','game.h','intro.h','snd.h']:
    if os.path.exists(f"include/{h}"):
        shutil.copy(f"include/{h}", f"{HDRS}/{h}")

print(f"cards: {len(worklist)} (max_size={MAXSZ}) ctx_ok={[m for m,c in ctx_cache.items() if c]}")
from collections import Counter
for m, n in Counter(w['mod'] for w in worklist).most_common():
    print(f"  {m}: {n}")
