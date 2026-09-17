# R6 — main-menu film-roll transition (RESOLVED)

**Symptom:** BAR's main menu is framed as a **film strip** (sprocket-hole borders). Navigating between
menu pages should *roll* the strip (a vertical scroll from one page to the next). Instead pages swapped
instantly.

## Resolution (fix)
The transition is a **VI-origin pan**. RT64's presentation mode decides whether it shows correctly:

| mode | film-roll pan | pre-pan flash |
|---|---|---|
| PresentEarly (was forced by us) | **no** — dropped, instant swap | — |
| SkipBuffering (RT64 default) | yes | **yes** — flashes the destination page 1 frame before the pan |
| **Console (our fix)** | **yes** | **no** |

- **Fix:** `src/main/rt64_render_context.cpp` — `enable_instant_present()` now selects RT64's **Console**
  presentation mode (present strictly from the VI origin, at VI time — what the console does). `BAR_PRESENT_MODE=skip|early|console` overrides; `BAR_INSTANT_PRESENT=1` is a legacy alias for `early`.
- **Two rounds:** first switched PresentEarly→SkipBuffering (roll animated but flashed the destination page
  for one frame during setup, because SkipBuffering presents the just-rendered framebuffer). Console
  presents only the VI-origin framebuffer, so the setup render of the destination is never shown early.
- **Verified** headlessly frame-by-frame with the burst tool (`BAR_SHOT_BURST` / `bar_rt64_start_burst`):
  title → clean vertical scroll → MAIN MENU, no flash. SkipBuffering `f0001` = destination page (flash);
  Console `f0001` = still the source page (no flash).

## The mechanism (how the roll actually works)
`func_filmroll_00400170` (`RecompiledFuncs/funcs_28.c`) **is the animation loop** (not "setup" — an early
misread). Each iteration:

1. `gGfxMgr(0x80025C08)->+0x04` — frame start (empty display list; **no per-frame drawing**)
2. `gGfxMgr->+0x20` — frame submit (RSP task → BAR's scheduler `gScheduler`)
3. **`osViSwapBuffer(s0)`** — set the VI origin to `s0`
4. `gGfxMgr->+0x74` — frame wait (`osRecvMesg` on the gfx-mgr done queue; the scheduler posts it at VI
   retrace, so the loop is paced to ~60 fps)
5. `s0 += direction * 0xF00` — advance the pan (0xF00 = 3840 B = 6 rows of a 320×240×16bpp buffer)

So the roll is a **pure VI-origin scroll**: the loop pans `osViSwapBuffer` from page A's framebuffer
(`0x1DA800`) down through contiguous memory into page B's framebuffer (`0x200000`) — one screen height
over ~40 steps / ~720 ms. The two pages are stacked contiguously in RDRAM, so scrolling the origin shows
A leaving the top while B enters from the bottom. The content is **not redrawn** per frame; only the VI
origin moves. (The `func_filmroll_00400720`/`0040035C` "draw" exports chased earlier are a *different*
effect and were a red herring.)

## Root cause (why it was instant)
Pipeline trace across one transition (env `BAR_DBG_VISWAP`, since removed):

| stage | count in the ~720 ms window | meaning |
|---|---|---|
| game `osViSwapBuffer` | 41 | game pans perfectly (distinct origins, correct 60 fps pacing) |
| vi_thread → ScreenUpdate | 43 | distinct sweeping origins reach the runtime |
| RT64 `updateScreen` present-decision | **1** | ← the break |
| RT64 present | **1** | display frozen; jumps to final page → "instant" |

`enable_instant_present()` (our code) set RT64's `presentation.mode = PresentEarly`. In that mode
`State::updateScreen` (`lib/rt64/src/hle/rt64_state.cpp`) early-returns for every vi_thread update — presents
are driven by freshly-**rendered** content instead. Because the film-roll only pans the VI origin over
static pre-rendered content (never redraws), PresentEarly never presented the pan → the 41 origin steps
collapsed to a single present. `SkipBuffering`/`Console` mode take the normal path where a changed VI
origin (`viDifferent`) advances a present, so every pan step shows.

## Ruled out along the way (all empirically)
Interpolation / high-FPS, menu frame-rate, `requeue_si`, `timeBeginPeriod`, input, cooperative-preempt,
loop pacing (the loop runs the correct ~720 ms at 60 fps), and the film-roll "draw" exports. The game side
was always correct; the fault was entirely in RT64's present mode.

## Reusable tooling produced
- **Burst capture** (`lib/rt64` fork, `bar_rt64_start_burst`): record the next N presents to
  `dir/fNNNN.png`, one per present — for animations the input-frame timeline can't sample (the game blocks
  in the render loop during the roll and stops polling input). Triggers: `BAR_SHOT_BURST="fc:dir:count"`
  (main.cpp, fires at input frame `fc`). See docs/HEADLESS_TESTING.md.

## Widescreen roll (17 Sep 2026)

**Symptom:** with the menu widened, every roll dropped to 4:3: the sprocket strips jumped in to the 4:3
border and both pages scrolled inside a 4:3 box, and the incoming page showed the outgoing page's content
for most of the roll.

**Cause (measured).** A temporary per-present log of the VI origin showed the Main Menu -> Race Type roll
as 39 presents with the origin inside framebuffer `0x1DA800` at row offsets 6, 12 ... 234, none of them a
framebuffer's start. `PresentQueue::threadPresent` looks a framebuffer up by its exact start address
(`fbManager.find`); when that fails it uploads the RDRAM bytes at the origin into a native 320x240 scratch
target. That picture is 4:3, and stale wherever RT64 had not written its render target back to RAM.

**Fix** (`lib/rt64/src/hle/rt64_present_queue.cpp`). When the origin lies on a whole row inside a known
framebuffer and another framebuffer starts where that one ends, the present builds the picture from the
two **widescreen render targets**: the top page's resolved texture shifted up by the row offset, the
bottom page's below it, copied into a composite texture with the same size and resolution scale, and handed
to the VI renderer. `BAR_NO_ROLL_COMPOSE=1` restores the RDRAM upload.

**First roll.** On the first Title -> Main Menu roll RT64 has no framebuffer at `0x1DA800` at all (it knows
`0x3DA800` and `0x200000`); the game put the outgoing page there without RT64 drawing it (inferred). The
fallback: when no framebuffer contains the origin but one starts a whole number of rows below it, that is
the bottom page, and the top page is the render target most recently presented at another address.

**Verified:** `BAR_SHOT_BURST` of the roll: widescreen throughout, strips stay at the frame edges, the
incoming page is current; Daniel's playtest. The incoming page still shows the outgoing page's menu text
early in the roll, exactly as the old 4:3 roll did, so that is the framebuffer's own content. Still open: a
one-present flash of the destination page just before the roll starts.
