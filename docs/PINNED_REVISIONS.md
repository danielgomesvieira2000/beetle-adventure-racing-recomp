# Pinned Revisions

Reproducibility record for this fork. Every dependency below is pinned so a future
checkout can rebuild the exact binary this tree produces. Update this file whenever a
submodule pointer or a toolchain version changes, in the same commit as the change.

_Baseline established: 2026-09-04 · Fork of [bryankruman/beetle-adventure-racing-recomp](https://github.com/bryankruman/beetle-adventure-racing-recomp)_

## Game ROM

Not committed, and never to be committed. Supply your own legally dumped USA cartridge.

| Property | Value |
|---|---|
| Title | `Beetle Adventure Rac` |
| Cartridge code | `NNSE` (USA) |
| SHA-1 | `e5ab4d226c08d22f68a2edcc48870203e67454b8` |
| MD5 | `cf97c336479ddbf1217e4dde89d9d2d3` |
| CRC1 / CRC2 | `EDF419A8` / `BF1904CC` |
| Entry point | `0x80000400` |
| Size / format | 16 MB, big-endian `.z64` |

## Submodules

Recorded here as history rather than as pointers: since the libraries are committed into this
repository as ordinary files, there is no submodule index to check and no revision to drift. The
revisions below are what each `lib/` tree was taken from, kept so that an upstream change can still be
diffed against a known starting point. (Verify with `git submodule status` — any `+` prefix
means the working tree has drifted from the pin.

| Path | Commit | Describes as |
|---|---|---|
| `lib/N64ModernRuntime` | `0a0c405e654af41331042a89d1290e9d51baafd7` | `heads/main` |
| `lib/N64Recomp` | `a940efa0bbc5566e52c868dea71830b6b888379e` | `mod-tool-release-23-ga940efa` |
| `lib/RmlUi` | `2cd28864ae25ed345b70598751703a5433b12356` | `6.1-163-g2cd28864` |
| `lib/bar-decomp` | `812bb7c450ddbf051308fe39d90c11dcfdd1569a` | `origin/recomp-ld-35-g812bb7c` |
| `lib/freetype-windows-binaries` | `2fd97db170b19a9dda26131a784707611b9a4da1` | `v2.14.3` |
| `lib/lunasvg` | `2872affa1027cad92a05408a7e6f2547efa7f364` | `v3.5.0-23-g2872aff` |
| `lib/rt64` | `d01e8d2d65f5c50dc76dea05afc66d8beacc8339` | `heads/main` |

### Known discrepancy — N64Recomp

`docs/STATUS.md` (inherited from upstream) states the verified recompiler checkout is
**`ffb39cd`** and explicitly warns against `81213c1`. The submodule here is pinned one
release later, at **`a940efa`** (`mod-tool-release-23` vs `ffb39cd`'s `release-22`).

We build the pinned `a940efa` because that is what the tree ships. If recompiled output
misbehaves in a way that smells like codegen rather than game logic, checking out
`ffb39cd` in `lib/N64Recomp` and regenerating is the first thing to try. Record the
outcome here either way, so this stops being an open question.

## Host toolchain (Windows — primary target)

Verified working for the recompiler build on 2026-09-04.

| Component | Version | Location |
|---|---|---|
| LLVM / clang-cl | 22.1.8 | `C:\Program Files\LLVM\bin` |
| VS Build Tools | 2022 | `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools` |
| CMake | 4.4.3 | `C:\Program Files\CMake\bin` |
| Ninja | winget package | `%LOCALAPPDATA%\Microsoft\WinGet\Packages` |
| Python | 3.12.10 | `%LOCALAPPDATA%\Programs\Python\Python312` |
| Git | 2.55.0.windows.5 | Git for Windows |

### CMake 4 note

CMake 4.x removes compatibility with `cmake_minimum_required(VERSION <3.5)`. This tree
contains six such files, all in vendored **examples, tests and `find_package` config
helpers** — imgui examples, implot's `.github`, SPIRV-Headers tests and buildHeaders, and
the SDL2 config modules under RT64's `mupen64plus-win32-deps`. None are in the build path,
and configuring the recompiler under CMake 4.4.3 succeeded without incident.

If a future dependency bump pulls a pre-3.5 `CMakeLists.txt` into the build, the escape
hatch is `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` rather than downgrading CMake.

## Decomp toolchain (WSL)

The decompilation builds only under a Linux IDO/MIPS toolchain; only the recomp builds
natively on Windows.

| Component | Value |
|---|---|
| Distro | Ubuntu 26.04 LTS (WSL2), distro name `Ubuntu` |
| Decomp checkout | `~/projects/bar-decomp` |
| Base ROM | `baserom.us.z64` in the decomp root |
| ELF target | `make recomp` → `build/recomp.elf` |

Upstream recommends Ubuntu 24.04 and warns that distros older than 22.04 (binutils 2.34)
miscompile the ELF→ROM step on the TLS `.tdata` section. We are on 26.04, which is
*newer* than the tested configuration rather than older — the documented hazard does not
apply, but if the final ROM fails its checksum with a plausible-looking build, a newer
binutils is a reasonable suspect.

`scripts/fetch-elf.sh` defaults to the original author's environment
(`Ubuntu-24.04`, `/home/brysl/projects/bar-decomp`). Override for this machine:

```bash
WSL_DISTRO=Ubuntu DECOMP_DIR=/home/daniel/projects/bar-decomp scripts/fetch-elf.sh
```

## Build order

Each step depends on the one before it.

1. `git clone` (there are no submodules; every library is committed under `lib/`)
2. Build the recompiler → `N64Recomp.exe`, `RSPRecomp.exe` at the repo root
3. (WSL) `cd ~/projects/bar-decomp && source .venv/bin/activate && make extract && make -j6`
   — must print `build/beetleadventurerac.us.z64: OK`
4. (WSL) `make recomp` → `build/recomp.elf`
5. `scripts/fetch-elf.sh` → `elf/recomp.elf`
6. `./N64Recomp beetle-adventure-racing-recomp.toml` → `RecompiledFuncs/*.c`
7. `scripts/fix-recompiled.sh` — **mandatory**, and re-run after every regeneration
8. `cmake -S . -B build-cmake -G Ninja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release`
9. `cmake --build build-cmake -j`

Step 7 is not optional and is not idempotent-by-accident: it encodes seven behavioural
fixes (heap cap, controller input, intro skip, MIDI stop-wait, cooperative preemption,
SP_STATUS guard) keyed to N64 instruction addresses in generated C. Retiring those into
named patches is Phase 1 of the project plan.

## RecompFrontend integration notes

`lib/RecompFrontend` (recompui + recompinput) is the reusable launcher/menu and input layer
extracted from Zelda 64: Recompiled. It consumes N64ModernRuntime and RT64 **by path** rather than
by target (`RECOMP_FRONTEND_N64MODERNRUNTIME_PATH`, `RECOMP_FRONTEND_RT64_PATH`), so it reuses the
runtime and renderer this port already builds instead of duplicating them.

Configure with:

```bash
cmake -S . -B build-frontend -G Ninja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl \
      -DCMAKE_BUILD_TYPE=Release -DBEETLE_ENABLE_UI=OFF -DBEETLE_ENABLE_FRONTEND=ON
```

**`recompinput::players::auto_assign_controllers` is part of the vendored source.** It used to be
applied by `scripts/patch-recompinput.py` after every `git submodule update`, which reverted it
silently; now that RecompFrontend is committed here rather than pointed at, the change simply stays.
The script is kept as the record of what was changed and why — it is idempotent and has nothing left
to do. See [05 — player assignment](technical/05-runtime-host.md#input-goes-through-recompinput) for
why the port needs it.

Four integration constraints, each of which broke the build once:

1. **`BEETLE_ENABLE_UI` and `BEETLE_ENABLE_FRONTEND` are mutually exclusive.** RecompFrontend
   vendors its own RmlUi and lunasvg under `recompui/lib`; adding ours as well defines the same
   targets twice. CMake now fails fast with an explanatory error. This is by design — RecompFrontend
   *replaces* `src/ui`.
2. **Do not pre-define `Freetype::Freetype`.** RecompFrontend ships its own
   `freetype-windows-binaries` submodule and a `FindFreetype.cmake` that creates the imported
   target itself, and CMake refuses to create one twice. Our Windows Freetype shim therefore runs
   only for the legacy `BEETLE_ENABLE_UI` path.
3. **SDL2 must be acquired before `add_subdirectory(lib/RecompFrontend)`.** Both frontend libraries
   `#include "SDL.h"` and read `sdl2_SOURCE_DIR` / `SDL2_INCLUDE_DIRS` at configure time; their own
   CMake flags this as a standalone-build limitation. Our SDL2 FetchContent originally sat beside
   the `beetle-adventure-racing-recomp` target, far below, so the frontend configured with an empty include path
   (visible as a bare `-I\include` on the command line) and failed to find `SDL.h`.
4. **Configure with `clang-cl`, not `clang++`.** Both are LLVM and both target the MSVC ABI, so
   CMake reports `CMAKE_CXX_SIMULATE_ID` as `MSVC` for either — which is the condition this
   project's root `CMakeLists.txt` (and RecompFrontend's own) use before appending `/W4`. Only
   `clang-cl` accepts an MSVC-style flag; `clang++` rejects it with
   `error: no such file or directory: '/W4'`, and it does so in lunasvg, RT64 and re-spirv rather
   than anywhere that names the real problem. A build directory configured the wrong way can appear
   to work for a long time, because the flag is only re-applied when a translation unit is
   recompiled; it fails the first time anything triggers a rebuild.

### ROM verification

The registered `recomp::GameEntry` for `bar.n64.us` carries
`rom_hash = 0x56cfec69d7951f9f`, which is the **XXH3-64 of the USA z64** and was verified against
the real ROM — despite the stale `TODO` beside it. `recompui`'s
`add_start_game_or_load_rom_option()` drives its `rom_valid` state from librecomp's validation, so
the launcher's ROM loading and verification are backed by that hash directly.
