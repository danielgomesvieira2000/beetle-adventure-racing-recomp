#!/bin/bash
# Usage: _permute.sh <module> <func> <candidate.c> <timeout_secs>
# Inserts a seed C body, imports into decomp-permuter, restores the module
# pragma, runs the permuter, and reports whether a score-0 source was found.
set -e
cd ~/projects/bar-decomp
source .venv/bin/activate
MOD="$1"; FN="$2"; CAND="$3"; TMO="${4:-220}"
SRC="src/modules/${MOD}.c"
ASM="asm/us/nonmatchings/modules/${MOD}/${FN}.s"
PRAGMA="#pragma GLOBAL_ASM(\"asm/us/nonmatchings/modules/${MOD}/${FN}.s\")"

python3 - "$SRC" "$CAND" "$PRAGMA" <<'PY'
import sys
src, cand, pragma = sys.argv[1], sys.argv[2], sys.argv[3]
t = open(src).read()
c = open(cand).read().rstrip()
assert pragma in t, "pragma not found"
open(src + ".permbak", "w").write(t)
open(src, "w").write(t.replace(pragma, c))
print("seed inserted")
PY

echo "=== import ==="
python3 tools/decomp-permuter/import.py "$SRC" "$ASM" 2>&1 | tail -3
# restore the live module to its pragma form
mv -f "${SRC}.permbak" "$SRC"
echo "restored $SRC"

echo "=== permute (timeout ${TMO}s) ==="
rm -rf "nonmatchings/${FN}/output"*
timeout "$TMO" python3 tools/decomp-permuter/permuter.py "nonmatchings/${FN}/" -j4 --stop-on-zero --best-only 2>&1 | grep -oE "base score = [0-9]+" | head -1 || true

D=$(ls -d nonmatchings/${FN}/output* 2>/dev/null | head -1 || true)
if [ -n "$D" ]; then
  echo "BEST SCORE: $(cat "$D/score.txt")"
  echo "=== winning source ==="
  sed -n "/${FN}/,/^}/p" "$D/source.c"
else
  echo "NO output saved (no improvement / no score-0)"
fi
