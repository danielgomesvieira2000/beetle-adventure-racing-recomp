# Stage a built Windows tree into a release folder and zip it. Adapted from the recomp framework's
# tools/package_release.ps1 (the series' shared packager).
#
#   powershell -ExecutionPolicy Bypass -File scripts/package-release.ps1 -Version 0.3.0-alpha
#
# What goes in: the executable, the three DLLs it cannot start without (SDL2, dxcompiler, dxil), the
# assets/ folder the menus draw from, this project's license (COPYING), the third-party notices, the
# README and the license texts of the vendored libraries. The debug symbols (PDB) go into a second zip,
# named -debug-symbols: only a crash report needs them. Nothing here is a ROM, a save or a user config,
# and the script refuses to continue if it finds one where it is staging.

param(
    [string]$BuildDir = "build-frontend",
    [Parameter(Mandatory = $true)][string]$Version,   # e.g. 0.3.0-alpha
    [string]$OutDir = "dist"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $root

$exeName = "beetle-adventure-racing-recomp"
$exe = Join-Path $BuildDir "$exeName.exe"
if (-not (Test-Path $exe)) {
    throw "no executable at $exe -- build first (see BUILDING.md)"
}

# Release asset naming as in earlier releases: beetle-adventure-racing-recomp-v0.2.0-alpha-windows-x64.zip
$name = "$exeName-v$Version-windows-x64"
$stage = Join-Path $OutDir $name
if (Test-Path $stage) { Remove-Item -Recurse -Force $stage }
New-Item -ItemType Directory -Force $stage | Out-Null

$files = @(
    "$BuildDir\$exeName.exe",
    "$BuildDir\SDL2.dll",
    "$BuildDir\dxcompiler.dll",
    "$BuildDir\dxil.dll",
    "COPYING",
    "THIRD_PARTY_NOTICES.md",
    "README.md"
)
foreach ($f in $files) {
    if (-not (Test-Path $f)) { throw "missing $f" }
    Copy-Item $f $stage
}
Copy-Item -Recurse "$BuildDir\assets" (Join-Path $stage "assets")

# License texts of the vendored libraries the executable contains. A missing source stops packaging
# rather than shipping without it.
$licenses = Join-Path $stage "licenses"
New-Item -ItemType Directory -Force $licenses | Out-Null
$licenseSources = [ordered]@{
    "N64ModernRuntime-COPYING.txt"      = "lib\N64ModernRuntime\COPYING"
    "N64Recomp-LICENSE.txt"             = "lib\N64Recomp\LICENSE"
    "RT64-LICENSE.txt"                  = "lib\rt64\LICENSE"
    "bar-decomp-LICENSE.txt"            = "lib\bar-decomp\LICENSE"
    "RmlUi-LICENSE.txt"                 = "lib\RecompFrontend\recompui\lib\RmlUi\LICENSE.txt"
    "lunasvg-LICENSE.txt"               = "lib\RecompFrontend\recompui\lib\lunasvg\LICENSE"
    "FreeType-LICENSE.txt"              = "lib\RecompFrontend\recompui\lib\freetype-windows-binaries\LICENSE.TXT"
    "GamepadMotionHelpers-LICENSE.txt"  = "lib\RecompFrontend\lib\GamepadMotionHelpers\LICENSE"
    "SDL2-COPYING.txt"                  = "$BuildDir\_deps\sdl2-src\COPYING.txt"
}
foreach ($entry in $licenseSources.GetEnumerator()) {
    if (-not (Test-Path $entry.Value)) { throw "missing license text $($entry.Value)" }
    Copy-Item $entry.Value (Join-Path $licenses $entry.Key)
}

# Belt and braces: no dump, no save, no user config, no log in the staging folder.
$forbidden = Get-ChildItem -Recurse $stage | Where-Object {
    ($_.Extension -in ".z64", ".n64", ".v64", ".rom", ".bin", ".eep", ".sra", ".fla", ".pak", ".mpk", ".log") -or
    ($_.Name -in "hud.json", "graphics.json", "controls.json", "input.json", "general.json", "sound.json", "portable.txt")
}
if ($forbidden) {
    $forbidden | ForEach-Object { Write-Error "refusing to package $($_.FullName)" }
    throw "game data or user files found in the staging folder"
}

$zip = Join-Path $OutDir "$name.zip"
if (Test-Path $zip) { Remove-Item $zip }
Compress-Archive -Path (Join-Path $stage "*") -DestinationPath $zip

$pdb = Join-Path $BuildDir "$exeName.pdb"
if (Test-Path $pdb) {
    $symbols = Join-Path $OutDir "$name-debug-symbols.zip"
    if (Test-Path $symbols) { Remove-Item $symbols }
    Compress-Archive -Path $pdb -DestinationPath $symbols
    Write-Host "symbols: $symbols"
}

Write-Host "package: $zip"
$stageFull = (Resolve-Path $stage).Path
Get-ChildItem -Recurse $stage | Where-Object { -not $_.PSIsContainer } |
    ForEach-Object { "{0,10:N0}  {1}" -f $_.Length, $_.FullName.Substring($stageFull.Length + 1) }
