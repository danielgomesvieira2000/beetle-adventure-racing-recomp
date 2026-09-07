# Bootstrap beetle-adventure-racing-recomp on Windows: fetch deps and build the recompiler tools.
$ErrorActionPreference = "Stop"
Set-Location (Join-Path $PSScriptRoot "..")

function Add-Sub($url, $path) {
  if ((Test-Path (Join-Path $path "CMakeLists.txt")) -or (Test-Path (Join-Path $path ".git"))) {
    Write-Host ">> $path already present, skipping"
  } else {
    git submodule add -f $url $path
  }
}

Write-Host ">> Adding submodules..."
Add-Sub "https://github.com/N64Recomp/N64ModernRuntime.git" "lib/N64ModernRuntime"
Add-Sub "https://github.com/rt64/rt64.git"                  "lib/rt64"
Add-Sub "https://github.com/mikke89/RmlUi.git"              "lib/RmlUi"
Add-Sub "https://github.com/sammycage/lunasvg.git"          "lib/lunasvg"
Add-Sub "https://github.com/N64Recomp/N64Recomp.git"        "lib/N64Recomp"

Write-Host ">> Updating submodules recursively..."
git submodule update --init --recursive

Write-Host ">> Building N64Recomp + RSPRecomp (Release)..."
cmake -S lib/N64Recomp -B lib/N64Recomp/build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build lib/N64Recomp/build --config Release

Write-Host ">> Copying recompiler executables to repo root..."
Get-ChildItem -Path lib/N64Recomp/build -Recurse -Include N64Recomp.exe,RSPRecomp.exe |
  ForEach-Object { Copy-Item -Force $_.FullName . }

Write-Host ">> Done. Next: scripts/fetch-elf.ps1, then .\N64Recomp.exe beetle-adventure-racing-recomp.toml"
