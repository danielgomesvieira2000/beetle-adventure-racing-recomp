"""Categorize every remaining GLOBAL_ASM function in src/modules/*.c by static
asm features + m2c quality, to scope remaining decomp work. Writes a full CSV
and prints aggregate tables.

Usage: _categorize.py [--m2c]   (--m2c also runs m2c per func for quality flags)
"""
import os, re, glob, sys, subprocess
from concurrent.futures import ThreadPoolExecutor

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
RUN_M2C = "--m2c" in sys.argv

CALL_MN = {"jal", "jalr"}
BR_MN = {"b", "beq", "bne", "beqz", "bnez", "blez", "bgez", "bltz", "bgtz",
         "bgezal", "bltzal", "bc1t", "bc1f",
         "beql", "bnel", "beqzl", "bnezl", "blezl", "bgezl", "bltzl", "bgtzl",
         "bc1tl", "bc1fl"}
FLT_RE = re.compile(r'^(lwc1|swc1|ldc1|sdc1|add\.[sd]|sub\.[sd]|mul\.[sd]|div\.[sd]|'
                    r'neg\.[sd]|abs\.[sd]|sqrt\.[sd]|cvt\.|c\.[a-z]+\.[sd]|mov\.[sd]|'
                    r'mtc1|mfc1|trunc\.|round\.|ceil\.|floor\.)')
INTMD = {"mult", "multu", "div", "divu", "dmult", "dmultu", "ddiv", "ddivu"}

def parse(path):
    rows = []
    for line in open(path):
        m = re.search(r'\*/\s+(\S+)\s*(.*?)\s*$', line)
        if m:
            mn, ops = m.group(1), m.group(2).strip()
            am = re.search(r'/\*\s+\w+\s+([0-9A-Fa-f]{8})\s', line)
            addr = int(am.group(1), 16) if am else None
            rows.append((addr, mn, ops))
    return rows

def feats(mod, fn, path):
    first = open(path).readline()
    sm = re.search(r'0x([0-9A-Fa-f]+)', first)
    size = int(sm.group(1), 16) if sm else 0
    rows = parse(path)
    ninstr = len(rows)
    has_call = any(mn in CALL_MN for _, mn, _ in rows)
    ncond = 0
    has_loop = False
    nflt = 0
    has_intmd = False
    for addr, mn, ops in rows:
        if mn in BR_MN:
            ncond += 1
            tm = re.search(r'\.L([0-9A-Fa-f]{8})', ops)
            if tm and addr is not None and int(tm.group(1), 16) <= addr:
                has_loop = True
        if FLT_RE.match(mn):
            nflt += 1
        if mn in INTMD:
            has_intmd = True
    rec = dict(mod=mod, fn=fn, size=size, ninstr=ninstr, has_call=has_call,
               ncond=ncond, has_loop=has_loop, nflt=nflt, has_intmd=has_intmd,
               m2c_ok="", m2c_struct="", m2c_flow="", m2c_calls="")
    if RUN_M2C:
        try:
            out = subprocess.run(["python3", "tools/m2c/m2c.py", "-t", "mips-ido-c", path],
                                 capture_output=True, text=True, timeout=30).stdout
            rec["m2c_ok"] = int(bool(out) and "MIPS2C" not in out and "?" not in out)
            rec["m2c_struct"] = int("->" in out)
            rec["m2c_flow"] = int(any(k in out for k in ("while", "for ", "goto", "do {", "do{")))
            rec["m2c_calls"] = int(bool([m for m in re.findall(r'\b(func_\w+)\s*\(', out) if m != fn]))
        except Exception:
            rec["m2c_ok"] = -1
    return rec

# collect
jobs = []
for c in glob.glob("src/modules/*.c"):
    mod = os.path.basename(c)[:-2]
    txt = open(c).read()
    for fn in re.findall(r'GLOBAL_ASM\("asm/us/nonmatchings/modules/%s/(func_%s_[0-9A-Fa-f]+)\.s"\)' % (mod, mod), txt):
        p = f"asm/us/nonmatchings/modules/{mod}/{fn}.s"
        if os.path.exists(p):
            jobs.append((mod, fn, p))

with ThreadPoolExecutor(max_workers=6 if RUN_M2C else 12) as ex:
    recs = list(ex.map(lambda a: feats(*a), jobs))

# write CSV
cols = ["mod", "fn", "size", "ninstr", "has_call", "ncond", "has_loop", "nflt",
        "has_intmd", "m2c_ok", "m2c_struct", "m2c_flow", "m2c_calls"]
with open("/tmp/remaining.csv", "w") as f:
    f.write(",".join(cols) + "\n")
    for r in recs:
        f.write(",".join(str(r[c]) for c in cols) + "\n")

# aggregates
n = len(recs)
def cnt(pred): return sum(1 for r in recs if pred(r))
print(f"TOTAL remaining functions: {n}")
print(f"\n-- by top-level structure --")
nocall_noloop = cnt(lambda r: not r['has_call'] and not r['has_loop'])
nocall_loop = cnt(lambda r: not r['has_call'] and r['has_loop'])
call = cnt(lambda r: r['has_call'])
print(f"  no-call, no-loop (straight-line/branch) : {nocall_noloop}")
print(f"  no-call, has-loop                        : {nocall_loop}")
print(f"  has-call (jal/jalr -> needs other fns)   : {call}")
print(f"\n-- float content --")
print(f"  pure-int (no float ops)                  : {cnt(lambda r: r['nflt']==0)}")
print(f"  some float                               : {cnt(lambda r: r['nflt']>0)}")
print(f"  float-heavy (>=6 float ops)              : {cnt(lambda r: r['nflt']>=6)}")
print(f"\n-- size buckets (bytes) --")
for lo, hi in [(0,0x20),(0x20,0x40),(0x40,0x80),(0x80,0x100),(0x100,0x200),(0x200,1<<20)]:
    print(f"  0x{lo:X}-0x{hi:X}: {cnt(lambda r: lo<=r['size']<hi)}")
print(f"\n-- PERMUTER CAMPAIGN POOL (no-call, no-loop; permuter can search) --")
p_int = cnt(lambda r: not r['has_call'] and not r['has_loop'] and r['nflt']==0)
p_flt = cnt(lambda r: not r['has_call'] and not r['has_loop'] and r['nflt']>0)
print(f"  no-call no-loop, pure-int  : {p_int}")
print(f"  no-call no-loop, float     : {p_flt}")
print(f"  no-call no-loop, SMALL(<0x60): {cnt(lambda r: not r['has_call'] and not r['has_loop'] and r['size']<0x60)}")
if RUN_M2C:
    print(f"\n-- m2c quality --")
    print(f"  m2c clean (no ?/MIPS2C)    : {cnt(lambda r: r['m2c_ok']==1)}")
    print(f"  m2c clean & no-call & no-flow & resolvable : {cnt(lambda r: r['m2c_ok']==1 and not r['m2c_calls'] and not r['m2c_flow'])}")
print(f"\n-- per-module remaining (top 30) --")
from collections import Counter
mc = Counter(r['mod'] for r in recs)
for mod, c in mc.most_common(30):
    mr = [r for r in recs if r['mod']==mod]
    nc = sum(1 for r in mr if r['has_call'])
    print(f"  {mod:20s} {c:4d}  (calls:{nc:3d}  loops:{sum(1 for r in mr if r['has_loop']):3d}  float:{sum(1 for r in mr if r['nflt']>0):3d})")
print(f"\nCSV written: /tmp/remaining.csv")
