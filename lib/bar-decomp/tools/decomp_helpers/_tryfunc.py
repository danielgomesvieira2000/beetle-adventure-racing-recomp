"""Try a candidate C body for a single function in a module.
Usage: _tryfunc.py <module> <funcname> <candidate_c_file>
Swaps the function's pragma for the candidate C, builds the module object,
reports MATCH/NOMATCH, dumps the objdump for the function on NOMATCH,
then ALWAYS restores the file to its prior state (clean iteration).
"""
import subprocess, os, sys

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
MOD, FN, CFILE = sys.argv[1], sys.argv[2], sys.argv[3]
SRC = f"src/modules/{MOD}.c"
PRAGMA = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{MOD}/{FN}.s")'

orig = open(SRC).read()
cand = open(CFILE).read().rstrip("\n")
assert PRAGMA in orig, f"pragma for {FN} not present in {SRC}"

try:
    open(SRC, "w").write(orig.replace(PRAGMA, cand))
    subprocess.run(["rm", "-f", f"build/bin/us/{MOD}.o", f"build/src/modules/{MOD}.o"])
    r = subprocess.run(f"make build/bin/us/{MOD}.o 2>&1", shell=True, capture_output=True, text=True)
    out = r.stdout + r.stderr
    if "Hash verification passed" in out:
        print("RESULT: MATCH")
    elif "Hash verification failed" in out:
        print("RESULT: NOMATCH")
        subprocess.run(f"make build/src/modules/{MOD}.o 2>/dev/null", shell=True)
        d = subprocess.run(
            f"mips-linux-gnu-objdump -d build/src/modules/{MOD}.o", shell=True,
            capture_output=True, text=True).stdout
        cap = False
        for line in d.split("\n"):
            if f"<{FN}>:" in line:
                cap = True
            if cap:
                print(line)
                if line.strip().endswith("jr\tra") or "\tjr\tra" in line:
                    pass
            if cap and line.strip() == "":
                break
    else:
        print("RESULT: BUILDERR")
        print("\n".join([l for l in out.split("\n") if "rror" in l or "rror:" in l][:15]))
finally:
    open(SRC, "w").write(orig)
    print("(restored)")
