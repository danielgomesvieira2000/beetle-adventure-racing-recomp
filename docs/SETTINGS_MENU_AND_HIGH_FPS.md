# Settings Menu & High-FPS — Research & Implementation Plan

_Last updated: 2026-06-29 · Status: research only, no code changed_

This report answers two questions for beetle-adventure-racing-recomp (static-recomp PC port of **Beetle
Adventure Racing**, built on the same N64Recomp + RT64 + N64ModernRuntime stack as
Zelda64Recompiled):

1. **A pre-game settings/launcher menu** (change settings, load mods) — what other recomp
   projects do, what we have, and how to add it.
2. **"Overclocking" the framerate** (smooth 60 / 144 fps) **while keeping in-game speed and
   pacing identical** — how the ecosystem does it, what we have, and how to enable it.

---

## TL;DR (the two headline findings)

- **Menu:** We have **none** today — `main()` auto-starts the game with a hard-coded ROM path
  and no UI. The *runtime fully supports* a menu-before-game model (it sits in
  `GameStatus::None` until something calls `recomp::start_game()`), and the RmlUi + lunasvg
  submodules are already vendored behind a CMake switch (`BEETLE_ENABLE_UI`, currently **OFF**
  and incomplete — no Freetype, no RT64 render bridge, no UI source). There are **three
  realistic tiers**: (0) RT64's built-in ImGui dev overlay [free], (1) a minimal ImGui
  settings + ROM-picker MVP [~days], (2) the full RmlUi launcher + mod manager via
  `RecompFrontend` [~weeks]. **Recommended: Tier 1 now, Tier 2 later.**

- **High-FPS:** This is **almost already wired.** RT64 does *renderer-side frame
  interpolation* that decouples the **display rate** from the **game/VI rate**, and our
  render context already maps the framerate config straight into RT64
  ([rt64_render_context.cpp:112-113](../src/main/rt64_render_context.cpp)). **Game speed is
  guaranteed identical** because the game/VI/audio clock is locked to `60 × speed_multiplier`
  with `speed_multiplier = 1` as a *compile-time constant*
  ([timer.cpp:19](../lib/N64ModernRuntime/ultramodern/src/timer.cpp)) that the rendering path
  cannot touch. The only reasons we don't get high-fps today: (a) nothing ever *sets*
  the refresh-rate option (it defaults to `Original` = no interpolation), (b)
  `get_display_framerate()` is hard-coded to 60, and (c) `enable_instant_present()` is a
  no-op. Fixing those three is a **small change**. The caveats are BAR-specific: races are
  CPU-bound (real frames already drop below 60), and artifact-free interpolation of the HUD /
  dust / replay-cuts needs per-game annotation work.

---

# Part 1 — Settings / Launcher Menu

## 1.1 How the recomp ecosystem does it

The flagship reference is **Zelda64Recompiled** (Majora's Mask). Key facts (verified against
our vendored libraries and the upstream repos/docs):

**UI toolkit: RmlUi** — an HTML/CSS-like retained-mode C++ UI library (`mikke89/RmlUi`),
with **FreeType** as the font engine and **lunasvg** for SVG icons. RmlUi renders *through*
RT64 via a custom `RmlRenderInterface_RT64` (it builds RT64/plume command lists; it is not a
separate GL/D3D path). Menus use RmlUi data-bindings and `.rml`/`.rcss` (authored as SCSS)
documents, with controller/keyboard navigation via RCSS `nav-*`/`tab-index`/`focus`.

**Where the code lives — there has been a deliberate extraction:**
- **Original / in-repo:** `Zelda64Recomp/src/ui/` (`ui_launcher.cpp`, `ui_config.cpp`,
  `ui_config_sub_menu.cpp`, `ui_mod_menu.cpp`, `ui_mod_details_panel.cpp`,
  `ui_mod_installer.cpp`, `ui_renderer.cpp`, …) plus assets (`config_menu.rml`,
  `recomp.rcss`).
- **Reusable library — `RecompFrontend`** (`github.com/N64Recomp/RecompFrontend`): provides
  `recompinput` (SDL2 input handling) and **`recompui`** (a UI API built on RmlUi that
  *already contains* the config menus, mod menu, and a mod-facing UI API). This is the
  forward direction, but it is young and tightly coupled to RmlUi + RT64 + plume + SDL2 — you
  adopt the whole frontend, not a drop-in widget.

**Settings exposed** (graphics tab maps to `ultramodern::renderer::GraphicsConfig` — the
*same struct we already have*, see §1.2): resolution scale, window mode, aspect ratio, HUD
ratio, MSAA, **refresh-rate / target FPS**, high-precision framebuffer, downsample. Plus
audio, controls/remapping (keyboard + controller profiles keyed by SDL GUID), and
general/gameplay tabs.

**Persistence:** per-user app folder, e.g. on Windows `%LOCALAPPDATA%\<ProjectName>\`,
written as a few JSON files (`graphics.json`, `controls.json`, `sound.json`, `general.json`).
Saves go in a `saves/` subfolder. A `portable.txt` next to the exe forces "use current dir."

**Mod system (the loader is in `librecomp`, independent of the UI):**
- Package format **`.nrm`** = a ZIP (read with miniz) containing **`mod.json`** (manifest),
  **`mod_binary.bin`** (recompiled mod code), **`mod_syms.bin`** (symbols), plus optional
  `additional_files` and per-platform native libs (`.dll`/`.so`/`.dylib`). Loose folders also
  work. Authors build them with **`RecompModTool mod.toml <out>`**.
- The user drops mods in **`<app folder>/mods/`**; enable/order state persists in
  `mods.json`; per-mod settings under `mod_config/`.
- Public API a menu calls: `scan_mod_folder()`, `load_mods()`, `get_all_mod_details()`,
  `enable_mod(id, enabled)`, `set_mod_index()`, `get/set_mod_config_value()`. Mods declare a
  config schema (Enum / Number / String) that the mod menu renders automatically.

**Lifecycle — it is both a launcher and an in-game overlay.** The launcher context shows
first (ROM pick + main menu); the config context is reachable before *and* during gameplay.
"Play" calls `recomp::select_rom()` then `recomp::start_game(game_id)` and hides the launcher.
The recompiled game runs on ultramodern's threads; the UI renders over it through RT64.

## 1.2 What beetle-adventure-racing-recomp has today

| Piece | State | Evidence |
|---|---|---|
| Menu / launcher UI | ❌ none | [main.cpp:311-338](../src/main/main.cpp) auto-starts with a hard-coded ROM path |
| Graphics config schema | ✅ full | `GraphicsConfig` + enums in [config.hpp:61-78](../lib/N64ModernRuntime/ultramodern/include/ultramodern/config.hpp), JSON-serializable |
| Config get/set API | ✅ | `get_graphics_config()` / `set_graphics_config()` ([config.hpp:80-81]) — but **we never call set** |
| Config file load/save | ❌ host's job | `register_config_path(cwd)` is called ([main.cpp:300]) but nothing reads/writes a file; should be a per-user folder, not cwd |
| Mod system (loader) | ✅ in librecomp | `mods.hpp` — `scan_mod_folder`, `load_mods`, `enable_mod`, … — **not initialized by us** |
| Runtime "menu before game" support | ✅ | `GameStatus::None → Running`; `start_game()` flips it; `is_game_started()`/`wait_for_game_started()` gate the game thread |
| RmlUi + lunasvg submodules | ✅ vendored | `.gitmodules` (`lib/RmlUi` = mikke89, `lib/lunasvg` = sammycage) |
| RmlUi build wiring | ⚠️ stub, OFF | `option(BEETLE_ENABLE_UI … OFF)` ([CMakeLists.txt:27](../CMakeLists.txt)); gated `add_subdirectory`/link block at [58-66] and [182-184] |
| Freetype dependency | ❌ missing | comment at [CMakeLists.txt:55-57]: needs a `lib/freetype-windows-binaries` submodule + `FREETYPE_*` vars |
| RmlUi→RT64 render bridge | ❌ missing | our `rt64_render_context.cpp` was "reduced … (no … UI hooks)" ([header comment](../src/main/rt64_render_context.cpp)) |
| `recompui`/`RecompFrontend` | ❌ not vendored | only raw RmlUi+lunasvg (the older in-repo `src/ui` approach) |

**Bottom line:** the *runtime and config plumbing are ready*; what's missing is the UI layer
itself (render bridge + input bridge + documents/widgets) and config-file persistence. The
CMake was scaffolded to mirror Zelda's **in-repo `src/ui`** approach, not the newer
`RecompFrontend`.

## 1.3 Implementation options

### Tier 0 — RT64's built-in ImGui dev overlay (free, today)
RT64 vendors **Dear ImGui** (`lib/rt64/src/contrib/imgui/`) and a per-frame draw-call
inspector, gated on `developerMode`. We already pass that flag into the render context
(`app->userConfig.developerMode = debug` at [rt64_render_context.cpp:204]). This is a
*developer* tool, not a settings menu, but it costs nothing and is useful for the bring-up
work in [STATUS.md](STATUS.md).

### Tier 1 — Minimal ImGui settings + ROM picker (MVP, ~2-4 days) ⭐ recommended first
Because RT64 already initializes ImGui, the cheapest *real* settings UI is a small ImGui
window we draw from the render context that reads/writes `GraphicsConfig` and persists a JSON.
A native file dialog (or `portable.txt`-style fixed path) covers ROM picking.

Steps:
1. **Config persistence (host-owned).** Add `src/game/config.cpp`: load/save a
   `config.json` using `GraphicsConfig`'s existing nlohmann serialization; store it under a
   real per-user dir (`%LOCALAPPDATA%\beetle-adventure-racing-recomp\`) and pass that to
   `register_config_path()` instead of `cwd` ([main.cpp:300]). On startup, load → build a
   `GraphicsConfig` → `set_graphics_config()`.
2. **Defer auto-start.** Today a detached thread calls `start_game()` once the VI ticks
   ([main.cpp:330-338]). Replace with: stay in `GameStatus::None`, draw the menu, and only
   call `recomp::select_rom()` + `recomp::start_game()` when the user clicks "Play."
3. **ImGui settings window.** Draw an overlay in/around `update_screen()` exposing the
   `GraphicsConfig` fields (resolution, window mode, MSAA, **refresh rate / target FPS** — see
   Part 2) and a ROM-picker button; on change call `set_graphics_config()` (the runtime
   already routes that to `update_config()` → `app->updateUserConfig(true)`).
4. **Mod loading (optional, no UI).** Call the librecomp mod init/scan at startup so a
   `mods/` folder is honored; a toggle list can come later.

Trade-off: no fancy controller navigation, look diverges from Zelda's themed UI, but it's
fast and unblocks "change settings before playing."

### Tier 2 — Full RmlUi launcher + mod manager (~2-4 weeks)
Two sub-paths:
- **2a. Adopt `RecompFrontend` (`recompui` + `recompinput`).** Add it as a submodule, wire the
  `RmlRenderInterface_RT64` into our render context, and switch startup to the launcher →
  `start_game()` flow. You get the themed launcher, config tabs, **mod manager UI**, and
  controller nav "for free," but inherit a young, tightly-coupled dependency.
- **2b. Port Zelda's in-repo `src/ui/`.** Matches what our CMake was scaffolded for (raw
  RmlUi + lunasvg already vendored). More code to adapt and de-Zelda-ify, but no new
  dependency and full control.

Either 2a or 2b additionally requires: add **Freetype** (the missing
`lib/freetype-windows-binaries` submodule + `FREETYPE_*` CMake vars), flip
`BEETLE_ENABLE_UI=ON`, author/port the `.rml`/`.rcss` documents, and route SDL input into the
UI.

**Recommendation:** Do **Tier 1** now (it delivers the user-visible "settings before the
game" + persistence and is the natural place to surface the FPS slider from Part 2). Graduate
to **Tier 2a (`RecompFrontend`)** once the game itself is stable (audio + input + the ~30s
race crash are still open per [STATUS.md](STATUS.md)) and a real mod ecosystem is wanted.

---

# Part 2 — High-FPS ("overclock") with identical game speed

## 2.1 Two different mechanisms — only one is relevant

| | (A) RT64 renderer-side **interpolation** | (B) Game-logic **high-fps patches** |
|---|---|---|
| What speeds up | only *presentation* (frames shown/sec) | the game's *update loop* |
| Touches game logic? | **No** — game is unaware extra frames exist | **Yes** — every timer/physics constant must be rescaled |
| Used by Zelda64Recomp et al.? | **Yes — this is the primary mechanism** | **No** (what looks like it is really *annotation* feeding A) |
| Risk to game speed | **None, by design** | High, easy to get wrong |
| Effort for BAR | small (enable) → large (artifact polish) | very large + risky (avoid) |

The ecosystem ships high-fps via **(A)**. The game keeps ticking at its native cadence; RT64
synthesizes the in-between frames. We should do the same. **Do not** pursue (B).

## 2.2 Why game speed stays *exactly* identical (the two-clock model)

There are two independent clocks, and game logic is wired only to the first:

```
  CLOCK 1 — game / VI / audio (FIXED, native)          CLOCK 2 — display present (VARIABLE)
  ───────────────────────────────────────────          ──────────────────────────────────
  vi_thread fires at 60 × speed_multiplier Hz           RT64 present/workload threads
  speed_multiplier = 1  (compile-time const)            target = monitor refresh or manual
  → posts VI-retrace + AI(audio) messages               → interpolates N game frames → M
  → THIS unblocks the game loop & paces audio              display frames (accumulator + lerp)
            │                                                        ▲
            └──────────── game produces frames ──────────────────────┘
                          (Clock 2 only CONSUMES them; never feeds back)
```

- Clock 1: [events.cpp:189](../lib/N64ModernRuntime/ultramodern/src/events.cpp) paces the VI
  thread at `60 * get_speed_multiplier()`; it posts the VI-retrace and AI/audio messages that
  actually advance the game and gate audio. `speed_multiplier` is a **`constexpr 1`**
  ([timer.cpp:19](../lib/N64ModernRuntime/ultramodern/src/timer.cpp), returned by
  `get_speed_multiplier()` at :148) — the render path has no way to change it.
- Clock 2: RT64 keeps a fractional-frame accumulator that turns the game's real frames into M
  display frames and blends between two real frames (translation/scale lerp, rotation slerp;
  `rt64_rigid_body.cpp`). `osViSwapBuffer` only chooses which buffer the next VI scans out — it
  does **not** pace the game.

Because Clock 2 is a pure *consumer* of frames Clock 1 already produced, generating 1, 2, or 6
display frames between two game frames changes nothing the game can observe. **That is the
guarantee** the user is asking for.

## 2.3 What beetle-adventure-racing-recomp has today — the plumbing is already wired

The framerate config flows **all the way through to RT64 already**:

```cpp
// src/main/rt64_render_context.cpp:112-115  (set_application_user_config)
app->userConfig.refreshRate       = to_rt64(config.rr_option);     // Original/Display/Manual
app->userConfig.refreshRateTarget = config.rr_manual_value;        // e.g. 144
...
app->userConfig.displayBuffering  = RT64::UserConfiguration::DisplayBuffering::Triple;
```

And the runtime side already resolves the target rate
([events.cpp:293-305](../lib/N64ModernRuntime/ultramodern/src/events.cpp)):

```cpp
uint32_t ultramodern::get_target_framerate(uint32_t original) {
    switch (config.rr_option) {
        case RefreshRate::Original: default: return original;                 // game rate, no interp
        case RefreshRate::Manual:           return config.rr_manual_value;    // user value
        case RefreshRate::Display:          return display_refresh_rate.load();// the monitor
    }
}
```

So **what's missing is tiny:**

| Gap | Where | Effect |
|---|---|---|
| Nobody ever sets `rr_option` | we never call `set_graphics_config()` | defaults to `Original` ⇒ RT64 presents at game rate ⇒ **no interpolation** |
| `get_display_framerate()` hard-coded `60` | [rt64_render_context.cpp:267-270](../src/main/rt64_render_context.cpp) | `RefreshRate::Display` mode can't track the real monitor (144/165/etc.); fed into `display_refresh_rate` at [events.cpp:384] |
| `enable_instant_present()` is a no-op | [rt64_render_context.cpp:262-265] | runtime calls it once the game starts ([events.cpp:357-359]); without it the game thread can block on RDP completion, coupling render time into frame production (latency/stutter, not speed) |

RT64 itself already supports all of this: `RefreshRate {Original, Display, Manual}` +
`refreshRateTarget` (clamped to the swap-chain rate), triple buffering, and the present-early
/ `SkipBuffering` enhancement mode.

## 2.4 BAR-specific reality (important caveats)

1. **Races are CPU-bound** (from prior profiling, see the project memory): the single game
   thread runs the sim (physics + AI for 7 cars) *and* builds the display list each frame; in a
   busy race that exceeds the 16.6 ms budget, so **real** frame production already drops to
   ~52→35→22 fps. Interpolation will smooth *presentation* on top of that, but it interpolates
   *between* real frames — it cannot make the sim produce more real frames, and from a low real
   base you get more "soap-opera"/floaty motion and added latency. So the **throughput work is
   complementary** (and arguably should come first for races): enable LTO/LTCG (+ optional
   PGO), strip the debug `fprintf`/`fflush` logging, restore `SetThreadPriority` /
   `timeBeginPeriod(1)`. The build currently has `/O2` but **no `/GL` `/LTCG`**.

2. **No transform annotation ⇒ best-effort interpolation with artifacts.** Zelda64Recomp got
   clean high-fps by tagging thousands of objects/frame with RT64's *extended GBI*
   (`gEXMatrixGroup*` in `rt64_extended_gbi.h`) so RT64 knows which draw calls correspond and
   how to interpolate them — and by *skipping* interpolation on particles, camera cuts,
   motion-blur trails, and 2D HUD elements. BAR is **recomp-only** (no such patches), so RT64
   falls back to automatic `G_EX_ID_AUTO` matching. Expect: the 3D track/cars interpolate
   reasonably; the **HUD (speed, lap, position), dust/smoke/spark particles, and replay/camera
   cuts may smear or jitter**. Artifact-free quality = a substantial, ongoing per-game
   annotation effort in `bar-decomp` (the largest possible scope here).

3. **Interpolation needs one `gDPFullSync` per frame** to interpolate correctly (an RT64
   constraint); worth checking BAR conforms.

## 2.5 Implementation steps (high-fps)

**Phase 1 — turn it on (small).**
1. Build a `GraphicsConfig` at startup with `rr_option = RefreshRate::Display` (match the
   monitor) *or* `Manual` + `rr_manual_value = 144`, and call `set_graphics_config()`. (Pairs
   naturally with the Tier-1 menu in Part 1 as an FPS dropdown/slider.)
2. Implement `get_display_framerate()` to return RT64's real swap-chain refresh rate instead
   of `60` (RT64 exposes the window/swap-chain refresh rate; this feeds `display_refresh_rate`
   for `Display` mode).
3. Implement `enable_instant_present()` via RT64's `enhancementConfig` presentation mode
   (present-early / `SkipBuffering`) so the game thread stops blocking on the renderer.
4. Verify: in a light scene (menu) you should see RT64 emit interpolated frames up to the
   target, with the game clock unchanged (timers/audio identical). Confirm with the
   screenshot/log loop in the debug memory.

**Phase 2 — make the real rate solid (throughput; complementary).**
5. Enable `/GL` + `/LTCG` (and consider PGO on `RecompiledFuncs`); strip the bring-up logging;
   re-enable thread priority + `timeBeginPeriod(1)`. This raises the *real* fps that feeds the
   interpolator, which matters most in races.

**Phase 3 — artifact polish (large, optional, per-game).**
6. Introduce extended-GBI transform tagging in `bar-decomp` for the car/track/camera, and
   interpolation-skip flags for the HUD, particles, and replay cuts. This is the
   Zelda64Recomp-scale effort and should only follow a stable, playable build.

## 2.6 Pitfalls that DO change game speed — avoid these

- **Never** raise the VI rate or make `speed_multiplier > 1` to "go faster" — that scales
  logic *and* audio (the game literally runs fast). Scale only the *display* target.
- **Don't** advance the simulation twice to fake a frame (that's mechanism B done wrong =
  double speed). Synthesize *display* frames by interpolation only.
- **Don't** tie audio (the AI message cadence) to the present rate — keep it on the native VI
  clock or pitch/sync drifts.
- **External** frame-rate limiters / "fast-forward" overlays (RTSS, driver caps) can fight
  RT64's pacing — set the target in-app instead.

---

# Part 3 — Recommended sequencing

These two features dovetail: the menu is where the FPS option lives.

1. **High-FPS Phase 1** (enable interpolation + fix `get_display_framerate` +
   `enable_instant_present`). Smallest change, biggest visible win, zero game-speed risk.
2. **Menu Tier 1** (config persistence in a per-user dir + ImGui settings/ROM-picker),
   surfacing the FPS option from step 1 as a user setting.
3. **High-FPS Phase 2** (LTO/LTCG/PGO + strip logging) — needed for races to feed
   interpolation from a solid real-fps base; overlaps the existing perf roadmap.
4. **Menu Tier 2** (RmlUi launcher + mod manager via `RecompFrontend`) once the game is stable.
5. **High-FPS Phase 3** (extended-GBI annotation in `bar-decomp`) — last, largest, optional.

> Note on current state (2026-06-30): the game now boots into races at 60 fps with working **audio**
> (latency + transition crackle fixed) and **keyboard input**; the Coventry Cove race crash is fixed
> (heap cap). Remaining work is polish — see the **Current WIP** items in [STATUS.md](STATUS.md)
> (legal-screen skip, menu-transition flash, track-map fps). Steps 1-2 here are safe to do now; steps
> 3-5 are larger and assume a stabler base.

---

## Appendix A — File / symbol reference

**Config & lifecycle**
- `GraphicsConfig` + enums (incl. `RefreshRate`, `rr_option`, `rr_manual_value`):
  [config.hpp:48-81](../lib/N64ModernRuntime/ultramodern/include/ultramodern/config.hpp)
- `get_target_framerate` / `display_refresh_rate`:
  [events.cpp:290-308](../lib/N64ModernRuntime/ultramodern/src/events.cpp)
- VI clock / `speed_multiplier`: [events.cpp:189,206]; [timer.cpp:19,148](../lib/N64ModernRuntime/ultramodern/src/timer.cpp)
- instant-present call site: [events.cpp:357-359]; `get_display_framerate` cache: [events.cpp:384]
- game start gate: `recomp::start_game`, `is_game_started`, `wait_for_game_started`
  (`librecomp/src/recomp.cpp`); our deferred start: [main.cpp:330-338](../src/main/main.cpp)

**Renderer (our code)**
- framerate config mapping: [rt64_render_context.cpp:60-67,112-115](../src/main/rt64_render_context.cpp)
- the three gaps: `get_display_framerate` [:267-270], `enable_instant_present` [:262-265],
  `update_config` [:246-260]

**Menu / UI**
- CMake UI gate: [CMakeLists.txt:27,55-66,182-184](../CMakeLists.txt); submodules: `.gitmodules`
- mod system: `librecomp/include/librecomp/mods.hpp` (`scan_mod_folder`, `load_mods`,
  `enable_mod`, `get_all_mod_details`, `get/set_mod_config_value`)

**RT64 internals (for Phase 1/3)**
- config: `lib/rt64/src/common/rt64_user_configuration.h` (`RefreshRate`, `refreshRateTarget`)
- interpolation math: `lib/rt64/src/hle/rt64_rigid_body.cpp`
- frame accumulator / target-rate resolve: `lib/rt64/src/hle/rt64_workload_queue.cpp`
- present pacing / `SkipBuffering`: `lib/rt64/src/hle/rt64_present_queue.cpp`
- extended GBI (annotation): `lib/rt64/include/rt64_extended_gbi.h`

## Appendix B — Sources

- N64ModernRuntime: https://github.com/N64Recomp/N64ModernRuntime
- N64Recomp (incl. `RecompModTool`): https://github.com/N64Recomp/N64Recomp
- RecompFrontend (`recompui` + `recompinput`): https://github.com/N64Recomp/RecompFrontend
- Zelda64Recompiled: https://github.com/Zelda64Recomp/Zelda64Recomp
  (mod loading PR #499, modding-menu PR #535, built-in mods PR #567)
- RT64: https://github.com/rt64/rt64 · RmlUi: https://github.com/mikke89/RmlUi
- Transform tagging / interpolation (DeepWiki):
  https://deepwiki.com/Zelda64Recomp/Zelda64Recomp/5.2-transform-tagging-and-interpolation
- "Snowboard Kids 2 is Recompiled" (interpolation internals):
  https://blog.chrislewis.au/snowboard-kids-2-is-recompiled/

_Method: findings were gathered by three parallel research passes (local code map + two
web-source passes) and then the load-bearing claims (framerate config wiring, the
`speed_multiplier` clock guarantee, the CMake/submodule UI state) were re-verified firsthand
against the files cited above._
