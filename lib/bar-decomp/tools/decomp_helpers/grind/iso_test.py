#!/usr/bin/env python3
"""Test each unmatched seed INDIVIDUALLY against the clean committed module (no cross-seed accumulation),
to separate 'bad seed' from 'accumulation conflict'. Then greedily bank the isolated-matches that also
combine. Run in WSL:  python3 .../.grind/iso_test.py
"""
import os, re, glob, shutil, subprocess, json
from collections import defaultdict

REPO = os.path.expanduser("~/projects/bar-decomp"); os.chdir(REPO)
GRIND = "/mnt/c/Users/Bryan/Projects/BeetleRecomp/.grind"
SENT = "/*__SEEDEXTERNS__*/"

def unit_key(text):
    t = text.strip()
    m = re.search(r'}\s*(\w+)\s*;', t)
    if m: return ("type", m.group(1))
    m = re.match(r'typedef\b.*\b(\w+)\s*;', t)
    if m: return ("type", m.group(1))
    m = re.search(r'\b(D_\w+|g[A-Z]\w+)\b', t)
    if m: return ("sym", m.group(1))
    m = re.search(r'\b(func_\w+)\s*\(', t)
    if m: return ("sym", m.group(1))
    return ("raw", t)

def parse_seed(path, fn):
    lines = open(path).read().split("\n"); di = None
    for i, l in enumerate(lines):
        if re.search(r'\b%s\s*\(' % re.escape(fn), l) and not l.rstrip().endswith(";"):
            di = i; break
    if di is None: return None, None, None
    depth = 0; started = False; end = None
    for i in range(di, len(lines)):
        depth += lines[i].count("{") - lines[i].count("}")
        if "{" in lines[i]: started = True
        if started and depth == 0: end = i; break
    if end is None: return None, None, None
    body = "\n".join(lines[di:end + 1]); pre = lines[:di]; comments, units = [], []; i = 0
    while i < len(pre):
        s = pre[i].strip()
        if s == "": i += 1; continue
        if s.startswith("//") or (s.startswith("/*") and s.endswith("*/")): comments.append(pre[i]); i += 1; continue
        if ("typedef" in s or s.startswith("struct")) and ("{" in s or s.endswith("{")):
            blk = []; d = 0; j = i
            while j < len(pre):
                blk.append(pre[j]); d += pre[j].count("{") - pre[j].count("}")
                if d <= 0 and "{" in "".join(blk) and pre[j].rstrip().endswith(";"): break
                j += 1
            units.append((unit_key("\n".join(blk)), "\n".join(blk))); i = j + 1; continue
        if s.endswith(";"): units.append((unit_key(pre[i]), pre[i])); i += 1; continue
        i += 1
    return "\n".join(comments), units, body

def ensure_sentinel(text):
    if SENT in text: return text
    lines = text.split("\n"); last_inc = max([i for i, l in enumerate(lines) if l.strip().startswith("#include")] + [-1])
    lines.insert(last_inc + 1, SENT); return "\n".join(lines)

def header_closure(root="include/common.h"):
    seen, stack = set(), [root]
    while stack:
        p = stack.pop()
        if p in seen or not os.path.exists(p): continue
        seen.add(p)
        for m in re.finditer(r'#include\s+"([^"]+)"', open(p, errors="ignore").read()):
            for cand in (os.path.normpath(os.path.join("include", m.group(1))),
                         os.path.normpath(os.path.join(os.path.dirname(p), m.group(1)))):
                if os.path.exists(cand): stack.append(cand)
    return seen
HS = set()
for hp in header_closure():
    t = open(hp, errors="ignore").read()
    for m in re.finditer(r'\bextern\s+[\w \t\*]+?\b(\w+)\s*(\[[^\]]*\])?\s*;', t): HS.add(("sym", m.group(1)))
    for m in re.finditer(r'}\s*(\w+)\s*;', t): HS.add(("type", m.group(1)))
    for m in re.finditer(r'typedef\b[^;{]*\b(\w+)\s*;', t): HS.add(("type", m.group(1)))
    for m in re.finditer(r'(?m)^[ \t]*[A-Za-z_][\w \t\*]+?\b(\w+)\s*\([^;{]*\)\s*;', t): HS.add(("sym", m.group(1)))

def build_ok(mod):
    subprocess.run(["rm","-f",f"build/bin/us/{mod}.o",f"build/src/modules/{mod}.o"],stdout=-3,stderr=-3)
    out = subprocess.run(f"make build/bin/us/{mod}.o 2>&1",shell=True,capture_output=True,text=True).stdout
    if "Hash verification passed" in out: return "MATCH"
    if "Hash verification failed" in out: return "NOMATCH"
    return "BUILDERR"

unm = json.load(open(f"{GRIND}/unmatched.json"))
res = defaultdict(list)
iso_match = []
for it in unm:
    mod, fn = it["mod"], it["fn"]; SRC = f"src/modules/{mod}.c"; sp = f"{GRIND}/seeds/{fn}.c"
    if not os.path.exists(sp): continue
    orig = open(SRC).read()
    pragma = f'#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/{mod}/{fn}.s")'
    if pragma not in orig: continue
    comment, units, body = parse_seed(sp, fn)
    if body is None: res[mod].append((fn,"PARSE")); continue
    new = [t for (k,t) in units if k not in HS]
    cand = ensure_sentinel(orig).replace(pragma, (comment+"\n" if comment else "")+body)
    cand = re.sub(r'(?m)^[ \t]*[A-Za-z_][\w \t\*]*\b%s\s*\([^;{]*\)\s*;[ \t]*\r?\n' % re.escape(fn),'',cand)
    if new: cand = cand.replace(SENT, SENT+"\n"+"\n".join(new),1)
    open(SRC,"w").write(cand); r = build_ok(mod); open(SRC,"w").write(orig)
    res[mod].append((fn,r))
    if r == "MATCH": iso_match.append(fn)

print("=== ISOLATED per-seed results (each vs clean module) ===")
tm=tn=tb=0
for mod in sorted(res):
    m=sum(1 for _,r in res[mod] if r=="MATCH"); n=sum(1 for _,r in res[mod] if r=="NOMATCH"); b=sum(1 for _,r in res[mod] if r=="BUILDERR")
    tm+=m;tn+=n;tb+=b
    print(f"  {mod:12s} MATCH={m}  NOMATCH={n}  BUILDERR={b}")
print(f"TOTAL isolated: MATCH={tm} NOMATCH={tn} BUILDERR={tb}")
json.dump(iso_match, open(f"{GRIND}/iso_match.json","w"))
print("iso-match fns saved -> iso_match.json")
