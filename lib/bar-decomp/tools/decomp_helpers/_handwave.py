"""Integrate hand-written C for a module from a JSON spec, build-gated + bisected.

Spec JSON: {"module": "<mod>", "externs": ["extern s32 D_x;", ...],
            "funcs": {"func_x": "<full C definition>", ...}}

Ensures `#include "common.h"`, inserts any missing extern lines right after the
includes, swaps each func's GLOBAL_ASM pragma for its C, builds the module .o,
and on hash failure bisects to the matching subset. Leaves the .c matching.
Prints MATCHED:/FAILED: lines and a final SUMMARY.

Usage: _handwave.py <spec.json>
"""
import os, re, sys, json, subprocess

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)

spec = json.load(open(sys.argv[1]))
MOD = spec["module"]
externs = spec.get("externs", [])
funcs = spec["funcs"]
SRC = f"src/modules/{MOD}.c"
orig = open(SRC).read()

def pragma(fn):
    return f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{MOD}/{fn}.s")'

for fn in funcs:
    if pragma(fn) not in orig:
        print(f"WARN: pragma for {fn} not found")

def sig_of(code):
    # first line "ret func_x(params) {" -> "ret func_x(params);"
    head = code.split("{", 1)[0].strip()
    return head + ";"

def prep(text, keep):
    # ensure common.h
    if '#include "common.h"' not in text:
        lines = text.split("\n")
        i = 0
        while i < len(lines) and (lines[i].strip() == "" or lines[i].lstrip().startswith(("//", "/*", "*"))):
            i += 1
        lines.insert(i, '#include "common.h"')
        text = "\n".join(lines)
    # fix existing forward prototypes for the funcs we define (avoid conflicts)
    for fn in keep:
        newproto = sig_of(funcs[fn])
        # match a standalone prototype line ending in ';' that declares fn
        pat = re.compile(r'^[^\n{}]*\b%s\s*\([^;{]*\)\s*;[ \t]*$' % re.escape(fn), re.M)
        if pat.search(text):
            text = pat.sub(newproto, text, count=1)
    # add missing externs. Skip if the symbol already appears ANYWHERE in the
    # original module (declared, or used by existing code -> already declared
    # somewhere): re-declaring risks a conflicting-types error. Checking `orig`
    # (pre-insertion) avoids the false-positive of the inserted body's own use.
    miss = []
    for e in externs:
        syms = re.findall(r'\bD_\w+|\bg[A-Z]\w+', e)
        if syms and re.search(r'\b%s\b' % re.escape(syms[0]), orig):
            continue
        if e in text:
            continue
        miss.append(e)
    if miss:
        lines = text.split("\n")
        last_inc = max((i for i, l in enumerate(lines) if l.startswith("#include")), default=0)
        for j, e in enumerate(miss):
            lines.insert(last_inc + 1 + j, e)
        text = "\n".join(lines)
    return text

def build_text(keep):
    t = orig
    for fn in keep:
        t = t.replace(pragma(fn), funcs[fn])
    return prep(t, keep)

def test(keep):
    open(SRC, "w").write(build_text(keep))
    subprocess.run(["rm", "-f", f"build/bin/us/{MOD}.o", f"build/src/modules/{MOD}.o"])
    r = subprocess.run(f"make build/bin/us/{MOD}.o 2>&1", shell=True, capture_output=True, text=True)
    out = r.stdout + r.stderr
    if "Hash verification passed" in out:
        return "MATCH"
    if "Hash verification failed" in out:
        return "NOMATCH"
    return "BUILDERR:" + " | ".join([l for l in out.split("\n") if "rror" in l][:3])

res = test(list(funcs))
if res == "MATCH":
    matched = list(funcs)
    failed = []
else:
    matched, failed = [], []
    for fn in funcs:
        r = test([fn])
        (matched if r == "MATCH" else failed).append((fn, r) if r != "MATCH" else fn)
    matched = [m for m in matched]
    test(matched)

if not matched:
    open(SRC, "w").write(orig)

for fn in matched:
    print("MATCHED:", fn)
for item in failed:
    print("FAILED:", item)
print(f"SUMMARY: {len(matched)}/{len(funcs)} matched")
