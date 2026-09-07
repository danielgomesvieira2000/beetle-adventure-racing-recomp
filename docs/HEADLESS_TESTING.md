# Headless testing: scripted input + internal-render screenshots

Everything below is **env-var driven and headless** — no window focus, no window-manager capture, no
manual clicking. Compose the vars on one launch. All are zero-cost when unset.

## 1. Hide the UI overlay — `BAR_SKIP_LAUNCHER=1`
Starts with the RmlUi launcher/overlay hidden (`src/ui/bar_ui.cpp` — sets `g_menu_open=false`, so
`draw_hook` draws nothing). The game renders directly. Use this for every headless test.

## 2. Scripted input — `BAR_AUTOPLAY="frames:hexbtn frames:hexbtn ..."`
Implemented in `src/main/main.cpp` (`bar_autoplay_poll`). Space-separated `frames:hexbuttons` phases,
played in order then held neutral. Bypasses the focus gate; `fc` counts input-poll frames (~60/s once
the game is polling). Logs each phase (`AUTOPLAY -> phase ...`).

N64 button mask (hex): `START=1000  A=8000  B=4000  Z=2000`; d-pad `U=0800 D=0400 L=0200 R=0100`
(d-pad bits also drive the analog stick). Keyboard equivalents: A=`X` B=`Z` Start=`Enter` d-pad=`T/F/G/H`.

### Boot sequence → main menu (verified)
The game boots to a **Controller Pak prompt** (`currentGameState` = 14/0xE and it STAYS 14 through the
prompts AND the menu — the state does not distinguish them, so use screenshots to verify, not the state).
Two prompts, then the menu:
1. "No Controller Pak present … **Start Game Without Saving**" (highlighted) → press **A**.
2. "You will be unable to save … Continue? **Yes / No**" (**No** highlighted by default) → press **Up** to
   move to Yes, then **A**.

Recipe that reaches the main menu:
```
BAR_SKIP_LAUNCHER=1 BAR_AUTOPLAY="120:0 30:8000 150:0 30:0800 150:0 30:8000 700:0"
#   wait ~2s, A(start), wait, Up(→Yes), wait, A(confirm), hold
```
Main-menu **screen transitions** are driven by **A** (select) — each fires the recompiled
`func_selection_00402E98` (the `E98start` marker in `BAR_DBG_SLIDE` traces).

## 3. Internal-render screenshot (RT64) — **the standard way to see the screen**
**Always capture RT64's internal render, never a window-manager screenshot.** It reads RT64's actual
presented image back GPU→PNG (960×720), needs no window focus, and works headless / in the background, so
it's the ground truth for "what's on screen." Built into the RT64 fork
(`lib/rt64/src/hle/rt64_present_queue.cpp`, at the draw-hook): on each present it copies the swapchain
texture → readback buffer → `stbi_write_png`. Three ways to request one:

- **`BAR_SHOTS="frame:path …"`** (preferred, scriptable) — capture at scripted input frames, on the same
  timeline as `BAR_AUTOPLAY`, so captures line up with input. `main.cpp bar_shots_poll`.
- **`BAR_SHOT_BURST="fc:dir:count"`** (animations) — see §3b.
- **`RT64_SHOT_TRIGGER=<file> RT64_SHOT_OUT=<png>`** (ad-hoc fallback) — `touch` the trigger, next present
  writes the PNG and deletes the trigger.

**Delete the target PNGs before each run.** They are only written when the run reaches the frame that
asks for them, so a run that diverges or dies early leaves the *previous* run's images in place — and
a script that waits for the file to appear then returns immediately with stale pictures. That has
already produced one round of confident, wrong conclusions. Wait on something the current run writes
(the last `AUTOPLAY -> phase` line in its log) or remove the files first.

```
BAR_SKIP_LAUNCHER=1 BAR_SHOTS="510:menu.png" BAR_AUTOPLAY="120:0 30:8000 150:0 30:0800 150:0 30:8000 700:0" \
  ./beetle-adventure-racing-recomp.exe    # -> menu.png is the main menu, no window needed
```
(A prior `tools/shot.ps1` did a window-manager grab — **superseded**; don't use it.)

## 3b. Burst capture (record an animation) — `BAR_SHOT_BURST="fc:dir:count"`
Captures the next `count` presents to `dir/f0000.png, f0001.png, …`, **one per RT64 present** (not per
input frame). Use this for animations the input-frame timeline can't sample — the main-menu **film-roll**
transition blocks the game thread in a render loop, so it stops polling input and `BAR_SHOTS` frames
freeze mid-roll. RT64 counts presents itself (`bar_rt64_start_burst` in the RT64 fork,
`lib/rt64/src/hle/rt64_present_queue.cpp`); `main.cpp` fires it at input frame `fc`. Trigger a few frames
**before** the transition (while `fc` still advances). `dir` must already exist.
```
mkdir -p roll
BAR_SKIP_LAUNCHER=1 BAR_SHOT_BURST="900:roll:60" \
  BAR_AUTOPLAY="120:0 30:8000 150:0 30:0800 150:0 30:8000 400:0 20:0400 300:0" ./beetle-adventure-racing-recomp.exe &
#   ...reach menu, wait, press Down (film-roll) ~fc910 -> roll/f0000..f0059 = the roll frames
```

## 4. Diagnostics
- `BAR_DBG_STATE=1` — logs `currentGameState` (gGameSettings 0x80025CF0 + 0xA4) transitions.
- `BAR_DBG_SLIDE=1` — logs the R6 film-roll/selection trace (`E98start`, `func_filmroll_*`).
- `BAR_DBG_FPS=1` — logs the SI-poll (game-loop) rate.
- `BAR_DEBUG_OVERLAYS=1` — logs every module load by 4-char tag (`overlay 'frol' ...`).
- `BAR_HUD_TRACE=1` — widescreen HUD anchoring: logs `currentGameState`, `raceState` and `pauseFlag`
  as they change, whether anchoring is armed, and every 2D rectangle it anchors with the class it was
  given. `BAR_HUD_ANCHOR=0` turns anchoring off, `BAR_HUD_ORTHO=0` leaves only the speedometer's
  needle unanchored. See `docs/RACING_DRAW_MAP.md`.
- `BAR_DBG_FRUSTUM=1` -- logs every perspective frustum BAR builds (destination, the six parameters,
  the aspect) and, once, what the draw-distance and culling-width adjustment did. `BAR_DRAW_DIST=<n>`
  scales the far plane (default 4), `BAR_CULL_WIDEN=<n>` widens the sides the game culls against
  (default 1.75); either at `1` is off. See `docs/RACING_DRAW_MAP.md`.
- `BAR_DBG_RECT=1` / `BAR_DBG_PROJ=1` (RT64 fork) — every distinct 2D rectangle, and every projection
  with whether it was widened. These are what the HUD map was measured with.

## Full example (reach menu + screenshot it)
```
cd build-cmake
RT64_SHOT_TRIGGER=shot_req RT64_SHOT_OUT=shot.png BAR_SKIP_LAUNCHER=1 \
  BAR_AUTOPLAY="120:0 30:8000 150:0 30:0800 150:0 30:8000 700:0" ./beetle-adventure-racing-recomp.exe &
sleep 26; touch shot_req; sleep 2.5   # -> shot.png is the main menu
```

## Debugging crashes under a debugger (cdb)

Launch the game under the console debugger with **`tools/cdb-run.sh`**. It catches a fatal
(second-chance) access violation, dumps registers + all thread stacks (with line numbers if
`beetle-adventure-racing-recomp.pdb` is next to the exe), and writes a full minidump — while passing first-chance
exceptions through so normal play is uninterrupted.

```sh
tools/cdb-run.sh                 # run under cdb; on a crash, dumps stacks + build-cmake/bar_crash.dmp
BAR_DUMP=/tmp/x.dmp tools/cdb-run.sh   # override the dump path (BAR_BUILD / BAR_LOG / BAR_CDB too)
```

**Why the script exists (do not launch cdb by hand without `-hd`):** a process *created by* a
debugger gets Windows' NT **debug heap**, which for a continuously-allocating game manifests as
steadily climbing memory, worsening frame times, and eventually timing artifacts (audio underruns,
visual glitches) — a debugger artifact, **not** a game bug, that never appears when the `.exe` is run
directly. The script bakes in `-hd` + `_NO_DEBUG_HEAP=1` to keep the normal heap. It resolves cdb from
the WinDbg Store package via `Get-AppxPackage` (the `WindowsApps` dir can't be globbed).
