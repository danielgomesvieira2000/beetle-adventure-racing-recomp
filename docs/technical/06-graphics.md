# 06 — Graphics

BAR draws through stock F3DEX2, which RT64 handles at HLE, so the renderer needed no microcode work.
Everything hard about the graphics is about **geometry of the frame**: the game was authored for a
320×240 framebuffer with a CRT overscan margin and a 4:3 frustum, and a modern display shows every
pixel of a window that is usually not 4:3.

This chapter is the reference; the running experiment log with every measurement and negative result
is [`../RACING_DRAW_MAP.md`](../RACING_DRAW_MAP.md).

## The drawing pipeline, game side

Per frame the UV gfx manager:

1. Clears the **whole** framebuffer to black — `gDPSetFillColor 0x00000000` plus a full-screen
   `gDPFillRectangle` (`uvgfxmgr_rom.c:585-586`, `func_uvgfxmgr_rom_00401914`).
2. Emits a viewport and scissor for the current clip rect —
   `func_uvgfxmgr_rom_00401C5C` (`uvgfxmgr_rom.c:669-680`):
   `gSPViewport(&vp->vp)` and
   `gDPSetScissor(G_SC_NON_INTERLACE, vp->x0, sScreenHeight - vp->y1, vp->x1, sScreenHeight - vp->y0)`.
3. Draws only inside that rectangle.

**Pixels outside the rect are never repainted, so they stay black. That *is* the bar.** There is no
"draw black bars" code to find, which is why the letterbox investigation initially looked for
something that does not exist.

The rectangle comes from `uvGfxClipRect` / `func_uvgfxmgr_rom_00401BD4` into the viewport stack
`D_uvgfxmgr_rom_004022C8[]`. For the 3D/race view it comes from the camera **channel**:
`func_uvchannel_rom_00401278` (`uvchannel_rom.c:435-463`) applies per-channel fields
`unk214/216/218/21A` = `x0/x1/y0/y1`, defaulting to full screen.

### The two axes have different causes

This distinction took a three-lens investigation to establish, and confusing them wastes days:

* **Top/bottom cinematic bars are game-drawn, in the ROM.** The camera shrinks the channel's
  `unk218` (top) / `unk21A` (bottom) inward over time. The exact per-frame writer is in still-unmatched
  `cam.c` assembly (around `func_cam_00400190` / `00400C0C` / `00401D3C`) — **inferred from the call
  shape, not read**.
* **Left/right pillars are produced host-side only**, by RT64's final VI-to-window blit,
  `VIRenderer::fromHDtoWindow` (`lib/rt64/src/render/rt64_vi_renderer.cpp`). No game code ever
  narrows `x0/x1` for effect. `fromHDtoWindow` fits the fixed-4:3 `hdSize` to the window with one
  uniform scale; on a window wider than 4:3 it fits to height, leaving unpainted margins.

**Why `ar_option = Expand` does not remove the pillars.** Expand acts on a *different layer*: it sets
`aspectRatioScale`, consumed by the 3D framebuffer renderer and projection processor, which widens
the field of view *inside* the still-4:3 framebuffer. It never touches `fromHDtoWindow`, which still
treats the result as 4:3 and pillarboxes it. Expand widens the view *within* the pillars.

## The overscan inset, and the patch that removes it

Measured from the scissor rectangles the game emits (`BAR_DBG_SCISSOR` in the RT64 fork): racing is
drawn into **x 22..297 of 320, y 17..224 of 240** — a 275×207 rectangle. That is a deliberate
CRT-overscan-safe margin. A television bezel hid it; a monitor shows it as black bars on all four
sides.

It caused two further problems downstream:

1. RT64 only widens a projection that reaches the framebuffer's edges, so the inset viewport was
   never widened and Expand did nothing during a race.
2. Once that check was relaxed, the widened 3D drew *outside* the game's own margin fills, which
   stayed at their 4:3 positions — the black columns visible in early widescreen captures.

`patches/viewport_patch.c` fixes the cause rather than compensating for each effect. It replaces
`uvGfxClipRect` with the original body plus one change: when the screen really is 320×240, the four
known overscan edges are snapped to the true screen edges before the maths runs.

```c
#define BAR_INSET_X0 22
#define BAR_INSET_X1 297
#define BAR_INSET_Y0 16     /* NOT 17 */
#define BAR_INSET_Y1 223    /* NOT 224 */
```

Two design decisions in that patch are worth copying:

* **Only the four known edges are snapped.** Interior edges — a split-screen boundary, the intro's
  letterbox — are left exactly as the game asked, so two-player viewports and the cinematic bars keep
  their proportions.
* **No data symbols are used.** The module is relocatable at runtime (its recompiled code reaches
  `sScreenWidth` through a section-relative relocation), so a patch baking absolute addresses could
  read or write the wrong memory. Screen size comes from the module's own exported
  `uvGetScreenWidth()` / `uvGetScreenHeight()`. The four "last rectangle" globals the original wrote
  are left alone, because the caller invokes `func_uvgfxmgr_rom_00401C5C` immediately afterwards,
  which stores the same values from `vp->x0..y1` anyway.

**And one trap that cost a full build cycle: Y is bottom-up in this API.** The scissor is emitted as
`(x0, height - y1, x1, height - y0)`, so scissor rows `17..224` are `y0 = 16, y1 = 223` in viewport
terms. The first version used the scissor rows directly, the vertical snap silently never matched,
and the patch compiled and linked and did half its job.

## The frustum BAR draws and culls against

**Every perspective projection in the game is built in one function** — `uvfmtx_rom`'s `glFrustum`:

```
func_uvfmtx_rom_00401F74(Mtx4F *dst, float left, float right, float top, float bottom,
                         float near, float far)
```

Its caller — `uvchannel_rom` case 4 — stores the same six values in the camera channel
(`unkDC..unkF0`) and then calls `func_uvchannel_rom_00401658`, which derives **six culling planes**
from them. `func_uvchannel_rom_004014E8(chan, x, y, z, radius)` is the sphere-vs-frustum test the
game asks before submitting anything.

So one set of six numbers decides both **what the projection draws** and **what the game bothers to
submit**. That coupling is the single most important renderer fact about this game.

Measured in a race with `BAR_DBG_FRUSTUM=1`, two projections are built per frame:

| Destination | left/right | top/bottom | near | far | What it is |
|---|---|---|---|---|---|
| `0x80099E1C` | ±0.7673 | ±0.5711 | 1 | **300** | the racing camera (`&channel->unk4`) |
| `0x80025920` | ±0.7673 | ±0.5711 | 1 | 27000 | a second, distant/sky matrix from the same channel fields |

`0.7673 / 0.5711 = 1.344` — **the game's own frustum is 4:3**, and the racing draw distance is
`far = 300`.

### What the port does about it

`src/main/bar_frustum.cpp`, called from the top of the recompiled `func_uvfmtx_rom_00401F74` via
[`fix-recompiled.sh` rule H](07-codegen-fixups-and-patches.md#rule-h--frustum-draw-distance-and-culling-width):

* **Draw distance.** The far plane is multiplied by the frontend's *Graphics → Draw Distance* setting
  (1× / 2× / 4×, default 4×), or by `BAR_DRAW_DIST` when set (which wins, so a debugging run can pin
  a value while the menu is being changed). It is applied to **both** the projection matrix and the
  channel field — moving the matrix's far plane alone would leave the game culling everything past
  the old one, and nothing new would appear. Projections at or beyond `far = 5000` are skipped, so
  the 27000 sky matrix keeps its depth range.
* **Culling width.** The channel's stored left/right are widened by `BAR_CULL_WIDEN` (default
  **1.75**, which covers 21:9; 16:9 needs 1.333). This is applied **only to the channel, never to the
  projection matrix**: RT64 already widens the drawn view, and widening it here as well would stack.
  So the game draws its usual 4:3 projection (which RT64 expands) while agreeing to submit what falls
  in the wider view. The default is deliberately generous, because the error is asymmetric — culling
  slightly too wide costs a few draws that are then clipped, culling too narrow is a visible pop-in
  as the camera turns.

**Reaching the channel without a module data symbol.** The channel is heap-allocated by a relocatable
module, so its address cannot be baked in. It does not need to be: `dst` **is** `&channel->unk4`, and
`unk4` is at offset 4, so the channel is `dst - 4`. Before writing anything, all six stored fields
are compared against the six arguments — if the memory does not hold exactly what was passed, this is
not a channel and it is left alone. That guard is what keeps the second, non-channel projection at
`0x80025920` safe.

**A trap this produced.** Widening the channel's left/right leaked: the game builds that second
matrix from the same fields on a later frame, and it came out at aspect 2.35 against the main view's
1.34. The hook now remembers the exact bit patterns it wrote and restores the originals whenever a
projection is built from them. Matching on bit patterns is exact — a value we did not write cannot
collide with one we did — so the culling planes see the widened sides and no projection matrix does.

## Anchoring the HUD to a widened frame

BAR emits no extended-GBI origins, so by default RT64 places every 2D element by scaling it about the
frame's centre. Proportions are correct, but the whole HUD huddles in the middle 4:3 region with
empty picture on either side. The RT64 fork (`src/hle/rt64_bar_hud.cpp`) classifies BAR's 2D draws
itself and supplies the origins a game written for the extended GBI would have emitted.

Measured HUD extents, in game pixels:

| Element | Extent | Anchor |
|---|---|---|
| Speedometer dial (gear digits at `(60,36)-(74,53)`) | `(25,21)-(77,53)` | left |
| Timer (digits along `y 27..51`) | `(208,21)-(293,42)` | right |
| Lap | `(236,51)-(292,68)` | right |
| Course map (blip at `(260,186)-(266,192)`) | `(235,149)-(291,205)` | right |
| Position | `(232,205)-(290,222)` | right |
| "GET READY" / "GO!" messages | `y 79..154`, centred | none |

Four findings, each of which bought a specific correctness property:

* **The HUD lives in two horizontal bands, with the race messages between them.** An element is
  anchored only if it lies entirely in the top 33 % or the bottom 40 % of the screen. Horizontal
  position alone cannot separate them — the timer's leading digit is at `x 205..212` and a message
  glyph at `x 218..230`.
* **Anchoring is a translation, not a stretch.** Giving both edges of an element the same origin maps
  a left-anchored element's game `x` to `x × mult` and a right-anchored one to `(x + 106.67) × mult`
  — pure shifts of 53⅓ game pixels at 16:9. Every element on a side moves by the same amount, so the
  map keeps its blip and the timer keeps its digits.
* **The scissor has to travel with the element.** BAR scissors its HUD to the whole 320×240 screen,
  which converts to the centred 4:3 region, so an element moved to an edge is drawn outside its own
  scissor and vanishes. This is exactly how the speedometer needle disappeared while its dial
  anchored correctly.
* **The needle is orthographic geometry, not a texture rectangle.** It draws under an orthographic
  projection (320 wide). Confirmed twice: leaving orthographic projections unscaled moved the needle
  out of the dial while nothing else changed, and anchoring that projection by giving it a left
  viewport origin moved nothing else in a race.

  The claim originally made here — "the *only* orthographic projection in a race frame", and so safe
  to anchor whole — is **false**, and was refuted by the pause screen. A paused race keeps
  `currentGameState == 5`, and the pause menu's darkened backdrop is orthographic geometry too.
  Anchoring every orthographic projection therefore pinned the backdrop to the left edge, where it
  could never be widened: `adjustAspectRatio` requires `viewportOrigin == G_EX_ORIGIN_NONE`. The
  layers are now classified **per projection** instead — see below.

### Projections are classified one at a time

`BarHud::classifyProjection(kind, projectionIndex, scissor, publish)` answers for one projection what
`classifyRect` answers for one rectangle, with the same precedence — panel override, `hud.json`,
built-in table, then the default. The default is the old whole-layer rule: `Left` while racing,
`Center` otherwise.

**Every projection is offered, not only the orthographic ones.** A projection is the only handle
there is on anything drawn as geometry, because individual triangle draws are not hooked. Only
orthographic layers carry a default, so listing the rest changes nothing until one is tagged:

| Identity | Kind |
|---|---|
| `persp:<n>` | the 3D world |
| `ortho:<n>` | 2D drawn as geometry — the needle, the pause backdrop |
| `rect:<n>` | the layer texture rectangles are drawn into |
| `proj:<n>` | anything else |

The identity's number *n* is the projection index **RT64's own debugger already puts on
screen** (`Orthographic #4` in its Game editor), so what is seen there and what is tagged are the
same thing. It is the weakest of the identity schemes, because it is positional: a projection order
that differs between screens moves the tag onto a different layer. Check the outline before promoting
one.

What the classes mean for a whole projection, which is not quite what they mean for a rectangle:

| Class | Effect |
|---|---|
| `left` / `right` | A viewport origin, pinning the layer to that edge. What the needle wants. |
| `center` | No origin; the normal widening path runs, which **compensates** the projection's aspect ratio — content stays proportional and more empty space is revealed either side. |
| `stretch` | Suppresses that compensation (`adjustAspectRatio = false`), so the layer's own 320-wide space maps across the full widened frame and its content stretches with it. |
| `spill` | No meaning for a projection; treated as `center`. |
| `cover` | `stretch`, then magnify the layer's content about its centre by 320/274 and 240/206 (≈ 1.167), putting BAR's overscan-safe rectangle on the frame's edges. `BAR_HUD_COVER_INSET="l,t,r,b"` overrides that rectangle. |

**Two call sites must agree.** `ProjectionProcessor` transforms the layer and `FramebufferRenderer`
places its viewport, and both ask `classifyOrtho` the same question. If they ever disagree the layer
is placed by one rule and transformed by the other. Only the first passes `publish`, so the inspector
lists each projection once.

**A measured negative result: `stretch` does not make BAR's pause backdrop reach the frame's edges.**
It does what it says — the layer's 320-wide space spans the frame — but the backdrop quad is drawn at
BAR's overscan-safe rectangle *inside* that space, so a margin survives. Measured off a 1536×864
capture of a paused race with `ortho:5` and `ortho:7` both tagged `stretch`: the dark region's left
edge sits at 6.8 % of the frame width (≈ 22 game pixels) and its top edge at 8.7 % of the height
(≈ 21 game pixels), against the inset of 22 and 17 recorded in
[`../KNOWN_ISSUES.md`](../KNOWN_ISSUES.md) for the overscan mask.

**`cover` is what closes it**, and it is the class that exists because of this measurement: it
magnifies the layer by the ratio that puts the inset rectangle on the frame's edges (320/274 and
240/206). Confirmed on the pause screen — the backdrop reaches every edge with the projection tagged
`cover` where `stretch` left the inset behind. The cost is inherent: anything the layer draws outside
that rectangle is magnified off the frame, so `cover` is tagged onto a backdrop, never applied by
default.

### Knowing when the HUD is on screen

`currentGameState == 5` is **not** "a race" — it also covers the track-loading card (with its wipe
tiles and record-time text) and the "Get Ready" card, which are full 4:3 compositions that come apart
when pulled to the edges. `raceState` (`gGameSettings + 0x88`) narrows it, but the naive reading was
wrong in a way only visible in motion: the HUD stayed at its 4:3 positions through the whole
countdown and then snapped to the edges the instant the player got control.

The game reads that field as an **s16** — the word's high half, so values arriving host-side as
`0x10000` and `0x30000` are **phases 1 and 3** — and `func_plyr_00402268` runs the player update for
**phase 0 or phase 3**. That is the game's own grouping for "the car is on the track":

| Phase | On screen | HUD drawn? | Anchored |
|---|---|---|---|
| 1 | "Coventry Cove" loading card, then "Get Ready" | no | no |
| 3 | 3-2-1 countdown, car on the grid | **yes** | **yes** |
| 0 | the race running | yes | yes |

One more wrinkle: phase 0 is also what the field reads *stale* at the moment `currentGameState`
becomes 5, over the film-roll wipe out of the Rumble Pak prompt. So phase 0 only counts once this
race has been seen setting up, reset on any state change. The final gate is
`currentGameState == 5 && sawSetup && (phase == 0 || phase == 3)`, which arms exactly once per race —
so there is no transition to see.

Only the host side can read that out of RDRAM, so `src/main/os_unimpl_stubs.cpp` pushes the answer
into RT64 with `bar_rt64_set_hud_anchor(int racing)` from the SI poll.

Switches: `BAR_HUD_ANCHOR=0` disables the positional heuristic (but **not** tags — see below),
`BAR_HUD_ORTHO=0` keeps the rectangles anchored but leaves the needle's layer centred (this is how
the two halves are told apart on screen), and `BAR_HUD_TRACE=1` prints state, the racing flag and
every rectangle with the class it was given. The player-facing control is **HUD Ratio**: *Full*
anchors to the frame's edges, *Clamp16x9* anchors only as far as 16:9 (so an ultrawide keeps 16:9
positions), *Original* restores centred placement.

### Tags, and the five classes

The positional heuristic above is only the second of two things that decide an element's class. The
first is a **tag**, looked up by the element's **identity**, which is built from whichever of three
schemes fits the draw:

| Draw | Identity | Second |
|---|---|---|
| Textured | `tex:<G_SETTIMG address>` | `dl:<display list>` |
| Fill cycle | `fill:<fill colour>` | `dl:<display list>` |
| Untextured, not fill | `dl:<display list>` | `untex` |

The split is not cosmetic, and was found the expensive way: an untextured rectangle carries whatever
`G_SETTIMG` last set, so naming one by texture address gives two unrelated draws the same identity.
The pause screen's translucent black overlay and the menu text beneath it shared a `tex:` address,
and tagging the overlay stretched the text instead. Tags come from the HUD inspector's live dropdown and
from `hud.json` in the settings folder, and unlike the heuristic they apply **everywhere**, menus
included. That split is deliberate: the heuristic is a guess that is only safe during a race, while a
tag is somebody's explicit decision about one element.

`classifyRect` therefore runs for every 2D rectangle now, not only during a race — it is also what
publishes the frame's elements to the inspector. It returns `Class::Center` after a single branch
when there is no inspector, no tag table and no race in progress.

The class an element can be given, and what `RDP::drawRect` emits for it:

| Class | Emitted |
|---|---|
| `center` | Nothing; RT64's default centring stands. |
| `left` / `right` | `G_EX_ORIGIN_LEFT` / `_RIGHT` on **both** edges (a translation, not a stretch), plus the widened scissor that has to travel with it. |
| `stretch` | `rectAspect = G_EX_ASPECT_STRETCH`, origins left `G_EX_ORIGIN_NONE`, **plus the widened scissor**. Means "do not squeeze this to 4:3" — a 320-wide rectangle reaches the frame's full width and a narrower one is widened about its centre by the same factor. It does **not** pin an element's edges to the frame's; that is what the origins do. The scissor has to travel with it for the same reason it travels with `left` and `right`: BAR scissors its 2D to the whole 320×240 screen, which converts to the centred 4:3 region, so a widened rectangle without a widened scissor is clipped straight back and looks exactly like the tag never applied. `wave-race-64-recomp` does not need this for its own stretch class. |
| `spill` | Placement untouched; only the scissor is widened, for that one draw. The only class about clipping rather than placement — for an element that is the right size in the right place and is merely cut off where the 4:3 frame used to end. BAR scissors a race to `x 8..311`, so an element drawn from `x 0` has eight units hidden even on hardware. |

`stretch` and `spill` are new with the inspector and have no built-in users yet: nothing in
`classifyByPosition` returns them, so they reach the renderer only through a tag. Treat them as
untested against real BAR elements until something has been tagged with each and looked at.

### Seeing what it decided

`BAR_HUD_TRACE=1` is a log read afterwards and matched to a screenshot by eye, which is a bad
instrument for something that lasts a fraction of a second and whose identity a picture cannot show.
**`docs/HUD-INSPECTOR.md`** documents the replacement: an F1 window listing every element of the
current frame with its identity, extent and class, hover-to-outline, and a dropdown that changes the
class from the next frame. The port half is `src/main/bar_inspector.cpp`; the hook it fills is four
null-by-default function pointers in `lib/rt64/src/hle/rt64_bar_hud.h`, so the fork still builds and
behaves identically standalone.

## The two coverage tests

A bug worth recording as a pattern. RT64 had **two different coverage thresholds** for "does this
projection reach the framebuffer edges": the projection processor widened at ≥ 80 % coverage
(`BAR_ASPECT_COVER`), while the framebuffer renderer's `useWideViewport` demanded 100 %. BAR's sky
projection has a viewport one pixel narrower than the scissor — 319 against 320, because
`uvGfxClipRect` clamps the viewport to `sScreenWidth - 1` — so it passed one test and failed the
other. Its matrix was widened while its viewport and scissor stayed 4:3: the wider view was squeezed
back into the old rectangle and clipped there, so the sky ended at the edges of the *un-widened*
frame, fixed on screen whatever the camera did.

**Whenever a test like this exists twice, they must be the same test.** The fix was to give the
framebuffer renderer the same threshold.

The general form of this trap: **any experiment that widens a projection matrix without widening its
viewport and scissor together is worthless**, because the layer is drawn wider and clipped straight
back, which looks identical to no change at all.

## Settings and where they are registered

Graphics settings persist to `graphics.json` in the config directory and are pushed into the runtime
before `recomp::start()`.

The **Draw Distance** option is registered from `src/frontend/bar_frontend.cpp`, not from
RecompFrontend, because that submodule is upstream and this port cannot push to it —
`create_graphics_tab()` returns the tab's `Config`, so an extra option can simply be added to it. Two
ordering constraints make it work:

* The option must be added **before** `recompui::config::finalize()`, which is what reads
  `graphics.json`. An option added after it would never see its saved value.
* The value is pushed to the renderer from `pump_events`, **not** through `set_save_callback`,
  because the Graphics tab has already installed its own save callback and replacing it would
  silently stop every other graphics setting from applying.

Current graphics surface: aspect ratio (`ar_option`), HUD ratio (`hr_option`), refresh-rate mode
(`rr_option`, default `Display` — frame interpolation on, with the game/VI/audio clock still locked
at 60 Hz), internal resolution and supersampling (`ds_option`), MSAA, the VI "divot" seam filter, and
draw distance.

`BAR_DBG_GFX=1` reports what actually reached RT64 — widescreen depends on `aspectRatio == Expand`
surviving into RT64's `userConfig`, and printing it beats trusting the JSON on disk.

## Rendering facts established by experiment

Positive:

* The track-select course name and text are **wide texture rectangles** (≥ 150 px) — `BAR_SKIP_WIDE=150`
  removes them.
* The overscan mask was four opaque black fill rectangles in fill cycle (`fill = 0x00010001`), the
  complement of the drawn area: `(0,0)-(320,17)`, `(0,224)-(320,240)`, `(0,17)-(21,223)`,
  `(296,17)-(320,223)`. The top and bottom span the full scissor width so RT64 stretched them across
  a widened frame; the sides did not, so they stayed at 4:3 positions and appeared as black columns
  *inside* the picture. Removed by the viewport patch.
* The intro/attract letterbox uses the same four-rectangle shape sized to the cinematic rect, and is
  **kept deliberately** — its bars are ~20 % of the height, far outside the overscan-margin test.

Negative, and worth keeping so nobody pays for them again:

* The sky is **not** wide texture rectangles (`BAR_SKIP_WIDE=150` leaves it untouched).
* The sky is **not** an orthographic layer.
* Stretching every rectangle at least N % of the framebuffer width did nothing — BAR's backdrop
  strips (`320×3`, `320×6`, `160×6`, `160×12`) are not the sky.

## Open graphics items

* A 6-pixel band of flat background colour along the very top of a race frame (framebuffer rows 0-1):
  the sky geometry starts two rows down. Not new — it was always there, hidden under the overscan
  mask's 17-row top bar.
* The **results screen** has not been checked with anchoring on. Finishing a race takes minutes, so
  its state and `raceState` were never measured; if it runs as state 5 / phase 0 its 2D layout will
  be anchored like the HUD, which is probably wrong for it.
* **Split screen** has not been checked. Classification measures every rectangle against the whole
  320-wide screen, so a two-player HUD drawn inside a half-screen viewport is classified against the
  wrong reference. Both the bands and the per-viewport reference need revisiting before 2P.
* The **pause menu's dimming panel** covers only the centred 4:3 region, leaving the widened margins
  undimmed. It is a 2D rectangle in the middle band, so anchoring does not touch it.
