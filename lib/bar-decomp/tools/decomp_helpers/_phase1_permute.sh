#!/bin/bash
# Parallel permute the "ready" Phase-1 functions (nonmatchings/<fn>/ dirs already
# created by _phase1_prep.py). base==0 -> seed already matches (winner=seed).
# 1<=base<=CAP -> permute (timeout each, P-way parallel, isolated dirs = safe).
# base>CAP -> defer. Collects winners into progress/wins/<fn>.c.
#
# Usage: _phase1_permute.sh [CAP_BASE] [TIMEOUT] [PAR]
set -u
cd ~/projects/bar-decomp
source .venv/bin/activate
CAP=${1:-1300}
TMO=${2:-300}
PAR=${3:-4}
mkdir -p progress/wins
LED=progress/ledger.csv

# build worklist: "fn base" for ready funcs
python3 - "$CAP" <<'PY' > /tmp/permlist.txt
import csv,sys
cap=int(sys.argv[1])
for r in csv.DictReader(open("progress/ledger.csv")):
    if r["status"]=="ready" and r["base_score"].isdigit():
        b=int(r["base_score"])
        if b==0: print(r["fn"],0,"SEED")
        elif b<=cap: print(r["fn"],b,"PERMUTE")
PY

echo "=== base==0 (seed matches) -> copy seed as winner ==="
grep " SEED$" /tmp/permlist.txt | while read fn b tag; do
  cp "progress/seeds/$fn.c" "progress/wins/$fn.c" && echo "  WIN(seed) $fn"
done

echo "=== permuting (cap base<=$CAP, ${TMO}s each, ${PAR}-way) ==="
permute_one() {
  fn="$1"
  d=$(ls -dt nonmatchings/$fn nonmatchings/$fn-* 2>/dev/null | head -1)
  [ -z "$d" ] && { echo "  NODIR $fn"; return; }
  rm -rf "$d"/output*
  timeout "$TMO" python3 tools/decomp-permuter/permuter.py "$d" -j2 --stop-on-zero --best-only >/dev/null 2>&1
  od=$(ls -d "$d"/output* 2>/dev/null | head -1)
  if [ -n "$od" ] && [ "$(cat "$od/score.txt" 2>/dev/null)" = "0" ]; then
    cp "$od/source.c" "progress/wins/$fn.c"; echo "  WIN $fn (score 0)"
  else
    s=$([ -n "$od" ] && cat "$od/score.txt" 2>/dev/null || echo "-")
    echo "  miss $fn (best=$s)"
  fi
}
export -f permute_one; export TMO
grep " PERMUTE$" /tmp/permlist.txt | awk '{print $1}' | xargs -P"$PAR" -I{} bash -c 'permute_one "$@"' _ {}

echo "=== winners collected ==="
ls progress/wins/ 2>/dev/null | sed 's/^/  /'
echo "total winners: $(ls progress/wins/ 2>/dev/null | wc -l)"
