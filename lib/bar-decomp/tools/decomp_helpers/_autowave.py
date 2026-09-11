"""Integrate auto-generated C for one module: apply candidates, ensure a
common.h include, build the module object, and on hash failure bisect down to
the matching subset (isolation per function). Leaves the .c in a matching state.
Prints the matched function names (one per line, prefixed MATCHED:).

Usage: _autowave.py <module>
Relies on _autogen2.gen for candidate generation.
"""
import os, re, sys, subprocess, importlib.util

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
WIN = "/mnt/c/Users/Bryan/Projects/BeetleAdventureDecomp"

spec = importlib.util.spec_from_file_location("ag2", f"{WIN}/_autogen2.py")
ag2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(ag2)

MOD = sys.argv[1]
SRC = f"src/modules/{MOD}.c"

orig = open(SRC).read()

# candidates
funcs = re.findall(r'GLOBAL_ASM\("asm/us/nonmatchings/modules/%s/(func_%s_[0-9A-Fa-f]+)\.s"\)' % (MOD, MOD), orig)
cands = {}
for fn in funcs:
    p = f"asm/us/nonmatchings/modules/{MOD}/{fn}.s"
    if os.path.exists(p):
        code = ag2.gen(fn, ag2.parse_asm(p))
        if code:
            cands[fn] = code
if not cands:
    print("NOCANDS")
    sys.exit(0)

def pragma(fn):
    return f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{MOD}/{fn}.s")'

def ensure_include(text):
    if '#include "common.h"' in text:
        return text
    lines = text.split("\n")
    i = 0
    while i < len(lines) and (lines[i].strip() == "" or lines[i].lstrip().startswith("//") or lines[i].lstrip().startswith("/*") or lines[i].lstrip().startswith("*")):
        i += 1
    lines.insert(i, '#include "common.h"')
    return "\n".join(lines)

def build_text(keep):
    t = orig
    for fn in keep:
        t = t.replace(pragma(fn), cands[fn])
    return ensure_include(t)

def test(keep):
    open(SRC, "w").write(build_text(keep))
    subprocess.run(["rm", "-f", f"build/bin/us/{MOD}.o", f"build/src/modules/{MOD}.o"])
    r = subprocess.run(f"make build/bin/us/{MOD}.o 2>&1", shell=True, capture_output=True, text=True)
    out = r.stdout + r.stderr
    if "Hash verification passed" in out:
        return "MATCH"
    if "Hash verification failed" in out:
        return "NOMATCH"
    return "BUILDERR"

# try all at once
res = test(list(cands))
if res == "MATCH":
    matched = list(cands)
else:
    # isolation bisect
    matched = []
    for fn in cands:
        if test([fn]) == "MATCH":
            matched.append(fn)
    test(matched)  # leave file with matching subset

if not matched:
    open(SRC, "w").write(orig)  # restore, nothing matched
    print("NONE")
else:
    for fn in matched:
        print("MATCHED:", fn)
