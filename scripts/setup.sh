#!/usr/bin/env bash
# Bootstrap beetle-adventure-racing-recomp: build the recompiler tools.
# Run once after cloning; safe to re-run.
#
# There is nothing to fetch. This project is STANDALONE: every library it builds against lives under
# lib/ as ordinary files of this repository, not as a git submodule. A clone is complete, and
# `git submodule update` has nothing to do here. See README.md ("Vendored, not submoduled") for what
# each library is, where it came from and who wrote it.
#
# One consequence worth knowing: the local change RecompFrontend needed -- the
# players::auto_assign_controllers that scripts/patch-recompinput.py used to add after every
# submodule update -- is now simply part of the vendored source. That script is kept because it is
# the record of what was changed and why, and it is idempotent, but it has nothing left to do.
set -euo pipefail
cd "$(dirname "$0")/.."

echo ">> Building N64Recomp + RSPRecomp (Release)..."
cmake -S lib/N64Recomp -B lib/N64Recomp/build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build lib/N64Recomp/build --config Release

echo ">> Copying recompiler executables to repo root..."
find lib/N64Recomp/build -maxdepth 2 -type f \
  \( -name 'N64Recomp' -o -name 'N64Recomp.exe' -o -name 'RSPRecomp' -o -name 'RSPRecomp.exe' \) \
  -exec cp -f {} . \;

echo ">> Done."
echo ">> Next: scripts/fetch-elf.sh, then ./N64Recomp beetle-adventure-racing-recomp.toml"
