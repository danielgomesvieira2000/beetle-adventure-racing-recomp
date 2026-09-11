#!/usr/bin/env python3
"""For each still-unmatched seed, apply it to the CLEAN module, build, and if it fails to COMPILE,
write a fix-card (the broken seed C + the exact compiler errors + the target asm path) so a fix agent
can repair it. Also record NOMATCH ones (for the permuter). Run in WSL."""
import os, re, glob, subprocess, json
REPO = os.path.expanduser("~/projects/bar-decomp"); os.chdir(REPO)
GRIND = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind"
FIX = f"{GRIND}/fixcards"; os.makedirs(FIX, exist_ok=True)
SENT = "/*__SEEDEXTERNS__*/"

def ensure_sentinel(text):
    if SENT in text: return text
    lines = text.split("\n"); li = max([i for i,l in enumerate(lines) if l.strip().startswith("#include")]+[-1])
    lines.insert(li+1, SENT); return "\n".join(lines)

def find_body(lines, fn):
    di = None
    for i,l in enumerate(lines):
        if re.search(r'\b%s\s*\('%re.escape(fn), l) and not l.rstrip().endswith(";"): di=i; break
    if di is None: return None
    d=0; st=False; end=None
    for i in range(di,len(lines)):
        d += lines[i].count("{")-lines[i].count("}")
        if "{" in lines[i]: st=True
        if st and d==0: end=i; break
    return None if end is None else (di,end)

unm = json.load(open(f"{GRIND}/unmatched.json"))
builderr, nomatch = [], []
for it in unm:
    mod, fn = it["mod"], it["fn"]; SRC=f"src/modules/{mod}.c"; sp=f"{GRIND}/seeds/{fn}.c"
    if not os.path.exists(sp): continue
    orig = open(SRC).read()
    pragma=f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")'
    if pragma not in orig: continue
    seedc = open(sp).read()
    lines = seedc.split("\n"); bb = find_body(lines, fn)
    if bb is None: continue
    # apply whole seed (externs+typedefs+body) at the pragma; strip pre-existing proto
    cand = ensure_sentinel(orig).replace(pragma, seedc)
    cand = re.sub(r'(?m)^[ \t]*[A-Za-z_][\w \t\*]*\b%s\s*\([^;{]*\)\s*;[ \t]*\r?\n'%re.escape(fn),'',cand)
    open(SRC,"w").write(cand)
    subprocess.run(["rm","-f",f"build/bin/us/{mod}.o",f"build/src/modules/{mod}.o"],stdout=-3,stderr=-3)
    out = subprocess.run(f"make build/bin/us/{mod}.o 2>&1",shell=True,capture_output=True,text=True).stdout
    open(SRC,"w").write(orig)
    if "Hash verification passed" in out: continue
    if "Hash verification failed" in out: nomatch.append(fn); continue
    errs = [re.sub(r'\x1b\[[0-9;]*[mK]','',l) for l in out.split("\n") if "error:" in l][:12]
    asm = open(f"asm/us/nonmatchings/modules/{mod}/{fn}.s").read()
    card = (f"FUNCTION: {fn}\nMODULE: {mod}\n\n=== YOUR PREVIOUS C SEED (it FAILS TO COMPILE) ===\n{seedc}\n\n"
            f"=== COMPILER ERRORS (fix ALL of these) ===\n" + "\n".join(errs) + "\n\n"
            f"=== TARGET ASM (your fixed C must still byte-match this) ===\n{asm}\n")
    open(f"{FIX}/{fn}.txt","w").write(card); builderr.append(fn)

json.dump(builderr, open(f"{GRIND}/builderr.json","w"))
json.dump(nomatch, open(f"{GRIND}/nomatch.json","w"))
print(f"BUILDERR fix-cards: {len(builderr)}   NOMATCH (permuter pool): {len(nomatch)}")
