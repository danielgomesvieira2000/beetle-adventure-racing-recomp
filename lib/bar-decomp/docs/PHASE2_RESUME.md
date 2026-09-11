# Phase 2 Resume Guide

How to resume the decompilation grind without losing progress. Engine + tooling
live in `tools/decomp_helpers/`; matching idioms are in the project memory and
`docs/AI_WORKFLOW.md`.

## State at pause (2026-06-28)

- **~50% functions matched** (~1980 / 3954); ~22%+ code bytes; 100% data.
- **1648 module `GLOBAL_ASM` function pragmas remain** (`grep -rh GLOBAL_ASM src/modules/*.c | grep -c func_`).
- Build gate: per-module SHA256 → whole-ROM SHA1 **`e5ab4d226c08d22f68a2edcc48870203e67454b8`**.
- Phases 0/1/1.5 complete; Phase 1.5 infra (engine protos, core structs) was
  found to already exist in the headers. Phase 2 (per-module decomp) is underway:
  **+14 functions** over 3 waves (commits `7be181b`, `6505a39`, `2239b9e`).

### On-disk resume accelerators (NOT in git; persist locally)
- `progress/ledger.csv` — per-function status/band/base_score (snapshot also at
  `tools/decomp_helpers/ledger_snapshot.csv`). Status values: `asm` (untouched),
  `banked` (matched+committed), `hs-ready`/`hs-import-ok` (agent seed written &
  imported to permuter, base score known — **PRIMED, just needs a permute run**),
  `no-seed` (m2c/agent couldn't seed → manual/struct), `deferred-manual`.
- `progress/seeds/<fn>.c` — agent-written compilable seeds (~250).
- `nonmatchings/<fn>/` — imported decomp-permuter dirs (skip re-import on resume).
- `progress/wins/<fn>.c` — permuter score-0 winners awaiting/after integration.

## The per-wave loop (each wave ≈ 1.5–2 h, yields ~4–7 matched)

All commands run in WSL: `cd ~/projects/bar-decomp && source .venv/bin/activate`.
Helpers are invoked from `tools/decomp_helpers/` (or the Windows mirror at
`/mnt/c/Users/Bryan/Projects/BeetleAdventureDecomp/`).

1. **Pick modules** (see tranche order below). Dump asm+m2c cards:
   `python3 tools/decomp_helpers/_p2cards.py <mod1> <mod2> ...`
   → writes cards + `_worklist.txt` to the Windows `p2cards/` dir.
2. **Agent decomp** (Workflow tool): re-invoke the saved `p2-decomp-trancheA`
   workflow script with `args` = the worklist's function names. Agents read the
   cards and write raw-cast C seeds (calls/loops/typed-global pointers).
   - If one agent stalls and blocks the barrier, recover the rest:
     `python3 tools/decomp_helpers/_extract_wf.py <workflow_subagents_dir> /tmp/wave.json`
3. **Process seeds**: `python3 tools/decomp_helpers/_process_handseed.py <wave.json>`
   (writes `progress/seeds/*.c` + `/tmp/hsspec_<mod>.json`; filters externs to data globals).
4. **Direct matches** (usually ~0 in Phase 2): for each module,
   `python3 tools/decomp_helpers/_handwave.py /tmp/hsspec_<mod>.json` → MATCHED lines.
5. **Import + permute the rest**:
   `python3 tools/decomp_helpers/_hs_import.py <p2cards>/_worklist.txt /tmp/<matched>.txt`
   then permute the `hs-ready` funcs with `base<400` (efficient subset), e.g. 200s,
   4-way parallel (see `_phase1_permute.sh` for the xargs pattern). Winners → `progress/wins/`.
6. **Review + integrate**: `_winreview.py seed win` (reject padding-var/no-effect/
   new-cast nonsense), then `_wins_integrate.py <fn...>` (build-verifies, bisects).
7. **Commit**: full `make -j6`, confirm SHA1 `e5ab4d22`, delete matched `.s`,
   `git add -A src/modules`, commit (trailer: `Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>`),
   update ledger statuses. Push when ready.

**Fast resume shortcut:** ~251 functions are already `hs-ready`/`hs-import-ok`
(seeded + imported). Skip steps 1–3 for them and go straight to step 5 (permute
`base<400` from the ledger) → 6 → 7.

## Hard matching constraints (apply every function)
- IDO -O2 scheduling is **source-line-sensitive** (e.g. `i=0; do {` on one line);
  insert permuter winners VERBATIM, never reformat matched bodies.
- Float loads are reordered independently of `add.s` operand order (vec add/dot/FMA).
- Params are register-width; narrow params emit a home-spill (sometimes correct).
- Typed globals (`gGameSettings`, `gXExports`) are POINTERS — use directly, not `&`.
- Pure ASCII only (asm-processor is euc-jp).
- Only score-0, isolation-verified output is committed; winreview gates nonsense.

## Recommended module order (reuse-driven; small→large)
- **Tranche A (done/started):** uvmodel_rom, scene, uvlight_rom, uvintersect_rom,
  uvcont_rom, menuslct.
- **Tranche B (started):** pause, cam, plyr, replay, results, race, tdata, med.
- **Tranche C:** caranim, caraudio, uvdobj_rom, uvpfx_rom, rain, uvgui_rom, snd.
- **Tranche D (last, hardest):** ted, misc, ai, uvterra_rom, uvisect_rom, battle,
  motion, uvdyn_rom, **selection (191 fns — own mini-campaign)**.

## Regression safety
After ANY header/struct edit, run `tools/decomp_helpers/_baseline.py` (full build
+ SHA1) and reject if it regresses. Per-module struct NAMING is deferred (raw
casts match fine); name fields only as a later non-gating cleanup.
