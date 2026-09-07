# beetle-adventure-racing-recomp

A work-in-progress **native PC port of _Beetle Adventure Racing!_ (N64, USA)** built by
**static recompilation** with the [N64Recomp][N64Recomp] toolchain — the same approach
behind [Zelda 64: Recompiled](https://github.com/Zelda64Recomp/Zelda64Recomp).

> **Status (2026-09-07): playable.** Boots through the intros and menus into races and renders at
> 60 fps, with keyboard + gamepad input and audio working. Ships the
> **[RecompFrontend](https://github.com/N64Recomp/RecompFrontend)** launcher / settings / input
> binding / pause menu, widescreen with a HUD that expands with the aspect ratio, a selectable
> 1x / 2x / 4x draw distance, internal-resolution scaling (defaults to display-native), MSAA + a VI
> "divot" seam filter, and high-FPS interpolation (phase 1). A handful of polish items remain — see
> **[docs/TODO.md](docs/TODO.md)** for the roadmap and **[docs/STATUS.md](docs/STATUS.md)** to
> resume. The reverse-engineering and porting reference is
> **[docs/technical/](docs/technical/README.md)**.
>
> A prebuilt Windows x64 build is on the
> **[releases page](https://github.com/danielgomesvieira2000/beetle-adventure-racing-recomp/releases)**.

> **No game data is included.** You must supply your own legally-dumped USA ROM
> (SHA-1 `e5ab4d226c08d22f68a2edcc48870203e67454b8`). No ROM, assets, or other copyrighted
> material may ever be committed to this repo.

## Lineage, and how this is built

This repository **continues an existing, in-progress port**: the original recomp work is
[bryankruman/BeetleRecomp](https://github.com/danielgomesvieira2000/BeetleRecomp), which got the game
booting, rendering, taking input and playing audio, along with the in-app launcher, settings and
cheats menus. None of that groundwork is mine. This repo picks that up and carries it forward as a
standalone project.

**The continued work is vibe coded with [Claude Code](https://claude.com/claude-code).** That is a
plain description of the process rather than a disclaimer: the features added here — widescreen, the
HUD that anchors to the widened frame, the extended draw distance and the matching culling fix — were
designed, measured, written, tested and documented in conversation with Claude Code, driven by
in-game testing and by the diagnostics the port carries (`BAR_DBG_*`, headless scripted runs and
screenshots). The commits carry a `Co-Authored-By: Claude` trailer where that is the case.

Two consequences worth stating plainly, because they affect how you should read this repo:

- **The documentation in `docs/` is the real artefact.** Findings, measurements and dead ends are
  written down as they are made — including the negative results, which are the expensive part. If
  you want to know *why* something is the way it is, `docs/` will usually tell you, in more detail
  than is normal for a hobby port. The curated reference distilled out of it is
  **[docs/technical/](docs/technical/README.md)** — see below.
- **It is tested by playing it.** Rendering changes are checked in a running game, not only in
  captured frames; several defects here were only ever visible in motion.

## What this is

Static recompilation translates the N64's MIPS machine code into C automatically, then links
it against a modern runtime (CPU via [librecomp], graphics via [RT64], OS/audio/input via
[ultramodern]). It is **not** a manual rewrite and does **not** require a finished
decompilation.

This port is a sibling to the **[BeetleDecomp](https://github.com/bryankruman/BeetleDecomp)**
decompilation. The decomp is *not* a prerequisite for the recomp, but it is the source of the
**symbol-rich ELF** and the **module/relocation metadata** the recompiler consumes — which is
what makes this port tractable.

## Technical documentation

**[`docs/technical/`](docs/technical/README.md) is a published reverse-engineering and porting
reference for this game.** It exists because the expensive part of a project like this is not the
code — it is knowing what the ROM contains, which of the game's mechanisms produce which pixels, and
which plausible-looking approaches quietly do nothing. All of that was measured here, and leaving it
in commit messages and scratch notes would waste it.

It is written for two audiences:

- **People who want to decompile *Beetle Adventure Racing*** — the ROM and memory layout, Paradigm's
  "UV" engine, the ~133 relocatable modules and how they are loaded, the toolchain (IDO 5.3, splat,
  WSL), and the three-phase matching strategy with the permuter economics behind it.
- **People who want to build their own PC port** — of this game or another N64 title. The static
  recompilation pipeline is documented end to end, including the parts that are in nobody's
  tutorial: overlay registration for a module-based game, codegen fixups and why they must verify
  themselves, hardware-register stubs that are not really stubs, cooperative scheduling and audio
  starvation, joybus-level Controller Pak emulation, and the renderer work needed to make a 320×240
  overscan-inset game look right on a modern display.

| Chapter | Covers |
|---|---|
| [01 — The game and the ROM](docs/technical/01-game-and-rom.md) | Cartridge facts, memory map, entry point, save hardware, microcode, the frame clock, the screen-state globals |
| [02 — The UV engine and the module system](docs/technical/02-engine-and-modules.md) | Export tables, the 133 relocatable modules, `ModuleCommInfo`, the loader, the two coordinate systems and two scissor paths |
| [03 — Decompiling BAR](docs/technical/03-decompilation.md) | Toolchain, the module build, the match/scribe/name strategy, permuter economics, exactly what the recomp takes from the decomp |
| [04 — Static recompilation](docs/technical/04-static-recompilation.md) | N64Recomp config, section/overlay tables, **the overlay bridge**, hardware stubs, the RSP audio ucode |
| [05 — The runtime host](docs/technical/05-runtime-host.md) | librecomp/ultramodern/RT64 wiring, cooperative preemption, audio, the low-level SI/PIF input path, Controller Pak |
| [06 — Graphics](docs/technical/06-graphics.md) | The gfx manager, the overscan inset, the frustum BAR culls against, widescreen, HUD anchoring |
| [07 — Codegen fixups and MIPS patches](docs/technical/07-codegen-fixups-and-patches.md) | Every `fix-recompiled.sh` rule and why it exists; the `RECOMP_PATCH` pipeline and the linker trap it hides |
| [08 — Diagnostics and methodology](docs/technical/08-diagnostics-and-methodology.md) | The `BAR_*` diagnostic surface, headless scripted runs, screenshot capture, how measurements are taken |
| [09 — Porting playbook](docs/technical/09-porting-playbook.md) | Generalised: the order to do this in, the traps ranked by cost, what to build before you need it |

Three conventions run through all of it, and they are the point rather than a style choice: every
number names the switch that produced it, **negative results are kept** (a refuted hypothesis that
gets deleted is one someone pays for again), and inference from unmatched assembly is labelled as
inference rather than stated as fact.

The reference is a **living document**: it is updated in the same change as the code it describes,
and [`docs/technical/README.md`](docs/technical/README.md) carries the mapping from each source area
to the chapter that owns it. The chronological working notes it distils — current state, roadmap and
live bug reports — remain in `docs/` alongside it.

## Relationship to the decomp

This repo vendors the **[BeetleDecomp](https://github.com/bryankruman/BeetleDecomp)** project as a
submodule at `lib/bar-decomp`. Two things are worth knowing up front:

**The recomp does *not* compile the decomp's C.** Static recompilation translates the original
ROM's MIPS *machine code* into C automatically; the decomp's hand-written C is never built into the
port. What the port takes from the decomp is:

| From the decomp | What for |
|---|---|
| The **symbol-rich ELF** that rebuilds the ROM byte-for-byte | Input to N64Recomp (built in WSL, never committed) |
| **Headers + symbol tables** | Readable names + typed interfaces for generated code and patches |
| **Segmentation / module + reloc layout** | Overlay handling for BAR's ~130 relocatable modules |

Because the decomp is *byte-matching*, its matching progress doesn't change the port — recompiling a
function yields identical output whether it started as hand-written C or raw asm. What flows
downstream from decomp work is better **names and types** (readability of generated code and patches)
and the **module layout** that makes the port tractable.

**Changes never go in the decomp** — keep it a faithful mirror of the original. Modifications live in
two recomp-owned layers:

- **`patches/`** — C cross-compiled to MIPS that **overrides** (`RECOMP_PATCH`) or **hooks**
  (`RECOMP_HOOK`) game functions by name. This is the game-logic layer: bug fixes, widescreen, new
  features. Patches link *before* the recompiled output, so they win.
- **`src/`** — the native C++ host: rendering (RT64), input, audio, saves, config, and the in-app UI.

See [BUILDING.md](BUILDING.md) for how the ELF is produced and how to pull decomp updates.

## Technical profile

The properties that made BAR a good recomp candidate, and which shape the runtime:

| Aspect | Detail |
|---|---|
| **Graphics microcode** | Stock **`gspF3DEX2_fifo`** (decomp `gfx_ucode: f3dex2`) — *not* a custom Paradigm ucode. RT64 supports F3DEX2 directly. |
| **Audio** | Standard libultra (`alAudioFrame`/`alSeqFileNew`/ALSndPlayer) wrapped in Paradigm's "UV" middleware. |
| **Saves** | Controller Pak only (`osPfs*`); no EEPROM/SRAM/Flash. The port emulates a Controller Pak. |
| **Memory** | 4 MB; **no Expansion Pak**, no TLB-mapped code (flat KSEG0). |
| **Entry point** | `0x80000400` (ROM header offset 0x8 == splat `entry` segment vram). |
| **Module layout** | The **~130 relocatable code modules** (`ai`, `battle`, `race`, …, `uv*_rom`) were the crux of the port; the uv module overlay bridge (`src/main/overlay_bridge.cpp`) registers them at load. |

The toolchain license analysis and research sources are in
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

## How it fits together

```
your ROM ─┐
          ├─► BeetleDecomp (WSL) ──► symbol-rich ELF ──► N64Recomp ──► RecompiledFuncs/*.c ─┐
recomp.ld ┘   (per-module layout)                       (beetle-adventure-racing-recomp.toml)                 ├─► CMake ─► beetle-adventure-racing-recomp(.exe)
                                                                                            │
 patches/*.c ─► clang -target mips ─► patches.elf ─► N64Recomp (patches.toml) ─► RecompiledPatches/*.c ┘
                                                            │
            runtime: librecomp + ultramodern + RT64 ───────┘
```

## Repository layout

```
beetle-adventure-racing-recomp/
├── beetle-adventure-racing-recomp.toml      # main N64Recomp config (entrypoint 0x80000400, ELF, overlays)
├── patches.toml           # config for the C patches (MIPS override/hook layer)
├── overlays.us.txt        # relocatable module/overlay section list
├── CMakeLists.txt         # host-app build (clang-cl + Ninja)
├── COPYING                # AGPL-3.0 (inherited from the decomp)
├── THIRD_PARTY_NOTICES.md # dependency licenses + research sources
├── BUILDING.md            # full build instructions (Windows + Linux/macOS)
├── docs/                  # STATUS.md (resume guide), TODO.md (roadmap), investigation notes
│   └── technical/         # the published RE + porting reference (start at its README.md)
├── src/
│   ├── main/              # native host: RT64 render context, input, audio, overlay bridge, config
│   ├── game/              # game-facing config schema (graphics.json)
│   └── frontend/          # RecompFrontend glue: launcher menus, config tabs, input forwarding
├── patches/               # C compiled to MIPS that overrides/hooks game functions
├── include/               # port headers
├── rsp/                   # RSP microcode glue
├── scripts/               # setup / fetch-elf / gen-overlays / fix-recompiled helpers
├── assets/  icons/        # bundled app assets
├── elf/  syms/            # decomp ELF + patch symbol TOMLs land here (git-ignored)
└── lib/                   # git submodules:
    ├── bar-decomp                 # BeetleDecomp — symbols, headers, module/reloc layout (source ref)
    ├── N64Recomp                  # the static recompiler (MIPS → C)
    ├── N64ModernRuntime           # librecomp (CPU) + ultramodern (OS/audio/input)   [bryankruman fork]
    ├── rt64                       # RT64 renderer (D3D12 / Vulkan / Metal)            [bryankruman fork]
    └── RecompFrontend             # launcher / settings / input UI (recompui + recompinput)
                                   #   -> pulls RmlUi, lunasvg and freetype as its own submodules
```

## Quickstart

> Full details and prerequisites: **[BUILDING.md](BUILDING.md)**.

```bash
# 1. Fetch dependencies and build the recompiler (N64Recomp + RSPRecomp).
scripts/setup.sh                  # or scripts/setup.ps1 on Windows PowerShell

# 2. Build the decomp ELF (in WSL), then copy it in.
#    (WSL) cd ~/projects/bar-decomp && source .venv/bin/activate && make recomp
scripts/fetch-elf.sh              # or scripts/fetch-elf.ps1

# 3. Recompile the game to C.
./N64Recomp beetle-adventure-racing-recomp.toml     # emits RecompiledFuncs/*.c
scripts/fix-recompiled.sh         # REQUIRED codegen fixup — re-run after every regeneration

# 4. Configure + build the port (clang-cl on Windows, clang on Linux/macOS).
cmake -S . -B build-cmake -G Ninja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release
cmake --build build-cmake -j
```

## Status & roadmap

The bring-up milestones — the per-module recomp ELF, overlay wiring, first boot, F3DEX2 rendering,
input, audio, and Controller Pak saves — are **done**, and the game is playable. Current work is
polish and features:

- **Rendering** — internal-resolution scaling (defaults to display-native), MSAA + a VI "divot"
  seam filter, and high-FPS interpolation (phase 1 live; throughput/artifact phases pending).
- **Known issues** — skippable legal/logo screens, a one-frame menu-transition flash, choppy
  track-map preview, audio latency-vs-clip tuning.
- **Features** — controller rebind UI, mod-manager UI, an app icon, and more settings surfaced in
  the in-app menu.

The live checklist is **[docs/TODO.md](docs/TODO.md)**; per-item status and resume detail is in
**[docs/STATUS.md](docs/STATUS.md)**.

## License

**AGPL-3.0**, inherited from the BeetleDecomp data this port derives from. See
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for the dependency-license analysis and why
AGPL is the correct umbrella. Vendored dependencies in `lib/` retain their own licenses.
**Distribute no game assets.**

## Credits

Nothing here starts from scratch. A static-recompilation port is mostly other people's work, and the
short version is: the recompiler, the runtime, the renderer, the decompilation and the original
reverse engineering were all done by other people, and this repo is a thin layer on top of them.

### This port

- **Bryan Kruman** ([@bryankruman](https://github.com/bryankruman)) — the original BeetleRecomp: the
  static-recompilation port itself, the overlay/module bridge that makes BAR's ~130 relocatable
  modules work, the runtime fixes, the launcher and in-app menus, and the decomp fork the port
  consumes. The great majority of the foundation this repo builds on.
- **Daniel Gomes Vieira** ([@danielgomesvieira2000](https://github.com/danielgomesvieira2000)) —
  continued work: Controller Pak saves, widescreen, HUD placement, draw distance and culling, and
  the testing that found the defects in them.

### The decompilation and reverse engineering

- **SynaMax** ([@synamaxmusic](https://github.com/synamaxmusic)) — the original
  [bar-decomp](https://github.com/synamaxmusic/bar-decomp) and
  [bar-notes](https://github.com/synamaxmusic/bar-notes): the splat configuration, the module/overlay
  build system, the symbol map and the IDO setup that everything downstream depends on.
- **LLONSIT** — the module-system work and build-system documentation in that project.
- **[BeetleDecomp](https://github.com/bryankruman/BeetleDecomp)** — the fork this port consumes for
  its symbol-rich ELF and module/relocation metadata.

### The recompilation toolchain and runtime

- **Wiseguy** ([@Mr-Wiseguy](https://github.com/Mr-Wiseguy)) and the [N64Recomp] contributors —
  [N64Recomp] (the static recompiler and RSPRecomp), [N64ModernRuntime] ([librecomp] and
  [ultramodern]) and [Zelda 64: Recompiled](https://github.com/Zelda64Recomp/Zelda64Recomp), which is
  the project that showed this approach works at all.
- **[RecompFrontend](https://github.com/N64Recomp/RecompFrontend)** — the launcher, settings, input
  and mod-menu frontend this port uses as-is.
- **[RT64](https://github.com/rt64/rt64)** contributors — the N64 renderer (D3D12 / Vulkan / Metal)
  that does the actual drawing, and whose extended-GBI aspect handling the widescreen HUD is built on.

### Libraries

- **[RmlUi](https://github.com/mikke89/RmlUi)** (mikke89) — the UI toolkit behind the in-app menus.
- **[lunasvg](https://github.com/sammycage/lunasvg)** (sammycage) — SVG rasterizer for the menu icons.
- **[SDL](https://www.libsdl.org/)** — windowing, input and platform layer.
- **[miniaudio](https://github.com/mackron/miniaudio)** (mackron) — audio backend.
- The libraries RT64 vendors in turn — Plume, plus Dear ImGui, hlslpp, spirv-cross, xxHash, zstd, stb
  and others; see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

### The wider N64 decompilation community

The shared toolchain that makes any of this possible: **splat**, **ido-static-recomp**,
**asm-differ**, **objdiff**, **m2c** and **decomp-permuter**.

### AI tooling

- **[Claude Code](https://claude.com/claude-code)** (Anthropic) — the continued work in this repo was
  written with it, as described in [Lineage, and how this is built](#lineage-and-how-this-is-built).

*If you contributed something that is credited wrongly or not at all, please open an issue — it is an
oversight, not a claim.*

[N64Recomp]: https://github.com/N64Recomp/N64Recomp
[N64ModernRuntime]: https://github.com/N64Recomp/N64ModernRuntime
[librecomp]: https://github.com/N64Recomp/N64ModernRuntime/tree/main/librecomp
[ultramodern]: https://github.com/N64Recomp/N64ModernRuntime/tree/main/ultramodern
[RT64]: https://github.com/rt64/rt64
