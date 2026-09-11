# BAR decomp grind — resume state (updated 2026-07-02, strategy-review session)

**Read `docs/GRIND_PLAYBOOK.md` first** — it is now the canonical loop (phases MATCH → SCRIBE →
NAME, permuter-primary with integration-gated wins). Analysis behind it: `docs/DECOMP_STRATEGY.md`
in the BeetleRecomp repo. This file is just current state + next actions.

## Toolchain: UP + byte-verified (unchanged)
WSL Ubuntu-24.04, /home/brysl/projects/bar-decomp. `source .venv/bin/activate && make -j6`
-> sha e5ab4d22. Modules compile with **IDO 5.3** (Makefile:412), not 7.1.

## State after 2026-07-02
- **+92 HUD/menu-campaign functions banked** total (84 from the wave sessions + **8 recovered by
  the integration sweep** — score-0 permuter outputs that had never been integration-tested,
  incl. battle's FIRST match func_battle_0040733C and func_selection_00404888).
  Commits local: HEAD = `ec61443` (bank 8). Push + bump recomp submodule pointer when ready.
- `.grind/nomatch.json` refreshed 92 → 87 live NOMATCH seeds (pool spans HUD + med/race/replay/
  tdata/uv*_rom from earlier campaigns).
- `.grind/sweep_results.json` = latest gate verdicts (4 true transfer-fails, 2 BUILDERR from
  context drift in uvcont_rom).
- New tools deployed to `tools/decomp_helpers/grind/` (+ mirrored to recomp `.grind/`):
  - `permute_campaign2.py` — fresh imports each run, budget ladder (300→900→1800s, survivors
    halved), **gates every score-0 at win time**, re-hunts transfer-fails without --stop-on-zero.
  - `sweep_wins.py` — report-only: scan ALL nonmatchings/*/output* zeros vs live modules.
  - `bank_wins.py` — banks BANKABLEs from sweep_results.json: per-bank accumulation gate, full
    ROM SHA, orphan .s removal, ledger touch, nomatch refresh, local commit.
  - `gen_cards2.py` — context-aware m2c (`--context` = cpp of the module minus pragmas).
    Validated: seeds arrive with real types (MenuNode etc.) instead of `void*` guesses.

## Next actions (in order of expected yield)
1. **Overnight laddered permuter on the 87-fn pool** (compute, no tokens):
   `nohup python3 tools/decomp_helpers/grind/permute_campaign2.py 600,1800,3600 6 4 > /tmp/perm_night.log 2>&1 &`
   Morning: `python3 tools/decomp_helpers/grind/bank_wins.py`.
2. **Regenerate cards with gen_cards2 (context m2c) and rerun a Sonnet seed wave** on the
   never-tried tail (>500B) + on BUILDERR/NOMATCH functions whose seeds predate the context
   upgrade — better seeds shrink the permuter's job everywhere.
3. **Scribe wave on the near-done modules** (uvgfxmgr_rom, uvchannel_rom, uvsprt_rom + export
   tables — HUD_MENU_WORKFLOW Track A order): promotes verified types into headers, which
   further improves gen_cards2 context for the remaining tail.
4. PERM-macro annotation pass on the 4 transfer-fails + permuter-plateaued functions; only then
   per-function Opus with the permuter diff in hand.

## Gotchas (still true)
- git-bash → wsl.exe mangles $vars/quotes/pipes: write scripts to /mnt/c, run by literal path.
- Don't consolidate while a permuter campaign runs (24-core thrash). campaign2's inline gates
  are fine (single module .o builds, serialized by a lock).
- Commits are local in this clone; push + bump lib/bar-decomp pointer in BeetleRecomp when ready.
- Pool/ledger drift: trust the pragma scan, not old jsons; campaign2/bank_wins refresh for you.
