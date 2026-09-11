#!/usr/bin/env python3
"""Rank UNMATCHED (still-GLOBAL_ASM) BAR functions by estimated decomp difficulty.

Source of truth for "unmatched" is the GLOBAL_ASM pragmas in src/ (a .s file alone is not enough —
decompiled functions leave their .s behind). Reads each unmatched function's .s for difficulty.
Writes the full ranking to score_functions.json. Does not modify the repo.
"""
import os, re, json, glob, statistics

ROOT = os.path.dirname(os.path.abspath(__file__))
ASM_ROOT = os.path.join(ROOT, "asm", "us", "nonmatchings")

INSN_RE = re.compile(r"/\*\s*[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s*\*/\s*([a-z0-9.]+)")
SIZE_RE = re.compile(r",\s*0x([0-9A-Fa-f]+)")
PRAGMA = re.compile(r'GLOBAL_ASM\("(asm/us/nonmatchings/[^"]+)\.s"\)')
FLOAT_HINTS = (".s", ".d", "lwc1", "swc1", "ldc1", "sdc1", "mtc1", "mfc1", "cvt", "c.")
BRANCH_HINTS = ("beq", "bne", "blez", "bgtz", "bltz", "bgez", "bc1t", "bc1f", "b ", "bnez", "beqz")

# 1. the still-unmatched set = .s paths referenced by GLOBAL_ASM pragmas in src/
unmatched = set()
for cf in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
    for line in open(cf, encoding="utf-8", errors="replace"):
        m = PRAGMA.search(line)
        if m and "__entrypoint" not in m.group(1):
            unmatched.add(m.group(1) + ".s")

def analyze(path):
    insns = floats = calls = branches = 0
    labels = 0
    size = None
    for line in open(path, encoding="utf-8", errors="replace"):
        if size is None:
            mm = SIZE_RE.search(line)
            if mm and "glabel" not in line and "/*" not in line:
                size = int(mm.group(1), 16)
        if line.strip().startswith(".L"):
            labels += 1
        m = INSN_RE.search(line)
        if not m:
            continue
        insns += 1
        if any(h in line for h in FLOAT_HINTS):
            floats += 1
        if m.group(1) in ("jal", "jalr"):
            calls += 1
        tail = line.split("*/", 1)[-1].strip()
        if any(tail.startswith(b.strip()) for b in BRANCH_HINTS):
            branches += 1
    has_loop = branches > 0 and labels > 0
    score = (insns + floats * 4 + calls + branches * 1.5 + (8 if has_loop else 0)
             + (size or insns * 4) / 64.0)
    return dict(insns=insns, floats=floats, calls=calls, branches=branches,
                loop=has_loop, leaf=(calls == 0), size=size or 0, score=round(score, 1))

rows = []
for rel in sorted(unmatched):
    p = os.path.join(ROOT, rel)
    if not os.path.exists(p):
        continue
    a = analyze(p)
    a["func"] = os.path.basename(rel)[:-2]
    a["rel"] = os.path.relpath(p, ASM_ROOT)
    rows.append(a)

code = [r for r in rows if not r["func"].startswith(("D_", "RO_STR", "jtbl"))]
code.sort(key=lambda r: r["score"])
print(f"UNMATCHED CODE functions (GLOBAL_ASM): {len(code)}\n")
print("=== difficulty buckets ===")
print(f"leaf + no-float : {sum(1 for r in code if r['leaf'] and r['floats']==0)}")
print(f"small (<=30)    : {sum(1 for r in code if r['insns']<=30)}")
print(f"float-using     : {sum(1 for r in code if r['floats']>0)}")
print(f"large (>=80)    : {sum(1 for r in code if r['insns']>=80)}")
if code:
    print(f"median insns    : {statistics.median(r['insns'] for r in code):.0f}")
print("\n=== 20 easiest ===")
for r in code[:20]:
    print(f"  score={r['score']:>5} insn={r['insns']:>3} flt={r['floats']} {r['rel']}")
with open(os.path.join(ROOT, "score_functions.json"), "w") as f:
    json.dump(code, f)
print("\n(full ranking -> score_functions.json)")
