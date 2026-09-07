# beetle-adventure-racing-recomp — TODO / Roadmap

_Last updated: 2026-07-03_

The single consolidated task list for the port. Pulls together the open items that were
previously scattered across [STATUS.md](STATUS.md) (the "Current WIP" + "Still stubbed" +
"Remaining roadmap" sections) and [SETTINGS_MENU_AND_HIGH_FPS.md](SETTINGS_MENU_AND_HIGH_FPS.md).

- **Status detail / how-to-resume** for each fixed item lives in [STATUS.md](STATUS.md); this file
  is the forward-looking checklist. When you finish something here, move the detail to STATUS.md
  and check the box here.
- Legend: 🚧 in progress / has a lead · ⬜ not started · ✅ done (kept briefly for context) ·
  ⚠️ caution / known wrong path.

---

## Active priorities — requested 2026-06-30 (round 2)

### R1. ✅ Restart-game function in the pause menu — DONE 2026-06-30 (c480a2f)
`bar_restart_game()` (main.cpp) relaunches the exe with the same args (CreateProcess + GetCommandLineA)
then quits — the recompiled game can't be cleanly re-entered in-process. Pause menu "Restart Game".

### R2. ✅ Pause menu "Main Menu" closes the game — DONE 2026-06-30 (c480a2f)
There is no non-running main menu under the overlay model, so the close-out action is "Quit Game"
(`ultramodern::quit()`), which replaced "Main Menu" in the pause menu.

### R3. ✅ Emulator-style true pause — DONE 2026-06-30 (c480a2f, runtime 5d7b382)
Freeze the sim (gate VI/AI in `vi_thread_func`) AND re-submit the last frame's display lists each
frame in the render context so RT64 keeps presenting the frozen scene → the overlay stays live.
Heartbeat-verified: pause freezes the sim with draw_hook still ~60fps; resume continues; no crash.
See memory `menu-and-high-fps-research` for the technique.

### R4. ✅ Settings to enhance render resolution — DONE 2026-06-30 (2bcf633)
Render-resolution scaling + supersampling now exposed in Settings (`res_option` + `ds_option` SSAA),
applied on restart; Native high-res scales added on the N64ModernRuntime fork. Default internal
resolution is now display-native/Auto (`a078074`). Same work as item **#3**. Detail in STATUS.md.

### R5. ⬜ Setting to disable car LOD
Disable level-of-detail swapping for cars (always render the high-detail model). Needs to find the
car LOD selection in the decomp (`lib/bar-decomp` car/model modules) and a host flag (bar_config-style)
or RDRAM poke / patch. **Files:** `lib/bar-decomp` (find LOD logic), `src/main/bar_config.cpp` or a cheat.

### R6. ✅ Main-menu film-roll transition animates again — DONE 2026-06-30
**Root cause:** the transition is a **VI-origin pan** — `func_filmroll_00400170` loops calling
`osViSwapBuffer(s0)`, stepping the origin `0xF00` B/frame from page A's framebuffer (`0x1DA800`) into
page B's (`0x200000`), one screen height over ~40 frames / ~720 ms at 60 fps. It never redraws; only the
VI origin moves. Our `enable_instant_present()` had put RT64 into **PresentEarly** mode, which presents
only freshly-**rendered** content and drops VI-origin-only changes — so the pan collapsed to an instant
swap. **Fix:** `src/main/rt64_render_context.cpp` selects RT64's **Console** presentation mode (present
strictly from the VI origin, at VI time — what the console does). `BAR_PRESENT_MODE=skip|early|console`
overrides; `BAR_INSTANT_PRESENT=1` = legacy `early`. (SkipBuffering also animates the pan but flashes the
destination page for one frame before it, because it presents the just-rendered buffer; Console doesn't.)
Verified headlessly frame-by-frame (burst capture — see docs/HEADLESS_TESTING.md). Full writeup:
docs/R6_FILMROLL_FINDINGS.md. (The earlier "STATUS-3 flash / framebuffer" theory was wrong.)

---

## Active priorities — requested 2026-06-30 (round 1)

These seven were raised together as the current focus.

### 1. ✅ Clean up the launcher/settings UI (our UI, not the game's) — DONE 2026-06-30
**Root cause (not dp ratio):** RmlUi ships **no HTML UA stylesheet**, so `<div>/<h1>/<p>` default to
`display:inline` → `#panel`/`#config_root`/`#cheats_root` ignored `width`/`margin:auto` (full-width
buttons, overlapping title, collapsed vertical flow). **Fix:** added `div,h1,h2,p{display:block}` to
each `.rml` `<style>`. Launcher, Settings, and Cheats now render as clean centered cards
(screenshot-verified). Also learned: RmlUi flexbox works **only** when the control has an explicit
width (config.rml `<select>` 200dp) and the container isn't `overflow-y:auto` (which collapses it to
min-content) — so the Cheats page uses full-width clickable block rows (the proven `.btn` pattern),
not flex. Remaining UI polish is minor (the Settings `range` slider has no visible track/thumb — RmlUi
needs a decorator for that; folded into a future pass).
**Files touched:** `src/ui/assets/{launcher,config,cheats}.rml`, `src/ui/ui_renderer.cpp` (build fix:
`createCommandList()` takes no args in this plume version), `src/ui/bar_ui.cpp`.

### 2. ✅ Fix see-through seams between world-geometry planes — BOTH FIXES SHIPPED 2026-06-30 (4a46488, 16cec5e)
_(Optional follow-up: one RenderDoc capture to reconfirm the seam class. Implementation below is done.)_
**Problem:** adjacent/abutting (often coplanar) world-geometry tris show a thin see-through crack at
the shared edge, revealing the background/skybox.
**Root cause (CONFIRMED, not depth/decal):** classic N64 HLE artifact. The RDP fills shared edges via
sub-pixel **coverage** (fixed-point grid, complementary coverage → seamless); HLE transforms vertices
in **float** and the host GPU rasterizer reproduces neither the coverage edge-fill nor the **VI "divot"
filter** (a per-channel median-of-3 that hardware uses at scanout to fill the "one-pixel holes [that]
appear where polygons are connected together" — Nintendo VI manual, verbatim our bug). Proven by
forcing angrylion's accurate RDP into non-AA mode → gaps appear ([GLideN64 #2397](https://github.com/gonetz/GLideN64/issues/2397)).
No HLE renderer fully eliminates these; they **mask** (AA/resolution) or **patch geometry**
(Zelda64Recomp's "Seam Fixer" mod). Our default is **native** rasterization (`res_option=Original`,
`ds_option=1`), so the holes are ≤1 native px but show because we run **no divot** — then get magnified
by the window scale-up. (Decal/Z-fight _flicker_ is a different bug, out of scope.)
**Full plan + exact code changes:** [SEAM_FIX_PLAN.md](SEAM_FIX_PLAN.md). Two fixes, each with a
Settings-menu option:

#### 2A. ✅ MSAA + Supersampling — DONE 2026-06-30 (4a46488): default 4× MSAA + SSAA select
Adds edge coverage + supersamples thin geometry → softens/reduces seams. MSAA control already exists;
this defaults it to 4× and exposes the existing (but menu-less) `ds_option` SSAA knob.
- Default `msaa_option` None → **MSAA4X** ([config.cpp:129](../src/game/config.cpp)).
- New **Supersampling (SSAA)** `<select id="ds_option">` (Off/2×/4×) — add to `config.rml` + the
  `config_to_controls`/`controls_to_config_and_apply` get/set in [bar_ui.cpp:144-178](../src/ui/bar_ui.cpp)
  (int, like `rr_manual`). Both apply **on restart** (the `m_live_*` lock).
- **Files:** `src/game/config.cpp`, `src/ui/assets/config.rml`, `src/ui/bar_ui.cpp`.

#### 2B. ✅ VI "divot" filter — DONE 2026-06-30 (4a46488), threshold tuned to 0.12 (16cec5e); default Auto
A per-channel **horizontal median-of-3 at native-pixel spacing**, folded into the existing VI pixel
shader (the shader already samples the fb at native-texel granularity → no new pass/pipeline/shader
file; runs at the correct resolution; composes with all filtering). New **Seam filter (VI divot)**
`<select id="divot_option">` (Auto/On/Off) — a per-frame flag, so it applies **live** (not under the
restart lock). Honest limit: without coverage a median can't distinguish a 1px hole from an intentional
1px line — mitigated by horizontal-only + threshold + Auto/Off toggle (B3 coverage-gated version is a
larger follow-up).
- **B0 ✅ verified (static):** BAR's LAN1 VI mode sets `VI_CTRL_DIVOT_ON` (0x10) with no dynamic
  toggle (`bar-decomp/.../vimodentsclan1.c:19`, `sched.c:74-97`) → divot bit always on → **default Auto**.
- **B2 plumbing ✅ verified:** mirror `filtering` — `userConfig.filtering` →
  `sharedResources->userConfig` ([present_queue.h:112](../lib/rt64/src/hle/rt64_present_queue.h)) →
  `renderParams` ([:323](../lib/rt64/src/hle/rt64_present_queue.cpp)); push-constant layout is
  `sizeof`-based so adding CB fields needs no layout edit.
- **✅ Prereq done:** `lib/rt64` forked to `bryankruman/rt64` + submodule repointed (2026-06-30, commit
  `2d8383a`); RT64 edits now commit on the fork (`upstream` = `rt64/rt64`, pinned `f0728a2` unchanged).
- **Files:** `lib/rt64/src/shared/rt64_video_interface.h` (CB field),
  `lib/rt64/src/shaders/VideoInterfacePS.hlsl` (median), `lib/rt64/src/render/rt64_vi_renderer.{h,cpp}`,
  `lib/rt64/src/hle/rt64_present_queue.cpp` (set `renderParams.divotFilter` next to `.filtering` at :323),
  `lib/N64ModernRuntime/.../config.hpp` (`DivotFilter` enum), `src/game/config.cpp`,
  `src/ui/assets/config.rml`, `src/ui/bar_ui.cpp`, `src/main/rt64_render_context.cpp` (map + plumb like
  `filtering`).

**Still worth one RenderDoc capture** of a seam to reconfirm it's the coverage/edge class (background
through the crack), not a decal/Z-fight case.

### 3. ✅ Add a high-**resolution** rendering mode — DONE 2026-06-30 (2bcf633; same work as R4)
**Note:** this is resolution scaling (internal render res), *distinct from* the high-**fps**
interpolation work in [SETTINGS_MENU_AND_HIGH_FPS.md](SETTINGS_MENU_AND_HIGH_FPS.md) (which is
already wired/active). RT64 supports internal-resolution upscaling natively.
**Verified:** the config schema already exists — `GraphicsConfig` (res scale / resolution option)
in [config.hpp](../lib/N64ModernRuntime/ultramodern/include/ultramodern/config.hpp). We just never
set it, and it isn't exposed in the menu.
**Next:** build a `GraphicsConfig` with the resolution option/scale set and call
`set_graphics_config()` (same path the menu uses); then surface it as a dropdown/slider in the
Settings document. Pairs with item 1 and the existing config-persistence (`graphics.json`).
**Files:** `src/game/config.cpp`, `src/ui/bar_ui.cpp` + `config.rml`,
`src/main/rt64_render_context.cpp` (config → RT64 mapping).

### 4. ⬜ Add texture smoothing / upscaling for the high-res mode  _(unblocked — #3 done; SSAA shipped, but the texture-filtering toggle + texture-pack support are still TODO)_
Depends on item 3. Two levers in RT64: (a) **texture filtering** (three-point vs bilinear / linear
filtering of N64 textures) exposed as a graphics option, and (b) **high-res texture-pack** support
(`lib/rt64/TEXTURE-PACKS.md`) for replacement textures. Start with (a) — a filtering toggle in the
Settings menu — then evaluate (b).
**Files:** `src/main/rt64_render_context.cpp`, `src/ui/...config.rml`, `lib/rt64/`.

### 5. ⬜ Fully fix audio clipping **without** adding latency
**Context:** the big audio bugs are already fixed (latency growth, transition crackle, thread
starvation — see [STATUS.md](STATUS.md) WIP item 2 / memory `bar-perf-audio-findings`). What
remains: the glitch-free **buffer floor is 2.5 VI** (`buffer_offset_frames`,
[audio.cpp](../lib/N64ModernRuntime/ultramodern/src/audio.cpp)); dropping toward ~1.0 VI to cut
latency reintroduces crackle from host/SDL device-callback jitter.
**Goal:** eliminate the underrun/clip source so the buffer can shrink (low latency *and* clean).
**Next:** drive it with `BAR_AUDIO_DBG` (queue min/max + underrun + peak/clip). Candidate levers:
shrink the SDL device buffer (`want.samples`, currently 1024 ≈ 46 ms) in `bar_open_audio` as a
separate latency knob; tighten cooperative preemption (`src/main/bar_preempt.cpp`) so the audio
thread is serviced more punctually; investigate true peak clipping in the PCM transform vs. pure
underrun. Tune with `BAR_AUDIO_BUFFER`.
**Files:** `lib/N64ModernRuntime/ultramodern/src/audio.cpp`, `src/main/main.cpp` (`bar_open_audio`),
`src/main/bar_preempt.cpp`, `src/main/hw_stubs.cpp`.

### 6. ✅ Input-management UI with modern-controller support — DONE (a: 2026-06-30 · b: 2026-07-01 `4b1f6f8`; in-game rebind/gameplay/save round-trip still needs a runtime pass)
**Done (a):** SDL game-controller support added (`bar_sample_gamepad`/`bar_poll_gamepad` in
[main.cpp](../src/main/main.cpp)) — hotplug open/close in `update_gfx`, sampled on the main thread into
an atomic snapshot (thread-safe vs the game/SI poll threads), folded into `bar_poll_keyboard` (additive
to the keyboard, focus-gated). Default map: A→A B→B Start→Start L/R shoulders→L/R, LT→Z, D-pad→D-pad,
right-stick→C-buttons, left-stick→analog. The controller also navigates the menu (D-pad→arrows, A→Enter
via synthesized key events through the existing RmlUi nav path). Build-verified; behavior not yet
runtime-tested (needs a controller). **(b) DONE 2026-07-01 (`4b1f6f8`):** Controls dialog — 4-port
device config, rebinding (keyboard + controller), Rumble + Controller Paks, persisted to `input.json`.
Remaining: an in-game rebind/gameplay/save round-trip test.
**Was verified:** input was **keyboard-only**. `input_get` ([main.cpp](../src/main/main.cpp))
reads `SDL_GetKeyboardState`; `SDL_INIT_GAMECONTROLLER` is initialized
([main.cpp:149](../src/main/main.cpp)) but no controller is ever opened/read; `input_set_rumble`
([main.cpp:341](../src/main/main.cpp)) is a no-op; and the `*_controls` functions in
[bar_ui.cpp](../src/ui/bar_ui.cpp) are settings-form widgets, **not** input remapping. (Matches
STATUS "Real gamepad input + a rebind UI … TODO".)
**Next:** (a) open SDL game controllers and map → N64 pad in `input_get` (sticks + buttons + rumble
via `input_set_rumble`); (b) add a **Controls** tab to the Settings menu for rebinding (keyboard +
controller), with profiles keyed by SDL controller GUID, persisted like `graphics.json`. The
reference implementation is `RecompFrontend`'s `recompinput` (see SETTINGS doc §1.1, Appendix B).
**Files:** `src/main/main.cpp` (input callbacks), `src/ui/bar_ui.cpp` + a `controls.rml`,
`src/game/config.cpp` (persist `controls.json`).

### 7. ⬜ Add an appropriate game icon
**Verified:** `icons/` and `assets/` are empty (`.gitkeep` only); no `.ico`, no Windows `.rc`
resource, and no `SDL_SetWindowIcon`/`WM_SETICON` call — so the title bar/taskbar/exe show the
default icon (visible in `ui_launcher.png`).
**Next:** create an icon (multi-size `.ico` for the Windows exe/taskbar + a PNG for
`SDL_SetWindowIcon` at runtime), drop it in `icons/`, add a `.rc` to the exe target in
[CMakeLists.txt](../CMakeLists.txt), and call `SDL_SetWindowIcon` after `create_window`. Use
original art only (no shipped game assets — see README license note).
**Files:** `icons/`, `CMakeLists.txt`, `src/main/main.cpp`.

### 8. ⬜ On-screen FPS counter, toggleable in Settings
**Verified current state:** `BAR_FPS` is a **log-only** probe — it prints fps to the log once per
second ([events.cpp:457](../lib/N64ModernRuntime/ultramodern/src/events.cpp)), *not* an on-screen
overlay. Separately, the `show_fps` entry in [bar_cheats.cpp](../src/main/bar_cheats.cpp) toggles
BAR's **own built-in** on-screen framerate display (a ROM debug feature).
**Next:** add a real on-screen FPS counter with a Settings checkbox. Either (a) draw a small overlay
through the existing RmlUi / [ui_renderer.cpp](../src/ui/ui_renderer.cpp) layer (host-side, also
visible in menus, counts display fps) — cleaner — or (b) wire the existing `show_fps` cheat into the
Settings menu. Persist the toggle with the other settings.
**Files:** `src/ui/ui_renderer.cpp` + `config.rml`, `src/main/bar_cheats.cpp` (if reusing
`show_fps`), `lib/N64ModernRuntime/ultramodern/src/events.cpp`. Relates to items 1 & 9.

### 9. ⬜ Expose audio buffer + SDL device settings in Settings
**Verified current state:** both are reachable only outside the menu today — the **audio buffer
depth** is `buffer_offset_frames = 2.5`
([audio.cpp:46](../lib/N64ModernRuntime/ultramodern/src/audio.cpp)), overridable via the
`BAR_AUDIO_BUFFER` env var; the **SDL device buffer** is `want.samples = 1024` (≈46 ms),
**hardcoded** in `bar_open_audio` ([main.cpp:255](../src/main/main.cpp)). The config schema is
graphics-only (`GraphicsConfig` has no audio fields) and we persist only `graphics.json`.
**Next:** add an audio config (e.g. `sound.json` via the same [config.cpp](../src/game/config.cpp)
pattern as graphics) with buffer-depth + SDL-sample-count fields, surface them as slider/dropdown
controls in Settings, and apply at runtime (set `buffer_offset_frames`; pass the sample count into
`bar_open_audio` and re-open the device on change). These are exactly the knobs **item 5** tunes, so
do them together.
**Files:** `src/game/config.*`, `src/ui/bar_ui.cpp` + `config.rml`, `src/main/main.cpp`
(`bar_open_audio`), `lib/N64ModernRuntime/ultramodern/src/audio.cpp`. Relates to items 1 & 5.

---

## Carried over from STATUS.md / docs (still open)

### Boot / gameplay
- 🚧 **Legal/logo screen skip** — make the boot legal screens (Nintendo / EA / trademark) and
  attract cinematics skippable on A/B/Start, *except* the controller-pak prompt. Attract part is
  done (`fix-recompiled.sh` rule D); the legal screens are driven by un-decompiled boot assembly —
  no handler pinned down yet. See [STATUS.md](STATUS.md) WIP item 1 for the next-step lead.

### Rendering
- 🚧 **Menu page-transition background flash** — one-frame background flash on each menu page slide.
  ⚠️ The previously-proposed fix was **refuted — do not ship it** (wrong gfx-mgr struct address).
  Likely an RT64 framebuffer-as-texture reconstruction with no tracked target yet. See
  [STATUS.md](STATUS.md) WIP item 3.
- 🚧 **Track-map preview low/choppy fps** — the spinning 3D track-map on the map-select screen
  animates choppily while the rest of the menu is smooth 60fps. Needs one decisive live trace
  (rotation angle + real sec/frame + present cadence) to pick the fix; must land as an address-keyed
  `fix-recompiled.sh` rule. See [STATUS.md](STATUS.md) WIP item 4.
- ⬜ **Per-game F3DEX2 rendering quirks** as they surface (item 2 above is the first concrete one).

### High-FPS (interpolation) — follow-on phases
Phase 1 (enable interpolation + `get_display_framerate` + `enable_instant_present`) is merged/active
(memory `menu-and-high-fps-research`). Remaining:
- ⬜ **Phase 2 (throughput, complementary):** enable `/GL` + `/LTCG` (build currently has `/O2` but
  no LTCG), consider PGO on `RecompiledFuncs`, strip bring-up logging, restore `SetThreadPriority` /
  `timeBeginPeriod(1)`. Raises the *real* fps that feeds the interpolator — matters most in races
  (CPU-bound). SETTINGS doc §2.5.
- ⬜ **Phase 3 (artifact polish, large/optional):** extended-GBI transform tagging in `bar-decomp`
  for car/track/camera + interpolation-skip flags for HUD/particles/replay cuts. Zelda64Recomp-scale
  effort; only after a stable base. SETTINGS doc §2.5.

### Menu / mods
- The Tier-2 RmlUi launcher is now **live + cleaned up** (Freetype added, `BEETLE_ENABLE_UI` on,
  layout fixed — item 1 done) and includes a **Cheats** page. Newer than STATUS.md.
- ⬜ **Mod manager UI** — the launcher shows a "No mods installed" stub; wire the librecomp mod
  loader (`scan_mod_folder`/`load_mods`/`enable_mod`) to an actual enable/order list. SETTINGS doc §1.
- ⬜ **Surface remaining `BAR_*` env flags in the menu** so they're not env-var-only — FPS counter
  (`BAR_FPS`) and audio buffer (`BAR_AUDIO_BUFFER`) are now items **8** and **9**; intro skip
  (`BAR_NO_INTRO_SKIP`) and the audio debug/transform flags still need a home.

### Runtime correctness (lower priority, not user-visible yet)
- ⬜ Most `src/main/os_unimpl_stubs.cpp` functions are link-only no-ops (`__osViSwapContext`,
  `__osTimerInterrupt`, PI/thread-queue). Only `__osSiRawStartDma` has real behavior. Revisit if a
  feature needs them.

---

## ✅ Recently done (context — detail in STATUS.md)

Kept short so these don't get re-attempted:
- ✅ **Settings buildout (2026-06-30/07-01):** render-resolution scaling + SSAA + default 4× MSAA + VI
  divot seam filter (R4/#2A/#2B/#3 — `2bcf633`, `4a46488`, `16cec5e`); default internal res → Auto (`a078074`).
- ✅ **Controls dialog + 4-port input** — rebinding, Rumble + Controller Paks, `input.json` persistence
  (item 6b — `4b1f6f8`). In-game rebind/gameplay/save round-trip still needs a runtime pass.
- ✅ **True in-game pause** — RT64 repeat-last-frame while the sim is frozen (`c270752`).
- ✅ **HUD/menu letterbox + widescreen controls** — `present_fill_mode` Pillarbox/Crop/Stretch (`d244b5a`).
- ✅ **First-run ROM picker + cached-ROM boot** + SP_STATUS dlist-recovery guard (`c3c8971`).
- ✅ Boots → races at **60 fps**, keyboard input, audio playing (overlay bridge, VI/SI/RSP).
- ✅ **Black screen** (controller/SI `osRecvMesg` deadlock) fixed.
- ✅ **Menu 60 fps** (SI requeue busy-spin) fixed.
- ✅ **Audio**: latency growth, transition crackle, thread starvation fixed (item 5 is the remaining
  latency-vs-clip tuning, not a regression).
- ✅ **Coventry Cove crash** (heap OOM) fixed.
- ✅ **Config persistence** (`graphics.json`) + **high-fps Phase 1** active.
- ✅ Fast-boot intro/attract skip (A/B/Start).
- ✅ **Tier-2 RmlUi UI ON by default** + build/run-verified (Freetype submodule added, `createCommandList`
  build fix, launcher/settings/cheats layout fixed — item 1).
- ✅ **Menu interactivity fixed** — the menu is now an OVERLAY over the auto-started game (RT64 only drives
  the UI render+input hook while the game produces frames, so a pre-game launcher froze). Mouse + wheel-scroll
  + arrow-key focus nav work; Play hides the overlay, F1 reopens it in-game. Cheats list is scrollable. See
  memory `menu-and-high-fps-research` for the architecture.
- ✅ **Cheats menu checkmark** — enabled cheats show a filled box + a CSS-drawn checkmark (LatoLatin has no
  ✓ glyph), disabled show an empty box.
- ✅ **Controller support (item 6a)** — SDL game controllers drive the game + navigate the menu (build-verified).
- ✅ **Graphics-settings crash fixed** — enabling MSAA / changing resolution crashed (RT64's runtime
  `updateMultisampling()` skips the device sample-count check that setup does). Now MSAA + resolution
  apply on restart via the validated setup path; other settings still apply live. See memory.
- ✅ **In-game pause menu (Esc / F1 / controller Back)** — Resume / Settings / Cheats / Main Menu / Quit.
  ⚠️ It's a **soft pause** (input frozen, but the sim keeps running behind): a true host-level freeze
  was proven to freeze the overlay too (RT64 stops presenting once the game thread is frozen). A real
  freeze needs BAR's own pause (renders the paused scene) — see follow-up below.

### Follow-up: true in-game pause (real sim freeze) — ✅ DONE 2026-07-01 (c270752)
Routed the runtime pause into RT64's repeat-last-frame path (`State::externalPaused` +
`Application::setPaused` on the rt64 / N64ModernRuntime forks), so frames keep flowing while the sim is
frozen and the overlay stays live. Built clean; recommend one in-game confirm. Memory: `pause-freeze-root-cause`.
- ✅ **BAR Cheats menu** (`src/main/bar_cheats.*` + `src/ui/assets/cheats.rml`, F1 in-game overlay) —
  host-side RDRAM pokes for unlock-all / reveal in-game cheat menu / solo race / infinite laps / super
  speed / steering assist / show-FPS / no-glare. Vetted fixed-address list + detail in memory
  `bar-cheats`. Pipeline verified end-to-end (load → SI-hook apply → race, no crash).
