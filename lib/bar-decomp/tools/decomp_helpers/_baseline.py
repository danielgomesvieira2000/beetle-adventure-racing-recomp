"""Regression gate: full build + whole-ROM SHA1 check. Because every module's
SHA256 must pass for the ROM to build, a clean build to e5ab4d22 proves no
already-matched function regressed. Used after any header/struct edit (Phase 1.5)
and after every wave.

Usage: _baseline.py            -> OK / REGRESSED (prints failing module if any)
"""
import os, subprocess, re

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
TARGET = "e5ab4d226c08d22f68a2edcc48870203e67454b8"

subprocess.run("rm -f build/*.z64", shell=True)
r = subprocess.run("make -j6 2>&1", shell=True, capture_output=True, text=True)
out = r.stdout + r.stderr

sha = subprocess.run("sha1sum build/*.z64 2>/dev/null", shell=True, capture_output=True, text=True).stdout
got = sha.split()[0] if sha.strip() else ""
if got == TARGET:
    print("OK e5ab4d22")
else:
    print("REGRESSED")
    # find the module whose hash failed
    fails = re.findall(r'build/bin/us/(\w+)\.o', "\n".join(
        [l for l in out.split("\n") if "Error" in l or "failed" in l.lower()]))
    if fails:
        print("  failing module(s):", ", ".join(sorted(set(fails))))
    errs = [l for l in out.split("\n") if "rror" in l or "verification failed" in l]
    for l in errs[:6]:
        print("  " + l.strip()[:160])
    print(f"  got sha1: {got or '(no ROM produced)'}")
