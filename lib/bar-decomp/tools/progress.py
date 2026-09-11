#!/usr/bin/env python3
"""Decomp progress for this fork.

Counts REMAINING functions from the GLOBAL_ASM pragmas in src/ (the source of truth) — NOT from
.s files, which linger after a function is decompiled and overcount. Grouped by module/segment.
The matched count is a heuristic (C function-definition lines) and is labelled approximate.

Usage: python3 tools/progress.py [--top N] [--json]
"""
import os, re, sys, json, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src")
PRAGMA = re.compile(r'GLOBAL_ASM\("asm/us/nonmatchings/(?:modules/([^/]+)|([^/]+))/([^"]+)\.s"\)')
FUNC_DEF = re.compile(r"^[A-Za-z_][\w\s\*]*\b[A-Za-z_]\w*\s*\([^;]*\)\s*\{?\s*$")

top_n = 20
if "--top" in sys.argv:
    top_n = int(sys.argv[sys.argv.index("--top") + 1])
as_json = "--json" in sys.argv

remaining = collections.Counter()   # GLOBAL_ASM (still-asm) functions per module/segment
matched = 0
for root, _, files in os.walk(SRC):
    for fn in files:
        if not fn.endswith(".c"):
            continue
        for line in open(os.path.join(root, fn), encoding="utf-8", errors="replace"):
            m = PRAGMA.search(line)
            if m:
                if m.group(3).startswith("__entrypoint"):
                    continue
                remaining[m.group(1) or m.group(2)] += 1
            elif FUNC_DEF.match(line) and "return" not in line and "=" not in line:
                matched += 1

total_remaining = sum(remaining.values())
total = total_remaining + matched
pct = (100.0 * matched / total) if total else 0.0

if as_json:
    print(json.dumps(dict(matched_approx=matched, remaining=total_remaining,
                          pct_approx=round(pct, 1), per_module=dict(remaining))))
    sys.exit(0)

print("=" * 60)
print(" Beetle Adventure Racing decomp — progress")
print("=" * 60)
print(f"  matched functions (approx)  : {matched}")
print(f"  remaining functions (exact) : {total_remaining}   [GLOBAL_ASM pragmas]")
print(f"  estimated completion        : {pct:.1f}%  (approx — function count)")
print(f"  (for byte-accurate %, use scripts/gen_report.sh)")
print()
mods = sorted(remaining, key=lambda m: -remaining[m])
print(f"  Top {top_n} modules by remaining functions:")
print(f"  {'module':<28}{'remaining':>10}")
print("  " + "-" * 40)
for m in mods[:top_n]:
    print(f"  {m:<28}{remaining[m]:>10}")
print()
print("  Tip: complete a module, then `scripts/check.sh --module <m>` verifies it fast.")
