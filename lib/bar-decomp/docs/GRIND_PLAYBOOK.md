# BAR decomp grind — playbook v2 (2026-07-02)

The operational strategy for matching-decompiling BAR, revised after the 2026-07-01/02 campaign
review. Analysis + rationale live in the recomp repo at `docs/DECOMP_STRATEGY.md` (BeetleRecomp);
this file is the *how to run it*. Supersedes the per-function cadence in `AI_WORKFLOW.md` for
campaign work (that doc's byte-gate + model-tier principles still apply).

## The three phases (strict order per function, interleaved per module)

1. **MATCH** — get to byte-exact C with the least effort per function. Cheap-model seed waves +
   permuter grind. Write *placeholder-styled* code: `unk<hexoffset>` members, `Node_<ADDR>` struct
   names, one-line purpose comment, no naming effort beyond that. The only gate that matters:
   DaisyBox module hash + full-ROM SHA.
2. **SCRIBE** — per module, once its match% is high (or its tail stalls): promote verified
   struct/signature knowledge into `include/*.h` shared headers, dedupe the per-function shim
   typedefs, name locals, comment behavior. **This phase feeds phase 1**: richer headers + module
   context make m2c seeds and agent seeds better for the module's remaining tail (see
   `gen_cards2.py --context`). Codegen-neutral edits (renames/comments) batch under one module-hash
   check; struct/type edits gate individually.
3. **NAME** — semantic `func_*`/`D_*` renames, last, per-module batches, when behavior is
   understood from phase 2. Renames never change codegen but touch every reference (C, headers,
   remaining `.s`, ledger) — do them with a helper script + full-ROM gate per batch, and log
   addr→name in `docs/SYMBOLS.md`.

Do NOT interleave naming/typing polish into phase-1 matching. The 2026-07 waves proved
match-first is ~3x cheaper per function; polish is a separate, gated, batchable pass.

## The per-wave loop (phase 1)

All scripts in `tools/decomp_helpers/grind/` (mirrored to the recomp `.grind/` staging dir).
Run from the repo root with `.venv` activated.

```
0. refresh   pool + cards regenerate from LIVE pragmas (never trust old jsons/ledger status)
1. cards     python3 …/grind/gen_cards2.py <max_size> <mods|ALL> [min_size]
             (context-aware m2c: preprocesses the module minus pragmas and passes --context;
              seeds arrive with the module's real types — validated to be near-final C)
2. seed      seed_wave*_run.js workflow (Sonnet writers, effort medium; GUIDE conventions:
             unk<hexoffset> export shims, Node_<ADDR> struct names, ASCII only)
3. gate      python3 …/grind/consolidate2.py [mods]   (byte-gate + bank into module .c)
4. fix       capture_builderr.py → fix_wave (Sonnet w/ exact error) → re-consolidate;
             escalate a stubborn BUILDERR to fix_wave_opus ONCE, then park
5. permute   python3 …/grind/permute_campaign2.py [budgets] [conc] [jobs] [pool.json]
             (see below — this is the primary matcher now, not a fallback)
6. bank      python3 …/grind/bank_wins.py   (accumulation-gated, full ROM SHA, ledger touch,
             nomatch.json refresh, local commit)
7. sweep     python3 …/grind/sweep_wins.py  (safety net: report-only scan of ALL
             nonmatchings/*/output* score-0s vs the live modules; run after any permuter work)
```

Commit per wave; push + bump the recomp submodule pointer when a session ends.

## Permuter rules (the heavy-lift engine)

- **A permuter “win” does not exist until it survives the module hash.** `permute_campaign2.py`
  gates every score-0 the moment it appears (integration_gate). If the zero doesn't bank
  (transfer-fail), the function is re-hunted WITHOUT --stop-on-zero so the permuter keeps
  producing *distinct* zeros; each new zero is gated. Historical data: ~4/14 zeros were
  transfer-fails; 8 recoverable wins sat unbanked on disk until the 2026-07-02 sweep.
- **Always re-import before permuting.** Imports freeze the module context into
  `nonmatchings/<fn>/base.c`; after any banking wave they are stale (27/41 sampled dirs were for
  already-banked functions). campaign2 refreshes pool + imports every run.
- **Ladder the budget.** Everyone gets a cheap round (300s); survivors are ranked by best score
  and halved each round (900s, 1800s+). Long uniform budgets waste cores on base-score>1500
  functions that need a structural rewrite (a new seed), not permutation.
- **Overnight runs are free matches.** The permuter costs CPU, not tokens. For an unattended run:
  `cd ~/projects/bar-decomp && source .venv/bin/activate && \
   nohup python3 tools/decomp_helpers/grind/permute_campaign2.py 600,1800,3600 6 4 \
   > /tmp/perm_night.log 2>&1 &`
  then `bank_wins.py` in the morning. Don't consolidate while it runs (24-core thrash).
- Escalate to per-function Opus ONLY functions the permuter proves close-but-stuck (best score
  plateaued under ~150 across two long rounds). Give the model the permuter's best output diff,
  not just the seed. Opus-on-far-functions was measured to produce compiles that NOMATCH.
- Stuck near-misses: try PERM macros (`PERM_GENERAL(a;|b;)`, `PERM_VAR`, …) in the seed to direct
  the search before burning Opus tokens; and/or import 2-3 *diverse* seeds for the same function
  (permuter dirs `<fn>`, `<fn>-2`, …).

## Facts that keep getting mis-stated (pin them)

- Modules compile with **IDO 5.3** (`Makefile:412` overrides `CC` to asm-processor + `$(IDO)` =
  `tools/ido-static-recomp/build/5.3/out/cc`). The 7.1 assignment at Makefile:82 is dead for
  modules. Prompts/settings updated 2026-07-02; don't reintroduce “IDO 7.1”.
- ROM gate: sha1 `e5ab4d226c08d22f68a2edcc48870203e67454b8`. Per-module gate: DaisyBox
  “Hash verification passed” on `make build/bin/us/<mod>.o`.
- The pragma in `src/modules/<mod>.c` is the single source of truth for matched-vs-not.
  `progress/ledger.csv` statuses and `.grind/*.json` pools DRIFT — refresh from pragmas
  (campaign2/bank_wins do this automatically for their pools).
- git-bash → wsl.exe mangles `$vars`/quotes/pipes. Write scripts to /mnt/c and run them by
  literal path.

## Phase 2 (scribe) mechanics

- Unit of work = one module. Inputs: the module's matched C + `include/structs.h`,
  `global_exports.h`, `functions.h`, `docs/SYMBOLS.md` (KNOWLEDGE_BASE.md rules apply).
- Order: shared export-struct shims → module-local structs → locals/comments. Every edit batch
  ends with `make build/bin/us/<mod>.o` (hash gate); a type/header edit that touches other
  modules ends with full `make` + SHA.
- Scribe targets in priority order: `uvgfxmgr_rom`, `uvchannel_rom`, `uvsprt_rom`, the central
  export tables, then per-screen modules (see HUD_MENU_WORKFLOW.md Track A table).

## Phase 3 (name) mechanics

- Per-module batches of renames once scribe understanding exists. A rename = repo-wide textual
  replace (C + headers + `asm/**/*.s` references + ledger + SYMBOLS.md) — script it, don't
  hand-edit; full-ROM SHA after each batch.
- Names follow the uv-engine conventions in SYMBOLS.md; record addr→name there.

## The night run (fully token-free staging)

`tools/decomp_helpers/grind/night_run.sh` chains the compute-only levers end-to-end so results
are staged for a later LLM session: (1) `m2c_seed_all.py` — context-aware m2c over every
unseeded live-pragma function (proto-strip + void*-member struct synthesis; MATCH banks,
NOMATCH becomes a permuter seed, BUILDERR is staged with its exact error in
`.grind/m2c_builderr/`), (2) `bank_wins.py`, (3) `permute_campaign2.py` over
`.grind/pool_night.json` (every seeded function) with auto budgets, (4) `bank_wins.py`,
(5) `sweep_wins.py`. Launch from WINDOWS with `.grind/start_night.ps1` (hidden detached wsl.exe client — a
plain setsid/nohup does NOT survive the launching wsl.exe exiting; the distro tears down
and kills it). Niced, refuses to double-start; log at `.grind/night_run.log`; stop with
`wsl.exe -d Ubuntu-24.04 pkill -f 'night_run.sh|permuter.py|m2c_seed_all.py'`; the PC must
stay awake (no sleep) for the duration. Morning worklist for the LLM: BANK-FAIL/
transfer-fail entries in `sweep_results.json` (near-misses with candidates on disk),
`.grind/m2c_builderr/` (fix-wave input), and the round tables in the log (best-score ranking).
Everything banked overnight passed the same module-hash + full-ROM SHA gates as interactive work.
