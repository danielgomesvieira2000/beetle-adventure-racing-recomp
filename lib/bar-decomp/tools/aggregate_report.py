#!/usr/bin/env python3
"""Aggregate a full-project decomp.dev report across the main segment and all overlay modules.

BAR's modules are partially linked, so each has its own map (build/partial_<mod>.map, emitted by
the patched partial-link rule). This runs `mapfile_parser objdiff_report` on the main map plus every
module map, then merges the measures and units into one report.json and adds a per-module category
breakdown (so decomp.dev can show progress per module).

Usage: python3 tools/aggregate_report.py [output.json]
"""
import json, os, sys, glob, subprocess, tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
OUT = sys.argv[1] if len(sys.argv) > 1 else "report.json"

MAIN_MAP = "build/beetleadventurerac.us.map"
MODULE_MAPS = sorted(glob.glob("build/partial_*.map"))
REPORT_ARGS = ["-a", "asm/us", "-n", "asm/us/nonmatchings", "--quiet"]

def cat_id(mappath):
    b = os.path.basename(mappath)
    if b.startswith("partial_"):
        return b[len("partial_"):-len(".map")]
    return "main"

def run_report(mappath, tmpdir):
    out = os.path.join(tmpdir, cat_id(mappath) + ".json")
    try:
        subprocess.run(["mapfile_parser", "objdiff_report", mappath, out, *REPORT_ARGS],
                       check=True, capture_output=True, text=True)
        with open(out) as f:
            return json.load(f)
    except Exception as e:
        print(f"  warn: skipping {mappath}: {e}", file=sys.stderr)
        return None

def i(v):  # measures store code/data byte totals as strings
    try: return int(v)
    except (TypeError, ValueError): return 0

def fnum(v):
    try: return float(v)
    except (TypeError, ValueError): return 0.0

def pct(n, d):
    return (100.0 * n / d) if d else 0.0

def main():
    if not os.path.exists(MAIN_MAP):
        print(f"error: {MAIN_MAP} missing — run `make` first.", file=sys.stderr); sys.exit(2)
    maps = [MAIN_MAP] + MODULE_MAPS
    print(f"aggregating {len(maps)} maps (main + {len(MODULE_MAPS)} modules)…", file=sys.stderr)

    units, categories = [], []
    tot_code = mat_code = tot_fn = mat_fn = tot_units = 0
    fuzzy_bytes = data_pct_w = complete_data_w = 0.0

    with tempfile.TemporaryDirectory() as td:
        for mp in maps:
            d = run_report(mp, td)
            if not d:
                continue
            m = d.get("measures", {})
            tc = i(m.get("total_code")); mc = i(m.get("matched_code"))
            tf = m.get("total_functions", 0) or 0; mf = m.get("matched_functions", 0) or 0
            tu = m.get("total_units", 0) or 0
            tot_code += tc; mat_code += mc; tot_fn += tf; mat_fn += mf; tot_units += tu
            fuzzy_bytes  += fnum(m.get("fuzzy_match_percent")) / 100.0 * tc
            data_pct_w   += fnum(m.get("matched_data_percent")) * tc
            complete_data_w += fnum(m.get("complete_data_percent")) * tc
            units.extend(d.get("units", []))
            cid = cat_id(mp)
            # normalize: ensure category measures always carry the standard keys
            m.setdefault("matched_functions_percent", pct(mf, tf))
            m.setdefault("matched_code_percent", pct(mc, tc))
            m.setdefault("total_functions", tf); m.setdefault("matched_functions", mf)
            categories.append({"id": cid, "name": cid, "measures": m})

    measures = {
        "fuzzy_match_percent": pct(fuzzy_bytes, tot_code),
        "total_code": str(tot_code),
        "matched_code": str(mat_code),
        "matched_code_percent": pct(mat_code, tot_code),
        "matched_data_percent": (data_pct_w / tot_code) if tot_code else 0.0,
        "complete_data_percent": (complete_data_w / tot_code) if tot_code else 0.0,
        "total_functions": tot_fn,
        "matched_functions": mat_fn,
        "matched_functions_percent": pct(mat_fn, tot_fn),
        "total_units": tot_units,
    }
    report = {"measures": measures, "units": units, "version": 2, "categories": categories}
    with open(OUT, "w") as f:
        json.dump(report, f)

    print(f">>> wrote {OUT} ({os.path.getsize(OUT)} bytes)")
    print(f">>> matched_code  : {measures['matched_code_percent']:.2f}%  ({mat_code}/{tot_code} bytes)")
    print(f">>> matched_funcs : {measures['matched_functions_percent']:.2f}%  ({mat_fn}/{tot_fn})")
    print(f">>> coverage      : main segment + {len(MODULE_MAPS)} modules  ({tot_units} units)")

if __name__ == "__main__":
    main()
