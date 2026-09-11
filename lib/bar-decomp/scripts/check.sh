#!/usr/bin/env bash
# Test apparatus: verify build status efficiently at checkpoints.
#
#   scripts/check.sh                 incremental build + ROM SHA-1 + progress summary (default)
#   scripts/check.sh --module <m>    incremental build + verify just module <m>'s hash (fast)
#   scripts/check.sh --diff <Func>   run asm-differ on one function (no full build)
#   scripts/check.sh --progress      progress report only (no build)
#   scripts/check.sh --full          same as default but always prints full progress
#
# Exit 0 = matching/OK, non-zero = mismatch or build error. Intended cadence:
#   per-function -> --diff ;  per-chunk -> --module ;  per-session/commit -> default.
set -uo pipefail

# Resolve repo root (parent of this script's dir) and cd there.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$ROOT" || exit 2

JOBS="${JOBS:-6}"
ROM="build/beetleadventurerac.us.z64"
SHA_FILE="beetleadventurerac.us.sha1"

# Ensure the venv is active (the Makefile needs the venv's mapfile_parser on PATH).
if [ -z "${VIRTUAL_ENV:-}" ]; then
  if [ -f .venv/bin/activate ]; then
    # shellcheck disable=SC1091
    source .venv/bin/activate
  else
    echo "WARNING: no .venv found; build may fail at pre-partial-link (Error 127)." >&2
  fi
fi

mode="default"; arg=""
case "${1:-}" in
  --module)   mode="module";   arg="${2:-}";;
  --diff)     mode="diff";     arg="${2:-}";;
  --progress) mode="progress";;
  --full)     mode="full";;
  "" )        mode="default";;
  *) echo "unknown option: $1"; sed -n '2,12p' "$0"; exit 2;;
esac

run_progress() { [ -f tools/progress.py ] && python3 tools/progress.py || true; }

case "$mode" in
  progress)
    run_progress; exit 0;;
  diff)
    [ -n "$arg" ] || { echo "usage: check.sh --diff <Func>"; exit 2; }
    exec ./diff.py -mwo "$arg";;
esac

echo ">>> incremental build (make -j$JOBS)…"
build_log="$(mktemp)"
if ! make -j"$JOBS" 2>&1 | tee "$build_log"; then
  echo ">>> BUILD FAILED"; rm -f "$build_log"; exit 1
fi

# Per-module fast path: confirm the module's hash check passed in this build.
if [ "$mode" = "module" ] && [ -n "$arg" ]; then
  if grep -qiE "MISMATCH.*$arg|$arg.*MISMATCH" "$build_log"; then
    echo ">>> MODULE $arg: MISMATCH"; rm -f "$build_log"; exit 1
  fi
  # best-effort explicit re-check if the helper supports it
  if [ -f tools/checkModuleHash.py ]; then
    python3 tools/checkModuleHash.py "$arg" 2>/dev/null || true
  fi
  echo ">>> MODULE $arg: OK (built & hash-checked)"
  rm -f "$build_log"; exit 0
fi

rm -f "$build_log"

# Full ROM verification.
if [ ! -f "$ROM" ]; then echo ">>> NO ROM PRODUCED"; exit 1; fi
built="$(sha1sum "$ROM" | cut -d' ' -f1)"
expect="$(cut -d' ' -f1 "$SHA_FILE" 2>/dev/null)"
sz="$(stat -c %s "$ROM")"
echo ">>> ROM size : $sz (expect 16777216)"
echo ">>> built    : $built"
echo ">>> expect   : $expect"
if [ "$built" = "$expect" ]; then
  echo ">>> RESULT   : ✓ MATCHING (build/beetleadventurerac.us.z64: OK)"
  echo; run_progress
  exit 0
else
  echo ">>> RESULT   : ✗ MISMATCH"
  exit 1
fi
