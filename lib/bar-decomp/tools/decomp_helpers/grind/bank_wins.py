#!/usr/bin/env python3
"""Bank the gate-verified wins recorded in .grind/sweep_results.json (BANKABLE entries):
sequential accumulation with a per-bank module-hash gate (keep iff pass, else revert that
one), then full make + ROM SHA gate, ledger touch, nomatch/pool refresh, local git commit.

Candidate source per function: .grind/wins/<fn>.c (staged verbatim by permute_campaign2 /
m2c_seed_all / sweep) if present, else reconstructed from the function's score-0 permuter
output + its seed preamble.
"""
import os, re, glob, subprocess, sys, json, csv

REPO = os.path.expanduser("~/projects/bar-decomp"); os.chdir(REPO)
GRIND = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind"
WINS = f"{GRIND}/wins"
SHA = "e5ab4d226c08d22f68a2edcc48870203e67454b8"
RESP = f"{GRIND}/sweep_results.json"

res = json.load(open(RESP)) if os.path.exists(RESP) else {}
TARGETS = set(fn for fn, r in res.items() if r == "BANKABLE")
# also pick up staged win files an interrupted run never recorded (re-gated below anyway)
for wf in glob.glob(f"{WINS}/func_*.c"):
    fn = os.path.basename(wf)[:-2]
    if res.get(fn) in ("BANKED", "BANK-FAIL"): continue
    m = re.match(r'func_([a-z0-9_]+?)_[0-9A-Fa-f]{8}$', fn)
    if not m: continue
    src = f"src/modules/{m.group(1)}.c"
    if os.path.exists(src) and f'/{fn}.s"' in open(src).read():
        TARGETS.add(fn)
TARGETS = sorted(TARGETS)
print(f"banking {len(TARGETS)}: {TARGETS}")
if not TARGETS: sys.exit(0)

st = subprocess.run("git status --porcelain -- src asm progress", shell=True,
                    capture_output=True, text=True).stdout.strip()
if st:
    print("WORK PATHS NOT CLEAN (src/asm/progress) — aborting:\n" + st); sys.exit(1)

def find_def(lines, fn):
    for i, l in enumerate(lines):
        if re.search(r'\b%s\s*\(' % re.escape(fn), l) and not l.rstrip().endswith(";"):
            return i
    return None

def extract_def(text, fn):
    lines = text.split("\n"); di = find_def(lines, fn)
    if di is None: return None
    depth = 0; started = False
    for i in range(di, len(lines)):
        depth += lines[i].count("{") - lines[i].count("}")
        if "{" in lines[i]: started = True
        if started and depth == 0:
            return "\n".join(lines[di:i + 1])
    return None

def seed_preamble(fn):
    for p in (f"{GRIND}/seeds/{fn}.c", f"progress/seeds/{fn}.c", f"progress/wins/{fn}.c"):
        if os.path.exists(p):
            lines = open(p).read().split("\n"); di = find_def(lines, fn)
            if di is not None:
                return "\n".join(lines[:di]).strip()
    return ""

def win_body(fn):
    for o in sorted(glob.glob(f"nonmatchings/{fn}/output*")):
        try: s = int(open(f"{o}/score.txt").read().strip())
        except Exception: continue
        if s == 0:
            return extract_def(open(f"{o}/source.c").read(), fn)
    return None

def reconstruct(fn, orig, pragma):
    body = win_body(fn)
    if not body: return None
    pre = seed_preamble(fn)
    hay = re.sub(r'#pragma GLOBAL_ASM\([^)]*\)', '', orig)
    keep = []
    if pre:
        units, buf, depth = [], [], 0
        for l in pre.split("\n"):
            s = l.strip()
            if not s or s.startswith("//"): continue
            buf.append(l); depth += l.count("{") - l.count("}")
            if depth <= 0 and s.endswith(";"):
                units.append("\n".join(buf)); buf = []; depth = 0
        for u in units:
            mm = re.search(r'}\s*(\w+)\s*;', u) or re.search(r'\b(D_\w+|g[A-Z]\w+|func_\w+)\b', u)
            key = mm.group(1) if mm else None
            if key and re.search(r'\b%s\b' % re.escape(key), hay):
                continue
            keep.append(u)
    return ("\n".join(keep) + "\n\n" if keep else "") + body

def gate(mod):
    subprocess.run(["rm", "-f", f"build/bin/us/{mod}.o", f"build/src/modules/{mod}.o"],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    r = subprocess.run(f"make build/bin/us/{mod}.o 2>&1", shell=True, capture_output=True, text=True)
    return "Hash verification passed" in (r.stdout + r.stderr)

banked = []
for fn in sorted(TARGETS):
    m = re.match(r'func_([a-z0-9_]+?)_[0-9A-Fa-f]{8}$', fn)
    if not m: continue
    mod = m.group(1)
    SRC = f"src/modules/{mod}.c"
    if not os.path.exists(SRC): continue
    orig = open(SRC).read()
    pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")'
    if pragma not in orig:
        print(f"  {fn:42s} SKIP (pragma gone)"); res[fn] = "BANKED"; continue
    wf = f"{WINS}/{fn}.c"
    cand = open(wf).read() if os.path.exists(wf) else reconstruct(fn, orig, pragma)
    if not cand:
        print(f"  {fn:42s} SKIP (no candidate)"); continue
    swapped = orig.replace(pragma, cand)
    swapped = re.sub(r'(?m)^[ \t]*[A-Za-z_][\w \t\*]*\b%s\s*\([^;{]*\)\s*;[ \t]*\r?\n'
                     % re.escape(fn), '', swapped)
    open(SRC, "w").write(swapped)
    if gate(mod):
        banked.append(fn); res[fn] = "BANKED"
        print(f"  {fn:42s} BANKED")
    else:
        open(SRC, "w").write(orig); res[fn] = "BANK-FAIL"
        print(f"  {fn:42s} REVERTED (gate failed at bank time)")

json.dump(res, open(RESP, "w"), indent=1)
if not banked:
    print("nothing banked"); sys.exit(0)

print("\nfull build + SHA gate ...")
subprocess.run("make -j6 >/dev/null 2>&1", shell=True)
sha = subprocess.run("sha1sum build/beetleadventurerac.us.z64", shell=True,
                     capture_output=True, text=True).stdout
if SHA not in sha:
    print(f"ROM SHA MISMATCH — reverting everything!\n{sha}")
    subprocess.run("git checkout -- src/modules", shell=True)
    sys.exit(1)
print(f"ROM OK {sha.split()[0]}")

for fn in banked:
    mod = re.match(r'func_([a-z0-9_]+?)_[0-9A-Fa-f]{8}$', fn).group(1)
    s = f"asm/us/nonmatchings/modules/{mod}/{fn}.s"
    if os.path.exists(s): subprocess.run(["git", "rm", "-q", "-f", s])

if os.path.exists("progress/ledger.csv"):
    rows = list(csv.DictReader(open("progress/ledger.csv")))
    for r in rows:
        if r["fn"] in banked: r["status"] = "banked"
    w = csv.DictWriter(open("progress/ledger.csv", "w", newline=""), fieldnames=rows[0].keys())
    w.writeheader(); w.writerows(rows)

for poolname in ("nomatch.json", "pool_night.json"):
    pp = f"{GRIND}/{poolname}"
    if not os.path.exists(pp): continue
    pool = json.load(open(pp))
    fresh = []
    for fn in pool:
        m = re.match(r'func_([a-z0-9_]+?)_[0-9A-Fa-f]{8}$', fn)
        if not m: continue
        src = f"src/modules/{m.group(1)}.c"
        if os.path.exists(src) and f'/{fn}.s"' in open(src).read():
            fresh.append(fn)
    json.dump(fresh, open(pp, "w"))
    print(f"{poolname} refreshed: {len(pool)} -> {len(fresh)}")

subprocess.run("git add -A src/modules progress/ledger.csv", shell=True)
msg = ("bank %d gate-verified wins (permuter/m2c pipeline, byte-exact, ROM OK)\n\n"
       "%s\n\nEach gated per-module (DaisyBox hash) with sequential accumulation, then full\n"
       "ROM rebuild verified sha e5ab4d22.\n\n"
       "Co-Authored-By: Claude Fable 5 <noreply@anthropic.com>") % (len(banked), "\n".join(sorted(banked)))
open("/tmp/bankmsg.txt", "w").write(msg)
subprocess.run("git commit -q -F /tmp/bankmsg.txt", shell=True)
print(subprocess.run("git log --oneline -1", shell=True, capture_output=True, text=True).stdout)
print("BANKED:", " ".join(sorted(banked)))
