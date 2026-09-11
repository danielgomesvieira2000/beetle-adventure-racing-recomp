# Bootstrap beetle-adventure-racing-recomp on Windows: build the recompiler tools.
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
$ErrorActionPreference = "Stop"
Set-Location (Join-Path $PSScriptRoot "..")

Write-Host ">> Building N64Recomp + RSPRecomp (Release)..."
cmake -S lib/N64Recomp -B lib/N64Recomp/build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build lib/N64Recomp/build --config Release

Write-Host ">> Copying recompiler executables to repo root..."
Get-ChildItem -Path lib/N64Recomp/build -Recurse -Include N64Recomp.exe,RSPRecomp.exe |
  ForEach-Object { Copy-Item -Force $_.FullName . }

Write-Host ">> Done. Next: scripts/fetch-elf.ps1, then .\N64Recomp.exe beetle-adventure-racing-recomp.toml"
