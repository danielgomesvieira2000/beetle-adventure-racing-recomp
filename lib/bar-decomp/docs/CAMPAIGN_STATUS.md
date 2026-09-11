# BAR Decomp - Campaign Status & Resume Guide

_Last updated: 2026-06-29 (paused)_

## Current state
- **Functions ~50.6%** (2000/3954, report.json) | pragma-based ~62% | **1594** module `GLOBAL_ASM` pragmas remain.
- **Code bytes 22.45%** | **Data 100%**.
- HEAD pushed to `origin/main`; ROM matches `e5ab4d226c08d22f68a2edcc48870203e67454b8`; tree clean.
- This session: **+54 functions** over 9 commits (`20f409d`..`db1ade2`).

## How we got here (this session)
1. Quick-win + Tranche B **seed consolidation** (existing agent seeds): +27.
2. Tranche B wave 2 (agent seeds + proto-strip): +9.
3. Manual vec-add operand-order fix: +1.
4. Permuter campaign (600s budget + tdata/med import): +3.
5. Typed module-sweep **pilot + wave 3** (parallel-workflow agents, typed/named/commented): +14.

## The validated pipeline (breadth-first typed harvest)
Per wave: **card -> workflow-seed -> consolidate (byte-gate) -> permute NOMATCH -> scribe -> commit/push.**
- Card: `python3 tools/decomp_helpers/_p2cards.py <mods...>` -> cards to the Windows `p2cards/` dir.
- Stage headers for agents: `cp include/{structs,functions,global_exports,common,file_manager,mem_allocator,uv_filesystem,uv_memory}.h p2cards/headers/`.
- Seed (parallel agents, typed/named/commented, lean, OVERWRITE): the saved workflow `tools/decomp_helpers/decomp-typed-sweep.workflow.js` (clear `seeds_out/` first!). Agents Read cards+headers, Write `seeds_out/<fn>.c`.
- Consolidate (byte-gate): `python3 tools/decomp_helpers/_seedconsolidate.py <mod> [--list f1,f2,...]` (proto-strip any-args, typedef hoist, first-decl-wins externs; logs MATCH/BUILDERR/NOMATCH).
- Permute NOMATCH: import (`_hs_import.py`) then permute `base<400` (600s, parallel) -> winreview -> `_wins_integrate.py`.
- Commit: full `make -j6`, verify sha `e5ab4d22`, delete orphaned `.s`, mark ledger banked, `git add src/modules`, commit, push.

## KEY FINDINGS (why progress is now slow/expensive - read before resuming %-grind)
- **m2c is DEPLETED** (0 matches across 8 fresh modules).
- **Agent-seed match rate is stuck at ~7-10%** - the small-size filter did NOT help (wave 3 = 7.4%), and staging engine-prototype headers did NOT drop BUILDERR.
- **~50% BUILDERR** = agent call/type correctness, varies per function, resists systematic fixing.
- **~40% NOMATCH** = permuter pool; permuter cracks ~base<400 only (600s budget helps a little); the rest are regalloc/scheduling/struct-bound.
- **Cost: ~100K tokens per matched function** via the agent pipeline.
- **70% function is a multi-week arc**, not a few-days target. Realistic few-days ceiling ~55-57%.
- **Readability WORKS**: matched agent seeds are clean (named locals, purpose comments, annotated export calls). See e.g. `func_caranim_00401AD0` in caranim.c.

## Decision options on the table (pick one to resume)
1. **Scribe campaign (readability)** - RECOMMENDED for the readability goal. Make the ~2000 matched functions readable: define export structs (`gSndExports` -> named `SndExports` with fields; types currently live scattered in `src/modules/gamegui.c` etc. - centralize into `include/structs.h`), name `func_XXXX`/`D_XXXX` symbols, add comments, update `docs/SYMBOLS.md`. Efficient; no per-function matching tax. Regression-gate every header edit with a full build + sha.
2. **Keep grinding %** - agent waves (`decomp-typed-sweep.workflow.js`) + permuter, ~100K tok/matched-func, ~8%/wave.
3. **Background permuter only** - import the NOMATCH pool, permute continuously (compute not tokens) for slow incremental %.
4. **Pivot to Track 2 (recomp)** - the BeetleRecomp black screen; VS-attach debug -> likely a visible title screen.

## Tooling reference (all in tools/decomp_helpers/)
`_seedconsolidate.py` (byte-gated consolidator), `decomp-typed-sweep.workflow.js` (parallel typed-seed agents), `_p2cards.py` (asm+m2c cards), `_hs_import.py` (permuter import), `_m2cwave.py`/`_handwave.py` (m2c auto - now depleted), `_tryfunc.py` (single-func byte test), `_winreview.py`/`_wins_integrate.py` (permuter-win review/integrate), `_baseline.py` (regression gate). Small-func worklist generator + the 486-func small list are in the Windows `p2cards/` dir.
