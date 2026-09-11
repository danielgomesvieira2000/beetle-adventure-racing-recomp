# Symbols & conventions (knowledge index)

Narrative companion to the typed headers (`include/structs.h`, `functions.h`, `global_exports.h`,
per-module headers). Grow this as you learn -- it's the human-readable half of the knowledge base
(see `KNOWLEDGE_BASE.md`).

## Naming conventions
- `func_<module>_<hexaddr>` -- a function not yet decompiled (symbol == ROM address). Rename once its
  behavior is clear.
- `D_<...>` / `D_<module>_<hexaddr>` -- a data symbol (global/rodata) not yet typed/named.
- `g<Name>Exports` -- a pointer to another module's export struct (a vtable of function pointers +
  data). Typed ones live in `global_exports.h`; others need a local typedef (see KNOWLEDGE_BASE.md).
- `uv*` / `_uv*` -- the Paradigm "uv" engine API (memory, scheduler, gfx, math, ...).
- 133 relocatable module overlays: `uvXxx_rom` (engine) + gameplay (`selection`, `race`, `pause`, ...).

## Matching rules (summary; full list in AI_WORKFLOW.md)
- Register-width params (s32/u32/void*) with narrow casts at use; a narrow param makes IDO spill.
- Load/store width <-> cast: lw=s32, lh=s16, lhu=u16, lb=s8, lbu/sb=u8.
- Source must be PURE ASCII (asm-processor uses euc-jp encoding).

## Notable globals (grow this)
- `gNumPlayers`, `gNumAiCars`, `gTransmissionType`, `gOptions*`, `gCheat*` -- selection/options state.
- `gCurrentTrack`, `gCurrentGameState`, `gGameStateFlag` -- game state.

## Module roles (grow this)
- `selection` -- menus / options / cheats / car select.
- `uvquery_rom` -- getters/setters over query globals.
- `uvgui_rom`, `gamegui` -- GUI widgets.
- `uvdyn_rom` -- dynamics objects (a vtable'd per-instance struct).
- (add modules as their purpose is confirmed)
