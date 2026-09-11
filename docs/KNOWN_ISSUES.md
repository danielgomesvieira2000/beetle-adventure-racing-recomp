# Known Issues

Open defects with what has already been ruled out, so nobody re-runs a dead experiment.
Add the negative results, not just the leads — they are the expensive part.

---

## OPEN -- Intermittent crash at boot in `_uvScDoneAud` -> `osSendMesg`

**Seen once, 11 Sep 2026**, on a launch of the frontend build that had started cleanly on the
immediately preceding and following attempts with the same binary and the same settings folder. The
crash handler caught it during bring-up, after `create_render_context: ok` and before the game had
been started from the launcher:

```
*** CRASH code=0xc0000005 addr=00007FF72498D4BA ***
  # 0 do_send +0x3a
  # 1 osSendMesg +0x4b
  # 2 osSendMesg_recomp +0x18
  # 3 _uvScDoneAud +0xd5
  # 4 _uvScHandleRSP +0x97
  # 5 _uvScMain +0x21b
  # 6 run_thread_function +0x1fc
```

An access violation inside `do_send`, on the scheduler's own thread, delivering the message that
retires a finished audio task. **Not reproduced**, so there is nothing measured here beyond the
stack -- it is written down because an intermittent crash with a stack is exactly the thing that gets
lost, and the next person to see it should not have to establish that it has been seen before.

What was and was not going on at the time: this was the first launch after the HUD inspector landed,
and a controller was connected where earlier launches that day had none, so the input path through
`recompinput` was live. Neither is evidence — the inspector's hooks are on the display-list thread
and touch nothing in this stack, and the port's audio changes that day are host-side, downstream of
the game's scheduler. **Do not treat either as the cause without measuring.**

The neighbourhood is known to be delicate: `_uvScDoneAud` sits under the cooperative-preemption and
audio-starvation work described in [`technical/05`](technical/05-runtime-host.md#threading-and-the-audio-starvation-problem),
and `BAR_NO_PREEMPT=1` is the first switch to try if it recurs.

---

## RESOLVED -- Widescreen: the picture sat in a 275x207 island even with a full-frame scissor and viewport

**Resolved (6 Sep 2026), rt64 `cebfce7`.** The island was not a transform at all: it was the game's
own **overscan mask** painted on top of a picture that already covered the whole frame. BAR draws
four opaque black fill rectangles that are the complement of the rectangle it renders into -- during
a race `(0,0)-(320,17)`, `(0,224)-(320,240)`, `(0,17)-(21,223)` and `(296,17)-(320,223)`, measured
with the new `BAR_DBG_RECT`. In Expand the top and bottom ones span the whole scissor width so RT64
stretches them across the widened frame, while the side ones do not and stay at their 4:3 positions:
black columns standing inside the picture with widened world visible outside them, which is exactly
what the "island" looked like. `RDP::drawRect` now drops such a rectangle when it hugs a scissor edge
and is no thicker than a tenth of it (`BAR_KEEP_OVERSCAN=1` restores it); the intro's cinematic
letterbox is ~20% of the height and is kept.

A second defect surfaced once the mask was gone -- the sky ended at the un-widened frame's edges --
and had a separate cause, also fixed in that commit: RT64 was applying two different coverage tests,
so BAR's racing projection had its matrix widened but its viewport and scissor left at 4:3. See
`docs/RACING_DRAW_MAP.md` for both, and for what each class of draw puts on screen.

The original investigation is kept below, because its negative results are still worth having.

### Original entry


**REVERTED (rt64 back to `b39a680`).** The rt64 commit `aad5142` that followed the verified build --
the horizontal-coverage widening qualifier (`BAR_ASPECT_FB_COVER`, default 90), the diagnostics and
the opt-in projection compensation -- was found by Daniel to reintroduce the fast intro and to put a
black bar over the HUD. The verified build (superproject `e69eb44`, rt64 `b39a680`) has the intro at
the correct speed and no HUD bar, and it is the checkpoint to build on. The reverted work is kept on
the rt64 branch `bar/widescreen-diagnostics`; of it, the coverage qualifier is the only behavioural
change and is the prime suspect, since it makes the unscissored boot-buffer pair and the letterboxed
intro pair render widened. Do not re-apply it without re-checking the intro speed first.


**What is landed.** Framebuffer pairs whose scissor is full-width but not 4:3-shaped -- the intro's
letterboxed `320x147` pair, and a pair drawn with no scissor (RT64's unbounded `2048x2048`) -- used to
fail RT64's per-pair aspect test and were drawn unwidened and centred. They now qualify by
horizontal coverage (`BAR_ASPECT_FB_COVER`, default 90%). Measured: `adjust=1 scale=1.3333` on those
pairs afterwards.

**What is not.** In plain 4:3, with `patches/viewport_patch.c` making the game's scissor and viewport
cover the whole 320x240 (confirmed with `BAR_DBG_SCISSOR`: the only rects emitted are `(0,0)-(320,240)`
and the intro's `(0,47)-(320,194)`), the picture STILL occupies a 275x207 island: content spans
225..1050 of the 160..1120 4:3 region in a 1280x720 window, i.e. exactly 22px and ~17px of the
framebuffer on each side. In Expand the same island appears with widened slivers of world outside it
and black columns between -- the "glitched widescreen" look. The 2D HUD also sits inside the island,
but that is layout (it is authored inside the safe area), not a transform.

**Ruled out, with evidence:**

- *The RDP scissor and RSP viewport.* Full-frame after the viewport patch; `BAR_DBG_PROJ` reports the
  projection's scissor-and-viewport intersection as 319-320 wide.
- *A CPU/RDRAM copy being presented instead of the widened render.* `BAR_DBG_VI` shows the VI
  presenting `0x1DA800`/`0x200000`, and `BAR_DBG_ASPECT` shows those same buffers receiving widened
  passes (`adjust=1 scale=1.3333`).
- *The projection matrix alone.* The inset was assumed to be baked into the perspective matrix
  (`BAR_DBG_PROJ` shows BAR's 3D projections with `[3][3]=0`, `[2][3]=-0.2955`, tiny `[0][0]`).
  `barCompensateProjectionInset` (rt64, opt-in `BAR_PROJ_INSET=1`) scales every such matrix by
  320/275 x 240/207 and the log confirms it runs on all of them -- yet the island's extent does not
  change at all. A game-side patch of `func_uvimtx_rom_004004D8(Mtx)` was also tried; passing `Mtx`
  by value through a recompiled patch corrupted the matrix (the world vanished) and was dropped.
- *The last-clip-rect globals* (`D_uvgfxmgr_rom_00402408..0E`): zero-initialised `.bss`, only read
  by two fill routines; cannot shape the 3D.

**Remaining candidates, untested:** the game's own frustum culling and sky/terrain construction
(`uvterra`, `uvdyn`, `motion` -- all still assembly) building geometry to the inset frustum, so that
nothing exists to draw outside it; or a second transform between the projection and the RSP. The
next diagnostic is to log, per draw call, the NDC extent of submitted vertices rather than the
matrix, which tells whether geometry outside the island is ever submitted.

**Note:** in the meantime `BAR_CONTENT_INSET=auto` (renderer-side crop to the game's drawn region)
still removes the island's margins for presentation, at the cost described in that code.

---

## RESOLVED — HUD anchoring in widescreen

**Resolved (6 Sep 2026)** by a different mechanism, so the display-list rewriter below was never
brought back. The racing HUD now anchors to the widened frame's edges — speedometer and needle left,
timer, lap, map and position right — and races do not glitch, because nothing copies display lists any
more. RT64 already places 2D elements from per-element extended-GBI origins and BAR simply never
emits any, so the fork classifies BAR's draws in `RDP::drawRect` and supplies the origins itself
(`lib/rt64/src/hle/rt64_bar_hud.cpp`). See `docs/RACING_DRAW_MAP.md`, "Anchoring the HUD to the
widened frame", for the measurements, the gate (`currentGameState == 5 && raceState == 0` — state 5
alone also covers the track-loading screen) and the run-time switches.

Two traps found on the way, worth keeping:

- **`FixedRect::fullyInside(r)` reads backwards.** It returns true when *`r` is inside the receiver*,
  not when the receiver is inside `r`, so `rect.fullyInside(screen)` silently rejected every draw.
- **`graphics.json` has two writers that disagree about one field.** `src/game/config.cpp` writes it
  through ultramodern's `NLOHMANN_JSON_SERIALIZE_ENUM` tables; RecompFrontend's settings menu writes
  the same path through librecomp's `Config` (`<tab id>.json`, and the tab's id is `graphics`) using
  the menu's own display names. Every enum's names agree except `HUDRatioMode::Full`, which is
  `"Full"` to the first and `"Expand"` to the second. Neither side throws on a name it does not know
  — both fall back to a default — so the failure is silent in both directions, and both directions
  were observed: with `"Expand"` in the file the loader read HUD Ratio as *Original*, which collapses
  every origin back to the frame's centre and disables anchoring; with `"Full"` in the file the menu
  reset HUD Placement to its own default, *Clamp16x9*. `src/game/config.cpp` now accepts either
  spelling and writes the menu's, since the menu is the writer that wins while the frontend runs.

The parked investigation is kept below, because its diagnosis of the rewriter's flicker is still the
best account of it and the branch still exists.

### Original entry — the display-list rewriter, parked

**Status.** The main branch is the pre-anchoring widescreen build (tree of `b7aae3e`, rt64 `b39a680`):
Expand widens the 3D world in menus and races, the menus do not flicker, the intro runs at the right
speed, and the HUD is drawn as RT64 draws it by default (2D rectangles stretched about the centre,
orthographic triangles centred). Daniel verified this state and asked to return to it after the
anchoring work turned out to glitch races. **Nothing from the anchoring work is compiled in.**

**What was built** (the `wip/hud-anchoring` branch, commit `55e93af`, with the raw experiments that
followed on `wip/island-experiments`; both branches were removed from this repository when it was
published, and survive only on the fork it came from,
[danielgomesvieira2000/beetle-adventure-racing-recomp](https://github.com/danielgomesvieira2000/beetle-adventure-racing-recomp)).** A host-side F3DEX2 display-list rewriter, `src/main/dlrewrite.cpp`
plus `dlrewrite.h`, modelled on `wave-race-64-recomp/src/dlrewrite.cpp`. It wraps the renderer
context (`bar::dlrewrite::wrap(...)` around both `create_render_context` calls in `src/main/main.cpp`),
copies each root display list the game submits into a scratch region at `0x80C00000` (above the 8 MB
the game is told it has, so the game can never allocate over it), and while copying:

- tracks segments, the modelview stack, projection loads (also inside called sub-lists), viewport,
  scissor and the current texture;
- classifies every 2D draw: full-width draws under a non-perspective projection are tagged *stretch*;
  draws under a perspective projection are left alone; during a race (`seen_world && seen_ortho`,
  sticky across the several lists BAR submits per frame) a draw whose centre is in the left third is
  anchored *left*, in the right third *right*, and the rest are centred;
- emits `gEXEnable`, `gEXSetViewportAlign` (plus a re-issued viewport), `gEXSetRectAlign` /
  `gEXSetRectAspect` for texture and fill rectangles, and `gEXMatrixGroup` for projections, so RT64
  places each element against the widened frame;
- is switched on only when the frontend's HUD ratio option is not *Original* (`hr_option !=
  HUDRatioMode::Original`), and can be disabled or traced at run time with `BAR_NO_REWRITE`,
  `BAR_HUD_OFF`, `BAR_HUD_NOEMIT`, `BAR_HUD_NO_ANCHORS`, `BAR_HUD_TRACE`.

It needs one rt64 change, commit `0203ffb` on our rt64 fork ("Bound the runaway-list guard at RT64's
16 MB address space"): without it RT64 refuses the scratch copy above 8 MB and the screen goes black.
`CMakeLists.txt` adds `src/main/dlrewrite.cpp` to the sources and `lib/rt64/include` to the include
directories (for `rt64_extended_gbi.h`; `F3DEX_GBI_2` must be defined before including it so the hook
opcode is `0xE0`).

**What was verified.** In a race with HUD ratio set to Expand, the speedometer and lap counter sit at
the left edge and the timer, map and position at the right edge, on the widened frame. Daniel: *"the
HUD is anchored now, besides the black columns."*

**Why it is parked.** With the rewriter enabled, races glitch: the car and the background flicker and
move around. This happens with the plain verified rewriter (`ba236b5` / `55e93af`), not only with the
later experiments, so the cause is in the rewriter itself, not in the island experiments. It was not
diagnosed. Candidates to check first when this is resumed, in order:

1. *The scratch copy is reused while RT64 may still be reading it.* The rewriter writes every list of
   every frame into the same `0x80C00000..+0x40000` region, on the game thread, while the renderer
   consumes lists on its own thread. If RT64 reads a list after the next one has been written over it,
   geometry from two frames mixes, which is what flicker and "moving around" look like. Fix to try:
   ring-buffer the scratch (several slots, advance per list) or copy only what must change.
2. *The projection group / matrix rewriting.* `gEXMatrixGroup` on the world projection (or the
   re-issued viewport after `gEXSetViewportAlign`) may disturb RT64's per-frame matrix interpolation,
   which is exactly what would make the car and background jitter. Test: `BAR_HUD_NO_ANCHORS=1` (keeps
   the copy, drops the anchors) and `BAR_HUD_NOEMIT=1` (copy only, no extended commands) to see which
   layer introduces the glitch.
3. *Race classification bleeding into 3D draws.* Anything classified as a HUD element that is really
   part of the world (car shadow, skybox rectangles) would be pinned to an edge and appear to move.
   `BAR_HUD_TRACE=1` prints the classification of every draw.

**How to bring it back.** On a branch: `git cherry-pick ddf8ac1 ba236b5` (rewriter and its race fix),
set `lib/rt64` to `0203ffb`, rebuild, then work through the candidates above with the run-time
switches before touching anything else. Keep the main branch on the verified build until a race is
confirmed clean with anchoring on.

**Separately parked: the black columns beside the world** (see the island section above). They come
from a full-frame backdrop layer BAR draws each frame as two texture rectangles carrying the overscan
margins. Dropping the layer (`BAR_HUD_SKIP_BLIT=1` on `wip/island-experiments`) removed the top and
bottom bars and moved the side bars to the frame edges, but the layer also erases the previous frame,
so dropping it causes ghosting. Cropping and pinning the rectangles at the list root had no effect,
most likely because in a race they are issued from a called sub-list the rewriter does not copy. The
viewport and projection were ruled out (all viewport loads are full-frame; scaling the projection
only magnifies the island's contents).

## Intermittent crash in the audio thread

Seen twice while capturing automated runs, roughly 50 seconds into the attract sequence, on builds
both before and after the presentation changes -- so it is not caused by them.

```
*** CRASH code=0xc0000005 addr=... ***
  # 0 func_8000DA6C +0x2ac
  # 1 alAudioFrame +0x452
  # 2 uvAudioMgrHandleFrameMesg +0x43a
  # 3 uvAudioMgrThreadFunc +0x535
```

An access violation inside libultra's audio library (`alAudioFrame`), reached from Paradigm's audio
manager thread. `func_8000DA6C` is the unnamed leaf, and is the place to start. Not yet
investigated; recorded so the stack trace is not lost, and so a future crash here is recognised as a
recurrence rather than a new regression.

---

## RESOLVED — Intermittent permanent black screen (game boots, audio plays, no picture)

Fixed in the N64ModernRuntime fork, branch `controller-pak`, commit `9503a9b`. Recorded here
because the symptom is misleading and the mechanism is worth knowing before touching VI code again.

**Symptom.** The game boots and audio plays normally, but nothing is ever drawn. Crucially it was
*intermittent*: the same binary would run correctly once and then stay black on the next launch,
which makes it look like a configuration or GPU problem rather than a code defect.

**Cause.** `osViGetCurrentFramebuffer()` returns the current `ViState`'s `framebuffer` field
verbatim. BAR boots with `osViBlack(TRUE)` and un-blacks *only* once its gfx manager observes that
value equal to `0x100000` (`uvgfxmgr_rom.c`); until then RT64 has nothing to present, because
`VI_STATE_BLACK` forces `hStart == 0`.

An earlier change (`0c1bf21`) had made the pre-game dummy VI alternate that framebuffer field
between `0x100000` and `0x125800`, so RT64 would see a changing origin and keep presenting while the
launcher is up — without which recompui's `draw_hook` never runs and the launcher paints once and
then ignores all input. But `set_dummy_vi` only runs while `!is_game_started()`, so whichever value
it wrote *last* is what the game reads back. Whether BAR ever un-blacked therefore came down to the
parity of the final dummy frame — a coin toss, hence the intermittency.

**Fix.** `update_vi()` computes `VI_ORIGIN_REG` as `framebuffer + fldRegs[field].origin`, so the
alternation was moved into the mode's field origin (a second dummy mode, `dummy_mode_alt`). The
scanout address still changes every frame, so the launcher keeps presenting; the address the game
reads back is pinned at `0x100000`, so BAR always un-blacks.

**If you touch this again:** the two requirements genuinely conflict, and satisfying only one of
them produces a different bug — pin the framebuffer and the launcher goes inert; alternate it and
the picture dies at random. Test *both* paths: the no-frontend auto-start build must reach gameplay,
and the frontend build's launcher must still respond to input.

**Ruled out along the way** (do not re-run these):

- `RecompiledPatches/patches.c` / `PatchesLib` — disabling it entirely still produced a black screen.
- The graphics config (`ar_option`, `hr_option`, `rr_option`, `wm_option`) — the fault reproduces on
  conservative windowed values and the fix holds on fullscreen `Expand`.
- The `scripts/fix-recompiled.sh` rewrite — its end-state verification passes and rendering is
  correct with it in place.

---

## Intro / boot sequence runs too fast

**Status 5 Sep 2026 (parked at Daniel's request).** Daniel reported the intro at the correct speed on
the build at superproject `e69eb44` / rt64 `b39a680`, then fast again on a source-identical rebuild.
Measured before parking, all on that source: the attract sequence reaches the same screens at the
same seconds windowed and fullscreen, idle and under 8 busy threads, and in the frontend build versus
the auto-start build; the frontend build creates its renderer with `rr_option=Original` (the
frontend uses RecompFrontend's *own* `recompui::renderer::create_render_context`, not
`src/main/rt64_render_context.cpp`, so our mapping there never applies to it -- a real difference
worth remembering); and `BAR_FPS=1` shows the game swapping at ~59-60/s through the logos, prompt
and cinematic and ~30/s in the race demo, identically in both builds. If the intro runs at 30 Hz on
hardware, that 60 is the bug itself and the game's VI retrace request is the place to look. No
configuration or build difference that reproduces "fixed" vs "fast" was found.


**Status:** open, deferred. **Severity:** cosmetic but immediately visible.

The boot sequence plays noticeably faster than a reference emulator running the same USA ROM —
"seconds too fast", verified by side-by-side comparison. Races feel correct.

### Ruled out (with evidence)

| Hypothesis | Verdict |
|---|---|
| RT64 frame interpolation speeding the simulation | **Refuted.** `rr_option` `Display` vs `Original` both hold 60–61 SI polls/sec. |
| Game loop periodically tripling (~180 polls/sec bursts) | **Refuted.** The bursts are Controller Pak I/O inflating the counter — every spike coincides with ~60 pak transactions, every quiet second has zero. The loop is a steady 60. |
| Emulated CPU counter running fast | **Refuted.** `counter_per_ms = 46'875` is exactly the N64's 46.875 MHz count rate, and `speed_multiplier == 1`. |
| Fixed delta-time override (`D_uvgfxmgr_rom_00402480`) | **Refuted.** Probed at both return sites of `func_uvgfxmgr_rom_00401004`: the override branch is **never** taken. |
| Measured delta-time being wrong | **Refuted.** Measured delta averages **0.0163 s ≈ 1/61**, i.e. correct. ~1020 calls/sec ≈ 17 active sequences × 60 fps. |
| Whole-frame divider (`D_8001F7C0`) | **Refuted.** Reads 0 in both the menu (state 14) and a race (state 2). |
| SI completion pacing (`requeue_si`) | **Refuted.** `BAR_REQUEUE_SI=1` restores stock re-queueing: the menu becomes choppy but the animation speed is unchanged — the signature of correct delta-timing. |
| PAL/NTSC confusion (PAL runs ~17% slower) | **Refuted** by the user's side-by-side against the same USA ROM. |

### The attract's motion and its length are on two different clocks (7 Sep 2026)

Daniel asked for the attract sequence to be capped at 30 Hz, on the theory that a game loop handed
60 frames where the console's retrace request gave it 30 would run frame-counted content at double
speed. `ultramodern::set_vi_divider()` was added to test it: it delivers VI retrace messages to the
game once every N fields, so the game's own loop -- which blocks on those messages -- runs at 60/N.

The attract sequence (`currentGameState` 2) was then timed end to end from `BAR_DBG_STATE`, headless,
using the autoplay recipe in docs/HEADLESS_TESTING.md:

| Configuration | Attract duration |
|---|---|
| Stock | **72.45 s** |
| Game loop capped to 30 Hz (`BAR_ATTRACT_HZ=30`) | **72.76 s** (+0.4%) |
| UV timeline delta halved (`BAR_TIME_SCALE=0.5`) | **72.46 s** (+0.01%) |

**Neither lever moves the length.** That measurement was, however, aimed at the wrong quantity.
Daniel then specified the actual complaint: it is not that the sequence is short, it is that *the
demo cars drive too fast and miss their scripted events* -- the attract loses sync with itself. The
attract is a canned replay run through the game's own race simulation, so what matters is the rate
that simulation steps at, not how long the segment lasts.

Capturing the demo at the same wall-clock offset into the attract, with and without the cap, shows
the car at visibly different points on the lap -- further along uncapped, still approaching the same
landmark capped. **So the cap does slow the driving**; the duration metric simply could not see it.

The two results together give the shape of the thing: **the attract's motion and its length are on
different clocks.** Motion steps with the game loop (and with the UV delta, per the older
`BAR_TIME_SCALE` observation); the segment's length is on the wall clock or the VI count, neither of
which the divider touches -- `total_vis`, the VI registers and the host's present rate all keep
advancing at 60 regardless. A consequence worth knowing: with the cap on, the demo drives at the
right speed but is still cut off after the same number of real seconds, so it covers less of the lap
than it used to.

The cap is therefore **on by default for state 2 only** (`ultramodern::set_vi_divider`, gated in
`src/main/os_unimpl_stubs.cpp`). Menus, races and the front end keep the full 60.
`BAR_ATTRACT_HZ=60` disables it; `BAR_VI_DIVIDER=<n>` forces a divider in every state.

**The cap fixes the speed but breaks the audio (7 Sep 2026).** Daniel confirmed the capped build
runs every part of the sequence at the correct speed -- and that the sound is then slowed and
glitched. Measured during the attract, the host audio queue:

| | queue floor | queue ceiling | `audio LOW` warnings |
|---|---|---|---|
| Capped to 30 Hz | **64** | 1088 | 1 |
| Uncapped | 448-480 | ~1568 | 0 |

So the queue runs at about half the headroom and dips to nearly empty. Starvation explains the
glitching, but not the *slowing*; that comes from the same root cause. BAR's audio engine -- buffer
generation and the music sequencer's tempo -- is advanced by the game loop, which is exactly what the
divider halves, so the music plays at half tempo and the buffers arrive half as often.

**This makes the VI divider the wrong lever in principle.** It cannot be tuned or buffered out of the
problem: a sequencer stepping at half rate is half-tempo no matter how deep the queue is. The divider
was removed again (reverted in the N64ModernRuntime fork) once the right lever was found.

### Fixed: the game's own frame limiter, D_8001F7C0

`D_8001F7C0` -- dismissed earlier in this document as a *cause* because it reads 0 -- turns out to be
the *mechanism*. It is BAR's own minimum frame length, and `lib/bar-decomp/src/sched.c` shows exactly
how it works. In `_uvScDoneGfx`, when the gfx task completes:

```c
if (D_8001F7C4 < D_8001F7C0) { D_8002F250 = (D_8001F7C0 - D_8001F7C4) + 1; }  // defer the swap
else                         { D_8002F250 = 1; osViSwapBuffer(...); }         // swap now
```

`D_8001F7C4` counts retraces since the last frame was released; the countdown `D_8002F250` releases
the frame and resets it. It ships as 0, so nothing is ever deferred and the sequence advances as fast
as our tasks complete -- 60 Hz, where the console's heavier RCP gave it 30.

Two things make this the right lever where the VI divider was not:

* **It gates the gfx path only.** `_uvScDoneAud` is a separate completion path, so the audio task,
  its buffer generation and the music sequencer's tempo all keep full rate. Measured during the
  attract with the limiter engaged: queue floor 416-480 and **zero** `audio LOW` warnings, matching
  the uncapped baseline rather than the divider's starved floor of 64.
* **It is the game's own knob**, not something imposed from outside the simulation.

**Mind the `+ 1`.** The field is not a frame count, it is one less than the frame length: a frame
lasts `D_8001F7C0 + 1` fields, which is why the stock 0 yields 60 Hz rather than stalling. Writing 2
gives three fields (20 Hz) and was reported as visibly too slow; **30 Hz is 1**. With 1 written during
state 2, the measured loop rate is 33-38 SI polls/sec during the attract against 52-59 in the menu.

Applied in `src/main/os_unimpl_stubs.cpp` for `currentGameState == 2` only. `BAR_ATTRACT_HZ` overrides
the target; 60 disables it.

**It does not actually fix the speed.** Daniel tested it: the audio is healthy again, but the demo is
back to running fast. The reason is visible in the code above -- `_uvScDoneGfx` defers the *buffer
swap*, not the simulation. The game keeps stepping its logic at full rate and simply presents fewer
frames, so the cars still drive at 60 Hz speeds. That is also why the audio is fine, and it is the
same distinction the VI divider got right for the wrong price: the divider slowed the loop (fixing
speed, wrecking audio), the limiter slows presentation (keeping audio, not fixing speed).

So the state of this issue is: **neither lever found so far slows the simulation alone.** What is
needed is whatever advances the demo's physics per frame -- the replay playback step -- rather than
either the swap rate or the retrace rate. Parked here at Daniel's request; the limiter is left in
place because it is harmless and is the correct pacing mechanism should the simulation side be found.

**Why 30 is believed to be the right target:** an actual race in this port runs its loop at ~30/s
while the attract runs at ~60/s (the `BAR_FPS` measurement recorded above). The demo is a race, so
the game's own race rate is the reference. That reasoning has not been checked against hardware, and
a headless recipe that reaches a real race would let the two loop rates be compared directly with
`BAR_DBG_FPS` -- worth doing before treating 30 as settled.

**Still open:** whether the segment being cut off at the same wall-clock length (now covering less of
the lap) is correct console behaviour or a second bug. If the console's attract shows the *whole*
demo, then the length timer is wrong too, and it lives on the real-time source (`uvClkGetSec` /
`osGetTime` / the VI counter) -- not on the frame loop and not on the gfx-manager delta.

### The strongest remaining lead

The UV timeline (`uvtseq_rom.c:243`) advances as `step -= rate * func_uvgfxmgr_rom_00401004()`,
which is genuinely delta-timed and measured correct. A temporary scale factor on that delta
(`BAR_TIME_SCALE`) **visibly changes the attract cinematics** — two captures at the same input
frame differ between scale 1.0 and 0.4 — but **does not change the boot logo screens at all**
(frame 1700 was byte-identical at both scales), and the user reported no perceptible change.

That splits the boot sequence in two:

- **Attract cinematics** — timeline-driven, delta-timed, responds to the scale knob.
- **Legal / logo screens** — *not* driven by this time base. Whatever paces them is elsewhere.

So if the "too fast" part is the logo/legal screens, every measurement above is consistent and the
cause has simply not been located yet. This also lines up with the pre-existing note that the
legal screens are driven by un-decompiled boot assembly whose per-frame handler was never pinned
down (see the legal-screen-skip item inherited from upstream).

### Next steps

1. Determine precisely *which* part is fast — logo/legal screens or attract — by timing each
   segment against the reference emulator with a stopwatch, rather than judging the whole boot.
2. If it is the logo screens, find their pacing source. They are likely on a `uvClkGetSec` timer
   in boot assembly; instrument that rather than the gfx-manager time base.
3. Only then decide on a fix. Do **not** reach for `speed_multiplier` or the VI clock — the
   simulation rate is measured correct and changing it would break lap times.

### Temporary scaffolding to remove

- `bar_timebase()` / `bar_dbg_timebase()` in `src/main/bar_config.cpp` (`BAR_TIME_SCALE`,
  `BAR_DBG_TIME`).
- Probe calls patched into `RecompiledFuncs/funcs_38.c` at both return sites of
  `func_uvgfxmgr_rom_00401004`. These live in **generated** code and disappear on the next
  `N64Recomp` run — re-apply manually if the investigation resumes.
- `BAR_DBG_DIV` frame-divider logging in `src/main/os_unimpl_stubs.cpp`.

`BAR_DBG_PAK` in the same file is worth keeping — it is what diagnosed the Controller Pak.
