# Beetle Adventure Racing — HUD & Menu System Map

> Generated 2026-07-01 by the `bar-hud-menu-map` workflow (26 agents mapping every HUD/menu
> overlay module + the shared UI framework, plus a 3-lens letterbox/pillar investigation).
> Load-bearing claims (RT64 pillarbox, the uv gfx-manager scissor pipeline, `letter`=decoy)
> were spot-verified against source. Companion: [`HUD_MENU_WORKFLOW.md`](HUD_MENU_WORKFLOW.md).

## What this covers

Every screen the player sees — boot logos, the attract/demo loop, the front-end menus
(car/track selection, file select, options), the in-race HUD, the pause menu, results, and the
2P/4P battle front-end — plus the reusable Paradigm **"uv" engine** primitives they are all built on
(font, sprite, gfx-manager viewport/scissor, color, string, and the `menuslct` widget model).

BAR ships these as **relocatable overlay modules** in `lib/bar-decomp/src/modules/<name>.c`. Screen
flow is a **state machine** keyed on the globals `gGameStateFlag` and `gGameSettings[0].gameStateFlag`;
each screen is its own overlay, reached through an engine export-table pointer. In the matching decomp,
a C function body = **byte-matched**; a `#pragma GLOBAL_ASM(...)` line = still **raw asm (unmatched)**.

## Executive summary
The BAR HUD/menu system is a set of ~24 relocatable "uv"-engine overlay modules layered on one shared framework (`__framework__`: font, sprite, gfx-manager, color, string, gui/menuslct primitives). Screen flow is a state machine keyed on `gGameStateFlag` / `gGameSettings[0].gameStateFlag`; each screen (selection, pause, results, gamegui HUD, battle, etc.) is its own overlay reached via an export-table pointer. Match state is uneven: the framework and boot screens (logo, intro) are largely matched and semi-legible, but the biggest interactive surfaces are mostly raw asm — `selection` 100/292, `battle` 0/70, `menuslct` 3/18, `pause` 29/55, `gamegui` 26/38. The single clearest answer to "how do the left/right black bars work": **the left/right pillars are produced by exactly one layer — RT64's final VI-to-window blit in `VIRenderer::fromHDtoWindow` (`lib/rt64/src/render/rt64_vi_renderer.cpp:26-42`) — NOT by any ROM/game code.** The ROM never narrows the horizontal viewport for a cinematic effect; the only game-drawn cinematic bars are TOP/BOTTOM, and they come from camera ASM shrinking a channel's y-insets.

## Letterbox & pillars — unified mechanism (verified)
All three lenses agree once you separate the two axes and the two layers (ROM framebuffer vs. RT64 present blit).

**TOP/BOTTOM cinematic bars = game-drawn, in the ROM (high confidence).** These are not fill-rectangles. Every frame the uv gfx manager clears the whole framebuffer to black (`uvgfxmgr_rom.c:585-586`, `gDPSetFillColor 0x00000000` + full-screen `gDPFillRectangle`), then draws only inside a clip rect emitted by `func_uvgfxmgr_rom_00401C5C` (`uvgfxmgr_rom.c:669-680`, `gSPViewport` + `gDPSetScissor`). Pixels outside the rect are never repainted, so they stay black — that *is* the bar. For the 3D/race view the rect comes from the camera **channel**: `func_uvchannel_rom_00401278` (`uvchannel_rom.c:435-463`) applies per-channel fields `unk214/216/218/21A` = `x0/x1/y0/y1`, defaulting to full screen. The cinematic letterbox is the camera shrinking `unk218` (top) / `unk21A` (bottom) inward over time; the exact per-frame writer lives in still-**unmatched `cam.c` ASM** (`func_cam_00400190/00400C0C/00401D3C` region) — inferred, not read.

**LEFT/RIGHT bars = RT64 pillarbox, host-side only (high confidence).** No game code ever narrows `x0/x1` for effect — the only x-insets are full-width defaults and the 320-wide `func_uvgfxmgr_rom_00401BD4(0,0x13F,0,0xEF)` setup in `game_init.c:213`. The pillars are created at present time in `fromHDtoWindow`: it fits the fixed-4:3 `hdSize` to the window with one uniform scale; when the window is wider than 4:3 it fits-to-height, leaving unpainted (black swap-chain clear) margins left and right. This is invisible at the default 960×720 (4:3) window (`src/main/main.cpp:197`) and only appears after the user maximizes/fullscreens onto a >4:3 monitor.

**Key reconciliation point — why "Expand" does NOT remove the pillars.** `ar_option=Expand` acts on a *different* layer: it sets `aspectRatioScale` consumed by the 3D framebuffer renderer/projection (`rt64_workload_queue.cpp:134-211`), stretching 3D geometry *inside* the still-4:3 framebuffer to widen FOV. It never touches `fromHDtoWindow`, which still treats the image as 4:3 and pillarboxes it. So Expand widens the view *within* the pillars; it does not delete them. Removing the pillars requires changing the VI blit itself.

**Split-screen margins** (2P/4P) are the same viewport-scissor mechanism (per-player channel sub-rects; gaps stay the black clear), computed in unmatched `scrn.c`/`plyr.c`/`battle.c` ASM. **Decoys to ignore:** the `letter` module is Beetle collectible letters (B-E-E-T-L-E, 6× `0x14` table), not letterbox; `cbars` is a color-bars *test pattern*, the opposite of black bars.

**To change the left/right bars, do X (most-likely knob first):**

1. **Change the VI blit fit mode** — `lib/rt64/src/render/rt64_vi_renderer.cpp` `fromHDtoWindow` (L31-41). This is the ONLY producer of L/R pillars. To crop-to-fill, make the window-wider branch scale by `windowSize.x/hdSize.x` (fit-to-width) instead of `.y`; to stretch, compute per-axis scale. Gate behind a new `present_fill_mode: Pillarbox|Crop|Stretch` config, threaded like `divot_option`.
2. **Widen FOV to match (companion to #1)** — set `ar_option` default `Original→Expand` in `src/game/config.cpp:130`; without this, crop/stretch just zooms/distorts a true-4:3 render. Note `rt64_render_context.cpp:129` re-applies config each frame, so the RT64-side default in `rt64_user_configuration.cpp` won't stick — change it in `config.cpp`.
3. **Also widen the HUD** — `src/main/rt64_render_context.cpp:115-127`: set HUD ratio to Full (`extAspectRatio=Expand`, `extAspectPercentage=1.0`), else 2D/menu sprites stay centered 4:3 over a stretched 3D scene.
4. **Cheapest "never see pillars"** — force/snap the window to 4:3 (`src/main/main.cpp:197`); `fromHDtoWindow` then adds no margin. Forgoes widescreen.
5. **For the TOP/BOTTOM cinematic bars (different problem)** — clamp the y-insets at the choke point `lib/bar-decomp/src/modules/uvchannel_rom.c:445-447` (just before the `func_uvgfxmgr_rom_00401BD4` call): force `unk218=0`, `unk21A=screenH-1`, keeping `unk214/216`. This overrides the camera ASM without decompiling `cam.c`. Gate behind an RDRAM-poked flag (SI/cheats-hook pattern) for a runtime toggle.
6. **Expose in menu** — mirror `ar_option`/`divot_option` in `src/ui/assets/config.rml`, `src/ui/bar_ui.cpp`, and `GraphicsConfig` (serialize in `graphics_to/from_json`, map in `set_application_user_config`).

## Module inventory (overview)

| Module | Category | Matched | Role | Letterbox relevance |
|---|---|---|---|---|
| `__framework__` | Framework | 15/18 | The reusable Paradigm "uv" engine UI/HUD rendering primitives (font, s... | DIRECTLY relevant — there are TWO scissor/viewport mechanisms. (1) RDP scissor+v... |
| `filerom` | Framework | 3/9 | A "uvmo" overlay module implementing the media/ROM file-loading backen... | None. filerom performs no rendering — no black bars, pillars, viewport, or sciss... |
| `fileux` | Framework | 11/12 | Buffered file/save-media I/O engine: an open/read/write/close/seek abs... | none. This module performs no rendering: it issues no gSPViewport/gDPSetScissor,... |
| `scene` | Framework | 11/21 | Relocatable overlay that orchestrates the loaded 3D game scene: it ena... | No black-bar / pillar / letterbox drawing in the matched bodies. The module does... |
| `scrn` | Framework | 2/8 | Screen/viewport (render-target) manager overlay for the Paradigm "uv" ... | Viewport/scissor-relevant (not letterbox specifically). scrn is the engine's SCR... |
| `battle` | Screen | 0/70 | The "Beetle Battle" 4-player battle-mode front-end and in-game HUD/log... | None resolvable from the checked-in decomp. The battle module is a uvmo overlay ... |
| `cbars` | Screen | 3/5 | Tiny standalone "uv" engine overlay module whose name denotes color ba... | None visible at the C level, and NOT gameplay/cinematic letterbox. "cbars" = col... |
| `logo` | Screen | 4/4 | Boot-logo overlay module: loads a sprite bank ('UVBT') at startup and ... | No letterbox/black-bar/scissor/pillar code. The only viewport-adjacent call is g... |
| `results` | Screen | 13/31 | Post-race results / standings overlay module: builds and drives the en... | None visible in the matched C. This module contains no black-bar, pillarbox, vie... |
| `splash` | Screen | 1/5 | Boot-time splash / logo screen module (Nintendo + VW/publisher logos) ... | None. The splash module contains no viewport, scissor, pillarbox, or black-bar s... |
| `flag` | HUD | 4/13 | A `uvmo`-type in-world animated object overlay (Paradigm uv engine) th... | none. flag.c contains no viewport (gSPViewport), scissor (gDPSetScissor), pillar... |
| `gamegui` | HUD | 26/38 | In-race HUD overlay drawing plus a large bank of one-line button/callb... | None found in the matched C. This module draws HUD text/sprites through the uv f... |
| `plyr` | HUD | 8/25 | Per-player entity/model-object ("uvmo") overlay: manages the array of ... | None visible. No matched function configures a viewport, scissor rectangle, pill... |
| `menuslct` | Menu | 3/18 | Reusable menu-select list widget: manages a list model (count + item a... | None. No black-bar, pillarbox, viewport, or scissor configuration found. The onl... |
| `pause` | Menu | 29/55 | In-game pause / options overlay module for BAR: builds the pause menu ... | No black-bar / letterbox / scissor drawing here. The only viewport-adjacent thin... |
| `selection` | Menu | 100/292 | Car & track selection front-end: the largest BAR menu/UI overlay. Driv... | None. This module draws no black bars, pillarboxes, or full-screen viewport/scis... |
| `filmroll` | Transition | 2/9 | Relocatable "uv"-engine overlay module implementing the film-roll / pa... | HIGHLY relevant to the viewport/framebuffer, though NOT the race cinematic lette... |
| `glare` | Effect | 1/8 | Self-contained "uv"-engine overlay module (type uvmo) that produces a ... | None. The reloc table shows only uvLoadFile / uvUpdateFileAllocPtr external call... |
| `demo` | Attract | 1/8 | Attract-mode / demo-playback overlay module (form0 entry "demo"): boot... | None found. The demo module contains no scissor/viewport/pillar/black-bar setup ... |
| `intro` | Attract | 6/6 | Attract-mode / intro-demo sequence controller: loads a car+track demo,... | None. This module draws no black bars, pillars, or scissor clips and configures ... |
| `letter` | Other | 1/5 | Relocatable "uv"-engine overlay module (fourcc 'lett'), reached via th... | No positive evidence of letterbox/black-bar/pillar/viewport/scissor behavior. De... |
| `med` | Other | 12/40 | Standalone UVMO overlay module (own entrypoint @0x400000) implementing... | None. This module draws no black bars, pillars, or color-bar test screens, and c... |

## Module inventory (detail)

### `__framework__` — Framework  (15/18 matched, confidence: high)

**Role:** The reusable Paradigm "uv" engine UI/HUD rendering primitives (font, sprite, gfx-manager viewport/scissor, color, string, gui/menuslct widget) that every BAR menu and HUD screen is built on top of — the layer a developer must edit to change layout, clipping, or letterbox/pillar behavior.

**Invoked by:** Not a screen — it is the shared primitive layer. Game modules (intro.c, logo.c, gamegui.c, txtview.c, menuslct.c, results.c, race HUD, etc.) reach the engine every frame through global export-table pointers (gUvFontExports, gUvSprtExports/gUvGuiExports, gUvGfxMgrExports, gUvColorExports, gUvStringExports, gUvGfxStateExports) and enqueue text/sprite draws. The gfx-manager runs the display-list build/scissor/viewport each frame regardless of which menu state (gGameStateFlag / gGameSettings[0].gameStateFlag) is active.

**Engine exports used:** `gUvFontExports (UvFont_Exports)`, `gUvSprtExports / gUvGuiExports (UvSprt_Rom_Exports + per-module GameguiGuiExports)`, `gUvGfxMgrExports (UvGfxMgr_Exports — viewport/scissor/screen size)`, `gUvGfxStateExports (UvGfxState_Rom_Exports)`, `gUvColorExports (UvColor_Exports)`, `gUvStringExports (UvString_Exports — uvStrlen/uvStrChr/uvSprintf)`, `gUvBlitExports (UvBlit_Rom_Exports — uvParseBlit)`, `gSndExports / gUvCmidiExports / gUvAudiomgrExports (menu SFX, via gamegui)`

**Letterbox/viewport relevance:** DIRECTLY relevant — there are TWO scissor/viewport mechanisms. (1) RDP scissor+viewport: func_uvgfxmgr_rom_00401C5C (uvgfxmgr_rom.c:669) emits gSPViewport + gDPSetScissor(G_SC_NON_INTERLACE, vp->x0, sScreenHeight-vp->y1, vp->x1, sScreenHeight-vp->y0); the rect comes from uvGfxClipRect / func_uvgfxmgr_rom_00401BD4 into D_uvgfxmgr_rom_004022C8[]. Shrinking vp->x0/x1 = left/right pillars; shrinking y0/y1 = top/bottom letterbox for the 3D/HUD viewport. (2) Software sprite scissor: sScissorXmin/Xmax/Ymin/Ymax in uvsprt_rom, reset to 0..320 / 0..240 by uvSpriteDrawInit (uvsprt_rom.c:665) and enforced per-sprite in uvDrawBitmap (uvsprt_rom.c:462). To bar the 2D HUD/menu sprite layer specifically, narrow those. Note: the 320x240 sScreenWidth/Height define the space, so any bar math keys off those. (Prior memory notes race letterbox is separately camera-driven in game ASM, not these primitives.)

**Readability:** Mixed. Sprite (uvsprt_rom.c) and gfx-manager (uvgfxmgr_rom.c) cores are well-matched real C — the two viewport/scissor emitters, uvGfxClipRect, uvDrawBitmap, uvSpriteDrawInit, uvSprtProps all have readable bodies and named globals (sScreenWidth, sScissor*). Struct members in the gfx-manager viewport are partly named (x0/x1/y0/y1 identified) but most UvGfxMgr_Exports/UvGfxState_Rom_Exports slots are still func_uvgfxmgr_rom_XXXX / func_uvgfxstate_rom_XXXX raw with inferred prototypes. Font: prototypes and struct fully typed but uvFontPrintStr/uvFontGenDList ship as GLOBAL_ASM (body exists only under NON_MATCHING). menuslct.c is mostly raw GLOBAL_ASM (15 asm vs 2 matched) — item struct + selection scan inferred from func_menuslct_004009FC, draw call shape from func_menuslct_00400BCC callers. Gui/Snd export tables are declared as local opaque {char pad; fnptr unkNN} in each consumer (gamegui.c), not centrally typed.

**Modification hooks:** To move/hide/resize menu or HUD elements: edit the (x,y) and scale args passed to uvSprtProps (SPRT_PROP_POS case 2 / SPRT_PROP_SCALE) and uvFontPrintStr in the specific game module — remember y is top-left-origin in 320x240 and the engine flips it. To recolor: SPRT_PROP_COLOR (case 7) / uvFontColor. To LETTERBOX or PILLARBOX the 3D+HUD viewport: change the rect fed to func_uvgfxmgr_rom_00401BD4 / uvGfxClipRect, which func_uvgfxmgr_rom_00401C5C turns into gSPViewport+gDPSetScissor (uvgfxmgr_rom.c:658-680). To bar only the 2D sprite/menu layer: override sScissorXmin/Xmax/Ymin/Ymax after uvSpriteDrawInit reset (uvsprt_rom.c:665) — enforced in uvDrawBitmap. To change the whole UI coordinate space: uvSetScreenWidth/uvSetScreenHeight (default 320x240). Menu item selection/layout lives in the menuslct item struct (count@0,items@8,stride0x14,sel-state@2) and its draw primitive func_menuslct_00400BCC.

**Key data symbols:**

- `sScreenWidth / sScreenHeight` — u16 globals in uvgfxmgr_rom (default SCREEN_WIDTH=320, SCREEN_HEIGHT=240). The UI coordinate system: all HUD/menu x/y are in this 320x240 space with TOP-LEFT origin; the engine flips y to N64 bottom-left framebuffer via (ScreenHeight - y - height).
- `D_uvgfxmgr_rom_004022C8[]` — uvGfxViewport[] stack (up to 11 entries, index D_uvgfxmgr_rom_00402450). Each entry holds the scissor rect x0/x1/y0/y1 (s16, top/left/right/bottom) plus a computed N64 Vp. Editing the pushed rect is how you'd pillarbox/letterbox at the RDP level.
- `uvGfxViewport struct` — {s16 unk0,unk2,unk4,unk6 (clamped copies); s16 x0(left),x1(right),y0(top),y1(bottom); Vp vp}. Defined uvgfxmgr_rom.c:23. The viewport/scissor record.
- `sScissorXmin/Xmax/Ymin/Ymax` — s32 globals in uvsprt_rom — the SOFTWARE (CPU-side) scissor for the sprite/HUD layer, reset to 0,0,320,240 by uvSpriteDrawInit each frame. Independent from the RDP gDPSetScissor. Narrowing Xmin/Xmax here bars off the sprite layer left/right.
- `D_uvgfxmgr_rom_00402408/0A/0C/0E` — s16 cached current clip rect (x0,x1,y0,y1) used by fill/copy ops; kept in sync by 00401C5C and uvGfxClipRect.
- `sFontMessages[] / sFontMesgCount` — FontMessage[0x70-each] queue (x,y,scaleX/Y,rgba,str16[50],font). Text is deferred: queued by uvFontPrintStr, flushed by uvFontGenDList. sFontScaleX/Y, sFontColorRed/Green/Blue/Alpha are the current text style state.
- `sSprites[] / sMaxSprites` — uvSprite_t pool indexed by sprite id; textureId 0xFFF = empty/skip. Menus allocate ids via export slot 0x04 and drive them with uvSprtProps.
- `menuslct item model` — Menu container: s16 count @+0x0, item-array ptr @+0x8; each item is 0x14 bytes with a selection-state s16 @+0x2 (1 = selected). Reused by most menus.
- `sGfxDisplayListHead / sGfxDisplayListBase[]` — The single growing Gfx* the whole UI appends to (uvGetDisplayListHead export 0x08). All primitives above ultimately post gDP/gSP commands here.

<details><summary>Functions (18)</summary>

| Function | Status | Purpose |
|---|---|---|
| `func_uvgfxmgr_rom_00401C5C` | ✅ matched | THE viewport+scissor emitter (export slot 0x5C). Emits gSPViewport(&vp->vp) AND gDPSetScissor(G_SC_NON_INTERLACE, vp->x0, sScreenHeight-vp->y1, vp->x1, sScreenHeight-vp->y0). Edit here (or the rect passed in) to create left/right pillars or top/bottom letterbox at the RDP level. In lib/bar-decomp/src/modules/uvgfxmgr_rom.c:669 |
| `uvGfxClipRect` | ✅ matched | Sets a uvGfxViewport's x0/x1/y0/y1 (clamped to 0..sScreenWidth/Height) and computes the N64 Vp vscale/vtrans from that rect. The place a viewport rectangle is defined before 00401C5C emits it. uvgfxmgr_rom.c:592 |
| `func_uvgfxmgr_rom_00401BD4` | ✅ matched | Push a new viewport onto the D_uvgfxmgr_rom_004022C8[] stack (max 0xB=11): calls uvGfxClipRect then 00401C5C. Export slot 0x58, signature (x0,x1,y0,y1). Primary caller-facing entry to set the active scissor/viewport rect. uvgfxmgr_rom.c:658 |
| `uvDrawBitmap` | ✅ matched | Core sprite-to-framebuffer rasterizer: CPU-side rect clip against sScissorXmin/Xmax/Ymin/Ymax, then emits gSPTextureRectangle. Second, software scissor path (separate from the RDP one) — narrowing sScissor bounds also produces bars for the sprite/HUD layer. uvsprt_rom.c:462 |
| `uvSpriteDrawInit` | ✅ matched | Per-frame sprite pass setup: RESETS sScissorXmin=0,Ymin=0,Xmax=SCREEN_WIDTH(320),Ymax=SCREEN_HEIGHT(240) and configures 1-cycle textured render mode. The single place the sprite-layer software scissor is (re)initialized each frame. uvsprt_rom.c:665 |
| `uvSprtProps` | ✅ matched | Variadic sprite property setter (export slot 0x1C, the ...-arg call seen as ->unk1C in modules). Case 2 = SPRT_PROP_POS: sprite->y = ScreenHeight - y - height (top-left origin -> N64 bottom-left flip). Cases: 1/13 scale, 7 color RGBA, 9 blit, 10 texture(UVTX), 11 transparent, 12 cutout, 14 extern. This is how menus position/tint every sprite. uvsprt_rom.c:343 |
| `uvSpriteDraw` | ✅ matched | Builds the display list for one Sprite (texture load + gSPTextureRectangle). uvsprt_rom.c:690 |
| `uvSpriteRender` | ✅ matched | Renders sprite by id from the sSprites[] pool if textureId != 0xFFF (export slot 0x10). uvsprt_rom.c:245 |
| `uvSpriteDrawAll / uvSpriteDrawFinish` | ✅ matched | Flush all queued sprites / close the sprite pass (export slots 0x0C, 0x28). uvsprt_rom.c |
| `uvGetSpriteWidth / uvGetSpriteHeight` | ✅ matched | Query sprite pixel dims (slots 0x14/0x18); menus use these to center (e.g. intro: x = 0xA0 - width/2). uvsprt_rom.c:329/336 |
| `uvFontPrintStr` | ⬜ asm | Queue a text draw at (x,y): applies y += font height*scaleY then y = uvGetScreenHeight()-y (top-left origin flip), maps chars to glyph indices, stores into sFontMessages[]. Body written under NON_MATCHING but shipped as GLOBAL_ASM. THE text-placement primitive. uvfont_rom.c:155 |
| `uvFontGenDList` | ⬜ asm | Flush queued sFontMessages[] into the display list at frame end (export slot 0x28). uvfont_rom.c |
| `uvSetFont / uvFontScale / uvFontColor` | ✅ matched | Select font asset, set sFontScaleX/Y, set sFontColorRed/Green/Blue/Alpha (export slots 0x04/0x08/0x0C). Text style state used before uvFontPrintStr. uvfont_rom.c |
| `uvFontWidth / uvFontHeight / uvFontStrlLen` | ✅ matched | Measure text for centering/layout (slots 0x14-0x1C). uvfont_rom.c |
| `func_menuslct_004009FC` | ✅ matched | Selection scan: over a menu container (count at +0x0, items ptr at +0x8, item stride 0x14), returns index of the item whose selection-state field (+0x2) == 1, else -1. Reveals the reusable menu item struct + selection model. menuslct.c:35 |
| `func_menuslct_00400BCC` | ⬜ asm | Menu widget draw primitive, called as (x,y,w,h,...): e.g. func_menuslct_004013F8 calls (0x16,0xE1,0x12A,0x10,...) — a highlight/box rect in 320-wide pixel space. Most of menuslct.c is still raw GLOBAL_ASM (15 asm vs 2 matched). menuslct.c:145 |
| `func_uvgfxmgr_rom_00401914` | ✅ matched | Full-screen clear: gDPFillRectangle(0, sScreenWidth, 0, sScreenHeight). Reference for how the whole viewport rect is addressed. uvgfxmgr_rom.c:582 |
| `uvSetScreenWidth / uvSetScreenHeight / uvGetScreenWidth / uvGetScreenHeight` | ✅ matched | Get/set sScreenWidth,sScreenHeight (default 320x240, export slots 0x7C-0x88). These two globals define the UI coordinate system and drive every origin flip and both scissor paths. uvgfxmgr_rom.c:750-758 |

</details>

**Gaps:** 1) menuslct.c is ~88% raw asm: the item struct beyond {count,items,sel@+2,stride0x14} and func_menuslct_00400BCC's full draw semantics are inferred, not verified. 2) uvFontPrintStr/uvFontGenDList are GLOBAL_ASM; the exact glyph-quad emission and how scaleX/Y map to gSPTextureRectangle isn't byte-confirmed (readable only via NON_MATCHING). 3) Many UvGfxMgr_Exports / UvGfxState_Rom_Exports / UvColor_Exports slots remain func_XXXX with only inferred prototypes — the color and gfxstate export member names are unknown. 4) gUvGuiExports (GameguiGuiExports) is only partially typed (unkC(void*), unk28(void*,s32,s32)); the full gui-widget API is not mapped. 5) Race cinematic letterbox is asserted (per prior memory) to be camera-driven game ASM, NOT these primitives — confirming that would need reading the race/cam module asm. 6) Only reachable via reading generated asm (asm/ is make-generated, not checked in) for any of the unmatched bodies above.


---

### `filerom` — Framework  (3/9 matched, confidence: medium)

**Role:** A "uvmo" overlay module implementing the media/ROM file-loading backend of the Paradigm "uv" engine: it DMA-copies file/module data from cartridge ROM (media) into RDRAM and loads/unloads engine modules and files. Despite the name-hint, this is ROM/cartridge media I/O (uvLoadFile/uvLoadModule/_uvMediaCopy), NOT controller-pak save-game handling.

**Invoked by:** Registered as an overlay in the form0 module table (config/us/form0.json entry "filerom", nullEntry:false) and modules.yaml (type: uvmo, start 0x52B40, follows main). The __entrypoint_func_filerom_400000 stub builds a small function-pointer/vtable (relocs at rom 0x64-0x88 wire func_filerom_00400060/00400068/0040007C/00400364/00400494 into a table) that the uv engine invokes when files/modules are loaded or unloaded — a media-loader callback provider, not tied to any single gGameStateFlag screen. Low-level service consumed by file/menu load paths; grep found zero external func_filerom_/D_filerom_ references, so invocation is entirely via engine export/vtable indirection.

**Engine exports used:** `uvLoadFile (file_manager.h: void* uvLoadFile(s32 formFileTag, s32 fileId))`, `uvLoadModule (functions.h: void* uvLoadModule(s32 tag))`, `uvUnloadModule (functions.h: void uvUnloadModule(s32 tag))`, `uvUpdateFileAllocPtr (file_manager.h: void uvUpdateFileAllocPtr(void* allocPtr))`, `_uvMediaCopy (functions.h: void _uvMediaCopy(void* vAddr, void* devAddr, u32 nbytes) — ROM->RDRAM DMA)`

**Letterbox/viewport relevance:** None. filerom performs no rendering — no black bars, pillars, viewport, or scissor clip, and configures no display/VI state. It is a pure media/ROM DMA-and-module-load service (uvLoadFile/uvLoadModule/_uvMediaCopy). No gSP/gDP, no uvGui/uvSprt/uvFont usage. Not letterbox-relevant.

**Readability:** Mostly RAW. Only 3 of 9 functions are matched, and all matched ones are trivial stubs (two return-constant/empty, one small flag guard). Every function is still named func_filerom_XXXX; all data symbols are raw D_filerom_XXXX / B_filerom_XXXX with no typedef'd struct or comments. filerom.c has no local comments (just SPDX header and one extern). The reloc table is the saving grace: it names the external engine calls (uvLoadModule/uvLoadFile/uvUnloadModule/uvUpdateFileAllocPtr/_uvMediaCopy), giving strong evidence of purpose. Overall readability low without disassembly of the 6 unmatched bodies.

**Modification hooks:** Not a visual/UI element — nothing to move/hide/recolor. Developer edit points: (1) to change what/how files stream from ROM, edit func_filerom_00400364 (the uvLoadModule/uvLoadFile/uvUnloadModule call site) — still GLOBAL_ASM, must be decompiled first. (2) The loader-busy latch is D_filerom_00400624, claimed/released in the matched func_filerom_00400494 (lib/bar-decomp/src/modules/filerom.c:23-29) — the one place already editable in C. (3) DMA copy sizing/behavior is in func_filerom_0040007C / 00400230 (the _uvMediaCopy call sites). (4) Symbol addresses/names to relocate/rename: lib/bar-decomp/linker_scripts/us/module_files/filerom_symbol_addrs.txt and filerom_reloc_addrs.txt; module registration: config/us/modules.yaml (start 0x52B40) and config/us/form0.json (entry alongside expl/fileux).

**Key data symbols:**

- `D_filerom_00400624` — Primary loader busy/loaded flag (0=free). Read/cleared by func_filerom_00400494 and referenced throughout load routines; a claim/release latch.
- `D_filerom_00400628` — Loader state word (referenced in func_00400364 dispatch region, relocs 0x3F0/0x438/0x480); likely current tag or file-count.
- `D_filerom_0040062C` — Loader state/pointer used alongside 0x624 in the media-copy path (relocs 0x1A8/0x4BC).
- `D_filerom_00400630` — Loader state/pointer (allocation or destination cursor) used by the copy path (relocs 0x118/0x4A4).
- `D_filerom_00400638` — Loader state/pointer used by func_0040007C copy routine (relocs 0x1A4/0x1B0).
- `D_filerom_004004D0/004004D4/004004DC` — Rodata jump-table or function-pointer constants (MIPS_32 relocs at 0x540/0x544/0x548) used by the entrypoint vtable/dispatch.
- `D_filerom_004004F0/004004FC` — Rodata constants referenced by the read state machine (func_00400308 region).
- `B_filerom_00400620` — BSS/uninitialized data region base at 0x400620 (holds the runtime state words 0x624/0x628/0x62C/0x630/0x638).

<details><summary>Functions (9)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_filerom_400000` | ⬜ asm | Module entrypoint/init stub; via HI16/LO16 relocs it populates a function-pointer table with the module's callbacks (00400060/00400068/0040007C/00400364/00400494) and calls uvUpdateFileAllocPtr — registers filerom as the media-loader handler. |
| `func_filerom_00400060` | ✅ matched | Empty no-op callback (matched, body is {}); likely an unused/optional vtable slot (e.g. shutdown/flush hook). |
| `func_filerom_00400068` | ✅ matched | Stub returning 0 for (u8,u8,u8) args (matched); likely a status/validation query callback that always reports success/idle. |
| `func_filerom_0040007C` | ⬜ asm | Media/file load routine; calls _uvMediaCopy (relocs at 0x154, 0x1D4, 0x1FC) — DMA-copies file data from ROM/media into an allocated RDRAM buffer, referencing state D_filerom_00400624/0062C/00400630/00400638. |
| `func_filerom_00400230` | ⬜ asm | Media-copy helper (calls _uvMediaCopy at reloc 0x234); performs a block/chunk ROM->RDRAM DMA copy for the loader. |
| `func_filerom_00400270` | ⬜ asm | Loader helper; likely computes offsets/sizes or advances the media read cursor for the copy routines. |
| `func_filerom_00400308` | ⬜ asm | Loader helper; participates in the file-read state machine (uses D_filerom_004004F0/004004FC data pointers seen in relocs at 0x2E4/0x3FC). |
| `func_filerom_00400364` | ⬜ asm | Module load/unload dispatcher: calls uvLoadModule (0x464), uvLoadFile (0x488) and uvUnloadModule (0x49C) — the main entry that loads or frees an engine module/file and updates alloc/instance state (D_filerom_00400624/00400628/0062C/00400630). |
| `func_filerom_00400494` | ✅ matched | Guard/consume routine (matched): if D_filerom_00400624 (busy/loaded flag) is 0 returns -1, otherwise clears it and returns 0 — a one-shot claim/release on the loader-busy flag. |

</details>

**Gaps:** 6 of 9 functions are unmatched (still #pragma GLOBAL_ASM), and the raw .s is make-generated and not checked in, so the bodies of __entrypoint_func_filerom_400000, func_0040007C, 00400230, 00400270, 00400308, and 00400364 cannot be read — their exact logic (copy sizes, tag handling, the vtable shape) is inferred from names, the reloc table, and file_manager.h prototypes. The types/layout of the B_filerom_00400620 BSS block are guessed. The task hint ('controller-pak / save-file ROM I/O') is NOT supported: filerom's external calls are cartridge-media DMA and module load/unload (read-only ROM streaming); controller-pak SAVE handling more likely lives in the sibling 'fileux' module (config/us lists fileux immediately after filerom). Confidence on media-loader role: medium-high; confidence it is NOT the save-pak handler: medium.


---

### `fileux` — Framework  (11/12 matched, confidence: medium)

**Role:** Buffered file/save-media I/O engine: an open/read/write/close/seek abstraction over ~20 file "slots" (0x14), each backed by a 0x10C-byte allocation with a 0x100-byte staging buffer, marshalling requests through a single variadic command dispatcher (func_fileux_0040014C) that speaks a byte-code protocol ('O'/'R'/'W'/'C'/'L' = 0x4F/0x52/0x57/0x43/0x4C). Despite the "ux" hint it is NOT a file-select screen — it draws nothing; it is the save/file access backend the front-end (filerom / file_manager / save UI) sits on top of.

**Invoked by:** Loaded as a Paradigm "uv" overlay module (type uvmo, ROM 0x532A8, VRAM 0x400000) by the module loader; its entrypoint __entrypoint_func_fileux_400000 receives a FileUx_Exports* and populates 5 function pointers (open/read/write/close and one nulled slot). Callers reach it exclusively through that exports table at runtime (a global FileUx_Exports pointer resolved by the loader) — no decomp C source references FileUx_Exports outside fileux.c itself, so the concrete caller (save subsystem / file_manager / filerom) invokes via the pointer table, not by symbol. Not tied to a menu game-state; used on demand whenever a file/save slot is opened, read, written, or closed.

**Engine exports used:** `gUvGfxMgrExports (func_uvgfxmgr_rom_00402090, called at top of the dispatcher)`, `uvUpdateFileAllocPtr (module alloc-pointer registration)`, `_uvMediaCopy / _uvMemAllocAlign8 / _uvMemFree (uv media + allocator runtime)`

**Letterbox/viewport relevance:** none. This module performs no rendering: it issues no gSPViewport/gDPSetScissor, configures no black bars, pillars, viewport, or scissor clip, and touches no framebuffer. It is pure file/save-media byte I/O over 20 slots. Not letterbox-relevant.

**Readability:** Partially readable. All 11 helper/exported functions have byte-exact C bodies; the module struct FileUx_Exports and the slot descriptor (UnkStruct_fileux_004005A0) are typed but with raw unk0/unk4/unk8/unkC members and inferred/unnamed comments. Every function is still named func_fileux_<addr> and data is D_fileux_<addr> (no semantic names). The central dispatcher func_fileux_0040014C is UNMATCHED (GLOBAL_ASM); a reconstructed body exists only under _NON_MATCHING and reveals the op-code protocol. No descriptive comments beyond /* inferred */.

**Modification hooks:** Because this module renders nothing, there is no layout/position/color to edit. Behavioral hooks: (1) slot capacity — the 0x14 (20) bounds checks in func_fileux_004005A0/00400768/00400970/004009EC gate max simultaneous open files. (2) buffering granularity — the 0x100 page size threaded through the read/write loops and the 0x10C descriptor allocation in func_fileux_00400970 (_uvMemAllocAlign8(0x10C)) control staging-buffer size. (3) op semantics — func_fileux_0040014C op codes 0x4F/0x52/0x57/0x43/0x4C map to open/read/write/close/seek; to change save behavior you edit the packet layout there. To alter the actual on-screen file/save-slot UI you must edit the front-end module (filerom / file_manager / the save UI), NOT fileux.

**Key data symbols:**

- `D_fileux_00400EF8` — Array of up to 20 (0x14) UnkStruct_fileux_004005A0* slot descriptors, one per open file. Each descriptor is 0x10C bytes: unk0=buffered byte count/position in the 0x100 staging area, unk4=read cursor within the buffer, unk8=direction flag (0=write,1=read), unkC=start of the 0x100-byte data buffer.
- `D_fileux_00400CF8` — 0x100-byte shared command/response scratch buffer used by the dispatcher; first word aliased as an s32 return status/handle.
- `D_fileux_00400C80` — Jump table (type:jtbl) backing the switch in func_fileux_0040014C op dispatch.
- `D_fileux_00400CE0 / B_fileux_00400CF0 / D_fileux_00400C80` — Small module-local data/bss (dispatcher state, jump-table, padding); not user-tunable content.

<details><summary>Functions (12)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_fileux_400000` | ✅ matched | Module entrypoint: calls uvUpdateFileAllocPtr(exports) then wires the FileUx_Exports table (open/read/write/close + nulled func_fileux_00400060). |
| `func_fileux_00400060` | ✅ matched | Empty exported stub (exports[0x00]); likely a no-op init/flush hook. |
| `func_fileux_00400068` | ✅ matched | Empty stub (not exported); unused/no-op. |
| `func_fileux_00400070` | ✅ matched | strlen: counts bytes in a NUL-terminated string. |
| `func_fileux_004000A0` | ✅ matched | memcpy-and-advance: copies len bytes into *d and advances the destination pointer (used to build the command packet). |
| `func_fileux_00400138` | ✅ matched | Empty stub (buffer/DMA submit or command-flush placeholder; called around each dispatch). |
| `func_fileux_00400144` | ✅ matched | Stub returning 1 (command-completion/status poll placeholder; always 'success'). |
| `func_fileux_0040014C` | ⬜ asm | Core variadic command dispatcher: builds a 0x100-byte request packet and switches on op code 'O'/'R'/'W'/'C'/'L' (open/read/write/close/seek-lock), returning a slot id, byte count, or status. (Matching C exists only under _NON_MATCHING; ships as GLOBAL_ASM.) |
| `func_fileux_004005A0` | ✅ matched | Exported WRITE (exports[0x04]): buffered write of arg2 bytes from arg1 into slot arg0, flushing full 0x100-byte pages via 'W' and staging the remainder in the slot's buffer. |
| `func_fileux_00400768` | ✅ matched | Exported READ (exports[0x08]): buffered read of arg2 bytes into arg1 from slot arg0, refilling 0x100-byte pages via 'R' and serving from the slot buffer. |
| `func_fileux_00400970` | ✅ matched | Exported OPEN (exports[0x0C]): issues 'O' for arg0/name, allocates a 0x10C slot descriptor via _uvMemAllocAlign8 into D_fileux_00400EF8[id], returns slot id or <0 error. |
| `func_fileux_004009EC` | ✅ matched | Exported CLOSE (exports[0x10]): flushes any pending write buffer via 'W', frees the slot descriptor, clears D_fileux_00400EF8[arg0], and issues 'C' to close. |

</details>

**Gaps:** The central dispatcher func_fileux_0040014C is unmatched (asm not checked in); its exact packet layout and 'L'/seek semantics are inferred from the _NON_MATCHING reference. No decomp caller of FileUx_Exports exists in source, so the concrete consumer (which save/file-manager module holds the global FileUx_Exports pointer and at which game state it is invoked) is not pinned — needs a grep of the module loader / exports-table wiring or asm to confirm. Slot-descriptor field names (unk0/unk4/unk8/unkC) and the meaning of the nulled export[0x00] slot are inferred, not verified. The op codes are strongly consistent with ASCII 'O/R/W/C/L' but the jump table (D_fileux_00400C80) ordering was not disassembled to confirm all cases.


---

### `scene` — Framework  (11/21 matched, confidence: medium)

**Role:** Relocatable overlay that orchestrates the loaded 3D game scene: it enables/toggles UVMD model files, wires up terrain/lighting/glare/rain/env/isect/midi subsystems for a race track, and looks up per-track weather/music parameters. It is scene/world orchestration, not a HUD or 2D menu drawer.

**Invoked by:** Loaded as a relocatable uv module via uvLoadModule; its exports jump-table is populated by __entrypoint_func_scene_400e0c (0x400e0c). Driven during in-race / active-3D-world game states (the module manages track terrain, lighting, weather, and midi music), not during the 2D front-end menus. Functions are reached through the module's exports struct (a pointer analogous to gScrnExports/gUvTerraExports) rather than by name from sibling decomp modules, so no direct in-tree callers appear via grep.

**Engine exports used:** `gScrnExports`, `gSndExports`, `gUvGuiExports`, `gUvDobjExports`, `gUvTerraExports`, `gUvEnvExports`, `gUvLightExports`, `gUvIsectExports`, `gUvMathExports`, `gUvCmidiExports`, `gLightExports`, `gGlareExports`, `gRainExports`

**Letterbox/viewport relevance:** No black-bar / pillar / letterbox drawing in the matched bodies. The module does touch the screen/viewport list: func_scene_00401254 walks gScrnExports+0x18 (screen array, s16 count at +0x10) and sets a per-screen flag at +0x21C to 1 - this is a per-screen scene state flag (dirty/enable), NOT scissor geometry, pillar width, or viewport rectangle configuration. No gDPSetScissor/gSPViewport-equivalent geometry is set in matched code. Cinematic race letterbox is camera-driven (in camera/ASM per project notes), not produced here. Possible but unconfirmed geometry work could hide in the unmatched routines (func_scene_004003F4, 00401324, 004012B8) which manipulate scene/screen state - would need the asm to rule in/out.

**Readability:** Mixed / mostly raw. All symbols are still raw func_scene_XXXXXXXX and D_scene_XXXXXXXX with no names, types, or comments. About half the functions are matched (byte-exact C) but expressed as raw pointer-offset arithmetic on opaque structs (e.g. gScrnExports+0x18, D_scene_00402DF8+0xE8); the export tables are typed only as void*. Nine functions plus the entrypoint remain unmatched (#pragma GLOBAL_ASM). No structs are named/typed for this module.

**Modification hooks:** To change which track models load/show: edit the model-index list between D_scene_00402D34 and D_scene_00402D40 (consumed by func_scene_004002A8) or the flag-bit test (byte +0x5 & 0x70) in func_scene_004002F0 and the +0x20 test in func_scene_00400374. To alter per-track weather/music selection: edit the lookup tables at D_scene_00402CF8 (6 entries) and D_scene_00402D10 (9 entries) or the clamp bounds in func_scene_00400000/00400030. To change scene object placement/scale: edit the f32 constants D_scene_00401814..00401834 (and the 32-byte transform copy in func_scene_004013F4). To toggle the per-screen scene flag behavior: func_scene_00401254 (offset +0x21C on each gScrnExports screen). To adjust the per-frame accumulator reset: the +0xE8/+0xEC zeroing gated on +0xD8 in func_scene_00400C60. Wiring of which subsystems (terrain/light/glare/rain/env/midi) are active lives in the unmatched func_scene_004003F4.

**Key data symbols:**

- `D_scene_00402CF8` — Base of a 6-entry s32 lookup table (indexed by func_scene_00400000; +0x4 alias D_scene_00402CFC), likely per-track/weather parameter.
- `D_scene_00402D10` — Base of a 9-entry s32 lookup table (indexed by func_scene_00400030), likely track/music parameter (+0x4 alias D_scene_00402D14).
- `D_scene_00402D34 / D_scene_00402D40` — Start/end of a static array of model-file indices iterated by func_scene_004002A8.
- `D_scene_00402DF8` — Large scene-state struct (referenced with many offsets: +0xD8 flag, +0xE8/+0xEC f32 accumulators, etc.) - the module's main runtime state object.
- `D_scene_00402EF0` — Pointer to a scene sub-object with a vtable-like layout (method called at +0x4 each frame by func_scene_00400C60).
- `D_scene_004018xx (00401814..00401834)` — Block of f32 scene constants (positions/scales/thresholds) used by the unmatched setup routines.
- `D_scene_004015F0..00401804` — Large rodata block referenced by MIPS_HI16/LO16 relocs - scene tables/strings/param arrays consumed by the unmatched functions.
- `D_scene_00402E00 / D_scene_00402E01` — Small s8 scene state flags.
- `gTrackWeather / gCurrentTrack / gCheatTrackMusic` — Externs read by scene to select weather/music per current track (drives the lookup tables).

<details><summary>Functions (21)</summary>

| Function | Status | Purpose |
|---|---|---|
| `func_scene_00400000` | ✅ matched | Clamp index to 0..5 and return table entry (&D_scene_00402CF8)[i]; a 6-entry lookup (likely per-track/weather param, tied to gTrackWeather/gCurrentTrack). |
| `func_scene_00400030` | ✅ matched | Clamp index to 0..8 and return (&D_scene_00402D10)[i]; a 9-entry lookup table (likely track/music param, tied to gCheatTrackMusic/gCurrentTrack). |
| `func_scene_00400060` | ⬜ asm | Scene setup/init helper (raw asm; references scene float constants D_scene_004018xx and subsystem exports) - likely positions/initializes a scene element. |
| `func_scene_004000FC` | ⬜ asm | Scene element setup/update (raw asm); part of the per-object configuration path used with func_scene_00400188. |
| `func_scene_00400188` | ⬜ asm | Per-UVMD-model operation applied to a model-file index (called in loops by 004002A8/004002F0/00400374); enables/toggles/initializes an individual scene model. |
| `func_scene_004002A8` | ✅ matched | Iterate a static list of model indices (D_scene_00402D34..00402D40) calling func_scene_00400188 on each - batch-enable a fixed set of scene models. |
| `func_scene_004002F0` | ✅ matched | Iterate all loaded UVMD ('UVMD'=0x55564D44) files; for each whose flag byte at +0x5 has bits 0x70 set, call func_scene_00400188 - conditional model enable by file flags. |
| `func_scene_00400374` | ✅ matched | Iterate all loaded UVMD files; for each with a nonzero field at +0x20, call func_scene_00400188 - conditional model enable by second criterion. |
| `func_scene_004003F4` | ⬜ asm | Large scene build/update routine (raw asm); central scene orchestration touching terrain/light/glare/rain/env/isect/midi exports. |
| `func_scene_00400C58` | ✅ matched | Empty stub (no-op) - placeholder/aligned entry. |
| `func_scene_00400C60` | ✅ matched | Per-frame scene tick: calls virtual method at +0x4 of object D_scene_00402EF0, and when a sub-field (+0xD8) is zero, zeroes two f32 accumulators at +0xE8/+0xEC (velocity/offset reset). |
| `func_scene_00400CB8` | ⬜ asm | Scene update/step routine (raw asm) - continuation of the per-frame scene logic around func_scene_00400C60. |
| `__entrypoint_func_scene_400e0c` | ⬜ asm | Module entrypoint (0x400e0c): calls uvUpdateFileAllocPtr and populates the scene exports jump table with pointers to this module's functions. |
| `func_scene_00400EC0` | ✅ matched | Empty stub (no-op) - placeholder/aligned entry. |
| `func_scene_00400EC8` | ⬜ asm | Scene routine (raw asm), immediately after the empty EC0 stub; likely a scene load/reset or draw-setup step. |
| `func_scene_00401254` | ✅ matched | Iterate every screen/viewport in gScrnExports+0x18 (count at +0x10) and set the flag at screen+0x21C to 1 - marks all screens with a per-screen scene flag (e.g. dirty/enable/redraw). |
| `func_scene_004012B0` | ✅ matched | Empty stub taking s32 (no-op) - placeholder/aligned entry. |
| `func_scene_004012B8` | ⬜ asm | Scene routine (raw asm) paired with func_scene_00401254 - likely the counterpart per-screen flag clear/apply. |
| `func_scene_00401324` | ⬜ asm | Scene routine (raw asm) - additional scene state/screen setup step. |
| `func_scene_004013F4` | ✅ matched | Copy an 8-word (32-byte) struct from arg1 to arg0 (1 s32 + 7 f32) - copies a transform/param block (e.g. camera or object placement). |
| `func_scene_00401438` | ⬜ asm | Scene routine (raw asm) - final function in module; likely scene teardown or per-frame post-step. |

</details>

**Gaps:** 10 of 21 entries are unmatched (raw asm not checked in), including the two largest orchestration routines func_scene_004003F4 and func_scene_00400CB8/00400EC8 and the entrypoint - so the exact subsystem wiring and any viewport/scissor setup cannot be confirmed. The export-table struct layouts are untyped (void*), so offsets like gScrnExports+0x18 and +0x21C, and D_scene_00402DF8's field map, are inferred from usage, not from named structs. No in-tree callers were found (functions dispatch through the module exports pointer), so the precise game-state(s) that invoke scene are inferred from the subsystems it touches (race/3D world) rather than proven. Letterbox/scissor involvement in the unmatched functions is unresolved pending the asm. Source: C:/Users/Bryan/Projects/beetle-adventure-racing-recomp/lib/bar-decomp/src/modules/scene.c and linker_scripts/us/module_files/scene_symbol_addrs.txt + scene_reloc_addrs.txt.


---

### `scrn` — Framework  (2/8 matched, confidence: medium)

**Role:** Screen/viewport (render-target) manager overlay for the Paradigm "uv" engine: owns the array of active on-screen "screens"/viewports (framebuffer + viewport descriptors) and exposes them to other modules via the gScrnExports jump table. Its unk4 export configures/creates the active screen(s) during logo, intro and race setup; it holds a screen-descriptor array (ptr at exports+0x18, s16 count at +0x10) that scene.c walks to touch every live viewport.

**Invoked by:** A uvmo relocatable overlay (config beetleadventurerac.us.yaml: name=scrn, type=uvmo, start=0xABBB0; modules.yaml entry between "scene" and "selection"). Its __entrypoint_func_scrn_400000 runs at module-load time: it calls uvUpdateFileAllocPtr (MIPS_26 call at reloc rom:0x58) to register the module and then writes the six exported function pointers into the exports struct published at the global gScrnExports = 0x80025C88. Thereafter it is invoked purely through gScrnExports->fn(...) by rendering/scene modules: logo.c func @0040013C calls gScrnExports->unk4(0,0) (set up a plain full screen for the boot logo), intro.c calls gScrnExports->unk4(6, &viewportDesc) (set up a screen bound to a camera/viewport for the intro), and scene.c func_scene_00401254 iterates the screen array via gScrnExports+0x18. Runs across boot/logo, intro/attract, and in-race scene states — wherever a screen/viewport must be created or updated.

**Engine exports used:** `gScrnExports (0x80025C88 — this module IS the publisher/owner of this table)`, `uvUpdateFileAllocPtr (module registration, not an export table)`

**Letterbox/viewport relevance:** Viewport/scissor-relevant (not letterbox specifically). scrn is the engine's SCREEN/VIEWPORT manager: it owns the array of active screen/render-target descriptors (gScrnExports+0x18 = ptr to array, +0x10 = s16 count) that scene.c walks to flag every live viewport, and its unk4 export (func_scrn_00400398) creates/configures a screen bound to a camera/viewport (intro passes a viewport descriptor, logo passes a plain full screen). This is exactly the layer where per-screen viewport rectangles and split-screen framebuffer regions are established, so it is the natural place viewport/scissor geometry originates — but no black-bar/pillarbox/letterbox drawing was observed in the (mostly unmatched) code, and letterbox proper lives in camera/letter/race code, not here. Cannot see the .s bodies to confirm explicit gSPViewport/gDPSetScissor calls; treat viewport-config relevance as likely, letterbox drawing as none.

**Readability:** Raw/low. All symbols are still func_scrn_XXXX and D_scrn_XXXX with no named struct for the exports table (callers access it as gScrnExports->unk4 and raw byte offsets +0x10/+0x18/+0x21C). Only 2 of 7 functions are matched, and both are trivial (one empty stub, one one-line reset); the 5 substantive functions are unmatched GLOBAL_ASM (bodies not checked in, so not directly readable). No comments, no typed Scrn_Exports struct. Purpose is inferred from name/address, the reloc-built vtable, the global-exports table position (between gSceneExports and gLightExports/gGlareExports), and the three cross-module callers (logo.c, intro.c, scene.c).

**Modification hooks:** To change screen/viewport behavior a dev works through gScrnExports (global 0x80025C88): (1) func_scrn_00400398 (the unk4 export) is where the active screen/viewport is created/configured — edit here (or intercept the gScrnExports->unk4 calls in logo.c:77 and intro.c:210) to alter viewport rectangle, count, or the mode arg (0=plain screen, 6=camera-bound). (2) The screen-descriptor array reached via gScrnExports+0x18 with count at +0x10 (walked in scene.c func_scene_00401254) is where per-screen viewport/framebuffer fields live; per-screen field +0x21C is set there. (3) D_scrn_004004C0 likely holds default viewport/screen constants — patch it to change default screen geometry. (4) D_scrn_004006DA (via func_scrn_004001DC) controls the active-screen count/reset. For a PC-port change, hooking the gScrnExports->unk4 setup call site is the cleanest lever since the module bodies are still asm.

**Key data symbols:**

- `D_scrn_004006C8` — Most-referenced scrn data symbol (rodata/bss, addr 0x4006C8) — likely the screen-manager state base or the screen-descriptor array/table header (referenced by nearly every function).
- `D_scrn_004006DA` — s16 counter/flag at 0x4006DA; zeroed by func_scrn_004001DC — probably the active-screen count or a 'screens dirty/reset' flag.
- `D_scrn_004006E8` — Data at 0x4006E8 referenced by entrypoint and func_004001E8 — a second screen-table pointer/parameter block.
- `D_scrn_004004C0` — Data at 0x4004C0 used by func_0040009C/00400398/00400474 — likely a config/default-viewport constant table used during screen setup.
- `B_scrn_004006C0` — BSS symbol at 0x4006C0 — module scratch/state buffer (BSS region backing the screen array/counters at 0x4006C8/0x4006DA/0x4006E8).

<details><summary>Functions (8)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_scrn_400000` | ⬜ asm | Overlay entrypoint: registers module via uvUpdateFileAllocPtr and fills the gScrnExports jump table with the 6 exported screen-manager functions (writes ptrs to func_00400474/094/09C/004001DC/0040027C/00400398 plus data ptrs D_004006C8/D_004006E8). |
| `func_scrn_00400094` | ✅ matched | Empty stub (no-op export slot; likely a per-frame/close/reset hook that BAR left unimplemented). |
| `func_scrn_0040009C` | ⬜ asm | Exported screen routine (largest body 0xE0 bytes); references D_004006C8/D_004004C0 — likely the core screen/viewport update or per-frame render-target processing. |
| `func_scrn_004001DC` | ✅ matched | Resets screen-manager state: sets the s16 counter/flag D_scrn_004006DA = 0 (e.g. clears active-screen count or a dirty flag). |
| `func_scrn_004001E8` | ⬜ asm | Exported screen routine referencing D_004006DA and D_004006E8; likely allocates/registers a new screen entry (increments the count reset by func_004001DC) or manages the screen descriptor table. |
| `func_scrn_0040027C` | ⬜ asm | Exported screen routine referencing D_004006C8; likely a screen accessor/query or teardown helper. |
| `func_scrn_00400398` | ⬜ asm | Exported screen-setup function reached as gScrnExports->unk4(mode, viewportDesc): configures/creates the active screen(s) — called by logo (mode 0), intro (mode 6 w/ camera-viewport desc), and race setup; references D_004006C8/D_004004C0. |
| `func_scrn_00400474` | ⬜ asm | First exported function pointer written by the entrypoint (exports offset ~0x0); an init/first-slot screen-manager routine referencing D_004006C8. |

</details>

**Gaps:** 5 of the 7 functions are unmatched (func_0040009C, 004001E8, 0040027C, 00400398, 00400474) with no checked-in .s, so exact viewport/scissor register writes (gSPViewport/gDPSetScissor) and the true meaning of the mode arg to unk4 cannot be confirmed from source. The exports struct is untyped: mapping of exported func -> struct offset is inferred from the entrypoint reloc write order (func_00400474 at ~0x0, then 094/09C/004001DC/0040027C/00400398) and the observed unk4 call, but not proven; the layout of the screen-descriptor struct (fields +0x10 count, +0x18 array, +0x21C per-screen flag) is only partially known from callers. Data symbols D_004006C8/E8/4004C0 have no defined types. Confirming whether scrn ever emits black bars/pillars requires reading the generated asm (build make; disassemble func_scrn_00400398/0040009C) or a runtime trace of gScrnExports->unk4.


---

### `battle` — Screen  (0/70 matched, confidence: medium)

**Role:** The "Beetle Battle" 4-player battle-mode front-end and in-game HUD/logic overlay: the code loaded for gameState BATTLE (multiplayer arena combat with mines/missiles, ladybug health pickups, mystery boxes, and a radar). One of the game's "uvmo" overlay modules (based at VRAM 0x400000), reached by pointer via its published export table rather than by C name.

**Invoked by:** Runs when gGameStateFlag / gGameSettings[0].gameStateFlag == BATTLE (=6, from enum gameState in include/game.h). Loaded as a relocatable "uvmo" overlay: config/us/beetleadventurerac.us.yaml defines segment `battle` (type uvmo, start 0x30638, follows_vram main) and modules.yaml lists it. The engine loads the overlay and calls its entrypoint (__entrypoint_func_battle_400000 @0x400000), which registers the module's function table into gBattleExports (0x80025CD8; zeroed at boot in src/game_init.c:190, populated when the overlay is resident). Thereafter the main game loop drives the module through that export table. Related battle gameplay knobs are exposed by the Cheats front-end in modules/selection.c (gCheatBattleNumLadybugs/LadybugColor/Health/Powerups/TimeLimit/Radar/Damage/MysteryBoxMode).

**Engine exports used:** `gBattleExports`

**Letterbox/viewport relevance:** None resolvable from the checked-in decomp. The battle module is a uvmo overlay whose 71 functions are all raw asm (no C bodies) and whose reloc table (battle_reloc_addrs.txt) only resolves intra-module func_battle_* targets — no named viewport/scissor/pillar/letterbox symbols appear. No gSPViewport/gDPSetScissor-equivalent engine export is symbolically referenced here. Battle mode is 4-player, so split-screen viewport/scissor setup very likely exists inside one of the unmatched functions (e.g. the draw/HUD cluster ~0x408000-0x409910), but it cannot be confirmed without the raw asm. Cinematic top/bottom letterbox bars are a race-camera feature (see race letterbox notes), not observed as owned by battle. Verdict: not confirmable; likely contains split-screen viewport/scissor clip setup but no evidence of black-bar/pillar letterboxing.

**Readability:** Raw. Every function is func_XXXXXXXX and every data symbol is D_battle_XXXXXXXX with no names, types, or comments. The symbol_addrs and reloc files carry only addresses. Only external context is named: the gameState enum (BATTLE=6, game.h), gBattleExports (global_exports.h / symbol_addrs.txt), and the gCheatBattle* knobs surfaced in selection.c. Zero functions matched; readability is at the lowest tier.

**Modification hooks:** Because all bodies are asm, PC-side edits must go through the recomp/host layer, not C source. Concrete handles: (1) gBattleExports @0x80025CD8 — the entry into battle-mode logic; hook here host-side to intercept or replace battle behavior. (2) The gCheatBattle* variables consumed in src/modules/selection.c (gCheatBattleNumLadybugs, gCheatBattleLadybugColor, gCheatBattleHealth, gCheatBattlePowerups, gCheatBattleTimeLimit, gCheatBattleRadar, gCheatBattleDamage, gCheatBattleMysteryBoxMode) — these are the exposed gameplay/tuning knobs and the natural place to add battle cheats/toggles (RDRAM pokes, per the cheats-menu pattern). (3) To relocate/hide/recolor the battle HUD you must first match/name one of the draw functions in the ~0x408000-0x409910 cluster (and the D_battle_0040B760+ data blobs) since layout constants live in raw data there. (4) Any split-screen viewport/scissor change would live inside the same draw cluster and requires disassembly to locate.

**Key data symbols:**

- `D_battle_00409A60 .. D_battle_0040A084 (dense cluster)` — Module .data/.bss globals — battle-mode runtime state, per-player arrays, timers, and small config words. All raw/unnamed.
- `D_battle_0040B760 / 0040BAC8 / 0040BE04 / 0040C2B4 / 0040C5C8 / 0040C5F0` — Larger data blobs (likely display-list fragments, sprite/texture tables, or HUD layout tables for the battle HUD). Raw/unnamed.
- `gBattleExports (0x80025CD8)` — Global pointer to this module's exported function table (the engine's handle to battle-mode entry points); zeroed at boot, populated when the overlay is resident. Not a D_battle_ symbol but the module's public interface.

<details><summary>Functions (70)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_battle_400000` | ⬜ asm | Overlay entrypoint at VRAM base 0x400000; registers the module's function table into gBattleExports and performs initial module setup when the battle overlay is loaded. |
| `func_battle_004006A4` | ⬜ asm | Early module init / per-instance setup helper (first exported routine after the entrypoint). |
| `func_battle_00400788` | ⬜ asm | Battle-mode state/data initialization (players, arena, powerups). |
| `func_battle_00400D38` | ⬜ asm | Small init/reset helper for a battle sub-object. |
| `func_battle_00400DB0` | ⬜ asm | Battle-state initialization or per-round reset routine. |
| `func_battle_00401318` | ⬜ asm | Battle sub-system setup helper. |
| `func_battle_004013D8` | ⬜ asm | Battle mode setup / large init routine (spawns players or arena objects). |
| `func_battle_00401A68` | ⬜ asm | Small helper (accessor or per-object update). |
| `func_battle_00401B34` | ⬜ asm | Small helper (accessor or per-object update). |
| `func_battle_00401BB8` | ⬜ asm | Battle logic update helper. |
| `func_battle_00401DB0` | ⬜ asm | Larger battle logic/update routine (per-frame gameplay step). |
| `func_battle_00402750` | ⬜ asm | Helper routine within battle update chain. |
| `func_battle_0040282C` | ⬜ asm | Battle logic routine (collision / powerup / pickup handling). |
| `func_battle_00402CD0` | ⬜ asm | Battle logic routine. |
| `func_battle_00402F6C` | ⬜ asm | Battle logic routine (weapon/mine/missile handling). |
| `func_battle_0040372C` | ⬜ asm | Battle logic routine. |
| `func_battle_00403A54` | ⬜ asm | Battle logic routine. |
| `func_battle_00403D08` | ⬜ asm | Battle logic routine (pickup/ladybug health handling). |
| `func_battle_00404074` | ⬜ asm | Battle logic helper. |
| `func_battle_0040425C` | ⬜ asm | Battle logic helper. |
| `func_battle_00404438` | ⬜ asm | Battle logic helper. |
| `func_battle_00404580` | ⬜ asm | Small helper (accessor / math). |
| `func_battle_00404694` | ⬜ asm | Battle logic routine. |
| `func_battle_004049C8` | ⬜ asm | Battle logic routine (larger; scoring or player-state update). |
| `func_battle_00404F18` | ⬜ asm | Small helper. |
| `func_battle_00404F9C` | ⬜ asm | Small helper. |
| `func_battle_00405034` | ⬜ asm | Battle helper (per-player state). |
| `func_battle_004050D4` | ⬜ asm | Battle logic routine. |
| `func_battle_00405538` | ⬜ asm | Battle logic helper. |
| `func_battle_00405764` | ⬜ asm | Battle logic helper. |
| `func_battle_00405A1C` | ⬜ asm | Small helper. |
| `func_battle_00405ADC` | ⬜ asm | Battle logic helper. |
| `func_battle_00405C3C` | ⬜ asm | Battle logic helper. |
| `func_battle_00405D34` | ⬜ asm | Battle logic helper. |
| `func_battle_00405E68` | ⬜ asm | Battle logic routine. |
| `func_battle_00406170` | ⬜ asm | Battle logic routine. |
| `func_battle_00406344` | ⬜ asm | Battle logic routine. |
| `func_battle_0040652C` | ⬜ asm | Battle logic routine. |
| `func_battle_00406784` | ⬜ asm | Small helper. |
| `func_battle_00406804` | ⬜ asm | Small helper. |
| `func_battle_00406900` | ⬜ asm | Small helper. |
| `func_battle_00406954` | ⬜ asm | Battle logic helper. |
| `func_battle_00406AD0` | ⬜ asm | Battle logic routine. |
| `func_battle_00406C84` | ⬜ asm | Battle logic routine. |
| `func_battle_00406E98` | ⬜ asm | Battle logic routine (larger). |
| `func_battle_00407254` | ⬜ asm | Battle logic helper. |
| `func_battle_0040733C` | ⬜ asm | Small helper. |
| `func_battle_004073BC` | ⬜ asm | Battle logic routine. |
| `func_battle_00407740` | ⬜ asm | Battle logic routine. |
| `func_battle_004078EC` | ⬜ asm | Battle logic helper. |
| `func_battle_00407A68` | ⬜ asm | Battle logic routine. |
| `func_battle_00407C58` | ⬜ asm | Battle logic helper. |
| `func_battle_00407DC4` | ⬜ asm | Small helper. |
| `func_battle_00407E78` | ⬜ asm | Small helper. |
| `func_battle_00407F30` | ⬜ asm | Small helper. |
| `func_battle_00407FF4` | ⬜ asm | Small helper. |
| `func_battle_0040807C` | ⬜ asm | Battle logic routine (HUD/element draw candidate). |
| `func_battle_004081D8` | ⬜ asm | Battle logic/draw routine (HUD element). |
| `func_battle_00408454` | ⬜ asm | Battle draw/HUD routine (radar or score display candidate). |
| `func_battle_004085F8` | ⬜ asm | Battle draw/HUD routine. |
| `func_battle_00408858` | ⬜ asm | Battle draw/HUD routine (larger). |
| `func_battle_00408B70` | ⬜ asm | Battle draw/HUD routine. |
| `func_battle_00408DC8` | ⬜ asm | Battle draw/HUD routine (larger; likely per-player HUD panel). |
| `func_battle_004090F0` | ⬜ asm | Small draw/HUD helper. |
| `func_battle_00409144` | ⬜ asm | Battle draw/HUD routine. |
| `func_battle_00409314` | ⬜ asm | Battle draw/HUD helper. |
| `func_battle_00409410` | ⬜ asm | Battle draw/HUD routine (larger). |
| `func_battle_0040969C` | ⬜ asm | Battle draw/HUD routine. |
| `func_battle_00409838` | ⬜ asm | Small helper (near end of module). |
| `func_battle_00409910` | ⬜ asm | Final routine in the module (cleanup/teardown or last HUD helper). |

</details>

**Gaps:** All 71 functions (1 entrypoint + 70 bodies) are unmatched raw asm — no C bodies are checked in, so exact purposes, arguments, and which engine export tables (gUvGui/gUvFont/gUvSprt/gUvBlit/gSnd) are actually called cannot be verified from source; the reloc table resolves only intra-module targets. Open items: (a) confirm split-screen viewport/scissor setup and whether any letterbox/pillar clipping exists — requires disassembling the draw cluster; (b) identify which funcs are logic vs HUD-draw (the update-vs-draw split above is inferred from address ordering/size heuristics, not verified); (c) name the D_battle_0040B760+ data blobs (display lists vs sprite/layout tables); (d) map the gBattleExports table layout (member offsets/signatures). To fully resolve, disassemble the module (asm/ is generated by make) and set watchpoints during a live BATTLE session.


---

### `cbars` — Screen  (3/5 matched, confidence: medium)

**Role:** Tiny standalone "uv" engine overlay module whose name denotes color bars — an NTSC/SMPTE color-bar test/diagnostic pattern screen. It is a self-contained overlay (entrypoint + init/tick stubs + one asm draw/update routine driven by two jump tables). It is a TEST-PATTERN screen, not gameplay/cinematic bars.

**Invoked by:** Loaded as a named uvmo overlay (module index "cbars" in config/us/form0.json module table and beetleadventurerac.us.yaml segment @ROM 0x4DE98, VRAM base 0x400000). Like every uvmo it is entered through __entrypoint_func_cbars_400000, which the engine's module loader calls after the overlay is DMA'd in; the entrypoint wires up the per-module init/tick vector (func_004000E4 / func_004000EC / func_004000F4). No other decomp module calls any func_cbars_* symbol directly (grep of lib/bar-decomp shows zero external callers), so it is dispatched purely by the generic module/state machinery, most plausibly a developer/diagnostic path (color-bar test screen) rather than a normal menu or race state. It is not referenced anywhere in the beetle-adventure-racing-recomp PC port (src/) or docs/ — effectively dormant in the running game.

**Letterbox/viewport relevance:** None visible at the C level, and NOT gameplay/cinematic letterbox. "cbars" = color-bars test pattern (a full-screen NTSC/SMPTE-style stepped color test image), which is the opposite of black bars: it fills the frame with colored vertical/horizontal bands. It does not draw a black top/bottom cinematic bar (that is the race camera's letterbox in ASM, per prior findings), nor a pillarbox/viewport/scissor clip (that is RT64/engine viewport code). The two jump tables and geometry constants could in principle set a scissor/viewport for the pattern region, but there is no C evidence of gSPViewport/gDPSetScissor use here and no engine export table is referenced. Treat as "none" for the letterbox/pillarbox investigation; the confusingly similar name is a decoy — the real letterbox lives elsewhere (race camera ASM), not in cbars.

**Readability:** Mostly raw. Only 3 of 5 functions are "matched," and those are empty stubs or a one-line forwarder — none are named, typed, or commented (all still func_cbars_XXXXXXXX). The core routine func_cbars_00400114 and the entrypoint are unmatched raw asm (asm/ not checked in, so bodies unreadable). All data is still D_cbars_XXXX / B_cbars_XXXX with no struct typing. No engine export member names are surfaced. Effectively unreadable/undocumented beyond the address-name level.

**Modification hooks:** To change this element a dev would work in lib/bar-decomp/src/modules/cbars.c: (1) func_cbars_00400114 is the single place that draws/updates the bars — recoloring, repositioning, resizing, or disabling the pattern all happen by decompiling and editing this function. (2) The bar colors/geometry are the data words D_cbars_00400510/530/534/538/558/55C/560 (addresses in linker_scripts/us/module_files/cbars_symbol_addrs.txt); patch these to recolor/resize bands. (3) The two jtbls D_cbars_00400514 / D_cbars_0040053C select which sub-pattern/mode draws — editing the switch changes bar layout/mode. (4) To hide the screen entirely, no-op func_cbars_004000F4 (it currently just calls 00400114) or remove the module from the module table (config/us/form0.json / beetleadventurerac.us.yaml). Runtime BSS state lives at B_cbars_00400580. Note this module is not invoked by the PC port, so edits here have no effect on normal beetle-adventure-racing-recomp gameplay unless the diagnostic screen is actually reached.

**Key data symbols:**

- `__entrypoint_func_cbars_400000` — Module entrypoint address (0x400000), stored back as MIPS_32 reloc at rom 0x5C4 (self-referencing header).
- `D_cbars_00400514` — Jump table (type:jtbl) — case targets .Lcbars_004001E4..00400244; a 7-entry switch inside func_00400114 (a state/mode selector).
- `D_cbars_0040053C` — Second jump table (type:jtbl) — case targets .Lcbars_00400328..00400388; another switch branch inside func_00400114.
- `D_cbars_00400510` — Data word referenced HI/LO at rom 0x1C0/0x1C4 — a config/table pointer or constant used by the draw routine (likely color/coord table base).
- `D_cbars_00400530 / _00400534 / _00400538` — Small data words (constants or per-bar parameters: colors, widths, or coordinates) referenced by func_00400114.
- `D_cbars_00400558 / _0040055C / _00400560` — Additional data-word parameters referenced by the draw routine (further color/geometry constants).
- `D_cbars_00400570` — Module data word (0x400570) — likely a state/counter or table tail.
- `B_cbars_00400580` — BSS block at 0x400580 — the module's zero-init runtime state (current mode/frame counter/scratch for the bars).

<details><summary>Functions (5)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_cbars_400000` | ⬜ asm | Module entrypoint/header thunk at VRAM base 0x400000; the uvmo loader jumps here to register this module's init/tick/draw callbacks (references func_004000E4/EC/F4 via relocs at rom 0x64-0x78). |
| `func_cbars_004000E4` | ✅ matched | Empty stub — one of the module's lifecycle callbacks (e.g. init or load hook); currently a no-op body {} in this build. |
| `func_cbars_004000EC` | ✅ matched | Empty stub — second lifecycle callback (e.g. shutdown/unload hook); no-op body {}. |
| `func_cbars_004000F4` | ✅ matched | Per-frame tick/draw entry: simply forwards to func_cbars_00400114 (the real color-bar draw/update routine). |
| `func_cbars_00400114` | ⬜ asm | The substantive routine — the actual color-bars draw/state update. ~124 raw-asm lines; uses two jump tables (D_00400514 / D_0040053C jtbl) implying a small state/mode switch, and reads/writes the module's D_00400510/530/534/538/558/55C/560 params plus BSS B_00400580. Best inference: renders the color-bar test pattern (bands / stepped colors) and advances its state each frame. |

</details>

**Gaps:** The real behavior is unverified: func_cbars_00400114 and the entrypoint are unmatched raw asm (not checked in), so the exact rendering — whether it emits Gfx display-list bar quads, blit calls, or configures any viewport/scissor — cannot be confirmed from source. The two jump tables prove a small state machine but its states are unknown. No caller/state that actually triggers cbars was found (zero references in bar-decomp modules and zero in the recomp src/), so the exact game state / menu path that shows the color bars is unidentified — likely a leftover Paradigm dev/diagnostic test screen. To fully resolve: decompile func_cbars_00400114 (or watchpoint the module load) to confirm it's a color-bar test pattern and to rule in/out any scissor/viewport setup.


---

### `logo` — Screen  (4/4 matched, confidence: high)

**Role:** Boot-logo overlay module: loads a sprite bank ('UVBT') at startup and draws a single centered logo sprite that fades in via a sine-eased alpha ramp over 90 frames. One of the pre-menu boot-splash screens (Nintendo / EA / Paradigm-style logo).

**Invoked by:** Loaded as a Paradigm "uvmo" overlay module. Registered in config/us/modules.yaml, form0.json (module table index for "logo"), and beetleadventurerac.us.yaml (segment "logo", type uvmo, start 0x5F3B8). Engine calls __entrypoint_func_logo_400000 (VRAM 0x400000, symbol_addrs 0x400000) once at module load; the engine's module dispatcher then invokes the three registered export slots (func_logo_00400174/0040017C/00400184) each frame while the logo screen is the active game state. This is a pre-menu boot screen (per project memory, pak-prompt -> Nintendo -> VW logo sequence; logo rendering also involves sibling module "splash"). The three exports are stored into the caller-supplied LogoModule_Exports table; they are not called from anywhere else inside the decomp (module public API consumed by the engine host).

**Engine exports used:** `gUvSprtExports`, `gUvGfxStateExports`, `gUvGfxMgrExports`, `gUvMathExports`, `gGameGuiExports`, `gScrnExports`

**Letterbox/viewport relevance:** No letterbox/black-bar/scissor/pillar code. The only viewport-adjacent call is gUvGfxMgrExports->unk58(0, width, 0, height) in the entrypoint, which sets the sprite draw region to the FULL screen (unk84()=width, unk88()=height) — i.e. it does the opposite of clipping; it opens the region to full extent. No gDPSetScissor equivalent, no cinematic top/bottom bars, no pillarbox. gUvGfxStateExports->unkC/unk10 set render/blend modes for the sprite, not clip rectangles. Letterbox relevance: none.

**Readability:** Partially readable. All 4 functions are byte-matched C bodies (no GLOBAL_ASM — 0 unmatched). Structs (LogoModule_Exports) and file/math engine calls (uvLoadFile, uvUpdateFileAllocPtr, sin via gUvMathExports->unk8) are named/typed. However export-table members remain raw opaque slots (unk4/unk10/unk1C/unk50/unk54/unk58/unk84/unk88/unkC), functions keep func_logo_00400184-style names, and data symbols are raw D_logo_*. The magic constants (0x04800000, 0x600000, sprite param list) are uncommented. Fade math is clear; blit/gfx-state semantics need the uv engine header member names to be fully legible.

**Modification hooks:** To change fade-in duration/curve: func_logo_00400184, the 0x5A (90-frame) threshold and the sin ramp expression (line 92-95). To change logo brightness/tint: the gUvSprtExports->unk1C RGB=var_a2 args in func_logo_00400184 (line 100). To swap the logo image: the uvLoadFile('UVBT',0x28) tag/id and the sprite-param unk1C call in the entrypoint (lines 58-65). To reposition/resize the draw region (e.g. to add manual bars or reposition the logo): gUvGfxMgrExports->unk58(0, width, 0, height) in the entrypoint (lines 67-72). To hide the logo entirely: no-op func_logo_00400184 or skip the module in the boot-state machine (see project memory: screen-advance dwell ~319 frames is driven elsewhere, not in this module).

**Key data symbols:**

- `D_logo_004002E8` — 2-int runtime state array. [0] = sprite handle returned by gUvSprtExports->unk4() (the loaded 'UVBT' logo sprite id); [1] = frame counter for the fade-in ramp (advances until 0x5A=90, then alpha held at 0xFF).
- `D_logo_004002E0` — 2-int data (declared extern, likely padding/alignment or unused constant block adjacent to the state array).
- `D_logo_004002D0` — Data array (declared extern, size unspecified) — likely sprite/layout constants in the module's rodata; not directly referenced in the matched C body.

<details><summary>Functions (4)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_logo_400000` | ✅ matched | Module init/entrypoint: sets the file alloc ptr, registers the 3 export slots, allocates a sprite handle (gUvSprtExports->unk4), uvLoadFile('UVBT',0x28) loads the logo sprite bank, configures sprite params (unk1C) and the gfx-manager draw region to full screen (unk58 with unk84/unk88 = width/height), sets game-gui state (unk10(0)/unk1C), and calls gScrnExports->unk4(0,0). |
| `func_logo_00400174` | ✅ matched | Empty export slot (no-op) — module init/enter or tick hook that does nothing for the logo screen. |
| `func_logo_0040017C` | ✅ matched | Empty export slot (no-op) — second module hook (e.g. update/input), unused for the logo screen. |
| `func_logo_00400184` | ✅ matched | Per-frame draw: for the first 90 frames computes alpha = sin(frame*(pi/2)/90)*255 (sine ease-in fade), then clamps to 0xFF; sets gfx state (unk50 open, unkC(0x04800000)/unk10(0x600000) render modes), tints the sprite RGB=alpha via gUvSprtExports->unk1C, blits it with unk10, then closes gfx state (unk54). |

</details>

**Gaps:** Fully matched (no asm work needed). Remaining gaps are naming/semantics: (1) export-table member offsets (unk4/unk10/unk1C/unk50/unk54/unk58/etc.) are still opaque — need the uv engine export-struct headers (uvsprt_rom.h, uvgfxstate_rom.h, uvgfxmgr) to name them (unk4=allocSprite, unk10=drawSprite, unk1C=setSpriteParams/tint, unk50/unk54=gfx state push/pop, unk58=setViewport region). (2) The magic render-mode constants 0x04800000 / 0x600000 are un-decoded (likely G_RM_* blend/cycle modes). (3) D_logo_004002D0/004002E0 contents unverified against rodata. (4) Which game state value selects this module (the boot-state index) is not defined here — the dwell/advance logic lives outside logo.c (project memory notes it is still unpinned). (5) Sprite param list (9,0x28,2,...,0xFF,0) fields are positional/undocumented.


---

### `results` — Screen  (13/31 matched, confidence: medium)

**Role:** Post-race results / standings overlay module: builds and drives the end-of-race results screen (finishing positions, times, point standings), plays result stingers/speech, and hands control back to the menu/replay flow.

**Invoked by:** A uvmo engine overlay (config: type uvmo, start 0x9F4A0, results VRAM base 0x00400000). Entered through the module's __entrypoint (__entrypoint_func_results_400000.s) when the uv engine loads/activates this overlay for the results game state; it is reached at the end of a race. Screen flow is driven by gGameSettings (structs.h): func_results_00401390 advances the state machine by setting gGameSettings->gameStateFlag = 5 (with introReplayState gating) to leave the results screen. func_results_004064C4 updates high-score/standings fields living at D_8002CD98+0x258..0x264 (host RAM state, likely the persistent standings block). No decomp caller references results' functions by name — like all uvmo overlays it is dispatched dynamically via the entrypoint and engine export tables, not via direct extern calls (grep of the rest of src found no callers/data references).

**Engine exports used:** `gSndExports`, `gUvEmitterExports`

**Letterbox/viewport relevance:** None visible in the matched C. This module contains no black-bar, pillarbox, viewport, or scissor/scissor-clip code in any of its readable (matched) functions — the matched bodies are sound/emitter init, state-machine exit, table lookups, and standings/speech bookkeeping. Any 2D scissor or full-screen background it may set would live in the unmatched drawing routines (func_results_00400AC8 / _00401B00 / _00402B9C / _00403284 / _00404474 / _00404D20 / _00405608 and the 0x40xxxx draw helpers), whose asm is not checked in, so it cannot be confirmed. Not a letterbox/cinematic-bars source of interest (that is the race/scene/letter/cbars modules, per project notes).

**Readability:** Partially readable. ~13 of ~30 functions are matched C, but ALL are named func_results_<addr> / D_results_<addr> (no semantic names, types, or comments). The heavy drawing/layout routines are still raw GLOBAL_ASM. Data tables are typed only minimally (s16[], s32[4][6]) with inferred meanings. Overall: skeleton matched, but low readability — no descriptive naming yet.

**Modification hooks:** To exit/advance the screen: func_results_00401390 (sets gGameSettings->gameStateFlag=5). To retune position/points values shown: edit the lookup tables D_results_00407508[4][6] (func_results_00406550), D_results_00407568[8] (func_results_00406528), D_results_004074F8 (func_results_004002A8). To change announcer/result sounds: the sample descriptors D_results_00407720..748 and the id constants (0xEB/0xEC/0xCF/0xF4/0xF5) in func_results_004066E8/00406710/00406738/004067CC/004067F4, plus gSndExports call sites. To change celebration particle effects: emitter ids D_results_00407750/751 in func_results_004001B8. To alter high-score/standings persistence: func_results_004064C4 and the D_8002CD98 offsets. Moving/hiding/recoloring the actual on-screen results text and sprites requires decompiling the unmatched draw routines first — no C hook exists for row positions/colors/visibility yet.

**Key data symbols:**

- `D_results_004074F8[]` — Config/data table indexed by func_results_004002A8 (per-entry results parameter).
- `D_results_00407508[4][6]` — 4x6 lookup table (func_results_00406550) — likely per-place x per-column values (e.g. points/positions).
- `D_results_00407568[]` — 8-entry lookup table (func_results_00406528) — e.g. per-finishing-position value.
- `D_results_004075A8/AC/BC/C0` — One-shot state flags paired with the four speech/SFX trigger wrappers (func_results_0040659C targets).
- `D_results_004075B8` — One-shot guard flag for the announcer speech sample in func_results_00406738.
- `D_results_00407718` — Object/exports pointer whose +0x3C method is called with 0x64 (func_results_00405DE8).
- `D_results_00407720/728/730/738/740/748` — Six sound-sample descriptors registered at init (gSndExports+0x70); 0x748 is also the announcer sample.
- `D_results_00407750/00407751` — Two particle-emitter ids configured at init via gUvEmitterExports+0x5C (celebration/confetti-style effects).
- `D_8002CD98 (+0x258..0x264)` — External (non-module) standings/high-score block updated by func_results_004064C4.
- `D_8002CB9A` — External flag cleared when the 0x25C standings field is beaten.
- `gOptionsSpeechVol` — Global speech-volume option gating announcer playback.

<details><summary>Functions (31)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_results_400000` | ⬜ asm | Overlay entrypoint / dispatch table the uv engine calls to run the results screen (init/update/draw dispatch). |
| `func_results_004001B8` | ✅ matched | Init/preload: registers 6 sound samples (D_results_00407720..748) via gSndExports+0x70 and configures 2 particle emitters (D_results_00407750/751) via gUvEmitterExports+0x5C. |
| `func_results_004002A8` | ✅ matched | Small accessor: returns D_results_004074F8[a0] (indexed lookup into a results data/config table). |
| `func_results_004002BC` | ⬜ asm | Results screen logic (likely setup/update of the standings display); body raw asm. |
| `func_results_00400AC8` | ⬜ asm | Large results routine — probably the main per-frame update/build of the results layout; body raw asm. |
| `func_results_00401390` | ✅ matched | State-machine exit: if introReplayState==0 sets it to 1 and gGameSettings->gameStateFlag=5 to leave the results screen. |
| `func_results_004013BC` | ⬜ asm | Results logic/update routine; body raw asm. |
| `func_results_00401AF8` | ✅ matched | Empty stub (no-op). |
| `func_results_00401B00` | ⬜ asm | Large results routine (layout/animation/draw of standings rows); body raw asm. |
| `func_results_00402B9C` | ⬜ asm | Results routine (drawing or scoring logic); body raw asm. |
| `func_results_00403284` | ⬜ asm | Results routine (drawing/animation); body raw asm. |
| `func_results_00404474` | ⬜ asm | Results routine (drawing/text/sprite build); body raw asm. |
| `func_results_00404D20` | ⬜ asm | Results routine (drawing/build); body raw asm. |
| `func_results_00405608` | ⬜ asm | Results routine (drawing/build); body raw asm. |
| `func_results_00405DE8` | ✅ matched | Calls (*(D_results_00407718+0x3C))(0x64) — invokes a method on an object/exports pointer with arg 100 (likely set volume/progress/timer). |
| `func_results_00405E18` | ⬜ asm | Results routine (draw/update helper); body raw asm. |
| `func_results_00405ED8` | ⬜ asm | Results routine (draw/update helper); body raw asm. |
| `func_results_00405F94` | ⬜ asm | Results routine (draw/update helper); body raw asm. |
| `func_results_004060F0` | ⬜ asm | Results routine (draw/update helper); body raw asm. |
| `func_results_00406184` | ⬜ asm | Results routine (draw/update helper); body raw asm. |
| `func_results_0040620C` | ⬜ asm | Results routine (draw/update helper); body raw asm. |
| `func_results_00406338` | ⬜ asm | Results routine (draw/update helper); body raw asm. |
| `func_results_004064C4` | ✅ matched | Records/updates 4 max values (arg0..3) into the standings block at D_8002CD98+0x258/0x25C/0x260/0x264, clearing flag D_8002CB9A when the 0x25C field is beaten — high-score/best-standings bookkeeping. |
| `func_results_00406528` | ✅ matched | Clamped lookup: returns D_results_00407568[a0] with a0 clamped to [0,7] (8-entry table, e.g. per-position value). |
| `func_results_00406550` | ✅ matched | Clamped 2D lookup: returns D_results_00407508[a0][a1] with a0 in [0,3], a1 in [0,5] (4x6 table, e.g. per-place/per-column values). |
| `func_results_0040659C` | ⬜ asm | Shared sound/voice trigger helper taking (id, &state) — used by the speech/stinger wrappers below; body raw asm. |
| `func_results_004066E8` | ✅ matched | Speech/SFX trigger wrapper: func_results_0040659C(0xEB, &D_results_004075A8). |
| `func_results_00406710` | ✅ matched | Speech/SFX trigger wrapper: func_results_0040659C(0xEC, &D_results_004075AC). |
| `func_results_00406738` | ✅ matched | Plays a result speech/announcer sample (D_results_00407748, id 0xCF) at gOptionsSpeechVol via gSndExports (set-vol +0x44, pitch +0x4C, play +0x8), guarded by D_results_004075B8 one-shot flag. |
| `func_results_004067CC` | ✅ matched | Speech/SFX trigger wrapper: func_results_0040659C(0xF4, &D_results_004075BC). |
| `func_results_004067F4` | ✅ matched | Speech/SFX trigger wrapper: func_results_0040659C(0xF5, &D_results_004075C0). |

</details>

**Gaps:** The core presentation is unmatched: the main update/build and all draw/layout/animation routines (func_results_00400AC8, _00401B00, _00402B9C, _00403284, _00404474, _00404D20, _00405608, _00405E18, _00405ED8, _00405F94, _004060F0, _00406184, _0040620C, _00406338, _004002BC, _004013BC, _0040659C, and __entrypoint) are raw asm not checked in, so exact HUD-element positions, colors, viewport/scissor usage, and which uv font/sprite/gui export tables are used for drawing are unknown. gUvGuiExports/gUvFontExports/gUvSprtExports usage is presumed but unconfirmed in C. No named callers were found in the decomp because uvmo overlays are dispatched via the entrypoint/export tables; the precise gameStateFlag value that selects the results overlay (vs the =5 it exits to) and the identity of the D_8002CD98 standings struct and D_results_00407718 object are not pinned down.


---

### `splash` — Screen  (1/5 matched, confidence: medium)

**Role:** Boot-time splash / logo screen module (Nintendo + VW/publisher logos) that also drives the Controller-Pak (Memory Pak) prompt screens shown before the main menu; loads the 'ripl' ripple-effect submodule and its UVTX textures and publishes its function table via gSplashExports (0x80025CC0).

**Invoked by:** Overlay module registered in modules.yaml/form0.json (form index alongside sparks/spray). Loaded via uvLoadModule('spla') by the game/state driver during the pre-menu boot sequence. Per prior investigation (boot-logo-skip-findings), the pre-menu logos + Controller-Pak prompt screens all run under game state 14 (gGameStateFlag / gGameSettings->gameStateFlag == 14), and the splash module was pinned as the renderer of those logos (func_splash_0040020C @0x8420020C found via an lldb watchpoint on the logo alpha at 0x80025A90). The entrypoint __entrypoint_func_splash_40063c is the module init/constructor that fills gSplashExports so the state machine can call in.

**Engine exports used:** `gSplashExports (this module's own published table @0x80025CC0)`

**Letterbox/viewport relevance:** None. The splash module contains no viewport, scissor, pillarbox, or black-bar setup. Its only resource activity is loading/unloading the 'ripl' ripple submodule and UVTX textures 0x79/0x7A and drawing full-screen logo/prompt bitmaps with a fade. Any black background around the logos is a full-screen clear, not a letterbox/pillar region. Not a candidate for widescreen/letterbox toggle work.

**Readability:** Mostly raw. 3 of 4 functions plus the entrypoint are still #pragma GLOBAL_ASM (unmatched, no C body). Only func_splash_004005F8 is matched and readable (and even it is unnamed func_/uses raw 'ripl'/UVTX tags). No struct types on D_splash_00400A08, no field names, no comments. gSplashExports member layout is undefined. Understanding the pre-menu logo/Controller-Pak flow requires reading the raw asm.

**Modification hooks:** To skip/hide logos or the Controller-Pak prompt: intercept at the state-14 driver (gGameStateFlag==14) that invokes gSplashExports, or short-circuit func_splash_0040020C's phase advance (this is the pre-menu fast-boot skip path noted in bar-perf-audio-findings/boot-logo-skip). To change logo timing/fade: the constants at D_splash_004008F0..0040090C and the alpha field near 0x80025A90 (logo alpha watchpoint) are the fade/duration knobs; the ~319-frame screen dwell is still UNPINNED (clocks 0x80025D6C and 0x80246640 were tested as decoys and did nothing — the real screen-advance write has not been found). To change which textures/effect: the 'ripl' tag and UVTX ids 0x79/0x7A in func_splash_004005F8 (and their matching load in func_splash_00400000). To move/recolor logos: the placement/color constants live in D_splash_00400968/D_splash_00400910.. tables (raw asm — requires decomp first).

**Key data symbols:**

- `D_splash_00400A08` — Primary splash state/data blob, size 0x2A8; referenced by every function (init, update, teardown). Holds the screen's runtime state (current phase, timers, logo/prompt selection, alpha/fade).
- `D_splash_00400968` — Secondary data table referenced by init and update (HI/LO relocs at multiple sites) — likely a small pointer/constant table (texture or logo descriptor list).
- `D_splash_004008F0 / 004008F4 / 004008F8 / 00400900 / 00400908 / 0040090C` — Word-sized constants (float or config) consumed one-per-call-site by 00400000/0040020C — likely fade rates, timer thresholds, or coordinates for logo placement.
- `D_splash_00400910..00400944` — Contiguous run of 15 word-sized entries (0x910,914,918,91C,920,924,928,92C,930,934,938,93C,940,944) referenced by func_splash_00400828 — a jump/phase table or per-logo parameter array.
- `D_splash_00400950 / B_splash_00400960 / D_splash_00400CB0` — Additional data/bss words (00400960 is .bss scratch); 00400CB0 referenced by init+teardown, likely a loaded-module/texture handle cache.

<details><summary>Functions (5)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_splash_40063c` | ⬜ asm | Module entrypoint/constructor: run on module load, populates the gSplashExports (0x80025CC0) function-pointer table with this module's init/draw/cleanup routines (00400000/0040020C/004005F8/00400828), so the game state driver can invoke the splash screen. |
| `func_splash_00400000` | ⬜ asm | Splash screen init/setup: likely loads the 'ripl' ripple submodule + UVTX textures 0x79/0x7A, initializes the state blob D_splash_00400A08 and logo alpha/fade fields (paired teardown is 004005F8). |
| `func_splash_0040020C` | ⬜ asm | Main per-frame splash update/draw: advances logo/prompt phase, animates the fade/ripple, and renders the current logo or Controller-Pak prompt (identified in prior work as the logo-drawing routine, watched via logo alpha @0x80025A90). |
| `func_splash_004005F8` | ✅ matched | Splash teardown/cleanup: uvUnloadModule('ripl') and uvUnloadFile('UVTX',0x79)/(0x7A) — frees the ripple submodule and its two textures when the splash screen exits. |
| `func_splash_00400828` | ⬜ asm | Small helper (196 bytes, 4 data refs) — likely per-phase advance / input poll (advance-on-button) or a table-driven state selector using D_splash_00400910..00400944; exact role unconfirmed without asm. |

</details>

**Gaps:** 3 of 4 functions plus the entrypoint are unmatched (raw asm not checked in), so exact draw/positioning code, the gSplashExports member layout, and the Controller-Pak prompt logic are inferred, not read. Key open item carried from prior investigation: the ~319-frame pre-menu screen-dwell timer is unpinned (0x80025D6C and 0x80246640 are confirmed decoys); pinning it needs a watchpoint on the actual screen-advance write, which likely lives inside func_splash_0040020C or func_splash_00400828. The D_splash_00400910..00400944 table's purpose (jump table vs. per-logo params) and the size/fields of D_splash_00400A08 (0x2A8) are unconfirmed without the asm.


---

### `flag` — HUD  (4/13 matched, confidence: medium)

**Role:** A `uvmo`-type in-world animated object overlay (Paradigm uv engine) that spawns and drives the waving race/checkered flag model. It binds a uv animation channel and registers a per-frame animation callback; the model list is walked and updated each frame, and torn down on unload.

**Invoked by:** Loaded as a `uvmo` overlay segment (config/us/beetleadventurerac.us.yaml: name=flag, type=uvmo, start=0x54CA0, follows_vram main; modules.yaml lists it as module "flag"). Entered through its module entrypoint (`__entrypoint_func_flag_400000.s`, wrapped by MODULE_ENTRY_POINT in include/module.h), which dispatches lifecycle/message codes (load/init/update/unload) the same way sibling uvmo overlays (spray, skid, ripple, caranim) are driven by the engine/level+race code. It is not called by name from any other decomp source (grep found references only inside flag.c) — invocation is via the overlay dispatch table, so it runs while racing when the flag object is active (start line / checkered finish / marshal flag).

**Engine exports used:** `gUvChanExports`, `gUvCbckExports`

**Letterbox/viewport relevance:** none. flag.c contains no viewport (gSPViewport), scissor (gDPSetScissor), pillarbox, or black-bar/letterbox code. It is a uv animation-channel driven world model object; it neither draws nor configures any clipping/bars. (The cinematic race letterbox is camera/ASM driven elsewhere, not here.)

**Readability:** Mostly raw. Only 4 of 13 functions are matched, and two of those are empty stubs; the substantive logic (init, per-object update func_flag_004008F4, animation callback func_flag_00401198, teardown func_flag_00400D54, entrypoint) is still unmatched GLOBAL_ASM. All functions are still func_flag_XXXX and the sole data symbol is D_flag_00401728. The export tables are opaque (UnkCbckExports/UnkChanExports with pad + unkNN members). No descriptive names, types, or comments — needs asm work to name the flag struct fields (esp. +0x48 next-ptr) and the channel/callback semantics.

**Modification hooks:** Because the flag is a 3D uv model object (not a 2D positioned sprite), a dev would not move it via simple x/y constants in this file. Concrete hooks: (1) To disable/hide the flag entirely, no-op the callback registration in func_flag_004012FC (skip the gUvCbckExports->unk10 call) or make func_flag_00400D10 early-return (skip the list walk / per-frame update). (2) To change its animation, alter the callback id 0x3D or the channel index 6 in func_flag_004012FC, or edit the (unmatched) animation callback func_flag_00401198. (3) Object placement/scale/model selection lives in the unmatched init path (func_flag_00400080 / func_flag_00400520 / func_flag_004012FC) and, more fundamentally, in the flag's uv model asset and the level/race code that spawns it — not in tunable constants here. Color/texture (checkered pattern) is baked into the uv model asset, not set in this file.

**Key data symbols:**

- `D_flag_00401728` — Head pointer of the linked list of active flag object instances; each node's next pointer is at offset +0x48. Walked by func_flag_00400D10 for per-frame updates.

<details><summary>Functions (13)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_flag_400000` | ⬜ asm | Module entrypoint/dispatch — routes engine lifecycle+message codes (load/init/update/unload) to the module's handlers. |
| `func_flag_00400078` | ✅ matched | Empty stub (no-op handler / unused lifecycle slot). |
| `func_flag_00400080` | ⬜ asm | Init/constructor — likely allocates the flag object instance and links it into the D_flag_00401728 list. |
| `func_flag_004004A4` | ⬜ asm | Helper (init/spawn or per-object setup); called during construction. |
| `func_flag_00400520` | ⬜ asm | Object setup — configures the flag model/animation channel (mesh, transform, or channel binding). |
| `func_flag_004006B4` | ✅ matched | Empty stub (no-op handler / unused lifecycle slot). |
| `func_flag_004006BC` | ⬜ asm | Per-object update or spawn routine for a single flag instance. |
| `func_flag_004008F4` | ⬜ asm | Per-instance per-frame update (called on each node during the list walk in func_flag_00400D10) — advances/draws the flag. |
| `func_flag_00400D10` | ✅ matched | Update-all: walks the linked list of flag objects starting at D_flag_00401728, calling func_flag_004008F4 on each (next ptr at +0x48). |
| `func_flag_00400D54` | ⬜ asm | Teardown/free or another list-walk handler (cleanup counterpart to the update walk). |
| `func_flag_00401198` | ⬜ asm | Animation callback — registered with gUvCbckExports->unk10 (id 0x3D); invoked by the uv engine each animation tick to update the flag's animated channel. |
| `func_flag_004012FC` | ✅ matched | Registers the animation callback: binds channel 6 via gUvChanExports->unk4, then registers func_flag_00401198 as the callback via gUvCbckExports->unk10 with id 0x3D. |
| `func_flag_00401354` | ⬜ asm | Entry/lifecycle handler (init or the routine that calls func_flag_004012FC to arm the callback). |

</details>

**Gaps:** 9 of 13 functions are unmatched raw asm (asm/ not checked in), so exact behavior is inferred from the identical sibling uvmo pattern (spray.c: same UnkChan/UnkCbck exports, same channel-6 + callback-register shape) plus the config's uvmo type and the linked-list walk. Unconfirmed: the flag struct layout (only the +0x48 next pointer is known), the meaning of channel index 6 and callback id 0x3D, the exact set of lifecycle message codes the entrypoint dispatches, and precisely which in-game moment(s) spawn the flag (start line vs. checkered finish vs. marshal/caution flag). Whether "flag" is a visible waving cloth model vs. a start-light element could not be pinned from source alone — the uvmo type + animation-channel callback strongly favors an animated 3D flag mesh. Export tables remain opaque (unk4/unk10 unnamed). No caller outside the module, so the exact overlay-dispatch entry is not directly visible in decomp C.


---

### `gamegui` — HUD  (26/38 matched, confidence: medium)

**Role:** In-race HUD overlay drawing plus a large bank of one-line button/callback handlers that drive the game state machine (gGameStateFlag and gGameSettings[0].gameStateFlag) — race start modes, track/cup selection, debug-menu toggles, and audio/emitter debug hooks.

**Invoked by:** Loaded as a BAR overlay module via its __entrypoint_func_gamegui_400000 (registered in config/us/modules.yaml). The draw/update functions (func_gamegui_004000B0, _00400194, _00400370, _004014F8, _00401748, _00401A7C) run during in-race/HUD game states off the module dispatch table. The dozens of s32-arg setters (func_gamegui_00401E24..00402180, _0040203C, _0040219C) are GUI button/menu callbacks invoked when the player activates a menu widget — each writes a new gameStateFlag to advance the state machine (e.g. 5=1P race variant, 6=2P, 9=confirm, 8/0xB/0xA/0xC/0xD/0xE/0xF debug/aux screens, 2/3/4/7 race/track setup). func_gamegui_004023B0 is a getter (returns the D_gamegui_00402C68 debug-HUD flag). Debug setters gate on D_debugEnable.

**Engine exports used:** `gUvGuiExports (unkC widget draw, unk28 mode config)`, `gUvFontExports (uvFontGenDList — emits HUD text display list)`, `gSndExports (unk34(u16), unk38(s32) — HUD/menu sound cues)`, `gUvCmidiExports (unk2C — MIDI/music debug)`, `gUvEmitterExports (unk80 — particle emitter debug)`, `gUvAudiomgrExports (unk40 — audio-manager debug)`, `gUvDebugExports (unk18 — debug overlay mode)`

**Letterbox/viewport relevance:** None found in the matched C. This module draws HUD text/sprites through the uv font/GUI export tables (uvFontGenDList, gUvGuiExports) and mutates game-state flags; it contains no gSPViewport / gDPSetScissor / pillar / black-bar / letterbox configuration. The race cinematic letterbox lives in the race/camera ASM (per project memory), not here. Caveat: several large functions (func_gamegui_00400370, _004014F8, _00401748, _004021DC, _004023BC) are still raw asm — their bodies were not readable, so a HUD-local scissor clip cannot be 100% ruled out, but nothing in the visible data symbols or matched code suggests viewport/clip setup.

**Readability:** Mixed. The small button-handler/setter/getter functions (~25) are MATCHED and readable, and touch named GameSettings fields (gameStateFlag, numPlayers, numAiCars, currentTrack, introReplayState, currentGameState) — good semantic clarity. However every function name is still raw func_gamegui_<addr>, no member of the export structs is named (all typed as opaque unkNN via local GameguiXxxExports shims defined at top of file), and the HUD-draw core (func_gamegui_004000B0/_00400194/_00400370/_004014F8/_00401748/_004021DC/_004023BC) is UNMATCHED raw asm. All ~200 D_gamegui_* data symbols are unnamed addresses. So: state-machine logic is legible; the actual HUD rendering and all data tables are not.

**Modification hooks:** To change HUD text/sprite POSITION or LAYOUT: edit the currently-unmatched draw routines func_gamegui_00400370 / _004014F8 / _00401748 (and their data tables in the 0x402450–0x402C60 block) — these must be decompiled first. To SHOW/HIDE the debug HUD overlay: func_gamegui_00401AD8 (sets D_gamegui_00402C68) and the branch in func_gamegui_00401A7C that selects gUvGuiExports->unkC vs uvFontGenDList; the getter is func_gamegui_004023B0. To retarget which STATE/SCREEN a menu button jumps to: edit the corresponding one-line setter (e.g. func_gamegui_00402074 for state 8, _004020DC for the currentTrack=0x1B start, _00402110 for track 0x21). To change AI-car/player defaults on race start: func_gamegui_00401E24 / _00401E50. Debug-only actions (MIDI/emitter/audio) are func_gamegui_00401D94/_00401DC4/_00401DF4 and are all gated at large by D_debugEnable elsewhere. Colors are not set in matched code — they would live in the unmatched draw funcs / data tables.

**Key data symbols:**

- `D_gamegui_00402C68` — Debug-HUD/font-mode flag (u8): when nonzero, func_gamegui_00401A7C routes to the GUI export path instead of normal font display list; toggled by func_gamegui_00401AD8 (debug-gated), read by func_gamegui_004023B0.
- `D_gamegui_00402C78` — GUI/font context object passed by address to gUvGuiExports->unkC and ->unk28 (the debug/HUD widget context).
- `D_gamegui_00402D04` — Selection/index state (s32) set by func_gamegui_00402030.
- `D_gamegui_00402D16` — u16 state value (declared extern; used in unmatched paths).
- `D_gamegui_00402D28` — Reference value compared against arg0 in func_gamegui_0040203C to decide the confirm result written to gGameSettings padE0[0].
- `D_gamegui_00402450..00402C60 (large block)` — Read-only data tables: HUD/menu string tables, sprite/glyph coordinate tables, and callback/widget descriptor tables (per gamegui_symbol_addrs.txt). B_gamegui_00402C60 is the module's BSS base.

<details><summary>Functions (38)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_gamegui_400000` | ⬜ asm | Module entrypoint/dispatch — registers this overlay's draw+callback functions with the engine module table. |
| `func_gamegui_004000A8` | ✅ matched | Empty stub (no-op init/teardown hook). |
| `func_gamegui_004000B0` | ⬜ asm | Likely main HUD init or per-frame update entry (large; sets up HUD elements/sprites). |
| `func_gamegui_00400194` | ⬜ asm | HUD draw/update helper (renders speed/lap/pos/timer elements). |
| `func_gamegui_00400370` | ⬜ asm | HUD draw/layout routine (large body; positions and blits HUD text/sprites). |
| `func_gamegui_004014F8` | ⬜ asm | HUD element render/update (secondary HUD panel or per-player readout). |
| `func_gamegui_00401748` | ⬜ asm | HUD render/update routine (additional HUD components). |
| `func_gamegui_00401A7C` | ✅ matched | HUD/debug-font flush: if debug flag D_gamegui_00402C68 set, calls gUvGuiExports->unkC(&D_gamegui_00402C78); else emits the font display list via gUvFontExports->uvFontGenDList(). |
| `func_gamegui_00401AD8` | ✅ matched | Setter: if D_debugEnable, store arg0 into debug-HUD flag D_gamegui_00402C68 (toggles debug overlay). |
| `func_gamegui_00401AF8` | ⬜ asm | HUD/debug helper (string or value formatting for on-screen debug readout). |
| `func_gamegui_00401D34` | ⬜ asm | HUD/menu helper (draws or updates a widget/panel). |
| `func_gamegui_00401D94` | ✅ matched | Debug hook: invokes gUvCmidiExports->unk2C() (MIDI/music debug action). |
| `func_gamegui_00401DC4` | ✅ matched | Debug hook: invokes gUvEmitterExports->unk80() (particle emitter debug action). |
| `func_gamegui_00401DF4` | ✅ matched | Debug hook: invokes gUvAudiomgrExports->unk40() (audio-manager debug action). |
| `func_gamegui_00401E24` | ✅ matched | Button handler: start race variant — if numPlayers>=2 zero numAiCars, set gameStateFlag=5. |
| `func_gamegui_00401E50` | ✅ matched | Button handler: 2-player race — force numPlayers>=2, set gameStateFlag=6. |
| `func_gamegui_00401E7C` | ⬜ asm | Button/menu handler (state or settings mutation; body not visible). |
| `func_gamegui_00401FE0` | ✅ matched | Debug action: gUvGuiExports->unk28(&D_gamegui_00402C78,-1,3) then gUvDebugExports->unk18(3) (configures debug GUI/overlay mode 3). |
| `func_gamegui_00402030` | ✅ matched | Setter: store arg0 into D_gamegui_00402D04 (selection/index state). |
| `func_gamegui_0040203C` | ✅ matched | Confirm handler: compare arg0 to D_gamegui_00402D28, write 1/0 into gGameSettings[0].padE0[0], set gameStateFlag=9. |
| `func_gamegui_00402074` | ✅ matched | Button handler: set global gGameStateFlag=8. |
| `func_gamegui_00402088` | ✅ matched | Button handler: set global gGameStateFlag=0xB. |
| `func_gamegui_0040209C` | ✅ matched | Button handler: gameStateFlag=3, unkC=2 (menu/screen transition). |
| `func_gamegui_004020BC` | ✅ matched | Button handler: gameStateFlag=4, unkC=2 (menu/screen transition). |
| `func_gamegui_004020DC` | ✅ matched | Button handler: gameStateFlag=2, currentTrack=0x1B, finishedIntroCount=0, unkC=1, pad178[5]=1 (start a specific track/intro). |
| `func_gamegui_00402110` | ✅ matched | Button handler: gameStateFlag=7, currentTrack=0x21 (start another specific track). |
| `func_gamegui_00402130` | ✅ matched | Button handler: set global gGameStateFlag=0xA. |
| `func_gamegui_00402144` | ✅ matched | Button handler: set global gGameStateFlag=0xC. |
| `func_gamegui_00402158` | ✅ matched | Button handler: set global gGameStateFlag=0xD. |
| `func_gamegui_0040216C` | ✅ matched | Button handler: set global gGameStateFlag=0xE. |
| `func_gamegui_00402180` | ✅ matched | Button handler: set global gGameStateFlag=0xF. |
| `func_gamegui_00402194` | ✅ matched | Empty stub (no-op callback). |
| `func_gamegui_0040219C` | ✅ matched | Toggle handler: flips gGameSettings[0].introReplayState; when enabling, copies currentGameState into gameStateFlag (intro/replay pause toggle). |
| `func_gamegui_004021D4` | ✅ matched | Empty stub (no-op callback). |
| `func_gamegui_004021DC` | ⬜ asm | HUD/menu handler (larger body; layout or list-building routine). |
| `func_gamegui_00402358` | ⬜ asm | HUD/menu helper (body not visible). |
| `func_gamegui_004023B0` | ✅ matched | Getter: returns debug-HUD flag D_gamegui_00402C68. |
| `func_gamegui_004023BC` | ⬜ asm | HUD/menu routine (final function; body not visible). |

</details>

**Gaps:** 9 functions remain UNMATCHED raw asm, including the entrypoint and essentially all of the actual HUD-drawing code (func_gamegui_004000B0, _00400194, _00400370, _004014F8, _00401748, _00401AF8, _00401D34, _004021DC, _00402358, _004023BC). Their bodies are not checked in (asm/ is generated), so the exact HUD elements drawn (speed/lap/pos/timer), their coordinates, colors, and any HUD-local scissor clip cannot be confirmed. The ~200 D_gamegui_* data symbols are unnamed — the string/sprite/callback tables are unidentified. Export struct members are all opaque unkNN (unk34/unk38 on gSndExports declared but not observed being called in the matched code). To fully understand: decompile the HUD-draw functions and label the 0x402450–0x402C60 data block; confirm which module-dispatch game states invoke each draw vs callback function.


---

### `plyr` — HUD  (8/25 matched, confidence: medium)

**Role:** Per-player entity/model-object ("uvmo") overlay: manages the array of active player structs (up to ~4 for split-screen), per-player update/tick, player-id lookup, per-player sound registration/playback, and a gDebugHudState-gated per-player debug HUD readout.

**Invoked by:** Loaded as a uvmo (uv model-object) overlay module — config/us/beetleadventurerac.us.yaml (name: plyr, type: uvmo, start 0x8E5B8, follows main) and modules.yaml. Entered via __entrypoint_func_plyr_400000 (the standard uvmo dispatch/vtable stub), which the uv engine's model-object update loop calls. func_plyr_00402220/00402268 are the per-frame update entry points (call into D_plyr_00406B1C vtable + child module D_plyr_00406B20). Active during gameplay/race states (per-player logic); no game-state global is referenced directly except the debug-HUD gate gDebugHudState and mode selector D_80025D78. Called only from within its own module in the decomp (engine-dispatched, not statically referenced by siblings).

**Engine exports used:** `gSndExports`

**Letterbox/viewport relevance:** None visible. No matched function configures a viewport, scissor rectangle, pillar, or black bar. The only split-screen-adjacent signal is the player-count logic (D_plyr_0040690C, the `<3` branch, and the D_80025D78 mode gate in func_plyr_00402268), which selects update paths by player count/mode — but any actual split-screen viewport/scissor setup (if it exists here) would live in the unmatched update routines (e.g. 00402340, 00402B90) and cannot be confirmed without the asm. Do not assume it draws bars; nothing in matched code does.

**Readability:** Raw. All symbols are func_plyr_XXXXXXXX / D_plyr_XXXXXXXX with no names, real types, or comments. Structs are opaque placeholders (char pad[..] + a single function pointer). 8 of 25 functions are matched (mostly small accessors/dispatchers); the large per-player logic and both debug-HUD draw routines are still GLOBAL_ASM unmatched. Field meanings (+0x14 sound idx, +0x18 player id) are inferred from matched accessors only.

**Modification hooks:** To change per-player behavior a dev edits: (1) player count / split-screen cap — the `< 3` comparison and D_plyr_0040690C in func_plyr_00402268 and the loop bound in func_plyr_004022E0; (2) the update-path mode gate — the D_80025D78 == 0/3 test in func_plyr_00402268; (3) debug-HUD visibility/layout — gDebugHudState bit tests in func_plyr_00405624 and the (unmatched) draw routines func_plyr_00405688 / func_plyr_00405C1C; (4) per-player sound assignment — D_plyr_00406B48 table + the +0x14 index in func_plyr_00406604, and the 8 handles registered in func_plyr_00400590. There is no viewport/scissor knob to edit in matched code.

**Key data symbols:**

- `D_plyr_0040690C` — Active player count (loop bound in 004022E0; the <3 branch in 00402268 implies max ~4, i.e. split-screen player count).
- `D_plyr_00406B28[]` — Array of pointers to per-player structs; each struct has an id at +0x18 (looked up by 004022E0) and a sound-index at +0x14 (used by 00406604).
- `D_plyr_00406B1C` — Pointer to a vtable/object (+0x30 unk30, +0x34) for the player module's own update methods.
- `D_plyr_00406B20` — Pointer to a child module/object; its +0x24 method takes an s32 (invoked each frame from 00402220).
- `D_plyr_00406B48[]` — Per-player sound-handle table (8-byte stride), indexed in 00406604.
- `D_plyr_00406B68..00406BA0` — Eight sound-handle/descriptor blocks registered with gSndExports+0x70 in 00400590.
- `gDebugHudState` — Global bitmask gating the per-player debug HUD (bit1=panel A, bit2=panel B).
- `D_80025D78` — Global mode/state selector (values 0/3 enable the low-player-count update path in 00402268).

<details><summary>Functions (25)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_plyr_400000` | ⬜ asm | uvmo module entrypoint / vtable-dispatch stub the uv engine calls to run this overlay. |
| `func_plyr_00400590` | ✅ matched | Registers/initializes 8 per-player sound handles (D_plyr_00406B68..BA0) via gSndExports+0x70. |
| `func_plyr_00400678` | ⬜ asm | Player subsystem init (likely allocates/populates the D_plyr_00406B28 player-struct array; paired with 00400590). |
| `func_plyr_00400C3C` | ⬜ asm | Per-player setup/spawn or large init routine (size suggests full player-object construction). |
| `func_plyr_00401484` | ⬜ asm | Per-player update/physics or state-machine step. |
| `func_plyr_00401DC4` | ⬜ asm | Per-player update/logic (secondary tick or camera/follow update). |
| `func_plyr_00402220` | ✅ matched | Per-frame update: invokes vtable D_plyr_00406B1C->unk30() then child module D_plyr_00406B20->unk24(arg). |
| `func_plyr_00402268` | ✅ matched | Per-frame update variant: calls vtable+0x34; if player count<3 and mode(D_80025D78) is 0/3, runs func_plyr_00402340. |
| `func_plyr_004022CC` | ✅ matched | Accessor: returns D_plyr_00406B28[index] (raw player-array slot). |
| `func_plyr_004022E0` | ✅ matched | Finds player-array index whose struct field +0x18 (player id) equals arg0; returns -1 if none. Loops over D_plyr_0040690C active players. |
| `func_plyr_00402338` | ✅ matched | Empty stub (no-op). |
| `func_plyr_00402340` | ⬜ asm | Player update/draw routine gated by mode in 00402268 (single/low-player-count path). |
| `func_plyr_00402B90` | ⬜ asm | Player logic/update (large; possibly input->movement or camera application). |
| `func_plyr_00402F14` | ⬜ asm | Player logic/update routine. |
| `func_plyr_0040397C` | ⬜ asm | Player logic/update routine. |
| `func_plyr_00403B40` | ⬜ asm | Player logic/update routine. |
| `func_plyr_00404088` | ⬜ asm | Player logic/update routine. |
| `func_plyr_004044F8` | ⬜ asm | Player logic/update routine. |
| `func_plyr_004047C8` | ⬜ asm | Player logic/update routine. |
| `func_plyr_00404CE0` | ⬜ asm | Player logic/update routine. |
| `func_plyr_004050AC` | ⬜ asm | Player logic/update routine. |
| `func_plyr_00405624` | ✅ matched | Debug-HUD dispatcher: if gDebugHudState bit1 set draw part A (00405688), if bit2 set draw part B (00405C1C). |
| `func_plyr_00405688` | ⬜ asm | Draws first per-player debug HUD panel/readout (text/values). |
| `func_plyr_00405C1C` | ⬜ asm | Draws second per-player debug HUD panel/readout. |
| `func_plyr_00406604` | ✅ matched | Plays a per-player sound: gSndExports->unk70(D_plyr_00406B48 + arg->[+0x14]*8) — indexes a sound-handle table by a field in the passed struct. |

</details>

**Gaps:** 17 of 25 functions are unmatched (raw asm not checked in, so bodies unreadable): the entrypoint dispatcher, all the heavy per-player update/logic routines (00400678, 00400C3C, 00401484, 00401DC4, 00402340, 00402B90, 00402F14, 0040397C, 00403B40, 00404088, 004044F8, 004047C8, 00404CE0, 004050AC), and both debug-HUD draw routines (00405688, 00405C1C). Confirming whether plyr sets split-screen viewports/scissor (the hinted role) requires the asm for 00402340/00402B90/etc. — matched code only proves player-array management, id lookup, and sound. Struct layouts for the player object and the two vtable/child objects are unknown beyond the few offsets used by matched accessors. Category could arguably be 'Other'/entity module rather than HUD since it is a uvmo model-object, not a sprite HUD; classified HUD due to the per-player debug-HUD draw path.


---

### `menuslct` — Menu  (3/18 matched, confidence: medium)

**Role:** Reusable menu-select list widget: manages a list model (count + item array of 0x14-byte item structs), tracks/queries the currently-selected item, and draws the list/highlight-bar UI. Self-contained uvmo overlay with no exports in the engine docking bay; dispatched via its own entrypoint jump table.

**Invoked by:** Loaded as a relocatable UVMO overlay (config type: uvmo, ROM start 0x674A8, vram base 0x400000). __entrypoint_func_menuslct_400000 calls uvUpdateFileAllocPtr(exports) and populates a jump table of the module's functions (reloc table at rom 0x64-0x104 lists all 17 funcs; entrypoint MIPS_32 self-ref at rom 0x16D0). None of its functions are referenced by any other decomp source file (grep of src/ hits only menuslct.c) and there is no gMenuslctExports pointer in the 80025BD8-80025CE8 docking-bay table, so it is a private/leaf widget invoked internally by its own entrypoint-installed table (likely used by menu-screen modules such as gamegui/selection at menu game states). Exact external caller is unconfirmed from C alone.

**Engine exports used:** `uvUpdateFileAllocPtr (module bootstrap in entrypoint)`

**Letterbox/viewport relevance:** None. No black-bar, pillarbox, viewport, or scissor configuration found. The only confirmed draw call (func_menuslct_004013F8 -> func_menuslct_00400BCC) renders a small screen-space highlight rectangle (x=22,y=225,w=298,h=16) — a menu selection bar, not a letterbox/pillar. All other draw routines are raw asm but the module's role (list widget) and constants are inconsistent with viewport/scissor/black-bar setup. Cannot 100% rule out an internal gDPSetScissor inside the unmatched draw funcs, but no evidence supports it.

**Readability:** Mostly raw. 4 of 18 functions are matched C (func_menuslct_004000F0 no-op, func_menuslct_004009FC selected-index query, func_menuslct_004013F8 highlight-bar wrapper, plus the trivial forward decls); the other 14 are #pragma GLOBAL_ASM (unmatched). Nothing is renamed/typed/commented: all identifiers remain func_menuslct_XXXX / D_menuslct_XXXX / B_menuslct_XXXX with no struct types for the 0x14-byte item or list-header. The item-struct layout (count@+0x0, itemArrayPtr@+0x8, per-item flag@+0x2, stride 0x14) is inferable only from the one matched scan function.

**Modification hooks:** To move/resize the selection highlight bar: edit the literal args in func_menuslct_004013F8 (menuslct.c lines 75-77): x=0x16, y=0xE1, w=0x12A, h=0x10 passed to func_menuslct_00400BCC. To change highlight color/style: trace/patch arg0 forwarded to func_menuslct_00400BCC (the color/style channel), or the fill color inside the still-raw func_menuslct_00400BCC. Item selection logic (which row is 'current') is governed by the per-item flag at item+0x2 read in func_menuslct_004009FC; list count is header+0x0 and item array ptr is header+0x8, stride 0x14. Additional layout/label constants live in rodata D_menuslct_00401670/00401674 (coordinate-like pairs) and D_menuslct_00401678 (likely string/table). Recoloring/repositioning other list elements requires first matching the raw draw funcs (00400E48/004010C4/004011B0/0040126C/00401320).

**Key data symbols:**

- `D_menuslct_00401660` — Rodata word (referenced rom 0x1A4/0x1A8); likely a constant/pointer used by an early draw func.
- `D_menuslct_00401664` — Rodata word (rom 0x28C/0x290); constant used by list/draw routine.
- `D_menuslct_00401668` — Rodata word (rom 0x504/0x508); constant used mid-module.
- `D_menuslct_0040166C` — Rodata word (rom 0x560/0x564); constant.
- `D_menuslct_00401670` — Rodata constant referenced repeatedly by func_menuslct_004010C4/004011B0 region (rom 0x1138,0x1174,0x11B0...); likely a shared draw parameter (e.g. base coordinate/color).
- `D_menuslct_00401674` — Rodata constant paired with 401670 across several draw sites; likely companion draw parameter (coordinate/stride).
- `D_menuslct_00401678` — Rodata constant referenced by func_menuslct_0040126C/00401320 (rom 0x12BC,0x12E4,0x131C,0x1358); likely a string/format or draw-table pointer.
- `B_menuslct_00401690` — BSS symbol (0x401690): module-private uninitialized state (likely the widget/list runtime state block or cursor globals).

<details><summary>Functions (18)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_menuslct_400000` | ⬜ asm | Module entrypoint: calls uvUpdateFileAllocPtr(exports) and fills the exports jump table with this module's function pointers (standard uvmo bootstrap). |
| `func_menuslct_004000F0` | ✅ matched | Empty/no-op stub (referenced by entrypoint reloc rom 0x64/0x80); placeholder or default handler. |
| `func_menuslct_004000F8` | ⬜ asm | Likely list/widget init or per-frame update entry (early function, referenced from entrypoint table). |
| `func_menuslct_00400330` | ⬜ asm | List/cursor state routine (exported via entrypoint table); probable update or navigation handler. |
| `func_menuslct_0040043C` | ⬜ asm | List/cursor state routine (exported); probable input/selection movement helper. |
| `func_menuslct_0040056C` | ⬜ asm | Exported list helper; purpose unclear from C (raw asm). |
| `func_menuslct_0040062C` | ⬜ asm | Exported list helper; purpose unclear from C (raw asm). |
| `func_menuslct_004009FC` | ✅ matched | Returns index of the currently-selected item: reads count at header+0x0, item-array ptr at header+0x8, scans items of stride 0x14 for flag+0x2==1; returns index or -1 if none. |
| `func_menuslct_00400A5C` | ⬜ asm | Exported list helper (near the selected-index query); likely set-selected / move-cursor counterpart. |
| `func_menuslct_00400B14` | ⬜ asm | Exported list helper; likely item-state or cursor mutation. |
| `func_menuslct_00400BCC` | ⬜ asm | Draw helper taking (x,y,w,h, and 4 more args); called by func_menuslct_004013F8 to draw a screen-space rectangle (highlight/selection bar). Core UI draw primitive of the widget. |
| `func_menuslct_00400E48` | ⬜ asm | Exported draw/update routine (referenced from entrypoint table rom 0x68/0x84). |
| `func_menuslct_004010C4` | ⬜ asm | Exported draw/update routine (entrypoint table rom 0x70/0x8C). |
| `func_menuslct_004011B0` | ⬜ asm | Exported draw/update routine (entrypoint table rom 0x78/0x94). |
| `func_menuslct_0040126C` | ⬜ asm | Exported routine referencing rodata D_menuslct data (rom 0xB8/0xE0); likely a draw or table-driven handler. |
| `func_menuslct_00401320` | ⬜ asm | Exported routine referencing D_menuslct_00401678 rodata; likely draw/label handler. |
| `func_menuslct_004013F8` | ✅ matched | Thin wrapper that draws the selection highlight bar: calls func_menuslct_00400BCC(0x16,0xE1,0x12A,0x10,0,0,0,arg0) i.e. rect at x=22,y=225,w=298,h=16 (a horizontal highlight strip); arg0 is likely a color/style/index param. |
| `func_menuslct_0040143C` | ⬜ asm | Last exported function (entrypoint table rom 0xD0/0xF8); purpose unclear from C. |

</details>

**Gaps:** 14 of 18 functions are unmatched (raw asm not checked in), so their exact behavior is inferred, not read. Unknowns: (1) the full 0x14-byte item struct beyond flag@+0x2, and the list-header struct beyond count@+0x0 / itemArrayPtr@+0x8; (2) the meaning of func_menuslct_00400BCC's 8 args beyond the x/y/w/h rectangle (last 4 args, incl. color/texture); (3) who actually calls this module (no C caller found — dispatch is via the engine-installed entrypoint jump table, and there is no gMenuslctExports docking-bay pointer, so the invoking screen module is unconfirmed); (4) whether any raw draw func issues an internal scissor/viewport (assessed unlikely but not asm-verified). Resolving these needs the generated .s or matching the remaining functions.


---

### `pause` — Menu  (29/55 matched, confidence: medium)

**Role:** In-game pause / options overlay module for BAR: builds the pause menu tree (resume, restart, quit, and audio/display/gameplay options), drives audio-volume live-preview via gSndExports, and routes exit choices back into the game state machine (race/battle/menu). It is a separate 'menu'-engine-backed overlay, not a HUD.

**Invoked by:** Runs as an overlay dispatched by the game state machine. func_pause_00400164 unloads the 'menu' engine module (uvUnloadModule(0x6D656E75)) on teardown, and D_pause_00405E44 is the loaded menu/gui module instance whose vtable (unk34, +0x8, +0xC) drives widget updates. Exit actions write gGameStateFlag / gGameSettings+0x8 (gameStateFlag): func_pause_00400D40 sets flag 5 when 'race' (0x72616365) is loaded or 6 when 'batl' (0x6261746C) is loaded; func_pause_00400DA8, func_pause_00402630, func_pause_004026A8 set flag 0xE (14 = menu/quit-to-menu). Menu-item callbacks are invoked indirectly through a function-pointer table indexed off D_pause_00405D48 in func_pause_004041A0 (button-press dispatch). No cross-module C caller — entry is via the overlay entrypoint __entrypoint_func_pause_400000 (unmatched) and function-pointer tables in D_pause_* menu-node data.

**Engine exports used:** `gSndExports (audio: volume set +0x3C/+0x40/+0x44, cue start +0xDC, SFX trigger +0xE0, one-shot play +0x8, timebase +0x4C, stereo/mono unkF4, handle free +0x70)`, `gReplayExports (unkC: enable/disable replay recording)`, `gUvGui/menu-engine exports via D_pause_00405E44 vtable (unk34 slider-register, +0x8 widget-state, +0xC toggle-state) and the 'menu' module (uvUnloadModule 0x6D656E75)`, `uvGetLoadedModule / uvUnloadModule (module.c) to detect race/batl/menu overlays`

**Letterbox/viewport relevance:** No black-bar / letterbox / scissor drawing here. The only viewport-adjacent thing is func_pause_00404160, which zeroes two gGameSettings fields at +0x6F98 and +0x6F96 (plausibly widescreen/aspect or a display-related setting) before requesting a transition — but this module does not set up gSPViewport/gDPSetScissor, pillar widths, or cinematic bars. It is a menu-logic/audio module; all rendering/clipping is delegated to the 'menu' engine module via D_pause_00405E44. Treat as effectively none for letterbox purposes (verify +0x6F96/+0x6F98 meaning against gamegui/gGameSettings struct if a widescreen toggle is being traced).

**Readability:** Partially readable. ~30 of the ~54 functions have matched C bodies but ALL remain named func_pause_<addr> with no comments; struct types are mostly ad-hoc inline casts or opaque pad-based typedefs (UnkStruct_pause_E44/E48/SndExports/ReplayExports). Data symbols are all raw D_pause_<addr> except a few shared globals (gGameStateFlag, gGameSettings, gOptions*). Menu-node struct is a bare {s32,void*,void*} guess. Roughly half the functions are still #pragma GLOBAL_ASM (unmatched, no C). Overall: logic is legible but nothing is properly named/typed/documented.

**Modification hooks:** To change pause behavior/appearance: (1) Exit/quit routing — edit func_pause_00400D40 (resume-state values 5/6 and resume flags), func_pause_00400DA8 / func_pause_00402630 / func_pause_004026A8 (quit-to-menu, gGameStateFlag/gGameSettings+0x8 = 0xE). (2) Menu page/layout — the static MenuNode data D_pause_00405618/_00405AA0/_004055BC/_00405950 and the page table D_pause_00405BEC (referenced in func_pause_004001F0) define which entries appear and where; func_pause_00400E48 tweaks a mode-dependent entry's item count/style (offsets 0x16/0x18/0x2A). (3) Option ranges/behavior — func_pause_00402268/22B0/22F8 (volume slider step counts = the '8' arg), func_pause_00402340/2380/23A4/2440 (toggles), func_pause_004023C8 (map cycle). (4) Audio feedback — func_pause_0040092C / func_pause_00403F60 / func_pause_00404574 (SFX/music cue ids 0xDB, 0xF6, 0xE). (5) Aspect/display fields — func_pause_00404160 zeroes gGameSettings+0x6F96/+0x6F98; edit here to preserve widescreen on pause. Visual position/color/scissor is NOT here — it lives in the 'menu' engine module reached through D_pause_00405E44.

**Key data symbols:**

- `D_pause_00405E44` — Pointer to the loaded 'menu'/gui engine module instance; its vtable (unk34 = slider register, +0x8 = get-widget-state, +0xC = get-toggle-state) drives all pause widgets.
- `D_pause_00405E48` — Pointer to a transition/sequence controller struct; unk8(s32) requests a menu transition (used by func_pause_00404160).
- `D_pause_00405E4C` — Current active menu-page descriptor (set from D_pause_00405BEC table in func_pause_004001F0).
- `D_pause_00405E50` — Current active menu root node (MenuNode*, from descriptor+0x10); passed to widget calls.
- `D_pause_00405BEC` — Table of menu-page descriptor pointers indexed by page id in func_pause_004001F0.
- `D_pause_00405E58` — 6-byte array of per-widget dirty/edge-handled latches (set in func_pause_004001F0, used as button edge-latch in func_pause_004041A0).
- `D_pause_00405E60 / _00405E68 / _00405E70` — Three sound handles owned by the pause module (music/sfx/nav-cue); allocated by the menu, freed in func_pause_00400164, used for nav SFX in func_pause_00403F60.
- `D_pause_00405D48` — Base of the menu-item callback function-pointer table indexed in func_pause_004041A0 to invoke the selected action.
- `D_pause_00405D44` — One-shot latch for playing the navigation SFX (func_pause_00403F60).
- `D_pause_00405D3C / _00405D40` — Resume flags set when returning to a race (func_pause_00400D40).
- `D_pause_00405D34 / _00405D38` — Menu selection cursor / timer (reset on page enter, f32 0.5 default).
- `D_pause_00405DF4` — Previous-frame widget/toggle state used for edge detection in func_pause_00400428.
- `D_pause_004054B4` — Root of a nested gui-node structure whose leaf widget fields (offsets 0x16/0x18/0x2A) are configured in func_pause_00400E48.
- `D_pause_00405618 / _00405AA0 / _004055BC / _00405950` — Static MenuNode records — the individual pause menu page/entry node data.

<details><summary>Functions (55)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_pause_400000` | ⬜ asm | Module entrypoint/relocation stub; overlay load entry that wires up the pause module. |
| `func_pause_00400164` | ✅ matched | Teardown: unloads the 'menu' (0x6D656E75) engine module and releases three sound handles (D_pause_00405E60/68/70) via gSndExports+0x70. |
| `func_pause_004001F0` | ✅ matched | Selects a menu page: sets active menu descriptor D_pause_00405E4C/root node D_pause_00405E50 from table D_pause_00405BEC[arg0], applies gameplay-mode tweaks and reinits audio, marks 6 dirty flags at D_pause_00405E58. |
| `func_pause_00400258` | ⬜ asm | Likely per-frame update/build of the pause menu (layout or navigation), given position among page-setup helpers. |
| `func_pause_004003B8` | ⬜ asm | Menu build/refresh helper (unknown exact role; sits between page setup and widget update). |
| `func_pause_00400428` | ✅ matched | Toggle/edge-detect helper for a boolean option: queries widget state via D_pause_00405E44+0xC and flips the byte at arg0 on a rising edge (used by stereo/mono, speed, display toggles). |
| `func_pause_0040048C` | ⬜ asm | Menu helper (unknown; option-related update). |
| `func_pause_00400544` | ⬜ asm | Menu helper (unknown; option/page-related update). |
| `func_pause_004006E4` | ✅ matched | Pushes current option volumes to audio engine: sets speech/sfx/music volume via gSndExports+0x44/0x40/0x3C. |
| `func_pause_00400750` | ⬜ asm | Menu/option update helper (unknown exact role). |
| `func_pause_0040092C` | ✅ matched | Enter-page audio setup: re-applies volumes, calls gSndExports+0xDC and +0xE0(0xDB) (start a menu SFX/music cue), resets D_pause_00405D34/D38 selection state. |
| `func_pause_0040098C` | ⬜ asm | Menu build/update helper (unknown exact role). |
| `func_pause_00400C6C` | ✅ matched | Empty stub (no-op callback). |
| `func_pause_00400C74` | ✅ matched | Replay toggle: calls gReplayExports->unkC(D_80025D76==0) — enable/disable replay recording based on a game flag. |
| `func_pause_00400CB0` | ⬜ asm | Menu callback (unknown; likely a button/action handler). |
| `func_pause_00400CE0` | ⬜ asm | Menu callback (unknown; likely a button/action handler). |
| `func_pause_00400D10` | ⬜ asm | Menu callback (unknown; likely a button/action handler). |
| `func_pause_00400D40` | ✅ matched | Resume/return-to-gameplay: if 'race' loaded sets gGameStateFlag=5 and resume flags D_pause_00405D3C/D40; if 'batl' loaded sets gGameStateFlag=6. |
| `func_pause_00400DA8` | ✅ matched | Quit-to-menu: sets gGameStateFlag=0xE (14). |
| `func_pause_00400DB8` | ⬜ asm | Menu update/build helper (unknown exact role). |
| `func_pause_00400E48` | ✅ matched | Configures a menu widget (indices 0x16/0x18/0x2A of a nested gui node reached via D_pause_004054B4) based on gameplay mode gGameSettings+0x6F74 and value at +0x6F64 — sets item count/style for a mode-dependent menu entry. |
| `func_pause_00400EEC` | ⬜ asm | Menu build/init helper (unknown; large function, likely constructs the pause menu widget tree). |
| `func_pause_0040197C` | ⬜ asm | Menu build/update helper (unknown; large function). |
| `func_pause_004020F0` | ⬜ asm | Menu callback/update helper (unknown). |
| `func_pause_004021BC` | ⬜ asm | Menu callback/update helper (unknown). |
| `func_pause_00402268` | ✅ matched | Speech-volume slider setup: registers gOptionsSpeechVol as an 8-step slider (channel 0) via D_pause_00405E44->unk34. |
| `func_pause_004022B0` | ✅ matched | SFX-volume slider setup: registers gOptionsSfxVol as an 8-step slider (channel 1). |
| `func_pause_004022F8` | ✅ matched | Music-volume slider setup: registers gOptionsMusicVol as an 8-step slider (channel 2). |
| `func_pause_00402340` | ✅ matched | Stereo/Mono toggle callback: flips gOptionsStereoMono via func_pause_00400428 then applies it via gSndExports->unkF4. |
| `func_pause_00402380` | ✅ matched | Game-speed toggle callback: flips gOptionsSpeed via func_pause_00400428. |
| `func_pause_004023A4` | ✅ matched | Toggle callback for option byte D_80025E6A via func_pause_00400428. |
| `func_pause_004023C8` | ✅ matched | Map option cycle callback: cycles gOptionsMap (via func_pause_00402854) and adjusts gGameSettings+0x17B based on mode gGameSettings+0x24. |
| `func_pause_0040244C` | ✅ matched | Display option toggle callback: flips gOptionsDisplay via func_pause_00400428. |
| `func_pause_00402470` | ⬜ asm | Option/menu callback (unknown exact role). |
| `func_pause_00402548` | ✅ matched | Empty stub (no-op callback). |
| `func_pause_00402550` | ✅ matched | Accept/apply audio options: reapplies volumes, triggers gSndExports+0xDC cue, and fixes gGameSettings+0x17B if mode==2. |
| `func_pause_004025B4` | ✅ matched | Save current option values into a persistent block at D_8002CD98+0x2D0.. (music/sfx/speech vol + 4 config bytes) — commits pause-menu option changes. |
| `func_pause_00402600` | ⬜ asm | Option/menu callback (unknown exact role). |
| `func_pause_00402630` | ✅ matched | Menu exit action: writes gGameSettings+0x17F from +0x44 and sets gameStateFlag (gGameSettings+0x8) = 0xE (quit-to-menu with a selection). |
| `func_pause_0040264C` | ⬜ asm | Menu callback (unknown; likely another exit/action handler). |
| `func_pause_004026A8` | ✅ matched | Menu exit action: writes gGameSettings+0x17E from +0x44 and sets gameStateFlag=0xE. |
| `func_pause_004026C4` | ⬜ asm | Menu callback/build helper (unknown exact role). |
| `func_pause_00402854` | ⬜ asm | Generic cycle-option helper (arg: value ptr, min, max) used by the map option; wraps a numeric setting through a range. |
| `func_pause_00402904` | ✅ matched | Sets sequence/transition state D_8002CC88 = 1 (request a menu transition). |
| `func_pause_00402914` | ⬜ asm | Menu update/transition helper (unknown exact role). |
| `func_pause_00402D78` | ⬜ asm | Menu build/update helper (unknown; large function). |
| `func_pause_00403E6C` | ⬜ asm | Menu update/draw helper (unknown). |
| `func_pause_00403EF0` | ⬜ asm | Menu update/draw helper (unknown). |
| `func_pause_00403F60` | ✅ matched | Plays a one-shot navigation SFX (id 0xF6) on sound handle D_pause_00405E70 when latch D_pause_00405D44 is set, then clears it. |
| `func_pause_00403FD0` | ⬜ asm | Menu update/input helper (unknown exact role). |
| `func_pause_00404150` | ✅ matched | Sets sequence/transition state D_8002CC88 = 8 (request a specific menu transition). |
| `func_pause_00404160` | ✅ matched | Clears two gGameSettings widescreen/aspect-ish fields (+0x6F98,+0x6F96=0) and requests transition 8 via D_pause_00405E48->unk8(8). |
| `func_pause_004041A0` | ✅ matched | Button-press dispatch: if current widget state ==8 (confirm) and edge not yet handled, invokes the selected menu item's callback from the function-pointer table at D_pause_00405D48; else resets the edge latch D_pause_00405E58. |
| `func_pause_0040423C` | ⬜ asm | Menu update/input helper (unknown; likely main per-frame menu tick). |
| `func_pause_00404574` | ✅ matched | Cancel/back action: reapplies volumes and fires two SFX cues (gSndExports+0xE0 with 0xDB and 0xE) — the back/close pause sound. |

</details>

**Gaps:** About half the functions are still raw asm (GLOBAL_ASM): the main menu-build (func_pause_00400EEC, _0040197C, _00402D78), per-frame tick/input (func_pause_0040423C, _00403FD0, _00402914), and many button callbacks (func_pause_00400CB0/CE0/D10, _004020F0/21BC, _00402470/2600/264C/26C4) have no C body — their exact roles are inferred from name/position/neighbors only. The entrypoint (__entrypoint_func_pause_400000) is unmatched, so the precise dispatch/registration of this overlay into the state machine is not visible in C. The MenuNode struct and the D_pause_004054B4 nested-gui node offsets (0x16/0x18/0x2A) are guessed from usage, not confirmed against the uv gui headers. Meaning of gGameSettings+0x6F96/+0x6F98 (possible widescreen/aspect) and the D_8002CC88 transition-code enum (1 vs 8) are unconfirmed. No engine-side rendering/scissor code is in this module, so letterbox impact can only be fully ruled out by inspecting the 'menu' module it delegates to.


---

### `selection` — Menu  (100/292 matched, confidence: medium)

**Role:** Car & track selection front-end: the largest BAR menu/UI overlay. Drives the pre-race setup flow (mode select, player count, car pick, transmission, track/cup pick, AI-car count, options/cheats screens) via a screen state machine, and builds each screen's button/widget layout tables.

**Invoked by:** Runs as a uvmo overlay module (ROM 0xAC3A0, VRAM follows `main`; declared in config/us/beetleadventurerac.us.yaml and modules.yaml under name "selection"). Entry via __entrypoint_func_selection_400000. Active while the game is in the front-end/menu game states — it reads and writes gGameSettings->gameStateFlag (see func_selection_00415AA0 setting it to 2 or 3) and the global gGameStateFlag. Its many small functions are menu-button/item callbacks and per-screen update/draw hooks invoked through the uv GUI export tables (function pointers), not called from other decomp C modules (grep shows no external callers — all references are intra-module, typical of a self-contained menu overlay). func_selection_00402E34 + func_selection_00402E98(&D_selection_00XXXXXX) is the recurring "tear down current screen / activate next screen layout table" idiom used by nearly all the button callbacks.

**Engine exports used:** `gSndExports (unkF4: set stereo/mono audio mode)`, `gUvSprtExports (unk8: free/release a sprite handle)`, `gUvEmitterExports (unk7C: query horn/emitter count for the horn cheat max)`

**Letterbox/viewport relevance:** None. This module draws no black bars, pillarboxes, or full-screen viewport/scissor clips. Its only geometry is fixed x-coordinates for placing menu widgets (e.g. 0x7B, 0xB5, 0xB6, 0xBC, 0xE4 in the 00415AD0 widget factory, and 0x74/0xCA in the 0041129C/004114A0 draw pairs) — these are individual widget positions, not viewport/scissor bounds. It does not configure gSPViewport/gDPSetScissor-equivalents. Race letterbox lives elsewhere (camera-driven ASM, per the race-letterbox memory note); pillarboxing is RT64/host-side. selection is not letterbox-relevant.

**Readability:** Mixed, leaning raw. About 90 of ~300 functions are matched (mostly the tiny button callbacks and empty stubs); the ~192 largest functions (screen builders/handlers) are still raw GLOBAL_ASM. All symbols remain unnamed: functions are func_selection_<addr>, all data is D_selection_<addr>, and struct fields are unk174/unk180/pad178 etc. The export tables are typed only as ad-hoc opaque structs local to this file (UnkSndExports_sel with a single named member unkF4, UnkUvSprtExports_sel.unk8, UnkUvEmitterExports_sel.unk7C). No comments, no semantic names. The matched callbacks are readable enough to infer intent (they set well-named globals like gCheatCars, gNumPlayers, gTransmissionType), but the screen layout tables (D_selection_0041Fxxx) and the builder functions that consume them are opaque.

**Modification hooks:** To change WHICH screen a button leads to: edit the D_selection_0041Fxxx pointer passed to func_selection_00402E98 in the relevant matched callback (e.g. change &D_selection_0041FC80 in func_selection_004038E0). To change spinner ranges (cheats/options): edit the [min,max] literals in the 00404xxx / 00403Exx callbacks (e.g. func_selection_004047D8 gCheatCars 0..6). To change race parameters set by a button: edit the literal in the CAB8/CAE8/CB18 (AI count), 00403B6C-family (gNumPlayers), 0040D234/0040D25C (transmission) callbacks. To move/reposition a menu widget: edit the fixed x-coord constants in func_selection_00415B90/BB8/C78/D9C/DC4 (the 00415AD0 factory calls) or the 0x74/0xCA coords in 0041129C/004114A0. To change the screen the whole flow commits into: func_selection_00415AA0 sets gGameSettings->gameStateFlag (2 vs 3) — edit here to redirect the start-race transition. Layout/graphics of each page (positions of every item, sprites, colors) lives in the still-ASM builder functions and their D_selection_0041Fxxx tables, which must be matched/decoded before fine layout edits are possible from C.

**Key data symbols:**

- `D_selection_0041F5B4 / 0041F728 / 0041F770 / 0041F794 / 0041F904 / 0041F94C / 0041F994 / 0041FB58 / 0041FC80 / 0041FD74 / 0041FE04 / 0041FED0 / 0041FF2C / 0041FFE4 / 00420074 / 00420104 / 00420218 / 00420260` — Screen/menu layout descriptor tables (button lists + callbacks). Each is passed by address to func_selection_00402E98 to activate that screen. These are the editable menu-page definitions for the selection front-end.
- `D_selection_00420DF0` — Current selection index (car/player/item index; used to index gGameSettings option array and transmission array by *6, and passed to per-item handlers).
- `D_selection_00420DFC` — Screen dirty/confirm flag (set to 1 before back/refresh).
- `D_selection_00420DF8` — Active sprite handle (freed via gUvSprtExports->unk8, reset to -1).
- `D_selection_00420DE8 / 00420DEC` — Selection sub-mode / cursor state reset on entering the 0041F770 screen.
- `D_selection_00421CE0` — Mode/tab flag (0 or 1) set by the class/mode screen setters.
- `D_selection_00421D1C` — Pointer to the current menu list/node structure (navigated in 00417240, 0041B11C to read the selected entry and next-screen pointer).
- `D_selection_00421D28` — Selection state flag reset by 00419B78.
- `D_selection_00420E18 / 00420E1C` — Extracted selected item id and its valid flag.
- `D_selection_00420E28` — A selection/handle reset to -1.
- `D_selection_00420E70 / 00420E74 / 00420E78 / 00420E84 / 00420E90` — Widget descriptor pointers placed at fixed x-coords (0xB6,0x7B,0xB5,0xE4,0xBC) by the 00415AD0 widget factory.
- `D_selection_00420E90.. and 00421BA0 / 00421CE0` — Assorted screen-state scalars for the selection UI.
- `gGameSettings (UnkStruct_80025CF0 @0x80025CF0)` — Shared race-setup struct: gameStateFlag (2/3 = start race / next), unk174 (confirm result), unk180/unk184 (class/difficulty), numAiCars, unk6F74, pad178[] flags, per-player option array at 0x13C.
- `gNumPlayers / gNumAiCars / gTransmissionType` — Global race parameters written by the player-count, AI-count, and transmission button callbacks.

<details><summary>Functions (292)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_selection_400000` | ⬜ asm | Overlay entrypoint/module init and screen dispatch. |
| `func_selection_004000DC` | ✅ matched | Empty stub. |
| `func_selection_004000E4` | ⬜ asm | Screen setup/init helper. |
| `func_selection_004003DC` | ⬜ asm | Screen setup/layout builder. |
| `func_selection_00400580` | ⬜ asm | Screen setup/layout builder. |
| `func_selection_004008B0` | ⬜ asm | Large screen/widget build routine. |
| `func_selection_00401570` | ⬜ asm | Widget/item helper. |
| `func_selection_004015D4` | ⬜ asm | Widget/item helper. |
| `func_selection_004016CC` | ⬜ asm | Widget/item helper. |
| `func_selection_004017C8` | ✅ matched | Empty stub (menu enter/exit hook). |
| `func_selection_004017D0` | ✅ matched | Empty stub. |
| `func_selection_004017D8` | ✅ matched | Empty stub. |
| `func_selection_004017E0` | ⬜ asm | Screen-back/refresh action (called by several back callbacks). |
| `func_selection_0040189C` | ⬜ asm | Increment/toggle a u8 option value (wrap helper); used by option callbacks like stereo/mono, speed, display. |
| `func_selection_00401964` | ⬜ asm | Adjust an s32 value clamped to [min,max] (spinner helper), used by battle-cheat callbacks. |
| `func_selection_00401A38` | ⬜ asm | Value-adjust helper variant. |
| `func_selection_00401B1C` | ⬜ asm | Adjust an s32/u8 value clamped [min,max] (spinner helper), used by option/cheat callbacks. |
| `func_selection_00401BF4` | ⬜ asm | Value/widget helper. |
| `func_selection_00401D04` | ⬜ asm | Screen/widget routine. |
| `func_selection_00401FB0` | ⬜ asm | Screen/widget routine. |
| `func_selection_00402254` | ⬜ asm | Screen/widget routine. |
| `func_selection_00402588` | ⬜ asm | Screen/widget routine. |
| `func_selection_00402994` | ⬜ asm | Screen/widget routine. |
| `func_selection_00402D7C` | ✅ matched | Empty stub. |
| `func_selection_00402D84` | ⬜ asm | Screen/widget routine. |
| `func_selection_00402E34` | ⬜ asm | Tear down / reset the current screen (paired with 00402E98 on every screen transition). |
| `func_selection_00402E98` | ⬜ asm | Activate a screen layout table (takes &D_selection_00XXXXXX descriptor) — the core screen-switch primitive. |
| `func_selection_00402F30` | ⬜ asm | Screen helper. |
| `func_selection_00402F80` | ⬜ asm | Screen helper. |
| `func_selection_00402FE4` | ⬜ asm | Screen helper. |
| `func_selection_00403050` | ⬜ asm | Screen helper. |
| `func_selection_004030E4` | ⬜ asm | Screen helper. |
| `func_selection_0040319C` | ⬜ asm | Screen helper. |
| `func_selection_004031E8` | ✅ matched | Go to screen table D_selection_0041F904 (sets D_selection_00421CE0=0, calls 00415CA0(3)). |
| `func_selection_00403220` | ✅ matched | Go to screen table D_selection_0041FED0 (mode flag 1). |
| `func_selection_00403254` | ✅ matched | Go to screen table D_selection_0041F94C (mode flag 0). |
| `func_selection_00403284` | ✅ matched | Go to screen table D_selection_0041F994 (calls 00415EF4 reset first). |
| `func_selection_004032BC` | ✅ matched | Go to screen table D_selection_0041FB58. |
| `func_selection_004032EC` | ⬜ asm | Screen/button callback. |
| `func_selection_00403338` | ⬜ asm | Screen/button callback. |
| `func_selection_0040372C` | ⬜ asm | Screen/button callback. |
| `func_selection_004037F4` | ✅ matched | Difficulty/class button 0: sets gGameSettings->unk180=0, unk184=3, goes to 0041FC80 screen. |
| `func_selection_0040382C` | ✅ matched | Difficulty/class button 1: unk180=1, unk184=4. |
| `func_selection_00403868` | ✅ matched | Difficulty/class button 2: unk180=2, unk184=5. |
| `func_selection_004038A4` | ✅ matched | Difficulty/class button 3: unk180=3, unk184=6. |
| `func_selection_004038E0` | ✅ matched | Common transition to screen table D_selection_0041FC80. |
| `func_selection_0040390C` | ✅ matched | Transition to screen table D_selection_0041FF2C. |
| `func_selection_00403938` | ⬜ asm | Screen/button callback. |
| `func_selection_004039B0` | ⬜ asm | Screen transition (AI-car/track flow target). |
| `func_selection_00403A38` | ✅ matched | Transition to screen table D_selection_0041FD74. |
| `func_selection_00403A64` | ✅ matched | Transition to screen table D_selection_0041FE04 (calls 00415CA0(4)). |
| `func_selection_00403A9C` | ✅ matched | Transition to screen table D_selection_0041F770. |
| `func_selection_00403AC8` | ⬜ asm | Screen/button callback. |
| `func_selection_00403B40` | ✅ matched | Transition to screen table D_selection_00420104. |
| `func_selection_00403B6C` | ✅ matched | Player-count = 2: sets gNumPlayers=2, 00415FB4(0), go to 0041FFE4. |
| `func_selection_00403BAC` | ✅ matched | Player-count = 3. |
| `func_selection_00403BEC` | ✅ matched | Player-count = 4. |
| `func_selection_00403C2C` | ⬜ asm | Screen/button callback. |
| `func_selection_00403CA8` | ⬜ asm | Screen/button callback. |
| `func_selection_00403D24` | ⬜ asm | Screen/button callback. |
| `func_selection_00403DA0` | ✅ matched | Options: toggle stereo/mono and push to gSndExports->unkF4 (audio mode set). |
| `func_selection_00403DE0` | ✅ matched | Options: toggle gOptionsSpeed (MPH/KPH). |
| `func_selection_00403E04` | ✅ matched | Options: toggle D_80025E6A option. |
| `func_selection_00403E28` | ✅ matched | Options: adjust gOptionsMap [0,2]. |
| `func_selection_00403E54` | ✅ matched | Options: toggle gOptionsDisplay. |
| `func_selection_00403E78` | ⬜ asm | Options screen build/handler. |
| `func_selection_0040437C` | ⬜ asm | Large screen build (options or cheats page). |
| `func_selection_004045F4` | ✅ matched | Cheat spinner: BattleNumLadybugs [0,7]. |
| `func_selection_00404620` | ✅ matched | Cheat spinner: BattleLadybugColor [0,3]. |
| `func_selection_0040464C` | ✅ matched | Cheat spinner: BattleHealth [0,3]. |
| `func_selection_00404678` | ✅ matched | Cheat spinner: BattlePowerups [0,8]. |
| `func_selection_004046A4` | ✅ matched | Cheat spinner: BattleTimeLimit [0,6]. |
| `func_selection_004046D0` | ✅ matched | Cheat spinner: BattleRadar [0,1]. |
| `func_selection_004046FC` | ✅ matched | Cheat spinner: BattleDamage [0,3]. |
| `func_selection_00404728` | ✅ matched | Cheat spinner: BattleMysteryBoxMode [0,3]. |
| `func_selection_00404754` | ✅ matched | Cheat spinner: TimeAttackBonusBoxes [0,1]. |
| `func_selection_00404780` | ✅ matched | Cheat spinner: Breakables [0,2]. |
| `func_selection_004047AC` | ✅ matched | Cheat spinner: Environment [0,2]. |
| `func_selection_004047D8` | ✅ matched | Cheat spinner: Cars [0,6]. |
| `func_selection_00404804` | ✅ matched | Cheat spinner: Player2Handicap [0,4]. |
| `func_selection_00404830` | ✅ matched | Cheat spinner: ColorChange [0,1]. |
| `func_selection_0040485C` | ✅ matched | Cheat spinner: FieldOfView [0,2]. |
| `func_selection_00404888` | ⬜ asm | Cheat item handler/build. |
| `func_selection_004048D8` | ⬜ asm | Cheat item handler/build. |
| `func_selection_00404A8C` | ✅ matched | Cheat spinner: HornSfxId [0, gUvEmitterExports->unk7C()] (horn count from emitter engine). |
| `func_selection_00404ACC` | ✅ matched | Cheat spinner: TrackMusic [0,1]. |
| `func_selection_00404AF8` | ✅ matched | Cheat spinner: HandbreakPower [0,2]. |
| `func_selection_00404B24` | ✅ matched | Empty stub. |
| `func_selection_00404B2C` | ⬜ asm | Cheat screen build/handler. |
| `func_selection_00404C1C` | ✅ matched | Empty stub. |
| `func_selection_00404C24` | ⬜ asm | Large screen build/handler. |
| `func_selection_00405078` | ⬜ asm | Screen build/handler. |
| `func_selection_00405140` | ⬜ asm | Large screen build/handler. |
| `func_selection_00405644` | ⬜ asm | Large screen build/handler. |
| `func_selection_00405CB0` | ⬜ asm | Screen build/handler. |
| `func_selection_00405E38` | ⬜ asm | Screen build/handler. |
| `func_selection_00405F48` | ⬜ asm | Screen build/handler. |
| `func_selection_004060D4` | ✅ matched | Adjust per-player gGameSettings option field (offset 0x13C + player*6) via 0040189C. |
| `func_selection_00406114` | ✅ matched | Empty stub. |
| `func_selection_0040611C` | ⬜ asm | Screen build/handler. |
| `func_selection_0040626C` | ⬜ asm | Screen build/handler. |
| `func_selection_004062F8` | ⬜ asm | Large screen build/handler. |
| `func_selection_00406924` | ⬜ asm | Large screen build/handler. |
| `func_selection_00406DC4` | ⬜ asm | Screen build/handler. |
| `func_selection_00406F78` | ✅ matched | Empty stub. |
| `func_selection_00406F80` | ⬜ asm | Screen build/handler. |
| `func_selection_0040705C` | ⬜ asm | Large screen build/handler. |
| `func_selection_004078D4` | ⬜ asm | Large screen build/handler. |
| `func_selection_00407AF8` | ⬜ asm | Large screen build/handler. |
| `func_selection_004080D0` | ⬜ asm | Screen build/handler. |
| `func_selection_004081B4` | ⬜ asm | Screen build/handler. |
| `func_selection_004082FC` | ✅ matched | Empty stub. |
| `func_selection_00408304` | ⬜ asm | Screen build/handler. |
| `func_selection_004083DC` | ⬜ asm | Large screen build/handler. |
| `func_selection_0040884C` | ⬜ asm | Screen build/handler. |
| `func_selection_00408B24` | ⬜ asm | Screen build/handler. |
| `func_selection_00408BEC` | ⬜ asm | Large screen build/handler. |
| `func_selection_004092F0` | ⬜ asm | Large screen build/handler. |
| `func_selection_004099A0` | ⬜ asm | Large screen build/handler. |
| `func_selection_00409C94` | ⬜ asm | Screen build/handler. |
| `func_selection_00409EA4` | ⬜ asm | Screen build/handler. |
| `func_selection_00409FC8` | ⬜ asm | Screen build/handler. |
| `func_selection_0040A104` | ⬜ asm | Large screen build/handler. |
| `func_selection_0040A5E0` | ⬜ asm | Screen build/handler. |
| `func_selection_0040A778` | ⬜ asm | Screen build/handler. |
| `func_selection_0040A820` | ⬜ asm | Large screen build/handler. |
| `func_selection_0040B070` | ⬜ asm | Screen build/handler. |
| `func_selection_0040B13C` | ✅ matched | Empty stub. |
| `func_selection_0040B144` | ⬜ asm | Large screen build/handler. |
| `func_selection_0040B458` | ⬜ asm | Screen build/handler. |
| `func_selection_0040B52C` | ✅ matched | Empty stub. |
| `func_selection_0040B534` | ⬜ asm | Screen build/handler. |
| `func_selection_0040B600` | ✅ matched | Empty stub. |
| `func_selection_0040B608` | ⬜ asm | Screen build/handler. |
| `func_selection_0040B724` | ⬜ asm | Small helper. |
| `func_selection_0040B74C` | ⬜ asm | Large screen build/handler. |
| `func_selection_0040BB14` | ⬜ asm | Large screen build/handler. |
| `func_selection_0040BEC8` | ⬜ asm | Screen build/handler. |
| `func_selection_0040BFD0` | ✅ matched | Adjust D_80025E70 [0,2] (some 3-way selection option). |
| `func_selection_0040BFFC` | ⬜ asm | Screen build/handler. |
| `func_selection_0040C454` | ⬜ asm | Large screen build/handler. |
| `func_selection_0040C96C` | ⬜ asm | Screen build/handler. |
| `func_selection_0040CA50` | ⬜ asm | Screen build/handler. |
| `func_selection_0040CAB8` | ✅ matched | AI-car count preset 0: gNumAiCars=7 then 00403A38. |
| `func_selection_0040CAE8` | ✅ matched | AI-car count preset 1: gNumAiCars=1 then 00403A38. |
| `func_selection_0040CB18` | ✅ matched | AI-car count preset 2: numAiCars=0, unk6F74=5, then 004039B0. |
| `func_selection_0040CB50` | ✅ matched | Set D_80025E70=0 then 004039B0. |
| `func_selection_0040CB74` | ✅ matched | Set D_80025E70=1 then 004039B0. |
| `func_selection_0040CB9C` | ✅ matched | Set D_80025E70=2 then 004039B0. |
| `func_selection_0040CBC4` | ✅ matched | Set D_80025E70=3 then 004039B0. |
| `func_selection_0040CBEC` | ⬜ asm | Screen build/handler. |
| `func_selection_0040D134` | ⬜ asm | Screen build/handler. |
| `func_selection_0040D234` | ✅ matched | Set transmission=automatic(1) for current player (indexes gTransmissionType by player*6). |
| `func_selection_0040D25C` | ✅ matched | Set transmission=manual(0) for current player. |
| `func_selection_0040D280` | ⬜ asm | Screen build/handler. |
| `func_selection_0040D488` | ⬜ asm | Screen build/handler. |
| `func_selection_0040D514` | ⬜ asm | Large screen build/handler. |
| `func_selection_0040D844` | ⬜ asm | Screen build/handler. |
| `func_selection_0040D8B4` | ⬜ asm | Screen build/handler. |
| `func_selection_0040D98C` | ⬜ asm | Screen build/handler. |
| `func_selection_0040D9F8` | ⬜ asm | Very large screen build/handler. |
| `func_selection_0040F0F8` | ⬜ asm | Screen build/handler. |
| `func_selection_0040F248` | ⬜ asm | Screen build/handler. |
| `func_selection_0040F294` | ⬜ asm | Screen build/handler. |
| `func_selection_0040F430` | ⬜ asm | Screen build/handler. |
| `func_selection_0040F4BC` | ⬜ asm | Per-index item handler (takes index arg). |
| `func_selection_0040F64C` | ⬜ asm | Screen build/handler. |
| `func_selection_0040F6E0` | ✅ matched | Invoke 0040F4BC with current selection index D_selection_00420DF0. |
| `func_selection_0040F704` | ⬜ asm | Screen build/handler. |
| `func_selection_0040F9EC` | ⬜ asm | Large screen build/handler. |
| `func_selection_0040FDB4` | ⬜ asm | Large screen build/handler. |
| `func_selection_004103CC` | ⬜ asm | Screen build/handler. |
| `func_selection_0041057C` | ⬜ asm | Screen build/handler. |
| `func_selection_00410614` | ⬜ asm | Large screen build/handler. |
| `func_selection_00410864` | ✅ matched | Empty stub (s16,s16). |
| `func_selection_00410870` | ✅ matched | Empty stub (s16,s16). |
| `func_selection_0041087C` | ✅ matched | Empty stub. |
| `func_selection_00410884` | ✅ matched | Empty stub. |
| `func_selection_0041088C` | ⬜ asm | Screen build/handler. |
| `func_selection_00410AA8` | ⬜ asm | Screen build/handler. |
| `func_selection_00410BD0` | ⬜ asm | Sprite/widget placement primitive (id,x,y) used by 0041129C/004114A0. |
| `func_selection_00410D20` | ⬜ asm | Screen build/handler. |
| `func_selection_00410E78` | ⬜ asm | Sprite/widget draw primitive (id,y,value). |
| `func_selection_00411038` | ⬜ asm | Sprite/widget draw primitive. |
| `func_selection_004111B8` | ⬜ asm | Screen build/handler. |
| `func_selection_00411204` | ⬜ asm | Screen build/handler. |
| `func_selection_00411250` | ⬜ asm | Screen build/handler. |
| `func_selection_0041129C` | ✅ matched | Draw a widget pair at fixed coords (0x74) via 00410BD0/00410E78. |
| `func_selection_004112D8` | ⬜ asm | Screen build/handler. |
| `func_selection_00411324` | ⬜ asm | Screen build/handler. |
| `func_selection_00411370` | ⬜ asm | Screen build/handler. |
| `func_selection_004113BC` | ⬜ asm | Screen build/handler. |
| `func_selection_00411408` | ⬜ asm | Screen build/handler. |
| `func_selection_00411454` | ⬜ asm | Screen build/handler. |
| `func_selection_004114A0` | ✅ matched | Draw a widget pair at fixed coords (0xCA) via 00410BD0/00411038. |
| `func_selection_004114DC` | ⬜ asm | Screen build/handler. |
| `func_selection_00411528` | ⬜ asm | Screen build/handler. |
| `func_selection_00411574` | ⬜ asm | Screen build/handler. |
| `func_selection_004115C0` | ⬜ asm | Screen build/handler. |
| `func_selection_0041162C` | ⬜ asm | Screen build/handler. |
| `func_selection_004116E0` | ⬜ asm | Large screen build/handler. |
| `func_selection_00411870` | ⬜ asm | Large screen build/handler. |
| `func_selection_00411B9C` | ⬜ asm | Large screen build/handler. |
| `func_selection_00411F40` | ⬜ asm | Very large screen build/handler. |
| `func_selection_004133E0` | ⬜ asm | Screen build/handler. |
| `func_selection_00413418` | ⬜ asm | Screen build/handler. |
| `func_selection_004134F4` | ✅ matched | Set D_selection_00420DFC=1 (dirty/confirm flag) then 004017E0 (back/refresh). |
| `func_selection_0041351C` | ⬜ asm | Screen build/handler. |
| `func_selection_00413568` | ✅ matched | Transition to screen table D_selection_00420218. |
| `func_selection_00413594` | ⬜ asm | Screen build/handler. |
| `func_selection_004136CC` | ⬜ asm | Screen build/handler. |
| `func_selection_0041378C` | ✅ matched | Reset selection state (D_selection_00420DE8=2, DEC=0, D_8002CC84=0) and go to screen D_selection_0041F770. |
| `func_selection_004137D0` | ⬜ asm | Screen build/handler. |
| `func_selection_00413A88` | ⬜ asm | Large screen build/handler. |
| `func_selection_00413DE0` | ✅ matched | Free/release a sprite via gUvSprtExports->unk8(D_selection_00420DF8), reset handle to -1. |
| `func_selection_00413E1C` | ✅ matched | Transition to screen table D_selection_00420074. |
| `func_selection_00413E48` | ⬜ asm | Screen build/handler. |
| `func_selection_00413FA4` | ✅ matched | Empty stub. |
| `func_selection_00413FAC` | ✅ matched | Empty stub. |
| `func_selection_00413FB4` | ✅ matched | Empty stub. |
| `func_selection_00413FBC` | ✅ matched | Empty stub. |
| `func_selection_00413FC4` | ⬜ asm | Large screen build/handler. |
| `func_selection_004146C8` | ⬜ asm | Very large screen build/handler. |
| `func_selection_00414CEC` | ✅ matched | Empty stub. |
| `func_selection_00414CF4` | ⬜ asm | Screen build/handler. |
| `func_selection_00415040` | ⬜ asm | Large screen build/handler. |
| `func_selection_0041532C` | ⬜ asm | Large screen build/handler. |
| `func_selection_004155A0` | ⬜ asm | Large screen build/handler. |
| `func_selection_004158DC` | ⬜ asm | Screen build/handler. |
| `func_selection_004159FC` | ✅ matched | Transition to D_selection_00420074, setting D_selection_00420DFC=1. |
| `func_selection_00415A34` | ⬜ asm | Screen build/handler. |
| `func_selection_00415AA0` | ✅ matched | Commit selection & set gGameSettings->gameStateFlag to 2 or 3 based on pad178[5] (start race / next stage). |
| `func_selection_00415AD0` | ⬜ asm | Configure/load a widget at x-coord with a data pointer (widget factory). |
| `func_selection_00415B90` | ✅ matched | Place widget at x=0xBC using D_selection_00420E90. |
| `func_selection_00415BB8` | ✅ matched | Place widget at x=0xE4 using D_selection_00420E84. |
| `func_selection_00415BE0` | ⬜ asm | Widget helper. |
| `func_selection_00415C78` | ✅ matched | Place widget at x=0x7B using D_selection_00420E74. |
| `func_selection_00415CA0` | ⬜ asm | Set active tab/page index (called with 3 and 4 by screen setters). |
| `func_selection_00415D48` | ⬜ asm | Widget/screen helper. |
| `func_selection_00415D9C` | ✅ matched | Place widget at x=0xB5 using D_selection_00420E78. |
| `func_selection_00415DC4` | ✅ matched | Place widget at x=0xB6 using D_selection_00420E70. |
| `func_selection_00415DEC` | ⬜ asm | Set selected class/difficulty index (called with 0..3 by 004037F4 family). |
| `func_selection_00415E80` | ⬜ asm | Set AI-car preset index (called with 0..2 by 0040CAB8 family). |
| `func_selection_00415EF4` | ⬜ asm | Reset selection state before entering a screen. |
| `func_selection_00415FB4` | ⬜ asm | Set player-count index (called with 0..2 by 00403B6C family). |
| `func_selection_00416028` | ⬜ asm | Screen build/handler. |
| `func_selection_004162D4` | ⬜ asm | Screen build/handler. |
| `func_selection_0041639C` | ⬜ asm | Large screen build/handler. |
| `func_selection_004166CC` | ⬜ asm | Screen build/handler. |
| `func_selection_00416794` | ⬜ asm | Screen build/handler. |
| `func_selection_004168C4` | ⬜ asm | Screen build/handler. |
| `func_selection_00416A50` | ⬜ asm | Small helper. |
| `func_selection_00416ABC` | ⬜ asm | Screen build/handler. |
| `func_selection_00416D94` | ⬜ asm | Screen build/handler. |
| `func_selection_00416E78` | ⬜ asm | Screen build/handler. |
| `func_selection_00416F28` | ✅ matched | Transition to screen table D_selection_0041F728. |
| `func_selection_00416F54` | ⬜ asm | Screen build/handler. |
| `func_selection_00416FF4` | ⬜ asm | Large screen build/handler. |
| `func_selection_00417240` | ✅ matched | Read selected item id from list node (D_selection_00421D1C chain) into D_selection_00420E18; set flag 00420E1C=1. |
| `func_selection_00417268` | ⬜ asm | Screen build/handler. |
| `func_selection_004172B4` | ⬜ asm | Very large screen build/handler. |
| `func_selection_004177A8` | ⬜ asm | Very large screen build/handler. |
| `func_selection_004181D0` | ⬜ asm | Large screen build/handler. |
| `func_selection_0041843C` | ✅ matched | Reset D_selection_00420E28 = -1 (clear a selection/handle). |
| `func_selection_0041844C` | ⬜ asm | Large screen build/handler. |
| `func_selection_00418800` | ⬜ asm | Very large screen build/handler. |
| `func_selection_00418E98` | ⬜ asm | Large screen build/handler. |
| `func_selection_00419020` | ✅ matched | Set two s16 fields inside D_selection_0041F794 (offsets 0x16,0x2A) to 1 (enable widgets). |
| `func_selection_00419038` | ⬜ asm | Screen build/handler. |
| `func_selection_00419084` | ⬜ asm | Screen build/handler. |
| `func_selection_00419194` | ⬜ asm | Large screen build/handler. |
| `func_selection_00419384` | ⬜ asm | Very large screen build/handler. |
| `func_selection_00419AA4` | ⬜ asm | Large screen build/handler. |
| `func_selection_00419B78` | ✅ matched | Reset D_selection_00421D28=0 then 004017E0 (back/refresh). |
| `func_selection_00419B9C` | ✅ matched | Transition to screen table D_selection_0041F5B4. |
| `func_selection_00419BC8` | ⬜ asm | Large screen build/handler. |
| `func_selection_00419D54` | ⬜ asm | Very large screen build/handler. |
| `func_selection_00419FB8` | ⬜ asm | Large screen build/handler. |
| `func_selection_0041A0CC` | ⬜ asm | Large screen build/handler. |
| `func_selection_0041A208` | ✅ matched | Empty stub. |
| `func_selection_0041A210` | ✅ matched | Place widget (00415D9C) and transition to screen D_selection_00420260. |
| `func_selection_0041A244` | ⬜ asm | Very large screen build/handler. |
| `func_selection_0041A5BC` | ⬜ asm | Large screen build/handler. |
| `func_selection_0041A7D0` | ⬜ asm | Large screen build/handler. |
| `func_selection_0041A960` | ⬜ asm | Large screen build/handler. |
| `func_selection_0041ABC8` | ✅ matched | Compute a time value: (min*60 + sec) + hundredths/100 as f32 (lap/record time formatting). |
| `func_selection_0041ABFC` | ⬜ asm | Very large screen build/handler (uses the time helper). |
| `func_selection_0041B11C` | ✅ matched | Confirm button variant 1: gGameSettings->unk174=1, pad178[1]=1, go to next screen from D_selection_00421D1C+4. |
| `func_selection_0041B15C` | ✅ matched | Confirm button variant 2: unk174=2, pad178[1]=1, go to next screen. |
| `func_selection_0041B19C` | ⬜ asm | Screen build/handler. |
| `func_selection_0041B28C` | ⬜ asm | Large screen build/handler. |
| `func_selection_0041B370` | ⬜ asm | Large screen build/handler (last function in module). |

</details>

**Gaps:** The 192 unmatched builder/handler functions (func_selection_004008B0, 0040437C, 00404C24, the 0040Dxxx–0041Bxxx large ones) are raw asm — their exact widget layout, coordinates, colors, and text/sprite ids can't be read from C, so per-page visual editing isn't yet possible. The screen-descriptor tables (D_selection_0041F5B4..00420260) are declared extern s32 with no struct type, so their internal layout (button count, callback slots, coords) is unknown. Export tables are typed as single-member opaque stubs; the full uv GUI/sprite/emitter/snd vtables are not mapped here. gGameSettings fields used (unk174/unk180/unk184/unk6F74/pad178/0x13C option array) are unnamed padding — semantics inferred, not confirmed. No external caller/dispatch was found in C (the overlay entrypoint __entrypoint_func_selection_400000 is asm), so the exact game-state values that load this overlay were inferred from gameStateFlag writes rather than read from a caller.


---

### `filmroll` — Transition  (2/9 matched, confidence: medium)

**Role:** Relocatable "uv"-engine overlay module implementing the film-roll / page-turn menu transition: it directly drives the N64 Video Interface (osViSwapBuffer / osViBlack) and a scheduler swap flag (func_80004B2C) to pan/black-out the framebuffer between menu screens.

**Invoked by:** Loaded on demand by the uv overlay/file loader (like sibling modules fileux, intro, ripple); its entrypoint __entrypoint_func_filmroll_400000 registers an export table via uvUpdateFileAllocPtr, so the menu state machine (gamegui/game modules driving gGameStateFlag) calls the module's exported slots (0x60/0x68/0x170/0x35C/0x720) to play the transition when moving between menu screens. No direct C caller exists in the decomp because callers reach it through the export-table jump slots, not by symbol. Confirmed by the project's own R6 investigation (docs/R6_FILMROLL_FINDINGS.md) that this transition is a VI-origin pan performed via osViSwapBuffer.

**Letterbox/viewport relevance:** HIGHLY relevant to the viewport/framebuffer, though NOT the race cinematic letterbox. This module does NOT draw top/bottom black bars or configure pillarboxes/scissor rects. Instead it directly manipulates the N64 Video Interface: it calls osViSwapBuffer (rom 0x2A4,0x328,0x4FC) to change the displayed framebuffer/VI origin — producing the page-turn PAN between menu screens — and calls osViBlack three times (rom 0x51C,0x52C,0x564 in func_00400588) to blank the entire screen to black during the transition (a full-screen VI black-out, not letterbox bars). It also pokes func_80004B2C (scheduler swap-pending flag D_8002F26C). This is exactly the VI-origin pan that docs/R6_FILMROLL_FINDINGS.md identified: the transition is realized by osViSwapBuffer origin changes, which is why RT64 present-mode choice (Console vs PresentEarly vs SkipBuffering) affects whether the pan renders correctly. A dev changing present-mode/VI-origin handling in the RT64 fork must account for this module.

**Readability:** Raw. Every meaningful function is still func_filmroll_XXXX and all data is D_/B_filmroll_XXXX with inferred meaning only. 7 of 9 functions are unmatched (GLOBAL_ASM); the only "matched" bodies are two empty stubs (00400060, 0040062C). No typedefs, no export-struct type (unlike sibling fileux which has FileUx_Exports), no comments. The .c file has no #include and no prototypes. Purpose is inferred from the reloc table (osViSwapBuffer/osViBlack/func_80004B2C/uvUpdateFileAllocPtr) and from docs/R6_FILMROLL_FINDINGS.md, not from named source.

**Modification hooks:** Because this is raw asm, in-module edits are impractical without decompiling. Practical hooks: (1) To change/disable the transition itself, override behavior at the RT64/runtime layer — docs/R6_FILMROLL_FINDINGS.md shows BAR_PRESENT_MODE and the default Console present mode govern whether the osViSwapBuffer pan renders; changing present mode is the real lever for this effect. (2) The animation timing/pan distance lives in the data tables D_filmroll_004009A0/A4/A8, D_filmroll_004009C8/D0/D8, and framebuffer/origin pointers D_filmroll_00400A78/A80 at ROM 0x9A0-0xA80 in the module image — patching those bytes tweaks pan offset/speed. (3) The black-out is func_filmroll_00400588's three osViBlack calls (module rom ~0x51C-0x564); NOP-ing those would remove the mid-transition full-screen black. (4) The export-table wiring in __entrypoint_func_filmroll_400000 (relocs at rom 0x64-0x88) is where a slot could be redirected to a custom handler.

**Key data symbols:**

- `__entrypoint_func_filmroll_400000` — Module entrypoint address (0x400000), self-referenced by a MIPS_32 reloc at rom 0xA04 (export-table/module descriptor).
- `B_filmroll_004009C0` — BSS/runtime state variable for the transition (uninitialized working state).
- `D_filmroll_004009B0` — Data/state variable near the module's data block (transition parameter or pointer).
- `D_filmroll_004009A0` — Tuning constant (pan/timing parameter) referenced by func_00400634-area code.
- `D_filmroll_004009A4` — Tuning constant referenced near rom 0x614.
- `D_filmroll_004009A8` — Tuning constant referenced by the func_00400720 phase (rom 0x7FC).
- `D_filmroll_004009C8` — Most-referenced data value (rom 0xF8,0x1FC,0x3E8,0x688,0x840...) — likely the main animation counter/step or a shared coordinate used across all phases.
- `D_filmroll_004009D0` — Data value used in the early phase (rom 0x430,0x460,0x570) — a position/offset for the pan.
- `D_filmroll_004009D8` — Data value used in the mid/late phase (rom 0x868,0x888,0x8B8) — another pan/offset coordinate.
- `D_filmroll_00400A78` — Data/table referenced in the black-out and late phases (rom 0x5D8,0x858,0x864) — likely a framebuffer/VI-origin pointer or table.
- `D_filmroll_00400A80` — Data/table referenced repeatedly (rom 0x7D8,0x808,0x980) — companion to A78, likely second framebuffer/origin pointer or coordinate table.

<details><summary>Functions (9)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_filmroll_400000` | ⬜ asm | Module entrypoint/ABI publisher: calls uvUpdateFileAllocPtr then writes the exported function pointers (0x60,0x68,0x170,0x35C,0x720) into the module's export table (per reloc HI16/LO16 at rom 0x64-0x88). |
| `func_filmroll_00400060` | ✅ matched | Empty stub (no-op export slot, e.g. per-frame update or teardown hook with no body in this module). |
| `func_filmroll_00400068` | ⬜ asm | Exported entry, likely transition init/setup (allocates/initializes the film-roll state at B_filmroll_004009C0 / D_filmroll_004009B0). |
| `func_filmroll_00400170` | ⬜ asm | Exported step function; calls func_80004B2C (sets scheduler swap flag D_8002F26C) and osViSwapBuffer — advances/renders one frame of the page-turn pan. |
| `func_filmroll_0040035C` | ⬜ asm | Exported entry that also calls func_80004B2C + osViSwapBuffer — a second phase of the swap/pan sequence (e.g. the reverse/settle step). |
| `func_filmroll_00400588` | ⬜ asm | Internal routine issuing osViSwapBuffer plus three osViBlack calls — performs the full-screen black-out (VI blanking) portion of the transition. |
| `func_filmroll_0040062C` | ✅ matched | Empty stub (no-op helper). |
| `func_filmroll_00400634` | ⬜ asm | Internal helper reading the tuning-data tables (D_filmroll_004009A0..A80) to compute pan offsets/timing for the roll animation. |
| `func_filmroll_00400720` | ⬜ asm | Exported entry (final slot in the export table); another update/finalize step of the transition, references data table D_filmroll_004009A8/A80. |

</details>

**Gaps:** All 7 real functions are unmatched asm (bodies not checked in), so exact per-function logic, the argument shapes of the exported slots, and the precise layout/meaning of the data tables (D_filmroll_004009A0..00400A80) are inferred from the reloc table and project docs, not read. In particular it is unconfirmed whether D_filmroll_00400A78/A80 are framebuffer pointers vs coordinate arrays, and which exact game state / caller module invokes each export slot (no direct C caller exists; dispatch is via the export-table jump slots). Matching the asm (especially func_00400170/0040035C/00400588/00400720) would be needed to fully confirm the pan math and the exact VI-origin sequence.


---

### `glare` — Effect  (1/8 matched, confidence: medium)

**Role:** Self-contained "uv"-engine overlay module (type uvmo) that produces a lens-glare / screen-flash sprite effect; it loads its own glare texture assets via uvLoadFile and registers an init/update/draw vtable through its entrypoint.

**Invoked by:** Loaded as a dynamically-relocated overlay module (uvmo) listed in config/us/modules.yaml and beetleadventurerac.us.yaml (segment "glare", start 0x5C0B8, vram base 0x400000). The engine loads the module and calls __entrypoint_func_glare_400000, which self-relocates (MIPS_32 self-reloc at rom 0xFBC) and installs a vtable of the module's lifecycle functions (the four func_glare_004000C8/00D0/0170/0518 pointers wired via HI16/LO16 relocs at rom 0x64-0x84). Exact game state that spawns the glare effect is not visible in the decomp (module is invoked by name/handle, not by a static C caller); by analogy to sibling effect overlays (expl/flag) it is triggered by gameplay/scene code when a glare/flash is needed. No static caller of any func_glare_* exists elsewhere in the decomp.

**Engine exports used:** `uvLoadFile (uv file/asset loader, called 4x from the entrypoint region — loads glare textures/sprites)`, `uvUpdateFileAllocPtr (uv file allocator bookkeeping)`

**Letterbox/viewport relevance:** None. The reloc table shows only uvLoadFile / uvUpdateFileAllocPtr external calls and a jump-table-driven state machine; there is no viewport, scissor, pillar, or black-bar configuration. This is an additive glare/flash sprite overlay, not a clipping/letterbox element. (No gSPViewport / gDPSetScissor-equivalent symbols referenced.)

**Readability:** Raw/unreadable. 6 of 7 functions are still #pragma GLOBAL_ASM (unmatched, no C body); the only matched function is an empty stub. All data symbols are unnamed D_glare_XXXXXXXX / B_glare_XXXXXXXX with no struct typing or comments. No engine export-table struct members are named here. Purpose is inferred from the module name, the uvmo vtable pattern, the jump table, and the uvLoadFile calls — not from decompiled logic.

**Modification hooks:** Because the module is still raw asm, edits require decompiling first. Conceptual hooks: (1) glare visibility/intensity/fade lives in the draw handler func_glare_00400518 and its params in the D_glare_00400DC4..E10 constants (alpha/color/timing). (2) glare position/scale would be set in the draw/placement path (func_glare_00400518 / helper func_glare_004006B0) where sprite coordinates are computed. (3) which texture is used is determined by the uvLoadFile calls in the entrypoint region and the asset handles stored at D_glare_00400F74/F78. (4) To disable the effect entirely without touching asm, gate the module's invocation at its (currently unidentified) call site, or make its update/draw vtable entry a no-op like func_glare_004000C8. To recolor/reposition robustly, decompile func_glare_00400518 and name the D_glare_* constants first.

**Key data symbols:**

- `D_glare_00400DA0` — Jump table (type:jtbl) for the effect state machine; entries are the .Lglare_00400510/004001C4/00400210/00400280/0040025C/0040035C/004003A8/004004CC/004004E4 local labels — phases of the glare animation.
- `D_glare_00400DC4 / DC8 / DD0 / DD8 / DE0 / DE4 / DF0 / E04 / E10 / EE0 / F04` — Read-only effect parameter constants / small data (timings, alpha/fade values, coordinates, or sprite-descriptor pointers) referenced by the update/draw handlers via HI16/LO16.
- `D_glare_00400F74 / D_glare_00400F78` — Data referenced from both the entrypoint (rom 0xF0/0x104) and the draw handler (rom 0x87C/0x898) — likely loaded-asset handle/pointer slots (glare texture file pointers) or shared effect config.
- `B_glare_00400F70` — BSS/uninitialized runtime state block for the module (effect instance state: active flag, timer, current phase, cached asset pointers).

<details><summary>Functions (8)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_glare_400000` | ⬜ asm | Module entrypoint/dispatcher: self-relocates and registers the module vtable (init/update/draw/free) pointing at the func_glare_* lifecycle handlers; also drives the jump table D_glare_00400DA0. |
| `func_glare_004000C8` | ✅ matched | Empty stub (no-op) lifecycle slot — likely the free/deinit or unused vtable entry; body is `{}`. |
| `func_glare_004000D0` | ⬜ asm | Lifecycle handler (likely init/setup) — one of the vtable entries registered by the entrypoint; probably loads glare assets and initializes effect state. |
| `func_glare_00400170` | ⬜ asm | Lifecycle handler (likely per-frame update): advances the glare/flash state, uses the .Lglare_* jump table (D_glare_00400DA0) to branch across effect phases. |
| `func_glare_00400518` | ⬜ asm | Lifecycle handler (likely draw/render): emits the glare sprite/blit via the uv engine using loaded textures and effect params. |
| `func_glare_004006B0` | ⬜ asm | Helper referenced by the entrypoint (HI16/LO16 at rom 0x194) — subroutine of the update/draw path (e.g. per-sprite placement or fade computation). |
| `func_glare_004009EC` | ⬜ asm | Internal helper (not vtable-registered; called intra-module) — supporting routine for the glare effect, purpose not pinnable without asm. |
| `func_glare_00400BA4` | ⬜ asm | Helper referenced by the entrypoint (HI16/LO16 at rom 0x14C) — likely the asset-load/setup routine (correlates with the 4 uvLoadFile calls in the entrypoint region). |

</details>

**Gaps:** The actual asm bodies are not checked in, so all logic is inferred. Unresolved: (1) which vtable slot is init vs update vs draw vs free (mapping of func_glare_004000D0/00400170/00400518 to lifecycle roles is a best guess). (2) The concrete game state / caller that spawns the glare effect — no static C caller references func_glare_* anywhere in the decomp; the module is invoked by handle through the engine's module system. (3) Meaning of each D_glare_* constant (fade timings vs colors vs coords) is unnamed and untyped. (4) Whether the effect is a fullscreen flash or a positioned lens-glare sprite cannot be confirmed without decompiling the draw handler. Full understanding requires the asm for func_glare_004000D0, 00400170, 00400518, 004006B0, 004009EC, 00400BA4 and the entrypoint.


---

### `demo` — Attract  (1/8 matched, confidence: medium)

**Role:** Attract-mode / demo-playback overlay module (form0 entry "demo"): boots a scripted demo race using the replay/scene engine so the game plays itself while idle at the attract loop.

**Invoked by:** Loaded as a form0 overlay module (config/us/form0.json entry index for "demo"; convPartialModule.py maps partial_demo.o -> "demo"; daisybox bar_module_files.c lists it with a reloc base). Its module entrypoint is __entrypoint_func_demo_400000 (rom 0x22A0 MIPS_32 reloc), which the engine's module/state dispatcher calls when the attract/demo game state becomes active. Lifecycle mirrors the sibling "intro" module: an enter, per-frame update, and exit-state function set (func_demo_00400244 is the exit-state hook that restores gNumAiCars and re-selects currentTrack via gSceneExports, exactly like intro's func_intro_004004F0). Driven by the gGameSettings/gGameStateFlag state machine; runs unattended until a button press or timeout returns to the menu/attract sequence.

**Engine exports used:** `gSceneExports (slot [1] used to (re)select currentTrack)`, `gReplayExports (inferred from intro sibling — replay playback drives the demo car)`, `gGameGuiExports (inferred — GUI draw callbacks, per intro pattern)`, `gSndExports (inferred — music/sfx during demo)`, `gUvModelExports / gUvSprtExports (inferred — model/sprite draw callbacks)`

**Letterbox/viewport relevance:** None found. The demo module contains no scissor/viewport/pillar/black-bar setup in the readable C. It reuses the race/scene rendering path (gSceneExports/replay), so any letterbox that appears during a demo race is drawn by the camera/scene code (the cinematic letterbox lives in race-adjacent ASM per prior findings), NOT here. func_demo_00400244 only touches gNumAiCars, currentTrack, and unk180 — no clipping/viewport. Caveat: the large unmatched func_demo_00400538 and the D_demo_00400C40 table are raw asm/data and were not disassembled, so a viewport/scissor call there cannot be 100% ruled out, but nothing in the symbols or prototypes suggests one.

**Readability:** Mostly raw. Only 1 of 7 functions is matched (func_demo_00400244) and even it uses raw D_demo_/gSceneExports slot-index access ((s32*)gSceneExports)[1]); the other 6 are GLOBAL_ASM stubs with func_XXXX names. Every data symbol is an unnamed D_demo_<addr>/B_demo_<addr>. The ledger_snapshot.csv shows all functions still status "asm" with no friendly names. No doc comments. Not modification-ready.

**Modification hooks:** To change which track/car count the demo uses: edit D_demo_00402270 (AI car count) and the gSceneExports slot-[1] track selection inside func_demo_00400244 (and its counterpart in the enter hook, likely func_demo_00400290). To change demo timing/sequence: the big D_demo_00400C40..00401DB8 script/jump table plus the update routine func_demo_00400538 are the levers (requires disassembling the unmatched asm first). To disable/skip attract demo entirely: intercept at the state-machine level (gGameStateFlag/gGameSettings) so the demo module is never entered — i.e. in the caller that dispatches to __entrypoint_func_demo_400000 — rather than inside this module. There is no color/position/visibility constant here that a dev can flip for a HUD element; this module orchestrates a full scene, it does not draw a discrete UI widget.

**Key data symbols:**

- `D_demo_00402270` — Number of AI cars for the demo race (copied into gNumAiCars in func_demo_00400244).
- `D_demo_00402274` — Module data pointer/handle referenced by the demo code (declared extern void*).
- `D_demo_00402268 / 0040226C / 00402260(B_demo)` — Small module state/config words and a BSS scratch var (B_demo_00402260) — per-run demo state (timers/indices).
- `D_demo_00402228..00402248` — Cluster of config words/floats near the top of .data (track id, flags, camera/timing params for the scripted demo).
- `D_demo_00401DCC..00402018` — Small parameter/float table used by the update routine.
- `D_demo_00400C40..00401DB8` — Large contiguous data/jump/pointer table (~0x1180 bytes, hundreds of entries) — the demo playback script/step table indexed by the main update func_demo_00400538 (sequence of scene/replay steps or a big case-jump table).

<details><summary>Functions (8)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_demo_400000` | ⬜ asm | Module entrypoint/header thunk the engine jumps to when the demo overlay is loaded; wires up the module's state callbacks (enter/update/exit). |
| `func_demo_00400244` | ✅ matched | Exit/teardown of the demo state: restores gNumAiCars from D_demo_00402270, re-selects gGameSettings->currentTrack via gSceneExports table slot [1], clears unk180 (mirrors intro's exit-state). |
| `func_demo_00400290` | ⬜ asm | Likely the demo enter/init hook (set up AI count, load track/replay, kick off playback) — small setup routine (396 bytes). |
| `func_demo_0040041C` | ⬜ asm | Demo helper/sub-step (284 bytes) — probably per-frame input poll (button-press to abort demo) or replay-event advance. |
| `func_demo_00400538` | ⬜ asm | Main per-frame demo update/render tick (largest at 984 bytes, 27 locals, 1 float) — drives the demo state machine, likely indexing the big D_demo_00400C40+ data/jump table. |
| `func_demo_00400910` | ⬜ asm | Callback registered with an export table (has 1 argument, no return) — likely a GUI/model draw or completion callback for the demo scene. |
| `func_demo_004009F8` | ⬜ asm | Support routine (460 bytes) — probable demo/replay data setup or timing/countdown handling. |
| `func_demo_00400BC4` | ⬜ asm | Small callback (116 bytes, 1 arg) registered with an export table — likely a load-complete or per-frame notify hook. |

</details>

**Gaps:** 6 of 7 functions are unmatched GLOBAL_ASM with no checked-in .s, so bodies are inferred from name/size/args/sibling (intro) parallels, not read. The exact semantics of func_demo_00400538 (the main tick) and the large D_demo_00400C40 table are unverified — could be a jump table, a replay-event script, or packed strings/coords. gSceneExports/gReplayExports slot meanings are inferred from intro.c. To fully resolve: disassemble func_demo_00400290/00400538/004009F8 and dump the D_demo_00400C40 region to confirm it is a step/jump table and to rule out any hidden viewport/scissor setup.


---

### `intro` — Attract  (6/6 matched, confidence: high)

**Role:** Attract-mode / intro-demo sequence controller: loads a car+track demo, plays a canned replay ("intro replay") with title-banner sprites and music, cycles through 6 demo segments, and bails to the main menu on button press.

**Invoked by:** Runs as the "intro" relocatable overlay module (uvLoadModule/entrypoint at 0x400000). It is the game-state driver for the attract/demo state: gGameSettings->gameStateFlag is set to 0xE (exit to menu) on button press or after the 6th segment, and to 2 (load/race a demo track) to advance to the next demo segment. gGameSettings->finishedIntroCount (0..5) indexes which demo segment is active. Entry point installs three exported hooks into Intro_Exports (0x00 exit=004004F0, 0x04 per-frame update=004005CC, 0x08 draw=00400820) which the engine's state loop calls each frame. Related fields (introReplayState, dbgOptsRecordIntro) are also touched in game_init.c, gamegui.c, results.c.

**Engine exports used:** `gUvCmidiExports`, `gUvModelExports`, `gUvSprtExports`, `gUvDynExports`, `gGameGuiExports`, `gSndExports`, `gScrnExports`, `gReplayExports`, `gUvAudiomgrExports`, `gSceneExports`, `gUvGuiExports`, `gAiExports`

**Letterbox/viewport relevance:** None. This module draws no black bars, pillars, or scissor clips and configures no viewport. The only "screen" interaction is gScrnExports->unk4(6, &sp70) at init, which registers the demo player's render context (a scrn slot handle), not a scissor rectangle or letterbox band. The visible on-screen elements it owns are two centered title-banner sprites (UVBT, x=0xA0-width/2, y=0x64) and the demo car model — no clipping geometry. Letterbox during the demo race itself comes from the race/camera code, not from intro.

**Readability:** Good/partial. All six functions are fully matched C with descriptive top-of-function comments (init, exit, per-frame, draw, gui callback, model callback). gGameSettings fields are named (finishedIntroCount, numAiCars, currentTrack, gameStateFlag, dbgOptsRecordIntro, introReplayState, optionsMusicVol). Weak spots: every engine export table member is still opaque (unkNN function pointers with pad structs), and the D_intro_* data tables are raw addresses with no named types; the player struct chain (UnkPlayer->unk0/unk4/unkC) is padded/guessed.

**Modification hooks:** To change demo behavior: segment count is the literal 6 in func_intro_004005CC (two `>= 6` checks) and the loop count in the entrypoint uses `finishedIntroCount` as table index. Per-demo car/character = D_intro_00400A04 / D_intro_00400A0C tables; per-demo track = D_intro_004009EC; per-demo music = D_intro_00400A14; per-demo duration = D_intro_004009D4; fade lead time = D_intro_004009C8. Title-banner sprite position: the temp_a2 computation and the gUvSprtExports->unk1C(...,2,temp_a2,0x64,0) calls at lines 228-230 (x = 0xA0-width/2 center, y = 0x64) in the entrypoint. To hide the banners: func_intro_00400820 / the D_intro_004009D0 flag. To change the skip input: the (sp1C & 0x1000)|(sp1C & 0x8000) button mask in func_intro_004005CC. Exit-to-menu target state = 0xE, advance-to-race state = 2, both set on gGameSettings->gameStateFlag.

**Key data symbols:**

- `D_intro_00400A90` — Main runtime state struct: unk0=spawned demo player, unk4=saved original numAiCars, unk8[2]=the two title-banner sprite handles.
- `D_intro_00400A84 / A88 / A8C` — Pointers to loaded engine modules: 'plyr' (player), 'motn' (motion/animation), 'cara' (car) export tables.
- `D_intro_00400A04 / A0C` — Per-segment byte tables indexed by finishedIntroCount, written into player fields unk408/unk404 (car/character selection for each demo).
- `D_intro_00400A14` — Per-segment u16 music/sound-cue table (passed to gSndExports->unk34).
- `D_intro_00400A20[][2]` — Per-segment pair of UVBT banner-sprite/texture ids loaded and shown as the title graphic for each demo.
- `D_intro_004009D4[] / 004009EC[] / 004009C8` — Per-segment timing table (seconds), per-segment next-track id table, and the fade-out lead time threshold.
- `D_intro_004009D0` — Fade/hide flag: set near end of segment so draw hides the banner sprites.
- `D_intro_00400A98 / 00400A9C` — Sprite handle ids for the two title banners (freed on fade).
- `D_intro_00400A94` — Saved AI-car count restored on exit.
- `D_intro_00400AA8[] / 00400AA0[] / 00400A50` — Save/restore buffers for D_8002CCB0 per-player structs and gGameSettings->unk704C, with D_00400A50 the override value applied during the demo.
- `D_intro_004009C0` — GUI descriptor passed to gUvGuiExports->unk78 in the draw callback.
- `gCurrentReplayEvent` — Handle for the intro-demo replay event (started/recorded/played via gReplayExports).

<details><summary>Functions (6)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_intro_400000` | ✅ matched | Module init/entrypoint: saves & zeroes AI-car/settings, loads plyr/motn/cara modules, spawns the demo player+car, loads UVBT banner sprites (positioned centered-x/y=0x64) and UVMD model 0x116, installs the draw callback and per-frame exports, starts the intro replay and music. |
| `func_intro_004004F0` | ✅ matched | Exported exit handler: restores saved numAiCars and per-player settings (D_8002CCB0/unk704C); if not advancing to a race (gameStateFlag!=2) it resets currentTrack/finishedIntroCount/record+replay flags. |
| `func_intro_004005CC` | ✅ matched | Exported per-frame update: reads replay/controller input; on Start/A (0x1000\|0x8000) jumps to menu state 0xE; otherwise counts down the segment timer, advances finishedIntroCount to the next demo segment (state 2) or ends the demo after 6, sets fade flag D_004009D0, and tears down the demo player/audio/dyn when leaving. |
| `func_intro_00400820` | ✅ matched | Exported draw/tick: when the fade flag D_004009D0 is set, hides/frees the two title-banner sprites (D_00400A98, D_00400A9C) via gUvSprtExports->unk10. |
| `func_intro_0040087C` | ✅ matched | GameGui draw callback (installed via gGameGuiExports->unk1C): builds a GUI object from the current uv gui handle, registers it with the replay system (gReplayExports->unk34) and AI system if present. |
| `func_intro_00400960` | ✅ matched | UVMD model-load callback for model 0x116: when the loaded record id matches 0x116 it hands the motion pointer to the motn module (D_00400A88->unk48); returns 1 on null arg else 0. |

</details>

**Gaps:** No raw-asm gaps — all functions are byte-matched C. Remaining unknowns are semantic, not match-status: (1) the exact meaning of many engine export members (e.g. gScrnExports->unk4 mode 6, gUvSprtExports->unk1C's long arg list, gUvDynExports->unk128/unk120/unk124 which gate on gGameSettings->unkPtr90) are opaque function pointers; (2) the UnkPlayer struct layout (unk0/unk4/unkC substructs) is padding-guessed, so fields like unk408/unk404/unk140/unk118 have inferred rather than confirmed roles; (3) D_intro_00400A20's second dimension pairing and D_intro_004009C0's GUI-descriptor contents are not decoded. Confirming these would require reading the plyr/motn/scrn/uvsprt module sources or the .rodata for the D_intro tables.


---

### `letter` — Other  (1/5 matched, confidence: low)

**Role:** Relocatable "uv"-engine overlay module (fourcc 'lett'), reached via the global export pointer gLetterExports (0x80025CE0). Almost entirely raw asm; the one matched function is a small accessor into a 6-entry x 0x14-byte data table (D_letter_00400480). Best evidence (game_init.c annotation + snd.h LADYBUG entry) indicates this is the Beetle Battle "ladybug/letter" collectible-pickup module, NOT cinematic letterbox.

**Invoked by:** Loaded on demand as a uv relocatable module; its export pointer gLetterExports is zero-initialized in gameInit()/game_init.c (annotated "Ladybug pickups") alongside the other in-game gameplay module pointers (gBattleExports, gPowerupExports, gWeaponExports, gRainExports, etc.), i.e. it belongs to the in-race / Beetle Battle gameplay state, not the menu or a boot/cinematic sequence. No explicit uvLoadModule('lett') call is present in the decompiled C (loading is done generically via the module system), and no sibling C module currently references func_letter_* or D_letter_* — all real logic still lives in unwritten asm.

**Letterbox/viewport relevance:** No positive evidence of letterbox/black-bar/pillar/viewport/scissor behavior. Despite the module NAME 'letter' and the priority hint, all concrete evidence points the other way: game_init.c annotates gLetterExports as "Ladybug pickups", and snd.h documents a LADYBUG sound as the "Beetle Battle ladybug pickup". A cinematic letterbox would need only 2 bar rectangles, whereas this module's table has 6 x 0x14 entries — inconsistent with top/bottom bars but consistent with a set of pickup/spawn definitions. The matched code (func_letter_004003E8) does pure table indexing with no viewport/scissor/color/draw calls. I could NOT locate any gSPViewport/gDPSetScissor-equivalent or bar-fill code because the three substantive functions are still raw asm. Recommend treating letterbox as MISATTRIBUTED to this module until the asm is decompiled; the actual cinematic letterbox is more likely driven by camera/ASM in a race/scene/scrn module per prior findings (bar-race-letterbox-widescreen memory).

**Readability:** Mostly RAW. 4 of 5 functions are unnamed func_letter_XXXX behind GLOBAL_ASM pragmas with no C bodies; the sole data symbol is raw D_letter_00400480 with undecoded contents. Only func_letter_004003E8 is matched, and even it is unnamed/untyped (returns void*, takes an unnamed s32). No Letter_Exports_s struct typedef exists (gLetterExports is a bare void*). The only human annotation is the single "// Ladybug pickups" comment in game_init.c.

**Modification hooks:** To change the 6 pickup/definition records, edit the bytes of D_letter_00400480 (the 6 x 0x14 table) in the letter overlay (splat segment 'letter', start 0x5DDE8 in beetleadventurerac.us.yaml). To gate behavior on index, func_letter_004003E8's clamp (arg0<0 || arg0>=6) in letter.c is the one editable C hook. To rename/type the module for modifiability: add a Letter_Exports_s typedef and retype gLetterExports (global_exports.h:18, symbol_addrs.txt:231 gLetterExports=0x80025CE0), then name the exported func_letter_* in the (still-asm) entrypoint. NOTE: if a dev is actually hunting for the cinematic letterbox black bars, this module is the WRONG place — redirect to the race/camera ASM per the letterbox memory note; do not expect to move/hide/recolor letterbox by editing letter.c.

**Key data symbols:**

- `D_letter_00400480` — Table of 6 entries x 0x14 (20) bytes = 0xC8 bytes of static definition data indexed by func_letter_004003E8. Contents not yet decoded (asm not checked in); given the 'Ladybug pickups' annotation on gLetterExports, most plausibly 6 pickup/spawn-type or ladybug-value definition records. NOT confirmed to be letterbox bar geometry.
- `gLetterExports` — Global export-table pointer for this module at 0x80025CE0; typed as opaque void* (no Letter_Exports_s typedef exists yet). Annotated 'Ladybug pickups' in game_init.c.

<details><summary>Functions (5)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_letter_400000` | ⬜ asm | Module entrypoint/relocation stub: populates the letter exports jump table (pointed to by gLetterExports) with this module's exported function pointers when the overlay is loaded. |
| `func_letter_00400140` | ⬜ asm | Unknown module routine (likely init/setup or per-frame update for the letter/ladybug pickups); body is raw asm, purpose inferred only from position as first real function after entrypoint. |
| `func_letter_00400230` | ⬜ asm | Unknown module routine (likely the main per-frame update or draw for the pickups); raw asm, largest gap between it and the accessor suggests substantial logic. |
| `func_letter_004003E8` | ✅ matched | Table accessor: given index arg0, returns D_letter_00400480 + arg0*0x14, clamping out-of-range (<0 or >=6) to entry 0 — i.e. fetches one of 6 fixed 0x14-byte pickup/definition records. |
| `func_letter_00400420` | ⬜ asm | Unknown module routine following the accessor (possibly a helper that consumes the table entries returned by func_letter_004003E8, e.g. spawn/render/collision per pickup); raw asm. |

</details>

**Gaps:** Three of the module's functions (__entrypoint_func_letter_400000, func_letter_00400140, func_letter_00400230, func_letter_00400420) are unmatched raw asm not checked in, so their actual behavior — and whether they call any uv export tables (gUvSprtExports/gUvGuiExports/gUvGfxMgrExports) or draw anything — is unknown. The contents/layout of the 6x0x14 D_letter_00400480 table are undecoded. Critically, the module's true role (ladybug/letter pickups per game_init.c + snd.h) CONTRADICTS the task's 'cinematic letterbox' hint; confirming this requires decompiling func_letter_00400140/230/420 to see whether they spawn/score pickups vs. draw bars. No caller of func_letter_* exists in the decompiled C (all invocation is via the generic module/exports mechanism), so the exact game-state trigger and which sibling (likely battle.c, all still asm) drives it is unconfirmed.


---

### `med` — Other  (12/40 matched, confidence: low)

**Role:** Standalone UVMO overlay module (own entrypoint @0x400000) implementing an animated 3D scene/object: it decodes an ARGB tint color, drives texture animation via gUvTexAnimExports, and wires a camera via gCamExports. Not a 2D HUD, medal, or menu — it uses no font/sprite/GUI export tables.

**Invoked by:** Dispatched by the uv-engine module loader (overlay table), not by direct C calls — it has its own module entrypoint __entrypoint_func_med_400000 (=0x400000) and self-contained address space, so no cross-module C caller appears in the decomp (confirmed: no external references to func_med_*/D_med_* anywhere in src/). Standard overlay lifecycle: __entrypoint -> init/load (func_med_00400160) -> per-frame update/draw (texanim + camera) -> unload. Exact triggering game state is unpinned; likely loaded for a specific in-game object/scene ("med" UVMO asset). Registered in config/us/modules.yaml and beetleadventurerac.us.yaml (name: med).

**Engine exports used:** `gCamExports`, `gUvTexAnimExports`

**Letterbox/viewport relevance:** None. This module draws no black bars, pillars, or color-bar test screens, and configures no viewport or scissor clip. It sets a camera parameter (gCamExports+0x8), an ARGB object tint, and steps texture animation (gUvTexAnimExports) — none of which touch letterbox/pillarbox/scissor geometry. It uses none of the gUvGfxState / scissor / viewport export tables that would control screen clipping. Not letterbox-relevant.

**Readability:** Mostly raw. All 12 data symbols are D_med_XXXXXXXX and all 40 functions are func_med_XXXXXXXX / __entrypoint_func_med_400000 with no descriptive names, no struct typedefs, and no comments. 12 functions have C bodies (matched); the color-decode (func_med_0040144C) and texanim tick (func_med_004063D4) are readable-in-effect but still unnamed. The remaining 28 are #pragma GLOBAL_ASM (unmatched, raw asm not checked in). Export accesses are still raw offset-into-pointer casts ((u8*)gCamExports + 0x8), not named struct members. Overall: needs a scribe/naming pass.

**Modification hooks:** Object tint/recolor: edit func_med_0040144C (lib/bar-decomp/src/modules/med.c:88-93) or the ARGB value its callers pass, which lands in D_med_00407FE4/E8/EC/F0. Camera behavior: func_med_004008C0 (med.c:70-72) forwards to gCamExports+0x8. Texture-animation on/off or rate: func_med_004063D4 (med.c:160-163) — remove/gate the two gUvTexAnimExports calls. Visibility/update-path toggle: flip D_med_00406C00 to change which draw routine func_med_00401174 runs (med.c:78-84). To hide the whole object you would stub the active draw routine(s) (func_med_00400D18 / func_med_004008F0) or prevent the module load via config/us/modules.yaml (name: med) / the loader. Cleanup/lifetime: D_med_00407FDC sentinel gates func_med_00406204 -> func_med_004014C4.

**Key data symbols:**

- `D_med_00406C00` — Mode/branch flag selecting update path in func_med_00401174 (0 -> func_med_00400D18, else func_med_004008F0).
- `D_med_00407F28` — Base of an object/state struct block; +0x0 = current object pointer, +0x2C = a byte value copied to object+0x23 (func_med_00404FB8).
- `D_med_004080A0` — Pointer to an engine/resource exports vtable; +0x10 is a register/play method (used by func_med_00405128/00405168).
- `D_med_00407F64` — Resource/handle id argument paired with D_med_00407FB0 and constant 0xFA.
- `D_med_00407F68` — Resource/handle id argument paired with D_med_00407FC0 and constant 0xFB.
- `D_med_00407FB0` — Buffer/param struct passed by-address for resource id 0xFA.
- `D_med_00407FC0` — Buffer/param struct passed by-address for resource id 0xFB.
- `D_med_00407FDC` — Allocated-handle sentinel; -1 = none, so func_med_00406204 only cleans up (func_med_004014C4) when it is set.
- `D_med_00407FE4` — Object tint color, Alpha channel as f32 0..1 (from func_med_0040144C ARGB decode).
- `D_med_00407FE8` — Object tint color, Red channel as f32 0..1.
- `D_med_00407FEC` — Object tint color, Green channel as f32 0..1.
- `D_med_00407FF0` — Object tint color, Blue channel as f32 0..1.

<details><summary>Functions (40)</summary>

| Function | Status | Purpose |
|---|---|---|
| `__entrypoint_func_med_400000` | ⬜ asm | Module entrypoint/dispatch table at load base 0x400000 (overlay entry vector). |
| `func_med_004000DC` | ⬜ asm | Early init/setup helper (referenced by module reloc at rom 0x64/0x70; likely called from entrypoint). |
| `func_med_00400160` | ⬜ asm | Main module init/load routine (largest early function, 1580 bytes; referenced by reloc 0x68/0x74). |
| `func_med_0040078C` | ✅ matched | Thin wrapper that calls func_med_004021A0 (kicks off a sub-init/update step). |
| `func_med_004007AC` | ✅ matched | Empty stub (no-op lifecycle hook). |
| `func_med_004007B4` | ⬜ asm | Setup/update helper (268 bytes). |
| `func_med_004008C0` | ✅ matched | Camera setter: forwards *(s32*)arg0 to gCamExports+0x8 (installs/updates a camera parameter). |
| `func_med_004008F0` | ⬜ asm | Object/scene update or draw routine (1064 bytes; called by func_med_00401174 when D_med_00406C00 != 0). |
| `func_med_00400D18` | ⬜ asm | Alternate update/draw routine (called by func_med_00401174 when D_med_00406C00 == 0). |
| `func_med_00401174` | ✅ matched | Per-frame dispatcher: branches on D_med_00406C00 flag to func_med_00400D18 or func_med_004008F0. |
| `func_med_004011B4` | ⬜ asm | Update/step routine. |
| `func_med_0040144C` | ✅ matched | Decodes packed ARGB32 (arg0) into 4 float color components (0..1) at D_med_00407FE4/E8/EC/F0 — object tint/lighting color. |
| `func_med_004014C4` | ⬜ asm | Reset/clear or reinit routine (invoked by func_med_00406204 when D_med_00407FDC != -1). |
| `func_med_00401C10` | ⬜ asm | Update/draw routine. |
| `func_med_00401EF8` | ⬜ asm | Update/draw routine. |
| `func_med_004021A0` | ⬜ asm | Sub-init/update step (target of wrapper func_med_0040078C). |
| `func_med_004023E0` | ⬜ asm | Update/draw routine. |
| `func_med_00402920` | ⬜ asm | Update/draw routine. |
| `func_med_00402C7C` | ⬜ asm | Update/draw routine (largest region ~0x2300 bytes to next fn — core object/scene logic). |
| `func_med_00404FB8` | ✅ matched | Copies a byte from D_med_00407F28+0x2C into (object ptr)+0x23 when object pointer (D_med_00407F28) is non-null (per-object field write). |
| `func_med_00404FE0` | ⬜ asm | Object field setter/update. |
| `func_med_0040505C` | ⬜ asm | Object field setter/update. |
| `func_med_00405128` | ✅ matched | Calls D_med_004080A0+0x10 with (D_med_00407F64, &D_med_00407FB0, 0xFA) — registers/plays a resource id 0xFA (likely sound or texanim handle via an exports table pointer). |
| `func_med_00405168` | ✅ matched | Same as above with (D_med_00407F68, &D_med_00407FC0, 0xFB) — registers resource id 0xFB. |
| `func_med_004051A8` | ⬜ asm | Update/draw routine. |
| `func_med_004052C8` | ⬜ asm | Update/draw routine. |
| `func_med_00405338` | ⬜ asm | Update/draw routine. |
| `func_med_004053B4` | ⬜ asm | Update/draw routine. |
| `func_med_00405488` | ⬜ asm | Update/draw routine. |
| `func_med_004057A8` | ⬜ asm | Update/draw routine. |
| `func_med_004059E8` | ⬜ asm | Update/draw routine. |
| `func_med_00405DC4` | ⬜ asm | Update/draw routine. |
| `func_med_00405FEC` | ⬜ asm | Update/draw routine. |
| `func_med_00406204` | ✅ matched | Conditional cleanup: calls func_med_004014C4 when D_med_00407FDC != -1 (frees/resets an allocated handle). |
| `func_med_00406238` | ⬜ asm | Update/cleanup routine. |
| `func_med_004062BC` | ⬜ asm | Update/cleanup routine. |
| `func_med_004063CC` | ✅ matched | Empty stub taking s16 (no-op hook). |
| `func_med_004063D4` | ✅ matched | Texture-animation tick: calls gUvTexAnimExports+0x8 then +0x4 (advance/draw texanim frames for the object). |
| `func_med_00406418` | ✅ matched | Empty stub (no-op lifecycle hook, likely shutdown). |
| `func_med_00406420` | ⬜ asm | Final routine (shutdown/unload or tail helper). |

</details>

**Gaps:** 28 of 40 functions are unmatched raw asm (bodies not checked in), so the module's exact object/scene identity and full draw pipeline are inferred, not proven — hence low confidence on the specific classification. The literal meaning of "med" (e.g. a named track object, medal 3D prop, or medium/scenery entity) is unconfirmed; no doc names it. The 0xFA/0xFB resource ids in func_med_00405128/168 and the D_med_004080A0 exports table (+0x10 method) are unidentified — could be sound (gSndExports-like) or texanim registration. Which game state loads this overlay is unpinned (no C-side caller because it is a dispatched overlay). To close: disassemble func_med_00400160 (init) and func_med_00402C7C (largest draw) and identify the D_med_004080A0 vtable and 0xFA/0xFB ids.


---


## Risks & unknowns
- **The exact `cam.c` writer of the TOP/BOTTOM y-insets is unread.** The `unk218`/`unk21A` mechanism is inferred from the channel struct + gfx-manager pipeline, not seen. Asm extraction of the `func_cam_00400190/00400C0C/00401D3C` region is required to name the writer and confirm a byte-exact edit site (vs. the choke-point clamp in `uvchannel_rom.c:445`).
- **Split-screen quadrant coordinates and any inter-view gutter are inferred.** They live in unmatched `scrn.c`/`plyr.c`/`battle.c` asm; resizing (not just black-margining) split-screen requires decompiling those first.
- **Crop-vs-pillarbox is a NEW code path on the RT64 fork**, not a flip of an existing flag — `fromHDtoWindow` has no config gate today. Needs a new runtime toggle (à la `BAR_PRESENT_MODE`) plus menu wiring.
- **Live-verification gap:** it's unconfirmed the user is actually seeing L/R pillars at their real window size — at 960×720 (4:3) there should be none. A screenshot at their true window size would disambiguate wide→pillars vs. tall→VI letterbox vs. the separate game cinematic letterbox.
- **Widescreen interaction unverified on a captured frame:** whether `Expand` changes `vi.fbSize()` (code says NO — it rescales within the same 4:3 framebuffer, so VI source stays 4:3), and whether cropping cuts ~1/8 off top+bottom unless `Expand` widens first. RenderDoc/screenshot confirmation recommended before shipping a combined widescreen default.
- **Cannot fully rule out HUD-local scissor in unmatched draw cores** of `gamegui`, `results`, `battle` — nothing in matched code or data symbols suggests it, but the `.s` bodies aren't checked in, so it's inference, not proof.
- **The whole matching track is blocked** on standing up baserom-SHA + `.venv`/IDO/splat asm extraction; none of track (b) is actionable until `make` emits the asm and a matching build is confirmed.

Key file paths: `lib/bar-decomp/src/modules/uvchannel_rom.c` (L242-247, L435-463), `lib/bar-decomp/src/modules/uvgfxmgr_rom.c` (L585-586, L592-680), `lib/rt64/src/render/rt64_vi_renderer.cpp` (L26-42, L91-126), `src/game/config.cpp` (L130), `src/main/rt64_render_context.cpp` (L115-129), `src/main/main.cpp` (L197), `lib/bar-decomp/src/modules/letter.c`, `lib/bar-decomp/src/modules/cam.c`.
