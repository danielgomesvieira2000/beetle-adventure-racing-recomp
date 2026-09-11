#!/usr/bin/env python3
"""Integrate a wave's Opus-verified C robustly. Run inside WSL from repo root:
  python3 _integrate_wave.py <result.json>
Apply per wave-module, build, revert any module that fails to compile or mismatches,
confirm a FRESH matching ROM, then (only then) delete the matched .s files.
"""
import json, os, re, subprocess, sys, html

SHA = "e5ab4d226c08d22f68a2edcc48870203e67454b8"
raw = open(sys.argv[1], encoding="utf-8", errors="replace").read()
data = json.loads(raw)
mods = (data.get("result") or data).get("modules") or data.get("modules")

def apply_module(entry):
    mod = entry["module"]
    cfile = f"src/modules/{mod}.c"
    if not os.path.exists(cfile):
        return None
    funcs = [(f["name"], html.unescape(f["code"])) for f in entry.get("functions", [])
             if not any(ord(c) > 127 for c in html.unescape(f["code"]))]   # unescape HTML entities; ASCII-only (euc-jp)
    if not funcs:
        return None
    lines = open(cfile, encoding="utf-8").read().split("\n")
    # ensure common.h is present (do NOT move an existing one); find its index
    ci = next((i for i, l in enumerate(lines) if l.strip() == '#include "common.h"'), None)
    if ci is None:
        ip = 0
        while ip < len(lines) and lines[ip].lstrip().startswith("//"):
            ip += 1
        lines.insert(ip, '#include "common.h"')
        ci = ip
    # add only NEW externs, right after common.h
    body = "\n".join(lines)
    new_ext = []
    for ex in entry.get("addExterns", []):
        ex = ex.strip()
        if not ex.endswith(";"):
            ex += ";"
        if ex not in body and ex not in new_ext:
            new_ext.append(ex)
    for j, ex in enumerate(new_ext):
        lines.insert(ci + 1 + j, ex)
    # forward-declare module functions that are STILL asm (so same-module calls resolve).
    # generic `void f();` (unspecified args) is matching-safe and doesn't conflict with the
    # applied functions' real signatures (we exclude those).
    whole = "\n".join(lines)
    applied_names = {f["name"] for f in entry.get("functions", [])}
    fn_names = sorted(set(re.findall(rf'\bfunc_{re.escape(mod)}_[0-9A-Fa-f]+\b', whole)))
    protos = []
    for n in fn_names:
        if n in applied_names:
            continue
        if re.search(rf'^[A-Za-z_][\w\s\*]*\b{re.escape(n)}\s*\(', whole, re.M):
            continue  # already declared/defined in C
        protos.append(f'void {n}();')
    pi = ci + 1 + len(new_ext)
    for k, pr in enumerate(protos):
        lines.insert(pi + k, pr)
    s = "\n".join(lines)
    applied = []
    for name, code in funcs:
        pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{name}.s")'
        if pragma in s:
            s = s.replace(pragma, code)
            applied.append(name)
    if not applied:
        return None
    open(cfile, "w", encoding="utf-8").write(s)
    return mod, applied

subprocess.run("git checkout -- src/modules/", shell=True)   # clean base (wipe any stray agent edits)
applied = {}
for entry in mods:
    r = apply_module(entry)
    if r:
        applied[r[0]] = r[1]
print(f"applied {sum(len(v) for v in applied.values())} fns across {len(applied)} modules")

def build():
    r = subprocess.run("make -k -j6", shell=True, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr

def detect_failures(log, candidates):
    bad = set()
    for line in log.splitlines():
        # make failure lines name the module's object regardless of where the error was reported:
        #   [Makefile:NNN: build/src/modules/<mod>.o] Error   (compile, incl. header cascades)
        #   [Makefile:NNN: build/bin/us/<mod>.o] Error         (module hash MISMATCH)
        m = re.search(r"build/(?:src/modules|bin/us)/([a-z_0-9]+)\.o\b.*Error", line)
        if m and m.group(1) in candidates:
            bad.add(m.group(1))
        if "MISMATCH" in line:
            m2 = re.search(r"partial_([a-z_0-9]+)\.o", line)
            if m2 and m2.group(1) in candidates:
                bad.add(m2.group(1))
        e = re.search(r"src/modules/([a-z_0-9]+)\.c.*error:", line)
        if e and e.group(1) in candidates:
            bad.add(e.group(1))
    return bad

# iterate: build -> revert failures -> repeat (keep .s intact so reverts build)
for _ in range(5):
    rc, log = build()
    open("/tmp/wint.log", "w").write(log)
    bad = detect_failures(log, set(applied))
    if rc == 0 and not bad:
        break
    if rc != 0 and not bad:           # unattributed break -> bail safely
        bad = set(applied)
    for m in bad:
        subprocess.run(f"git checkout -- src/modules/{m}.c", shell=True)
        applied.pop(m, None)
    print(f"reverted {sorted(bad)}; {len(applied)} modules remain")
    if not applied:
        break

# FINAL confirmation on a FRESH ROM (avoid stale-sha false positive)
subprocess.run("rm -f build/beetleadventurerac.us.z64", shell=True)
rc, _ = build()
sha = ""
if os.path.exists("build/beetleadventurerac.us.z64"):
    sha = subprocess.run("sha1sum build/beetleadventurerac.us.z64", shell=True,
                         capture_output=True, text=True).stdout.split()[0]
match = (rc == 0 and sha == SHA)
print(f"FINAL rc={rc} sha={sha[:12]} match={match}")

if match:
    for mod, names in applied.items():          # delete matched .s ONLY now
        for n in names:
            p = f"asm/us/nonmatchings/modules/{mod}/{n}.s"
            if os.path.exists(p):
                os.remove(p)
    print(f"LANDED {sum(len(v) for v in applied.values())} fns across {len(applied)} modules: {sorted(applied)}")
else:
    print("NOT MATCHING -> reverting all touched modules")
    subprocess.run("git checkout -- src/modules/", shell=True)
