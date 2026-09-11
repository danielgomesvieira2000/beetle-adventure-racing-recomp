"""Anti-nonsense review of a permuter score-0 winner. A byte-identical match can
still be garbage C (dead vars, casts to unrelated types, magic constants where a
struct field belongs) that would poison callers. This diffs the winner against
the seed and flags suspicious mutations. Output: PASS or REVIEW: <reasons>.

Usage: _winreview.py <seed.c> <winner.c>
"""
import re, sys

seed = open(sys.argv[1]).read()
win = open(sys.argv[2]).read()

def body(t):
    i = t.find("{")
    return t[i:] if i >= 0 else t
sb, wb = body(seed), body(win)

reasons = []

# 1) new local variables declared but never used (permuter 'new_var' padding)
decls = re.findall(r'\b(?:s8|u8|s16|u16|s32|u32|f32|f64|void|int|char|float|double)\s*\*?\s*(\w+)\s*;', wb)
for v in decls:
    uses = len(re.findall(r'\b%s\b' % re.escape(v), wb))
    if uses <= 1:
        reasons.append(f"unused-local:{v}")
if re.search(r'\bnew_var\b|\bsp[0-9A-Fa-f]{1,3}\b\s*;', wb):
    reasons.append("permuter-padding-var")

# 2) volatile introduced by permuter (classic nonsense-match crutch)
if "volatile" in wb and "volatile" not in sb:
    reasons.append("introduced-volatile")

# 3) statements with no effect the seed lacked: (0, 0) comma exprs, if(x){}
if re.search(r'=\s*\(0,\s*0\)', wb) or re.search(r'if\s*\([^)]*\)\s*\{\s*\}', wb):
    reasons.append("no-effect-stmt")

# 4) casts to types not present in the seed
seed_casts = set(re.findall(r'\(([A-Za-z_]\w*)\s*\*?\)', sb))
win_casts = set(re.findall(r'\(([A-Za-z_]\w*)\s*\*?\)', wb))
new_casts = win_casts - seed_casts - {"void", "f32", "s32", "u32", "s16", "u16", "s8", "u8"}
if new_casts:
    reasons.append("new-casts:" + ",".join(sorted(new_casts)))

# 5) gross size blowup (winner much longer than seed)
if len(wb) > len(sb) * 1.6 + 80:
    reasons.append("size-blowup")

print("PASS" if not reasons else "REVIEW: " + "; ".join(reasons))
