#!/usr/bin/env bash
# Bootstrap beetle-adventure-racing-recomp: fetch dependencies and build the recompiler tools.
# Run once after cloning; safe to re-run.
set -euo pipefail
cd "$(dirname "$0")/.."

add_sub() { # add_sub <url> <path>
  if [ -f "$2/CMakeLists.txt" ] || [ -e "$2/.git" ]; then
    echo ">> $2 already present, skipping"
  else
    git submodule add -f "$1" "$2"
  fi
}

echo ">> Adding submodules..."
add_sub https://github.com/N64Recomp/N64ModernRuntime.git lib/N64ModernRuntime
add_sub https://github.com/rt64/rt64.git                  lib/rt64
add_sub https://github.com/mikke89/RmlUi.git              lib/RmlUi
add_sub https://github.com/sammycage/lunasvg.git          lib/lunasvg
# N64Recomp arrives transitively via N64ModernRuntime, but we also need the
# standalone recompiler executables, so vendor it directly too:
add_sub https://github.com/bryankruman/N64Recomp.git        lib/N64Recomp

echo ">> Updating submodules recursively..."
git submodule update --init --recursive

echo ">> Building N64Recomp + RSPRecomp (Release)..."
cmake -S lib/N64Recomp -B lib/N64Recomp/build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build lib/N64Recomp/build --config Release

echo ">> Copying recompiler executables to repo root..."
find lib/N64Recomp/build -maxdepth 2 -type f \
  \( -name 'N64Recomp' -o -name 'N64Recomp.exe' -o -name 'RSPRecomp' -o -name 'RSPRecomp.exe' \) \
  -exec cp -f {} . \;

echo ">> Done."
echo ">> Next: scripts/fetch-elf.sh, then ./N64Recomp beetle-adventure-racing-recomp.toml"
