import subprocess, re, os, sys

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
MOD = sys.argv[1]
SRC = f"src/modules/{MOD}.c"

head = subprocess.check_output(["git","show",f"HEAD:{SRC}"]).decode()
work = open(SRC).read()

head_lines = head.split("\n")
work_lines = work.split("\n")

pragma_re = re.compile(r'#pragma GLOBAL_ASM\("asm/us/nonmatchings/modules/%s/(func_%s_[0-9A-Fa-f]+)\.s"\)' % (MOD, MOD))
head_funcs = []
for ln in head_lines:
    m = pragma_re.search(ln)
    if m:
        head_funcs.append(m.group(1))

def pragma_line(f):
    return f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{MOD}/{f}.s")'

decompiled = [f for f in head_funcs if pragma_line(f) not in work]
print("HEAD funcs:", len(head_funcs))
print("Decompiled in working:", decompiled)

first_pragma_idx = next(i for i,l in enumerate(work_lines) if l.startswith("#pragma GLOBAL_ASM"))
spdx = work_lines[0]
header_block = work_lines[1:first_pragma_idx]

def extract_block(fname):
    defidx = None
    for i,l in enumerate(work_lines):
        if re.search(r'\b%s\s*\(' % re.escape(fname), l) and not l.rstrip().endswith(";") and "#pragma" not in l:
            defidx = i
            break
    if defidx is None:
        raise RuntimeError("no def for "+fname)
    depth=0; started=False; endidx=None
    for i in range(defidx, len(work_lines)):
        depth += work_lines[i].count("{") - work_lines[i].count("}")
        if "{" in work_lines[i]:
            started=True
        if started and depth==0:
            endidx=i
            break
    start=defidx
    while start-1>=0 and (work_lines[start-1].lstrip().startswith("//") or work_lines[start-1].lstrip().startswith("/*") or work_lines[start-1].lstrip().startswith("*")):
        start-=1
    return work_lines[start:endidx+1]

blocks = {f: extract_block(f) for f in decompiled}
for f in decompiled:
    print(f"  block {f}: {len(blocks[f])} lines")

def build_candidate(funcs_to_impl):
    out = [spdx] + header_block
    for l in head_lines[1:]:
        m = pragma_re.search(l)
        if m and m.group(1) in funcs_to_impl:
            out.extend(blocks[m.group(1)])
        else:
            out.append(l)
    return "\n".join(out)

def test(funcs_to_impl):
    open(SRC,"w").write(build_candidate(funcs_to_impl))
    subprocess.run(["rm","-f",f"build/bin/us/{MOD}.o"])
    r = subprocess.run(f"make build/bin/us/{MOD}.o 2>&1", shell=True, capture_output=True, text=True)
    out = r.stdout + r.stderr
    if "Hash verification passed" in out:
        return "MATCH"
    if "Hash verification failed" in out:
        return "NOMATCH"
    if "Error" in out or r.returncode!=0:
        return "BUILDERR"
    return "UNKNOWN"

results={}
for f in decompiled:
    res = test([f])
    results[f]=res
    print(f"ISOLATED {f}: {res}")

matching=[f for f in decompiled if results[f]=="MATCH"]
print("MATCHING SUBSET:", matching)
final_res = test(matching) if matching else "NONE"
print("COMBINED:", final_res, matching)
