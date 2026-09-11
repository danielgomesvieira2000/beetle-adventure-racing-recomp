"""Parse the handseed workflow output, write each seed to progress/seeds/<fn>.c,
and build per-module handwave specs at /tmp/hsspec_<mod>.json for later testing.

Usage: _process_handseed.py <workflow_output.json>
"""
import os, json, csv, sys, re

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
out = json.load(open(sys.argv[1]))
seeds = out["result"]["seeds"]

mod_of = {r["fn"]: r["mod"] for r in csv.DictReader(open("progress/ledger.csv"))}
os.makedirs("progress/seeds", exist_ok=True)

def data_extern(e):
    # keep only data-global externs (extern <type> D_x; / g_x;), drop function
    # prototypes (they conflict with headers / module forward decls) and junk
    e = e.strip()
    if "(" in e or not e.startswith("extern ") or not e.endswith(";"):
        return False
    return bool(re.search(r'\b(D_\w+|g[A-Z]\w+)\b', e))

by_mod = {}
skipped = []
for s in seeds:
    fn = s["fn"]; code = s["code"].rstrip()
    externs = [e for e in (s.get("externs", []) or []) if data_extern(e)]
    mod = mod_of.get(fn)
    if not mod:
        skipped.append(fn); continue
    # non-ASCII guard
    try:
        code.encode("ascii"); "".join(externs).encode("ascii")
    except UnicodeEncodeError:
        skipped.append(fn + "(nonascii)"); continue
    open(f"progress/seeds/{fn}.c", "w").write("\n".join(externs + [code]) + "\n")
    by_mod.setdefault(mod, {"funcs": {}, "externs": set()})
    by_mod[mod]["funcs"][fn] = code
    for e in externs:
        by_mod[mod]["externs"].add(e)

for mod, d in by_mod.items():
    spec = {"module": mod, "externs": sorted(d["externs"]), "funcs": d["funcs"]}
    json.dump(spec, open(f"/tmp/hsspec_{mod}.json", "w"), indent=1)

print(f"seeds written: {sum(len(d['funcs']) for d in by_mod.values())} across {len(by_mod)} modules")
print("modules:", " ".join(sorted(by_mod)))
if skipped: print("skipped:", skipped)
open("/tmp/hs_modules.txt", "w").write("\n".join(sorted(by_mod)))
