# 02 — The UV engine and the module system

BAR is built on Paradigm Entertainment's in-house **"UV" engine**. Almost everything the game does
goes through it, and almost everything it does lives in a **relocatable module**. Understanding the
module system is the single highest-leverage thing to do before either decompiling the game or
porting it — it is what makes the codebase legible, and it was the crux of getting the port to boot.

## The shape of the binary

| Layer | Where it lives | Character |
|---|---|---|
| Boot / entry / libultra | Base game at `0x80000400`+ | Fixed, always resident. Includes the scheduler (`sched.c`), memory allocator, and the game's own libultra 2.0I |
| UV engine modules | `uv*_rom` relocatable modules | The reusable engine: math, matrices, geometry, textures, fonts, sprites, sound, the gfx manager |
| Game modules | Named relocatable modules (`race`, `selection`, `pause`, `intro`, …) | One per screen or subsystem |

There are **133** relocatable sections, listed one per line in `overlays.us.txt` in the order
N64Recomp assigns overlay ids (`.ai` = id 0 … `.weapon` = id 132). Of those, roughly 60 are
`uv*_rom` engine modules and the rest are game content.

Regenerate the list from a rebuilt ELF with `scripts/gen-overlays.sh`, which runs `readelf -SW`,
strips base and system sections, and writes a **candidate** list that needs review. Do not
regenerate it casually: overlay ids are positional, and the `nameTag → id` table in
`src/main/overlay_bridge.cpp` is indexed by that ordering.

## Export tables: how modules call each other

A module never links directly against another module's symbols. Instead each engine module publishes
a table of function pointers, reached through a global:

```
gUvFontExports      (UvFont_Exports)          uvSetFont, uvFontScale, uvFontColor, uvFontPrintStr, …
gUvSprtExports      (UvSprt_Rom_Exports)      sprite pool, uvSprtProps, uvSpriteDraw*
gUvGfxMgrExports    (UvGfxMgr_Exports)        viewport / scissor / screen size
gUvGfxStateExports  (UvGfxState_Rom_Exports)
gUvColorExports     (UvColor_Exports)
gUvStringExports    (UvString_Exports)        uvStrlen, uvStrChr, uvSprintf
gUvBlitExports      (UvBlit_Rom_Exports)      uvParseBlit
gSndExports / gUvCmidiExports / gUvAudiomgrExports
```

Consumers reach them as `gUvSprtExports->unk1C(...)`. In the decomp, most consumer modules declare
these tables as **local opaque structs** (`{char pad; fnptr unkNN}`) rather than sharing a central
typed header, so the same slot has a different name in every consumer. Centralising those types is
one of the highest-value scribe-phase tasks — see [03](03-decompilation.md).

The practical consequence for a porter: **you cannot find a call site by grepping for a function
name.** Calls go through an offset in an export table. Finding "who draws the HUD" means tracing
the table slot, not the symbol.

## The module loader

The loading path, from `lib/bar-decomp/src/module.c`:

```
uvLoadModuleCode(...)          allocate a heap buffer, DMA the module's code into it
  → uvDoModuleRelocs(ovlStartPtr, ModuleCommInfo*)   apply MIPS relocations in place
  → entryPointFunction(...)                          jump into the loaded module
```

`ModuleCommInfo` is the per-module descriptor. The field the port depends on:

| Offset | Field | Notes |
|---|---|---|
| `+0x1C` | `nameTag` | Four ASCII bytes packed big-endian into a word — `'aimd'`, `'race'`, `'AMGR'`, … |

The nameTag is the only identifier available at load time that survives into the host. The
authoritative `module name → tag` mapping is the decomp's `tools/convPartialModule.py`
`partial_to_module` dict; the table in `src/main/overlay_bridge.cpp` was produced by joining that
dict with `overlays.us.txt`.

Tags are **not** a mechanical truncation of the module name, and this is where a table like this
goes wrong. `.ai` is `aimd`, `.filmroll` is `frol`, `.menuslct` is `menu`, `.uvblit_rom` is `UVBT`,
`.uvintersect_rom` is `ISCT` while `.uvisect_rom` is `isct` (case is significant), and the `.mult1`
… `.mult9` modules are `mp01` … `mp09`. Engine modules are mostly uppercase, game modules mostly
lowercase, but not reliably: `.uvdyn_rom` is `udyn` and `.uvgrph_rom` is `grph`.

### Why the loader matters to a static recomp

In a static recompilation, a module's functions live in the generated `func_map` keyed by their
**recomp-ELF VRAM** (`0x80800000 + id × 0x100000`), not by the heap address the game just loaded
them to. So the entry-point call after relocation fails with
`Failed to find function at 0x...`, because nothing has told the runtime where this module now
lives.

The fix, and the design of the overlay bridge, is in
[04 — The overlay bridge](04-static-recompilation.md#the-overlay-bridge). It hooks
`uvDoModuleRelocs`, reads the nameTag, and calls librecomp's `load_overlay_by_id(id, ovlStartPtr)`
before running the original relocation.

One measured detail worth recording, because it was established by experiment rather than derived:
**the section base is `ovlStartPtr` itself**, i.e. the code start. Registering at
`ovlStartPtr - exportsSize` breaks function resolution.

## Screen flow

Every screen is its own overlay, reached through an engine export-table pointer, and dispatched by
the state machine on `gGameStateFlag` / `gGameSettings[0].gameStateFlag`. Modules load and unload
continuously — during a race the overlay traffic is heavy enough that per-load logging is a
measurable performance drag, which is why `BAR_DEBUG_OVERLAYS` gates it off by default.

The screen and HUD modules, with match state as of the mapping pass, are catalogued in
[`../HUD_MENU_SYSTEM_MAP.md`](../HUD_MENU_SYSTEM_MAP.md) — 1,500 lines covering every module's role,
its export usage, its key data symbols and its per-function match status. The summary:

| Module | Category | Role |
|---|---|---|
| `__framework__` (`uvgfxmgr_rom`, `uvsprt_rom`, `uvfont_rom`, `menuslct`) | Framework | Every UI primitive: viewport/scissor, sprite pool, text queue, the menu widget model |
| `logo`, `splash`, `cbars` | Boot | Boot logos; `cbars` is a colour-bars **test pattern**, not letterboxing |
| `intro`, `demo` | Attract | The attract cinematic and the demo race replay |
| `selection`, `menuslct`, `pause` | Menu | Car/track selection (the largest UI module, 292 functions), the reusable list widget, the pause menu |
| `gamegui`, `plyr`, `flag` | HUD | The in-race HUD, per-player entities |
| `race`, `scene`, `scrn`, `cam` | Race | Simulation, scene orchestration, render-target management, camera |
| `results`, `victory` | Post-race | Standings |
| `battle` | Multiplayer | The 4-player Beetle Battle front end and HUD |
| `filmroll` | Transition | The main-menu film-roll page transition |
| `filerom`, `fileux` | I/O | ROM/media loading and buffered file I/O — despite the name, `filerom` is **cartridge media I/O**, not save handling |

### Naming decoys

Three module names mislead, and all three have cost time:

* **`letter`** is the collectible B-E-E-T-L-E letters, not letterboxing.
* **`cbars`** is a colour-bars test pattern — the opposite of black bars.
* **`filerom`** is ROM/cartridge media DMA, not Controller Pak saves.

## The two coordinate systems

The UI is authored in a **320×240, top-left-origin** space defined by `sScreenWidth` /
`sScreenHeight` in `uvgfxmgr_rom`. The N64 framebuffer is bottom-left-origin, and the engine flips
between them — `uvSprtProps` case 2 computes `sprite->y = ScreenHeight - y - height`, and
`uvFontPrintStr` does the same for text.

**This flip is a live trap for anyone patching the engine.** `patches/viewport_patch.c` cost a full
build cycle to it: the scissor is emitted as `(x0, height - y1, x1, height - y0)`, so the measured
scissor rows `17..224` correspond to `y0 = 16, y1 = 223` in the viewport API, not `17`/`224`. The
first version of that patch used the scissor rows directly and its vertical snap silently never
matched — it compiled, it linked, it changed nothing.

There are also **two independent scissor mechanisms**, which is the second trap:

1. **The RDP scissor and viewport**, emitted by `func_uvgfxmgr_rom_00401C5C` as
   `gSPViewport` + `gDPSetScissor(G_SC_NON_INTERLACE, vp->x0, sScreenHeight - vp->y1, vp->x1, sScreenHeight - vp->y0)`.
   The rectangle comes from `uvGfxClipRect` / `func_uvgfxmgr_rom_00401BD4` into the viewport stack
   `D_uvgfxmgr_rom_004022C8[]` (up to 11 entries).
2. **A software scissor for the sprite layer**, `sScissorXmin/Xmax/Ymin/Ymax` in `uvsprt_rom`, reset
   to `0,0,320,240` by `uvSpriteDrawInit` every frame and enforced per-sprite in `uvDrawBitmap`.

Changing one does not change the other. Bars around the 3D view come from (1); bars around the
sprite/HUD layer come from (2).

## Key data symbols worth knowing

Collected from the framework mapping; all are in relocatable modules unless noted.

| Symbol | What it is |
|---|---|
| `sScreenWidth` / `sScreenHeight` | u16, default 320×240. Defines the UI coordinate space and drives both scissor paths and every origin flip |
| `D_uvgfxmgr_rom_004022C8[]` | The `uvGfxViewport` stack (max 11), index in `D_uvgfxmgr_rom_00402450` |
| `uvGfxViewport` | `{s16 unk0,unk2,unk4,unk6 (clamped copies); s16 x0,x1,y0,y1; Vp vp}` — `uvgfxmgr_rom.c:23` |
| `D_uvgfxmgr_rom_00402408/0A/0C/0E` | Cached current clip rect (x0,x1,y0,y1) used by fill/copy ops |
| `sScissorXmin/Xmax/Ymin/Ymax` | The sprite layer's software scissor |
| `sFontMessages[] / sFontMesgCount` | Deferred text queue (0x70 bytes each: x, y, scale, rgba, 50-char string, font). Queued by `uvFontPrintStr`, flushed by `uvFontGenDList` |
| `sSprites[] / sMaxSprites` | Sprite pool indexed by id; `textureId == 0xFFF` means empty |
| `sGfxDisplayListHead / sGfxDisplayListBase[]` | The single growing `Gfx*` every UI primitive appends to |
| menuslct item model | Container: `count` at `+0x0`, item array pointer at `+0x8`; items are `0x14` bytes with a selection-state s16 at `+0x2` (`1` = selected). Reused by most menus |
| camera channel | Heap-allocated by `uvchannel_rom`. `unk4` = projection matrix; `unkDC..unkF0` = left/right/top/bottom/near/far; `unk214/216/218/21A` = clip-rect x0/x1/y0/y1 |

The camera channel is the single most important struct for anyone doing rendering work — it is where
the projection, the culling planes and the clip rectangle all come from. See
[06](06-graphics.md#the-frustum-bar-draws-and-culls-against).
