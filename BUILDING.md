# Building beetle-adventure-racing-recomp

Two **separate** toolchains are involved — don't conflate them:

- **Host app** (runtime + RT64 + the recompiled C) → built with **clang-cl** on Windows /
  **clang** on Linux & macOS, via CMake + Ninja.
- **MIPS patches** (optional, later) → cross-compiled with **`clang -target mips` + `ld.lld`**,
  pinned to **LLVM 18.1.8** (LLVM 19.x miscompiles MIPS; Apple Clang can't target MIPS).

Cross-platform by design (RT64: **D3D12** on Windows, **Vulkan** on Linux, **Metal** on macOS).
Windows is the primary target.

> **Status:** the runtime host (`src/main`, `CMakeLists.txt`) builds and runs the game. See
> [docs/STATUS.md](docs/STATUS.md) for the current state and the verified build environment.

## Prerequisites — Windows (no full Visual Studio IDE needed)

JetBrains **CLion** + the **VS Build Tools** is enough.

1. **Build Tools for Visual Studio 2022** (IDE-less) with:
   - Workload **Desktop development with C++** → MSVC v143 + a Windows 10/11 SDK.
   - Component **C++ Clang tools for Windows** → provides `clang-cl`.
   ```
   winget install Microsoft.VisualStudio.2022.BuildTools --override ^
     "--quiet --add Microsoft.VisualStudio.Workload.VCTools ^
      --add Microsoft.VisualStudio.Component.VC.Llvm.Clang --includeRecommended"
   ```
   (Or get clang-cl from a standalone LLVM: `winget install LLVM.LLVM` — any LLVM 17–19.)
2. **CLion** — bundles CMake + Ninja (≥ 3.20 required); it drives the build.

RT64's D3D12 backend needs only the **Windows SDK** + RT64's **bundled DXC**
(`lib/rt64/src/contrib/dxc`) — **no** DirectX Agility SDK. SDL2 is auto-fetched; the runtime DLLs
(`SDL2.dll`, `dxil.dll`, `dxcompiler.dll`) are copied next to the exe automatically.

## Prerequisites — Linux
```bash
sudo apt-get install cmake ninja-build libsdl2-dev libgtk-3-dev libfreetype-dev lld llvm clang
```
Build with **clang** (GCC fails the final link due to recompiled-symbol collisions). RT64 → Vulkan.

## Prerequisites — macOS
Officially supported (native Metal). Needs Homebrew LLVM + CMake/Ninja. Apple Clang builds the
host app but **cannot** build MIPS patches (use Homebrew LLVM 18.x for those).

## One-time setup
```bash
# 1. Nothing to fetch: every library lives in lib/ as ordinary files of this repository.
#    `git clone` is complete on its own -- there are no submodules to initialise.
# 2. Build the recompiler tools + the per-module ELF, then recompile to C:
#    - build N64Recomp/RSPRecomp           (scripts/setup.*)
#    - (WSL) cd ~/projects/bar-decomp && source .venv/bin/activate && make recomp
#    - scripts/fetch-elf.*                 -> elf/recomp.elf
#    - ./N64Recomp beetle-adventure-racing-recomp.toml       -> RecompiledFuncs/*.c
```

## Build — CLion (recommended on Windows)
1. **Settings → Build, Execution, Deployment → Toolchains**: add a **Visual Studio** toolchain
   (so CLion loads the MSVC environment), then set **C and C++ Compiler both to `clang-cl.exe`**.
2. **Settings → CMake**: generator **Ninja**, build type **Release**.
3. Do **not** use CLion's bundled **MinGW**, and avoid the `-T ClangCL` / Visual Studio generator
   route (CLion bug CPP-18848) — the Ninja + clang-cl setup above sidesteps it.
4. Open the project folder → CLion configures CMake → build the **beetle-adventure-racing-recomp** target.

## Build — command line (from an "x64 Native Tools Command Prompt")
```bash
cmake -S . -B build-cmake -G Ninja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release
cmake --build build-cmake -j
```

## Notes
- Re-run `./N64Recomp beetle-adventure-racing-recomp.toml` whenever `recomp.elf` or a `*.toml` changes.
- The **decomp** builds only on a Linux/WSL IDO/MIPS toolchain; only the **recomp** builds natively
  on Windows. Keep this repo LF-normalized (`.gitattributes`) even on Windows.
