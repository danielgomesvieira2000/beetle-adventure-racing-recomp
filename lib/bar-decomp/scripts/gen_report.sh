#!/usr/bin/env bash
# Generate a decomp.dev-compatible progress report (report.json) covering the WHOLE project:
# the main segment plus all 133 overlay modules.
#
#   scripts/gen_report.sh [output.json]
#
# How it works: the patched partial-link rule emits a per-module map (build/partial_<mod>.map) for
# every overlay during `make`. tools/aggregate_report.py runs mapfile_parser's objdiff_report on the
# main map + every module map and merges them (with a per-module category breakdown). Run `make`
# first so the maps exist.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"; cd "$ROOT" || exit 2
[ -z "${VIRTUAL_ENV:-}" ] && [ -f .venv/bin/activate ] && source .venv/bin/activate

OUT="${1:-report.json}"
[ -f build/beetleadventurerac.us.map ] || { echo "no build map — run 'make' first"; exit 1; }
NMAPS=$(ls build/partial_*.map 2>/dev/null | wc -l)
[ "$NMAPS" -gt 0 ] || echo "WARNING: no build/partial_*.map found — report will cover the main segment only. Rebuild after the -Map Makefile patch."

exec python3 tools/aggregate_report.py "$OUT"
