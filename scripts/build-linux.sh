#!/usr/bin/env bash
# Build the Linux port. Adapted from Hybrid Heaven: Recompiled's tools/build_linux.sh.
#
#   bash scripts/setup-linux.sh
#   bash scripts/build-linux.sh
#   ./build-linux/beetle-adventure-racing-recomp
#
# The generated C (RecompiledFuncs/, RecompiledPatches/) is plain C, the same for every platform, and is
# git-ignored. Produce it once with the pipeline in BUILDING.md -- the decomp's `make recomp`,
# scripts/fetch-elf.sh, `./N64Recomp beetle-adventure-racing-recomp.toml`, then scripts/fix-recompiled.sh --
# or reuse the tree a Windows build already generated. If elf/recomp.elf is present and RecompiledFuncs is
# not, this script runs the last two steps itself.
#
# Environment:
#   BAR_BUILD_DIR   where to build (default build-linux)
#   BAR_JOBS        parallelism (default: the number of processors)
#   BAR_CC, BAR_CXX the compiler, instead of the newest Clang found
set -euo pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO"

if [ "$(uname -s)" != Linux ]; then
    echo "This script is for Linux. On Windows, see BUILDING.md." >&2
    exit 1
fi

BUILD_DIR="${BAR_BUILD_DIR:-build-linux}"
JOBS="${BAR_JOBS:-$(nproc)}"

for tool in cmake ninja python3; do
    command -v "$tool" > /dev/null || {
        echo "$tool is not on PATH. Run: bash scripts/setup-linux.sh" >&2
        exit 1
    }
done

# Which Clang: an explicit choice, then the plain names, then the highest versioned clang-NN present
# (Debian and Ubuntu often lack the unversioned names).
if [ -n "${BAR_CC:-}" ] && [ -n "${BAR_CXX:-}" ]; then
    CC="$BAR_CC"
    CXX="$BAR_CXX"
elif command -v clang > /dev/null && command -v clang++ > /dev/null; then
    CC=clang
    CXX=clang++
else
    CC=""
    for candidate in $(ls /usr/bin/clang-[0-9]* 2> /dev/null | sort -V -r); do
        version="${candidate##*/clang-}"
        case "$version" in
            *[!0-9]*) continue ;;   # clang-format-21, clang-tidy-21, and friends
        esac
        if [ -x "/usr/bin/clang++-$version" ]; then
            CC="$candidate"
            CXX="/usr/bin/clang++-$version"
            break
        fi
    done
    if [ -z "$CC" ]; then
        echo "No Clang found. Install one -- apt install clang -- or set BAR_CC and BAR_CXX." >&2
        exit 1
    fi
fi
echo "compiler: $CC / $CXX"

# ------------------------------------------------------- generated sources ----
if [ ! -f RecompiledFuncs/funcs.h ]; then
    if [ -f elf/recomp.elf ]; then
        echo "=== recompiler ==="
        [ -x ./N64Recomp ] || bash scripts/setup.sh
        ./N64Recomp beetle-adventure-racing-recomp.toml
        bash scripts/fix-recompiled.sh
    else
        echo "RecompiledFuncs/ is missing, and so is elf/recomp.elf." >&2
        echo "Generate them with the pipeline in BUILDING.md first." >&2
        exit 1
    fi
fi

# The shader compiler RT64 and recompui run at build time. A checkout made on Windows can lose its
# executable bit.
chmod +x lib/rt64/src/contrib/dxc/bin/x64/dxc-linux 2> /dev/null || true

# ------------------------------------------------------------ configure ----
# CMAKE_POLICY_VERSION_MINIMUM is for CMake 4, which refuses the cmake_minimum_required(VERSION <3.5)
# still declared by some vendored libraries.
echo
echo "=== configure ==="
cmake -S . -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DBEETLE_ENABLE_FRONTEND=ON

echo
echo "=== build ==="
cmake --build "$BUILD_DIR" --target beetle-adventure-racing-recomp -j "$JOBS"

echo
echo "Built: $BUILD_DIR/beetle-adventure-racing-recomp"
echo "Settings and saves live in \${XDG_CONFIG_HOME:-~/.config}/beetle-adventure-racing-recomp."
