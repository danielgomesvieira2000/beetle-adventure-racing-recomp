#!/usr/bin/env python3
"""Per-FUNCTION salvage of a wave's verified C. Run in WSL from repo root:
  python3 _salvage_wave.py <result.json>
For each module: try all its functions at once (fast); if the module doesn't hash-match,
add functions one at a time and keep only those that preserve the match. Comprehensive
forward declarations (real return types) resolve intra-module calls. Final full build confirms.
"""
import json, os, re, subprocess, sys, html

SHA = "e5ab4d226c08d22f68a2edcc48870203e67454b8"
raw = open(sys.argv[1], encoding="utf-8", errors="replace").read()
d = json.loads(raw)
mods = (d.get("result") or d)["modules"]

def sh(c): return subprocess.run(c, shell=True, capture_output=True, text=True)
def read(f): return open(f, encoding="utf-8").read()
def write(f, s): open(f, "w", encoding="utf-8").write(s)
def rettype(code, name): return (code[:code.find(name)].strip() or "void")

def prep(entry):
    mod = entry["module"].split("/")[-1].removesuffix(".c"); cf = f"src/modules/{mod}.c"
    lines = read(cf).split("\n")
    ci = next((i for i, l in enumerate(lines) if l.strip() == '#include "common.h"'), None)
    if ci is None:
        ip = 0
        while ip < len(lines) and lines[ip].lstrip().startswith("//"): ip += 1
        lines.insert(ip, '#include "common.h"'); ci = ip
    ins = ci + 1
    whole = "\n".join(lines)
    for h in ("structs.h", "global_exports.h"):   # shared knowledge-base headers
        line = f'#include "{h}"'
        if line not in whole:
            lines.insert(ins, line); ins += 1
    whole = "\n".join(lines)
    for inc in entry.get("addIncludes", []):
        h = inc
        if '"' in inc: h = inc.split('"')[1]
        elif "<" in inc: h = inc.split("<")[1].split(">")[0]
        h = h.replace("#include", "").strip().strip('"<>')
        if not h.endswith(".h") or h == "common.h":
            continue
        line = f'#include "{h}"'
        if line not in whole:
            lines.insert(ins, line); ins += 1
    whole = "\n".join(lines)
    for ex in entry.get("addExterns", []):
        ex = ex.strip(); ex = ex if ex.endswith(";") else ex + ";"
        if ex not in whole: lines.insert(ins, ex); ins += 1
    whole = "\n".join(lines)
    for item in entry.get("addStructs", []) + entry.get("addPrototypes", []):   # writer discoveries (local scope)
        item = html.unescape(item).strip()
        if item and all(ord(c) < 128 for c in item) and item not in whole:
            lines.insert(ins, item); ins += 1
    whole = "\n".join(lines)
    wave = {f["name"]: rettype(html.unescape(f["code"]), f["name"]) for f in entry.get("functions", [])}
    names = sorted(set(re.findall(rf'\bfunc_{re.escape(mod)}_[0-9A-Fa-f]+\b', whole)) | set(wave))
    for n in names:
        if re.search(rf'^[A-Za-z_][\w\s\*]*\b{re.escape(n)}\s*\([^;]*\)\s*\{{', whole, re.M):
            continue  # already defined
        decl = f"{wave.get(n, 'void')} {n}();"
        if decl not in whole:
            lines.insert(ins, decl); ins += 1
    write(cf, "\n".join(lines))
    return cf

def mod_ok(mod):
    return sh(f"make build/bin/us/{mod}.o -j6").returncode == 0

sh("git checkout -- src/modules/")
landed = {}
for entry in mods:
    mod = entry["module"].split("/")[-1].removesuffix(".c"); cf = f"src/modules/{mod}.c"
    funcs = [(f["name"], html.unescape(f["code"])) for f in entry.get("functions", [])
             if not any(ord(c) > 127 for c in html.unescape(f["code"]))]
    if not funcs or not re.fullmatch(r'[a-z_0-9]+', mod) or not os.path.exists(cf):
        continue
    prep(entry)
    s = read(cf)
    for n, code in funcs:
        s = s.replace(f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{n}.s")', code)
    write(cf, s)
    if mod_ok(mod):
        landed[mod] = [n for n, _ in funcs]
        print(f"{mod}: ALL {len(funcs)}", flush=True)
        continue
    sh(f"git checkout -- {cf}"); prep(entry)
    cur = read(cf); kept = []
    for n, code in funcs:
        trial = cur.replace(f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{n}.s")', code)
        if trial == cur:
            continue
        write(cf, trial)
        if mod_ok(mod):
            cur = trial; kept.append(n)
        else:
            write(cf, cur)
    if kept:
        landed[mod] = kept
    else:
        sh(f"git checkout -- {cf}")
    print(f"{mod}: kept {len(kept)}/{len(funcs)}", flush=True)

sh("rm -f build/beetleadventurerac.us.z64")
rc = sh("make -j6").returncode
sha = sh("sha1sum build/beetleadventurerac.us.z64").stdout.split()[0] if os.path.exists("build/beetleadventurerac.us.z64") else ""
match = rc == 0 and sha == SHA
total = sum(len(v) for v in landed.values())
print(f"FINAL match={match} LANDED {total} fns across {len(landed)} modules", flush=True)
if match:
    for mod, names in landed.items():
        for n in names:
            p = f"asm/us/nonmatchings/modules/{mod}/{n}.s"
            if os.path.exists(p): os.remove(p)
else:
    print("final mismatch -> reverting"); sh("git checkout -- src/modules/")
