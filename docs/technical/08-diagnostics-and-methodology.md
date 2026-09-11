# 08 — Diagnostics and methodology

This port carries a large environment-variable diagnostic surface, all of it zero-cost when unset,
plus scripted input and internal-render screenshot capture so a full boot-to-race sequence can be
driven and photographed without touching the window. Most of the facts in
[06](06-graphics.md) exist because of these; treat them as first-class tooling, not scaffolding.

## The measurement discipline

Four rules, each learned by paying for its absence:

**1. A single captured frame does not show everything.** Establishing what a draw call produces
requires flipping a switch, capturing a frame, **and** having someone say what they saw change in the
live window. A race capture will never tell you that the track-select screen lost its text — that is
exactly how `BAR_SKIP_WIDE=150` was identified as removing the course-name text.

**2. Some defects are only visible in motion.** The HUD anchoring bug where the HUD stayed at 4:3
positions through the entire countdown and snapped out the instant the player got control does not
appear in any still frame. Rendering changes get checked in a running game, not only in captures.

**3. Delete target PNGs before every run.** Screenshots are only written when the run reaches the
frame that asks for them, so a run that diverges or dies early leaves the *previous* run's images in
place — and a script that waits for the file to appear then returns immediately with stale pictures.
That has already produced one round of confident, wrong conclusions. Either remove the files first,
or wait on something the current run writes (its last `AUTOPLAY -> phase` log line).

**4. Half a change looks identical to no change.** Widening a projection matrix without widening its
viewport and scissor together draws the layer wider and clips it straight back. Any experiment of
that shape is worthless unless all three move together.

## The interactive instrument: the HUD inspector

The four rules above are about the limits of a captured frame and a log read afterwards. The HUD
inspector exists to lift two of them for 2D work specifically: **F1** opens a window, inside RT64's
own developer UI, that lists every 2D element of the current frame with its identity, extent and the
class the classifier gave it; hovering a row outlines that element on screen, and a dropdown changes
its class from the next frame with no rebuild and no restart.

That is a different kind of instrument from everything else in this chapter — it answers while the
frame is in front of you rather than in a log matched to a screenshot by eye, and rule 1 (someone has
to say what they saw change) is satisfied by the person using it. It does not replace rule 2: a
change made in the panel still has to be watched in motion before it is believed.

It is on in every build, F1, nothing to set. **[`../HUD-INSPECTOR.md`](../HUD-INSPECTOR.md)** is the
full guide; `BAR_INSPECTOR=0` turns the port's half off for an A/B.

## Headless scripted runs

### Scripted input — `BAR_AUTOPLAY`

Space-separated `frames:hexbuttons` phases, played in order and then held neutral. It bypasses the
window-focus gate, so the run needs no focus and can sit in the background. `fc` counts input-poll
frames (~60/s once the game is polling). Each phase is logged as `AUTOPLAY -> phase ...`.

N64 button mask: `START=1000  A=8000  B=4000  Z=2000`; d-pad `U=0800 D=0400 L=0200 R=0100`. D-pad
bits also drive the analog stick, so menus reading either respond.

Live keyboard equivalents (port 0): stick = arrows, A = `X`, B = `Z`, Start = `Enter`, Z = Shift,
L = `Q`, R = `W`, C-buttons = `I/J/K/L`, d-pad = `T/F/G/H`.

**Reaching the main menu.** The game boots to a Controller Pak prompt and then a confirmation, and
`currentGameState` stays 14 through both prompts *and* the menu, so the state cannot tell you where
you are — verify by screenshot.

```
BAR_SKIP_LAUNCHER=1 BAR_AUTOPLAY="120:0 30:8000 150:0 30:0800 150:0 30:8000 700:0"
#   wait, A (Start Game Without Saving), wait, Up (-> Yes), wait, A (confirm), hold
```

**Reaching a real race** (~110 s, verified; frame ~5700 is "GO!", ~6100 is racing):

```
BAR_SKIP_LAUNCHER=1 BAR_WINDOW_SIZE=1280x720 BAR_NO_AUDIO=1 \
  BAR_SHOTS="6100:C:/tmp/race.png" \
  BAR_AUTOPLAY="250:0 30:8000 1120:0 20:1000 1480:0 20:8000 280:0 20:8000 280:0 20:8000 \
                280:0 20:8000 280:0 20:8000 280:0 20:8000 280:0 20:8000 200:0 20:8000 1200:0" \
  ./beetle-adventure-racing-recomp.exe
```

That is: A on the Controller Pak prompt, START to skip the intro movie, then six A presses through
Race Type (Single Race) → Opponents (Full Grid) → Difficulty (Easy) → Track (Coventry Cove) → Car →
Transmission, then one more A for the Rumble Pak prompt.

**Two caveats.** Give `BAR_SHOTS` **Windows-style paths** (`C:/...`) — an MSYS `/c/...` path is
written somewhere else and the files never appear. And boot timings are machine-dependent: an
inherited recipe that reached the logos by frame 510 needs roughly frame 1450 on a different machine.
Re-calibrate rather than trusting a recorded frame number.

### Screenshots — always the internal render

**Never use a window-manager screenshot.** Capture RT64's internal render: it reads the actual
presented image back GPU → PNG, needs no window focus, works headless and in the background, and is
therefore the ground truth for what is on screen. It lives in the RT64 fork's present queue, copying
the swapchain texture to a readback buffer and writing it with `stb_image_write`.

| Variable | Use |
|---|---|
| `BAR_SHOTS="frame:path frame:path …"` | Preferred. Captures at scripted input frames, on the **same** timeline as `BAR_AUTOPLAY`, so captures line up with input |
| `BAR_SHOT_BURST="fc:dir:count"` | Captures the next `count` **presents** to `dir/f0000.png…`, one per present. `dir` must already exist |
| `BAR_BURST_ON_ROLL="dir:count"` | Deterministic burst trigger: fires when the film-roll animation loop is entered, robust to boot-timing variance |
| `RT64_SHOT_TRIGGER=<file> RT64_SHOT_OUT=<png>` | Ad-hoc fallback: `touch` the trigger and the next present writes the PNG and deletes it |

**Use burst capture for animations the input timeline cannot sample.** The main-menu film-roll blocks
the game thread in a render loop and stops polling input, so `BAR_SHOTS` frames freeze mid-roll.
Trigger a burst a few frames *before* the transition, while `fc` is still advancing; RT64 then counts
presents on its own.

**Screenshot caveat for host-side capture**: capture with a DPI-aware process. A non-aware one sees a
virtualised 1536×864 view of a 1920×1080 window and produces an image that looks shifted and clipped
— an artefact that was briefly mistaken for a rendering bug.

## Environment variable reference

### Run control

| Variable | Effect |
|---|---|
| `BAR_WINDOW_SIZE=WxH` | Window size; default is 4:3. Essential for QAing present modes, which only differ on a non-4:3 window |
| `BAR_SKIP_LAUNCHER=1` | Start with the launcher/overlay hidden so the game renders directly |
| `BAR_AUTOPLAY` | Scripted input (above) |
| `BAR_FORCESTATE="frame:state …"` | Write `gGameSettings->gameStateFlag` at listed SI frames, to reach later screens without navigating |
| `BAR_NO_AUDIO` | No audio device and the silent stub ucode |
| `BAR_NO_PREEMPT=1` | Disable cooperative preemption — first thing to try when bisecting a timing regression |
| `BAR_NO_TIMER_FIX` | Skip the 1 ms Windows timer resolution raise |
| `BAR_REQUEUE_SI=1` | Restore ultramodern's stock SI re-queueing (control experiment: menus get choppy, animation speed must **not** change) |
| `BAR_ATTRACT_HZ=<n>` | Target rate for the attract-sequence frame limiter; `60` disables it |
| `BAR_NO_INTRO_SKIP` | Disable the injected B-button attract skip |
| `BAR_PRESENT_MODE`, `BAR_INSTANT_PRESENT` | Override RT64 presentation mode (default is `Console` — see [05](05-runtime-host.md#presentation-and-the-render-context)) |

### Diagnostics

| Variable | What it prints |
|---|---|
| `BAR_DBG_STATE=1` | `currentGameState` transitions (`gGameSettings + 0xA4`) |
| `BAR_DBG_FPS=1` | The SI-poll rate, i.e. the game loop rate — the main loop polls once per iteration |
| `BAR_DEBUG_OVERLAYS=1` | Every module load by 4-char nameTag and load address |
| `BAR_DBG_PAK=1` | Controller Pak / joybus traffic, and every distinct SI frame shape both directions |
| `BAR_DBG_UI=1` | Frontend diagnostics to `bar_ui_trace.log`. **Required** to see frontend faults at all: the release build is `/SUBSYSTEM:WINDOWS`, so stderr goes nowhere and shell redirection captures nothing |
| `BAR_DBG_GFX=1` | What actually reached RT64's `userConfig` — widescreen depends on `aspectRatio == Expand` surviving, so print it rather than trusting the JSON |
| `BAR_DBG_FRUSTUM=1` | Every perspective frustum built (destination, six parameters, aspect), and once, what the adjustment did |
| `BAR_DBG_SLIDE=1` | The menu page-transition / film-roll trace |
| `BAR_DBG_DIV=1` | The whole-frame divider `D_8001F7C0` alongside the game state |
| `BAR_AUDIO_DBG=1` | Peak sample magnitude and saturation count |
| `BAR_AUDIO_CAPTURE=<path>` | Dump the raw untransformed received audio stream (~16 MB cap) for offline analysis with no device open |
| `BAR_HUD_TRACE=1` | Game state, race flag, whether anchoring is armed, and every 2D rectangle with the identity and class it was given. **Prefer the HUD inspector** (F1) — see below |
| `BAR_HUD_COVER_INSET="l,t,r,b"` | The overscan-safe rectangle the `cover` class magnifies to the frame's edges; default `22,17,296,223`. Dial a backdrop in without a rebuild |
| `BAR_INSPECTOR=0` | Turns the port's half of the F1 debug menu off, for an A/B against the fork's own classification. RT64's half stays on F1 either way, and `hud.json` tags still apply |
| `BAR_DBG_RECT=1` *(RT64 fork)* | Every **distinct** 2D rectangle in framebuffer pixels, with cycle type, texturing, fill colour, scissor and framebuffer width |
| `BAR_DBG_PROJ=1` *(RT64 fork)* | Every projection with its type (1 perspective / 2 orthographic / 3 rectangle), width, and whether the widescreen adjustment applied |
| `BAR_DBG_SCISSOR=1` *(RT64 fork)* | The scissor rectangles the game emits — how the overscan inset was measured |

### Behaviour knobs

| Variable | Default | Effect |
|---|---|---|
| `BAR_DRAW_DIST=<n>` | 4 | Far-plane multiplier. Overrides the menu setting, so a debugging run can pin a value |
| `BAR_CULL_WIDEN=<n>` | 1.75 | How much wider than 4:3 the game culls. `1` disables |
| `BAR_HUD_ANCHOR=0` | on | Disable HUD anchoring entirely |
| `BAR_HUD_ORTHO=0` | on | Keep rectangles anchored but leave the speedometer needle's ortho layer centred — this is how the two halves are told apart on screen |
| `BAR_AUDIO_XFORM=<0-3>` | 2 | Sample transform: 0 as-is, 1 byteswap, **2 channel-swap (correct)**, 3 both |
| `BAR_SKIP_WIDE=<px>` *(RT64 fork)* | off | Drop textured rectangles at least that wide — a probe for "which element is made of wide texture rectangles" |
| `BAR_ASPECT_COVER` *(RT64 fork)* | — | The coverage threshold at which a projection is widened |

Historical variables appear in `KNOWN_ISSUES.md` and other investigation notes (`BAR_TIME_SCALE`,
`BAR_VI_DIVIDER`, `BAR_NO_REWRITE`, the display-list rewriter's switches). Those belong to parked or
reverted experiments; check `src/` before relying on one.

## Debugging crashes

Use `tools/cdb-run.sh`. It catches a fatal second-chance access violation, dumps registers and all
thread stacks (with line numbers when the `.pdb` is beside the exe), and writes a full minidump,
while passing first-chance exceptions through so normal play is uninterrupted.

```sh
tools/cdb-run.sh
BAR_DUMP=/tmp/x.dmp tools/cdb-run.sh     # BAR_BUILD / BAR_LOG / BAR_CDB also override
```

**Do not launch `cdb` by hand without `-hd`.** A process *created by* a debugger gets Windows' NT
debug heap, which for a continuously-allocating game shows up as steadily climbing memory, worsening
frame times, and eventually audio underruns and visual glitches. That is a debugger artefact, not a
game bug, and it never appears when the executable is run directly. The script bakes in `-hd` and
`_NO_DEBUG_HEAP=1`, and resolves cdb from the WinDbg Store package via `Get-AppxPackage` (the
`WindowsApps` directory cannot be globbed).

## Verifying a build is fully patched

`scripts/fix-recompiled.sh` prints one `ok` line per rule and exits non-zero naming any anchor that
rotted. Read that output — a half-patched tree builds and runs, and misbehaves in ways that look
nothing like a missing patch. See [07](07-codegen-fixups-and-patches.md#why-it-verifies-itself).
