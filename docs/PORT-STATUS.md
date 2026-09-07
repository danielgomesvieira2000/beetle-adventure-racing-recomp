# Port status

Current state of **this fork**, as of 2026-09-04. `docs/STATUS.md` is the *inherited* status from
`bryankruban/beetle-adventure-racing-recomp` and describes the tree before this fork diverged — it is still useful for
the BAR-specific findings it records, but it is not a description of this repository any more.

## What works today

The full chain runs, from a cartridge dump to a playable race driven from a menu:

| | |
|---|---|
| **Decomp → ELF** | `bar-decomp` rebuilds the ROM byte-for-byte and emits `build/recomp.elf` (133 module sections, 135 `.rel.*`, 665 `__recomp_*` symbols) |
| **Recompilation** | N64Recomp produces **22,246 functions**; `fix-recompiled.sh` applies six rules and verifies every one, failing the build if an anchor rots |
| **Boot** | Logos, attract cinematics, BAR's own main menu and Options, and races — all rendering correctly at 60 fps |
| **Frontend** | RecompFrontend launcher: Start Game / Controls / Settings / Exit, navigable by keyboard **and** mouse |
| **ROM handling** | Verified by the library against the registered XXH3-64 hash; "Load ROM" with a native picker until a valid ROM exists, "Start Game" after |
| **Saves** | Controller Pak emulated end to end — a real note (`NNSE`, "BEETLE RACING") is written to a 32 KiB `mempak_p0.pak` |
| **Rumble** | Motor register served alongside the Controller Pak, so one port presents both accessories with no swapping |

Build: `-DBEETLE_ENABLE_FRONTEND=ON` (the default). See `docs/PINNED_REVISIONS.md` for the exact
revisions, toolchain versions and build order.

## How this fork differs from its parent

**The bespoke UI is gone.** `src/ui/` and the top-level `lib/RmlUi`, `lib/lunasvg` and
`lib/freetype-windows-binaries` submodules were deleted along with the `BEETLE_ENABLE_UI` option —
about 5,400 lines. The submodule set now matches the reference port
(`danielgomesvieira2000/wave-race-64-recomp`): N64ModernRuntime, RT64 and RecompFrontend, plus
`bar-decomp` and `N64Recomp` which this project needs for the ELF and the recompiler.

RmlUi still exists, but only as RecompFrontend's own vendored dependency nested inside
`lib/RecompFrontend`. `recompui` is built on RmlUi, so that is not removable while using the recomp
frontend, and wave-race-64-recomp carries it the same way.

**UI assets** come from that reference port: a deliberately tiny `recomp.rcss`, its icon set, and
PromptFont. See `assets/ui/NOTICE.md` for provenance and licensing.

**`lib/N64ModernRuntime` points at a personal fork** (`danielgomesvieira2000/N64ModernRuntime`,
branch `controller-pak`) carrying two changes upstream does not have: `Pak::ControllerPak`, and the
pre-game dummy-framebuffer alternation described below.

## Bugs fixed here, and why they were hard

Each of these cost real time and would be easy to reintroduce.

**Controller Pak saves needed two independent fixes.** BAR gates its whole pak path on
`OSContStatus.status`, so with no pak reported it never issues an SI query at all; and the SI stub
understood only one of the two PIF command layouts, silently skipping BAR's short-format
(`txsize, rxsize, cmd`) pak status query and leaving the `0xFF` placeholders untouched. Neither fix
works without the other.

**The launcher rendered once and then ignored all input.** Events reached recompinput and were
queued, but recompui's UI loop — which lives in RT64's `draw_hook` — never ran, because the renderer
had stopped presenting. Pre-game presentation is driven by ultramodern alternating a dummy VI
framebuffer address each frame; an earlier merge resolution kept BAR's `0x100000` base but dropped
the alternation, and a constant address looks like the same frame forever. Especially so in Console
presentation mode, which this port pins so the main-menu film-roll pan animates.

**recompinput must be the sole SDL poller.** A second `SDL_PollEvent` loop in the host races it and
each swallows events the other needs. It is pumped from `update_gfx` rather than `input_poll`,
because `input_poll` is not driven before a game starts and the launcher lives there — pumping only
from `input_poll` leaves the window unresponsive to Windows itself.

**Mouse coordinates were offset by exactly the display scale.** `SDL_HINT_WINDOWS_DPI_SCALING=1`
reports geometry and mouse positions in logical units while the drawable stays in physical pixels;
at 125% that is a 1.25× disagreement between where a control draws and where a click lands. DPI
*awareness* is set, DPI *scaling* is explicitly off.

**Font family names are matched from inside the file.** `LatoLatin-Regular.ttf` declares itself
`LatoLatin`, not `Lato`. Registering the wrong name fails silently: every element lays out and draws
in the right place with no text in any of them.

## Known issues

- **Widescreen is not finished.** `ar_option` is `Expand`, so RT64 widens the 3D frustum, but this
  has not been assessed in motion: whether the view is genuinely wider, whether geometry pops at the
  new left/right edges (the culling risk), and whether the HUD is right. `hr_option` was `Expand`,
  which visibly stretched the 2D layer, and is now `Original`.
- **The intro runs too fast.** Fully documented with eight refuted hypotheses in
  `docs/KNOWN_ISSUES.md`; do not re-run those experiments.
- **Controller hotplug is not seen by the game.** With recompinput owning the SDL queue, the host
  never receives `SDL_CONTROLLERDEVICEADDED`, so `bar::input` misses pads plugged in after launch.
  Pads present at startup work. The reference port solves this with a periodic re-scan
  (`refresh_primary_controller`).
- **`rr_option` defaults to `Display`**, i.e. frame interpolation on, with none of the per-object
  matrix annotation done. It is reachable from Settings → Graphics → Framerate.
- **`fix-recompiled.sh` carries six address-keyed rules** against generated C (down from seven).
  They no longer fail silently: every rule now verifies its end state and the script exits non-zero
  naming the anchor that rotted, so a missing behavioural fix stops the build instead of shipping a
  game that misbehaves in unrelated-looking ways. `FIXRECOMP_STRICT=0` downgrades that to a warning,
  for deliberate regeneration only.

  Migrating the remainder to named `RECOMP_PATCH`/`RECOMP_HOOK` functions — which bind to symbol
  names rather than instruction addresses, and so cannot rot at all — is now **unblocked**; see
  "Building MIPS patches" below. Rule F (the per-prologue cooperative-preempt poll, 3,147 injection
  sites) cannot become a patch in any case, since it applies to every recompiled function body
  rather than to a named function.

## Building MIPS patches

Patches are C cross-compiled to MIPS that override or hook game functions **by name**, which is what
makes them immune to the address rot that `fix-recompiled.sh` rules suffer from. The pipeline is
wired up and verified end to end.

The Windows LLVM used for the host build (22.1.8) has **no MIPS backend** — `llc --version` lists no
MIPS target and `-target mips` fails with "No available targets are compatible with triple mips".
That is not a flags problem. Ubuntu's packaged clang carries every target, so patches are built in
WSL exactly as the decomp is, using `clang-18` because `BUILDING.md` pins LLVM 18.1.8 (19.x is
documented as miscompiling MIPS) and Ubuntu ships precisely 18.1.8.

One-time setup, inside WSL:

```bash
sudo apt install -y clang-18 lld-18
```

Then, from the repo root:

```bash
scripts/build-patches.sh                 # patches/*.c -> patches/patches.elf (runs in WSL)
wsl -d Ubuntu -- python3 tools/gen_reference_syms.py elf/recomp.elf     syms/beetleadventurerac.us.syms.toml syms/beetleadventurerac.us.datasyms.toml
./N64Recomp patches.toml                 # patches.elf -> RecompiledPatches/patches.c
cmake -S . -B build-frontend             # re-run once after patches.c first appears (it is globbed)
cmake --build build-frontend -j
```

**How a patch actually replaces the original — and the way it silently failed to.** N64Recomp does not
remove a patched function from `RecompiledFuncs`; the patch simply carries the same C name and the
*linker* has to prefer it. Under `clang-cl`, `RECOMP_FUNC` expands to a *weak* definition for the
originals and the patches alike, and with two weak definitions the linker keeps whichever it meets
first. The first attempt built patches into a static `PatchesLib`: a static library only contributes
members for symbols that are still undefined when it is scanned, and by then the `RecompiledFuncs`
object holding the original had already been pulled in for some other function — so the original won
every time. The viewport patch below linked cleanly and changed nothing, which is how this was found.
`RecompiledPatches/patches.c` is therefore compiled **directly into the executable**: an object file's
definition is seen before any archive is opened, so the patch is the first definition. This mirrors
`wave-race-64-recomp`, which compiles its patches as executable sources for the same reason.

**The first real patch: `patches/viewport_patch.c`.** BAR draws racing into a 275×207 rectangle inset
at (22,17) — a CRT overscan-safe margin — which showed up as black bars on a monitor, stopped RT64
from ever widening the racing projection (it only widens a projection that reaches the framebuffer's
edges), and once that was relaxed let the widened 3D draw outside the game's own margin fills. The
patch replaces `uvGfxClipRect` with the original body plus one change: the four known inset edges are
snapped to the screen edges before the maths runs, leaving interior edges (split-screen boundaries,
the intro's letterbox) untouched. It uses **no data symbols** — the module is relocatable at runtime
(its recompiled code reaches `sScreenWidth` through a section-relative relocation), so a patch baking
absolute addresses could touch the wrong memory; screen size comes from the exported
`uvGetScreenWidth()/uvGetScreenHeight()` instead. One gotcha cost a full build cycle: **Y is bottom-up
in this API.** The scissor is emitted as `(x0, height - y1, x1, height - y0)`, so measured scissor rows
17..224 correspond to `y0 = 16, y1 = 223`, not 17/224.

The reference-symbol TOMLs let the recompiler resolve patch relocations against the game. Nothing
generated them before, so `tools/gen_reference_syms.py` derives them from the decomp's
`recomp.elf` (135 executable sections / 3,253 functions, plus data symbols). Two shapes are easy to
confuse: executable sections use `functions = [{ name, vram, size }]` and data sections use
`symbols = [{ name, vram }]`.

Patches are **optional**: CMake compiles `RecompiledPatches/patches.c` into the executable only when
it exists, so a fresh clone still builds before anyone has installed a MIPS toolchain.

## Diagnostics worth knowing

- `BAR_DBG_UI=1` — frontend diagnostics to `bar_ui_trace.log`. This build links
  `/SUBSYSTEM:WINDOWS`, so it has no console and shell redirection of stderr captures nothing;
  without this, frontend faults are completely silent.
- `BAR_DBG_PAK=1` — Controller Pak / joybus traffic. This is what diagnosed the save path.
- `BAR_DBG_STATE=1` — `currentGameState` transitions (menu = 14, race = 2).
- `BAR_SHOTS`, `BAR_AUTOPLAY`, `RT64_SHOT_TRIGGER` — headless capture and scripted input, per
  `docs/HEADLESS_TESTING.md`. Note that doc's boot recipe is calibrated to another machine: logos
  here run to roughly frame 1450, not 510.

**Screenshot caveat:** capture with a DPI-aware process. A non-aware one sees a virtualised
1536×864 view of a 1920×1080 window and produces an image that looks shifted and clipped — an
artefact that was briefly mistaken for a rendering bug.
