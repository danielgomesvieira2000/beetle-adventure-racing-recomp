#!/usr/bin/env bash
# cdb-run.sh — launch beetle-adventure-racing-recomp under cdb (console WinDbg) with the NT debug heap DISABLED.
#
# WHY -hd IS MANDATORY (do not remove it):
#   When a process is *created by* a debugger, Windows silently switches its heap into the NT debug
#   heap: every allocation gets tracking headers + fill patterns and frees get progressively more
#   expensive as the heap grows. For a game that allocates continuously this looks like steadily
#   climbing memory, worsening frame times, and eventually timing artifacts (audio underruns, visual
#   glitches) until it falls over. It is a DEBUGGER ARTIFACT, not a game bug, and it never happens
#   when the .exe is run directly. `-hd` + _NO_DEBUG_HEAP=1 keep the normal heap so long debugging
#   sessions behave exactly like a normal run. Always launch the debugger through this script.
#
# Usage:   tools/cdb-run.sh [extra cdb args...]
# Env overrides:
#   BAR_BUILD  build dir holding beetle-adventure-racing-recomp.exe + .pdb   (default: build-cmake, relative to repo root)
#   BAR_CDB    path to cdb.exe                             (default: newest Microsoft.WinDbg package)
#   BAR_LOG    cdb command log                             (default: <build>/cdb.log)
#   BAR_DUMP   crash minidump path                         (default: <build>/bar_crash.dmp)
#
# Default behaviour: run the game; on a *fatal* (second-chance) access violation, dump registers,
# the faulting-thread stack, all thread stacks with line numbers, and a full minidump, then quit.
# First-chance AVs and benign C++ exceptions are passed through so normal play is uninterrupted.
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${BAR_BUILD:-$repo_root/build-cmake}"
[ -d "$build_dir" ] || { echo "cdb-run: build dir not found: $build_dir" >&2; exit 1; }
build_win="$(cygpath -m "$build_dir")"          # drive-letter path cdb understands
exe_win="$build_win/beetle-adventure-racing-recomp.exe"
[ -f "$build_dir/beetle-adventure-racing-recomp.exe" ] || { echo "cdb-run: beetle-adventure-racing-recomp.exe not built in $build_dir" >&2; exit 1; }

cdb="${BAR_CDB:-}"
if [ -z "$cdb" ]; then
    # WinDbg ships as a Store app under C:\Program Files\WindowsApps, whose directory can't be
    # enumerated (restrictive ACLs) — so glob it and you get nothing. Query the package registry
    # for its install location instead; that works regardless of version and needs no enumeration.
    loc="$(powershell.exe -NoProfile -Command '(Get-AppxPackage Microsoft.WinDbg).InstallLocation' 2>/dev/null | tr -d '\r')"
    [ -n "$loc" ] && cdb="$(cygpath -u "$loc")/amd64/cdb.exe"
fi
[ -n "$cdb" ] && [ -x "$cdb" ] || {
    echo "cdb-run: cdb.exe not found. Install WinDbg (winget install Microsoft.WinDbg) or set BAR_CDB." >&2
    exit 1
}

log_win="$(cygpath -m "${BAR_LOG:-$build_dir/cdb.log}")"
dump_win="$(cygpath -m "${BAR_DUMP:-$build_dir/bar_crash.dmp}")"

# sxd av: don't break on first-chance access violations (pass to the app) so normal play isn't
# interrupted; the real, fatal crash surfaces as a second-chance break which returns from `g`.
cmds=".echo ===CDB_RUN_STARTED===; sxd av; g;"
cmds+=" .echo ===CRASH_CAUGHT===; .lines -e; .reload /f; .exr -1; .ecxr; r;"
cmds+=" .echo --FAULTING_INSTR--; u @rip L8;"
cmds+=" .echo --FAULTING_THREAD--; kb 200;"
cmds+=" .echo --ALL_THREADS--; ~*kb 30;"
cmds+=" .echo --DUMP--; .dump /ma $dump_win; .echo ===CDB_RUN_END===; q"

export _NO_DEBUG_HEAP=1
cd "$build_dir"                                  # child inherits cwd (finds ROM/assets next to the exe)
echo "cdb-run: $cdb -hd  (debug heap OFF)  ->  $exe_win" >&2
exec "$cdb" -hd -logo "$log_win" -y "$build_win" -c "$cmds" "$@" "$exe_win"
