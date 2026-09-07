# BAR Decomp Strategy Review — 2026-07-02

A review of the matching-decomp campaign strategy (requested: "lean into the permuters for the
heavy lifting, clean up the code afterwards, name functions last"), with the evidence, the leaks
found, the fixes **already applied**, and the revised operating strategy. The operational runbook
that implements this lives in the decomp repo: `docs/GRIND_PLAYBOOK.md` (bar-decomp), with current
state in `RESUME_GRIND.md` (WSL clone).

## Verdict on the proposed strategy

**Adopted, with two amendments.** The three-phase order — (1) match cheaply at volume, (2) clean
up matched code, (3) name functions last — is correct and is now the documented canon. It's also
what the successful waves were already converging on: the grind pipeline's placeholder conventions
(`unk<hexoffset>` shims, `Node_<ADDR>` structs) hit ~25% direct match on ≤200B functions vs the
~7–10% of the earlier polish-while-matching workflow, at far fewer tokens per function.

The amendments:

1. **The permuter can't do the *initial* heavy lifting alone** — it mutates a compiling,
   structurally-close seed; it cannot invent control flow. "Lean into permuters" therefore means:
   cheap-model seed waves exist *to feed the permuter*, the permuter (CPU, not tokens) is the
   primary matcher, and expensive models are reserved for (a) fixing compile errors and (b) the
   few functions the permuter *proves* close-but-stuck. That's exactly what the yield data
   supports — Opus hand-matching on far functions produced compiles that didn't match.
2. **Phase 2 is not purely cosmetic, so it interleaves per-module.** Correct struct/prototype
   context *changes codegen* and therefore raises match rates on a module's remaining tail
   (wrong prototypes → wrong call-site codegen, e.g. float promotion). Scribing a
   mostly-matched module's types into shared headers feeds directly back into phase 1 for its
   tail — especially now that m2c consumes the module context automatically (see gen_cards2
   below). Renames and comments stay codegen-neutral and can genuinely wait.

## What the review found (and fixed)

### 1. Permuter wins were never integration-tested — the biggest leak *(fixed + recovered)*

A permuter "score 0" is judged in its own flattened translation unit; the module build can still
disagree (different visible declarations → different codegen; noted historically as "~50% of
wins integrate"). Nothing ever *swept* past outputs, and campaign wins were only collected when a
campaign script ran to completion.

A report-only sweep of all 392 permuter import dirs (`sweep_wins.py`, now in the repo) found **41
score-0 outputs**: 27 for functions already banked (stale dirs), 2 BUILDERR from context drift,
4 genuine transfer-fails — and **8 fully bankable functions sitting on disk**, some for days.
All 8 were banked with per-module accumulation gates + full-ROM SHA (`e5ab4d22…` OK), committed
in the WSL clone as `ec61443`:

> func_battle_0040733C (battle's FIRST match, was 0/70), func_gamegui_00401D34,
> func_med_004007B4, func_med_0040505C, func_pause_0040048C, func_pause_004026C4,
> func_selection_00404888, func_uvdyn_rom_00409A48

**Rule going forward: a win doesn't exist until it survives the module hash.**
`permute_campaign2.py` gates every score-0 the moment it appears; on a transfer-fail it keeps
hunting that function *without* `--stop-on-zero`, gating each distinct zero. `sweep_wins.py` is
the recurring safety net; `bank_wins.py` banks whatever is verified.

### 2. Stale pools, stale imports, drifting ledger *(fixed where it bites)*

- `nomatch.json` listed 92 functions; 5 no longer had a pragma (already banked — e.g.
  `func_letter_00400140`, still described as "the near-miss target" in session notes, was
  banked long ago). Refreshed to 87.
- 27 of 41 swept win-dirs belonged to already-banked functions — permuting those again would
  have been pure waste, and imports frozen before a banking wave carry outdated context.
- `progress/ledger.csv` statuses disagree with reality (88 "banked" vs 92+ actual).

**Rule: the `#pragma GLOBAL_ASM` scan is the only source of truth.** campaign2 and bank_wins
refresh their pools from live pragmas every run; imports are re-done fresh each campaign.

### 3. m2c ran context-free — seeds guessed every type *(fixed, big quality jump)*

`gen_cards.py` called m2c with no `--context`, so seeds arrived as `extern ? gGameSettings;`
`void*` soup, and every agent re-derived (or mis-guessed) types the module already declares.
`gen_cards2.py` preprocesses the module (pragmas stripped) with the real build flags and passes
it via `--context`. Validated on `pause`: the seed for `func_pause_0040264C` now types
`D_pause_00405E50` as `MenuNode*` and emits near-final C (`D_pause_00405E50->unk8->unk4 = 1;`).
This compounds with every banked wave and every scribe pass — the context is *generated from*
the module source, so phase-2 work automatically improves phase-1 seeds.

### 4. Uniform permuter budgets ignored what the base score predicts *(fixed)*

v1 gave every function the same timeout in pool order, though the data says the permuter cracks
mostly `base<400` and long budgets only pay on near-misses. campaign2 runs a **ladder**: every
function gets a cheap round (300s), survivors are ranked by best score and halved each round
(900s → 1800s+), so overnight compute concentrates where zeros are plausible. Overnight runs are
the cheap lever: tokens cost money, the 24-core box is free while you sleep.

### 5. Two documented "facts" were wrong *(fixed)*

- **Modules compile with IDO 5.3, not 7.1.** `Makefile:412` overrides `CC` to
  asm-processor + `$(IDO)` = `build/5.3/out/cc`; the 7.1 assignment at line 82 is dead for
  modules. The seed/fix prompts and `permuter_settings.toml` said 7.1 — all corrected (matters
  for model reasoning about compiler idioms; the byte gate caught nothing wrong, but prompts
  shouldn't lie).
- **`AI_WORKFLOW.md` prescribed name-and-type polish at match time.** The waves that beat the
  baseline 3× skipped that. The doc now carries a 2026-07-02 revision note deferring polish to
  the scribe phase (same audit checks, batched per module).

## The revised strategy (canon)

**Phase 1 — MATCH** (per wave, all scripts in `tools/decomp_helpers/grind/`):

```
refresh pools from live pragmas
→ gen_cards2 (context-aware m2c cards)
→ Sonnet seed wave (placeholder conventions, effort medium)
→ consolidate2 (per-seed byte gate, accumulating)
→ capture_builderr → fix_wave (Sonnet w/ exact error; one Opus escalation, then park)
→ permute_campaign2 (fresh imports, budget ladder, gate-on-zero, transfer-fail rehunt)
→ bank_wins (accumulation gate + ROM SHA + ledger/pool refresh + commit)
→ sweep_wins (safety net after any permuter activity)
```

**Phase 2 — SCRIBE** (per module, once match% is high or its tail stalls): promote verified
structs/signatures into `include/*.h` (KNOWLEDGE_BASE rules), dedupe shim typedefs, name locals,
comment behavior. Codegen-neutral edits batch under one module-hash check; type edits gate
individually. Priority: `uvgfxmgr_rom` → `uvchannel_rom` → `uvsprt_rom` → central export tables →
per-screen modules (HUD_MENU_WORKFLOW Track A order).

**Phase 3 — NAME** (per module, last): semantic renames of `func_*`/`D_*` via a helper script
(repo-wide replace across C, headers, remaining `.s`, ledger, SYMBOLS.md), full-ROM SHA per
batch. Names come from phase-2 understanding; renames never change codegen but touch everything,
which is exactly why they go last, when the fewest `.s` references remain.

**Escalation policy:** Opus only for functions the permuter proves close-but-stuck (best score
plateaued small across two long rounds), and always with the permuter's best-output diff in the
prompt. Before Opus: try a PERM-macro annotation pass (`PERM_GENERAL`, `PERM_VAR`, …) to direct
the permuter's search — annotation is cheap-model work.

## Current state & immediate next steps

- HUD/menu campaign: **+92 banked** total (84 wave + 8 sweep-recovered); ROM byte-exact.
  WSL clone HEAD `8e7eda9` (3 new local commits: bank, pipeline v2, fixup) — push + submodule
  bump when ready.
- NOMATCH pool: 87 live seeds (spans HUD + med/race/replay/tdata/uv*_rom).
- Next, by expected yield:
  1. Overnight `permute_campaign2.py 600,1800,3600 6 4` on the pool → `bank_wins.py` in the
     morning (compute-only matches).
  2. Regenerate cards with gen_cards2 and rerun a seed wave on the untried >500B tail and on
     functions whose seeds predate the context upgrade.
  3. First scribe wave (framework modules above) — pays into every later wave's context.
  4. PERM-macro pass on the 4 transfer-fails + plateaued near-misses; then targeted Opus.

## Doc changes made (this session)

| Where | What |
|---|---|
| `bar-decomp/docs/GRIND_PLAYBOOK.md` *(new)* | Canonical 3-phase strategy + wave loop + permuter rules + pinned facts |
| `bar-decomp/docs/AI_WORKFLOW.md` | 2026-07-02 revision note: phase separation, permuter-primary, IDO 5.3 correction |
| `bar-decomp/RESUME_GRIND.md` | Rewritten: current state + next actions, points at the playbook |
| `bar-decomp/tools/decomp_helpers/grind/` | +`permute_campaign2.py`, `gen_cards2.py`, `sweep_wins.py`, `bank_wins.py` (mirrored to `.grind/`) |
| `bar-decomp` prompts/settings | "IDO 7.1" → "IDO 5.3" everywhere |
| `beetle-adventure-racing-recomp/docs/DECOMP_STRATEGY.md` | This report |

Still recommended (not yet built): a `rename_symbol.py` helper for phase 3; multi-seed permuter
imports (`<fn>-2`, `<fn>-3` dirs from diverse attempts); a ledger-regeneration script so
`progress/ledger.csv` is derived from pragmas instead of hand-maintained; optionally a Windows
scheduled task for the nightly permuter run (left manual so it's opt-in).
