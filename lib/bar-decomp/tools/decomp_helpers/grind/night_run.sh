#!/bin/bash
# Unattended compute-only decomp run: expand seeds with context-aware m2c across the whole
# incomplete module codebase, bank direct matches, then a laddered integration-gated permuter
# campaign over every seeded function, banking wins at the end. No LLM tokens involved.
# Everything it banks passed the module DaisyBox hash + full-ROM SHA gates; commits are local.
set -u
cd /home/brysl/projects/bar-decomp
source .venv/bin/activate
G=/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind
TS() { date '+%F %T'; }

echo "[$(TS)] ===== NIGHT RUN start ====="
if [ -n "$(git status --porcelain -- src asm progress)" ]; then
  echo "[$(TS)] work paths (src/asm/progress) not clean — aborting"; exit 1
fi

echo "[$(TS)] step 1/5: m2c seed expansion (context-aware, <=3000B, all modules)"
python3 tools/decomp_helpers/grind/m2c_seed_all.py 3000 ALL

echo "[$(TS)] step 2/5: bank direct m2c matches"
python3 tools/decomp_helpers/grind/bank_wins.py

POOL=$G/pool_night.json
N=$(python3 -c "import json;print(len(json.load(open('$POOL'))))")
if   [ "$N" -gt 250 ]; then B=240,720
elif [ "$N" -gt 120 ]; then B=300,900
else                        B=300,900,1800
fi
echo "[$(TS)] step 3/5: permuter ladder over $N seeded fns, budgets $B (conc 6 x -j4)"
python3 tools/decomp_helpers/grind/permute_campaign2.py "$B" 6 4 "$POOL"

echo "[$(TS)] step 4/5: bank permuter wins"
python3 tools/decomp_helpers/grind/bank_wins.py

echo "[$(TS)] step 5/5: safety-net sweep (report-only)"
python3 tools/decomp_helpers/grind/sweep_wins.py

echo "[$(TS)] ===== NIGHT RUN done ====="
git log --oneline -5
echo "[$(TS)] morning worklist: .grind/sweep_results.json (BANK-FAIL/transfer-fail),"
echo "         .grind/m2c_builderr/ (LLM fix-wave input), night_run.log round tables (near-misses)"
