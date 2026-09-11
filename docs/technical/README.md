# Beetle Adventure Racing — Technical Documentation

Reverse-engineering and porting notes for **Beetle Adventure Racing!** (N64, USA, `NNSE`),
written for two audiences:

1. **People who want to decompile the game** — what the ROM contains, how Paradigm's "UV" engine
   is structured, how the ~133 relocatable modules work, and the matching workflow that has
   actually produced results on this codebase.
2. **People who want to build their own PC port** — of this game or of another N64 title with a
   similar shape. The static-recompilation pipeline is described end to end, including the parts
   that are not in anyone's tutorial: overlay registration, codegen fixups, hardware-register
   stubs, cooperative scheduling, and the renderer-side work needed to make a 320×240
   overscan-inset game look right on a modern display.

Everything here was measured against a real build. Where something is inferred rather than
observed, it says so.

## Table of contents

| Document | What it covers |
|---|---|
| [01 — The game and the ROM](01-game-and-rom.md) | Cartridge facts, memory map, entry point, save hardware, microcode, why BAR is a tractable recomp target |
| [02 — The UV engine and the module system](02-engine-and-modules.md) | Paradigm's UV engine, export tables, the 133 relocatable modules, `ModuleCommInfo`, the loader, screen state machine |
| [03 — Decompiling BAR](03-decompilation.md) | splat, IDO 5.3, the module build, the three-phase matching strategy, permuter economics, what the decomp gives the recomp |
| [04 — Static recompilation](04-static-recompilation.md) | N64Recomp configuration, the symbol-rich ELF, section/overlay tables, the overlay bridge, RSPRecomp for the audio ucode |
| [05 — The runtime host](05-runtime-host.md) | librecomp/ultramodern/RT64 wiring, threading and cooperative preemption, audio, SI/PIF input, Controller Pak, saves |
| [06 — Graphics](06-graphics.md) | The UV gfx manager, viewport/scissor, the frustum BAR culls against, widescreen, HUD anchoring, VI present |
| [07 — Codegen fixups and MIPS patches](07-codegen-fixups-and-patches.md) | Every `fix-recompiled.sh` rule and why it exists; the `RECOMP_PATCH` pipeline and the linker trap it hides |
| [08 — Diagnostics and methodology](08-diagnostics-and-methodology.md) | The env-var diagnostic surface, headless scripted runs, screenshot capture, how measurements are taken |
| [09 — Porting playbook](09-porting-playbook.md) | Generalised: the order to do this in, the traps, and what to measure before believing anything |

Working notes that are *not* part of this reference — current state, roadmap and live bug reports —
live one directory up: [`../STATUS.md`](../STATUS.md), [`../TODO.md`](../TODO.md),
[`../KNOWN_ISSUES.md`](../KNOWN_ISSUES.md), [`../PORT-STATUS.md`](../PORT-STATUS.md),
[`../PINNED_REVISIONS.md`](../PINNED_REVISIONS.md). Deep single-topic investigations live in
[`../RACING_DRAW_MAP.md`](../RACING_DRAW_MAP.md), [`../HUD_MENU_SYSTEM_MAP.md`](../HUD_MENU_SYSTEM_MAP.md),
[`../SEAM_FIX_PLAN.md`](../SEAM_FIX_PLAN.md), [`../SETTINGS_MENU_AND_HIGH_FPS.md`](../SETTINGS_MENU_AND_HIGH_FPS.md)
and [`../HUD-INSPECTOR.md`](../HUD-INSPECTOR.md), which is the guide to the F1 debug menu;
this reference links into them rather than duplicating them.

---

## Keeping this documentation current

**This is a maintenance contract, not a suggestion.** The value of these documents is that they
describe the build that exists, so a claim that has quietly become false is worse than no claim at
all. If you are an AI agent continuing development on this repository, treat the following as
binding.

**Update the reference in the same change that makes it wrong.** Not afterwards, not in a
follow-up commit. The mapping from work to document:

| If you change… | Update |
|---|---|
| `beetle-adventure-racing-recomp.toml`, `overlays.us.txt`, the ELF input, `scripts/fetch-elf.*`, `scripts/gen-overlays.sh` | [04](04-static-recompilation.md) |
| `scripts/fix-recompiled.sh` — **any** rule added, removed, or re-anchored | [07](07-codegen-fixups-and-patches.md) |
| `patches/`, `patches.toml`, `scripts/build-patches.sh`, `tools/gen_reference_syms.py` | [07](07-codegen-fixups-and-patches.md) |
| `src/main/overlay_bridge.cpp`, `register_overlays.cpp`, `hw_stubs.cpp`, `os_unimpl_stubs.cpp` | [04](04-static-recompilation.md) and [05](05-runtime-host.md) |
| `src/main/main.cpp` callbacks, audio, input, `bar_input.cpp`, `bar_preempt.cpp` | [05](05-runtime-host.md) |
| `src/frontend/`, `assets/ui/`, `scripts/make-launcher-logo.py`, `scripts/patch-recompinput.py` | [05](05-runtime-host.md) |
| `src/main/bar_frustum.cpp`, `rt64_render_context.cpp`, the RT64 fork's BAR-specific code | [06](06-graphics.md) |
| `src/main/bar_inspector.cpp`, `lib/rt64/src/hle/rt64_bar_hud.*`, the HUD classes or `hud.json`'s format | [06](06-graphics.md) and [`../HUD-INSPECTOR.md`](../HUD-INSPECTOR.md) |
| A new `BAR_*` environment variable, or one removed | [08](08-diagnostics-and-methodology.md) |
| A new fact about the game itself — an address, an offset, a struct field, a state value | [01](01-game-and-rom.md), [02](02-engine-and-modules.md) or [06](06-graphics.md), whichever owns it |

**Three rules that matter more than completeness.**

1. **Record the measurement, not the conclusion alone.** "The racing far plane is 300" is worth
   little; "measured with `BAR_DBG_FRUSTUM=1` in a race, two projections per frame, the racing
   camera at `dst=0x80099E1C` with `far=300`" can be re-checked and re-derived. Every number in
   these documents should be traceable to a switch someone can flip.
2. **Keep negative results.** They are the expensive part. A refuted hypothesis that gets deleted
   is a hypothesis someone will pay for again. `KNOWN_ISSUES.md` carries the long-form versions;
   this reference carries the short form with a pointer.
3. **Mark inference as inference.** Several claims here are derived from unmatched assembly or
   from the shape of the calls around it. Those are labelled. Do not launder an inference into a
   fact by rewriting the sentence.

**When a document goes stale rather than wrong**, say so in place — a dated note beats a deletion,
because the reason something was believed is usually still useful.
