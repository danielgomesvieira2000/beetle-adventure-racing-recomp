"""Phase 2.1 harvester: like _m2cwave.py but ACCEPTS candidates that call other
functions, provided every callee is already DECLARED (in include/ headers or as a
same-module forward prototype). Still rewrites struct argN->unkOFF to raw casts,
still rejects loops / unknown-types / m2c artifacts. m2c's own function-extern
guesses are dropped (the module's forward protos + headers declare callees);
build gate + bisect filter any wrong-width-proto mismatches.

Usage: _m2cwave2.py <module> <out.json>
"""
import os, re, sys, json, subprocess, glob

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
MOD, OUT = sys.argv[1], sys.argv[2]
SRC = f"src/modules/{MOD}.c"
src_txt = open(SRC).read()
funcs = re.findall(r'GLOBAL_ASM\("asm/us/nonmatchings/modules/%s/(func_%s_[0-9A-Fa-f]+)\.s"\)' % (MOD, MOD), src_txt)

LOAD = {'lw': 's32', 'lh': 's16', 'lhu': 'u16', 'lb': 's8', 'lbu': 'u8', 'lwc1': 'f32'}
STORE = {'sw': 's32', 'sh': 's16', 'sb': 'u8', 'swc1': 'f32'}
BAD = ["?", "MIPS2C", "goto", "while", "for ", "do {", "do{", "loop", "phi_", "temp_", "(sp", "__"]

# declared-function set: headers + this module's forward protos + its own func names
def declared_funcs():
    d = set()
    for h in glob.glob("include/**/*.h", recursive=True):
        try:
            d |= set(re.findall(r'\b([a-zA-Z_]\w+)\s*\(', open(h, errors="ignore").read()))
        except Exception:
            pass
    # same-module: forward protos AND all func_ names (mod .c forward-declares them)
    d |= set(re.findall(r'\b(func_%s_[0-9A-Fa-f]+)\s*\(' % MOD, src_txt))
    d |= set(funcs)
    return d
DECL = declared_funcs()

def parse_asm(path):
    out = []
    for line in open(path):
        m = re.search(r'\*/\s+(\S+)\s*(.*?)\s*$', line)
        if m: out.append((m.group(1), m.group(2).strip()))
    return out

def width_map(insns):
    wm = {}
    for mn, ops in insns:
        mm = re.match(r'\$\w+,\s*(-?0x[0-9A-Fa-f]+|0)\((\$a[0-3])\)$', ops)
        if mm and (mn in LOAD or mn in STORE):
            off = int(mm.group(1), 16) if mm.group(1) != '0' else 0
            wm[(int(mm.group(2)[2]), off)] = LOAD.get(mn) or STORE.get(mn)
    return wm

def resolve_fields(body, wm):
    failed = [False]
    def rep(m):
        var, offs = m.group(1), m.group(2)
        am = re.match(r'arg([0-3])$', var)
        if not am: failed[0] = True; return m.group(0)
        cast = wm.get((int(am.group(1)), int(offs, 16)))
        if cast is None: failed[0] = True; return m.group(0)
        return f"(*({cast} *)((u8 *)arg{am.group(1)} + 0x{int(offs,16):X}))"
    new = re.sub(r'\b(arg[0-3])->unk([0-9A-Fa-f]+)', rep, body)
    if failed[0] or "->" in new: return None
    return new

externs, cands = {}, {}
for fn in funcs:
    p = f"asm/us/nonmatchings/modules/{MOD}/{fn}.s"
    if not os.path.exists(p): continue
    r = subprocess.run(["python3", "tools/m2c/m2c.py", "-t", "mips-ido-c", p], capture_output=True, text=True)
    out = r.stdout.strip()
    if not out or fn not in out: continue
    lines = out.split("\n")
    # keep only DATA externs (extern <type> D_...;), drop m2c function-extern guesses
    ex = [l.strip() for l in lines if l.strip().startswith("extern ") and re.search(r'\bD_\w+', l) and "(" not in l]
    body = "\n".join(l for l in lines if not l.strip().startswith("extern ")).strip()
    if not re.search(r'\b%s\s*\(' % re.escape(fn), body): continue
    sig = body.split("{", 1)[0]
    if "?" in sig: continue
    if "->" in body:
        nb = resolve_fields(body, width_map(parse_asm(p)))
        if nb is None: continue
        body = nb
    if any(b in body for b in BAD): continue
    # ACCEPT calls only if every callee is declared
    callees = [m for m in re.findall(r'\b([a-zA-Z_]\w+)\s*\(', body) if m != fn]
    callees = [c for c in callees if c not in ("if", "while", "for", "return", "sizeof", "switch")]
    undeclared = [c for c in callees if c.startswith("func_") and c not in DECL]
    # engine/SDK callees not in DECL: allow (likely in SDK macro headers); build gate filters
    if undeclared: continue
    # retype ? externs used only as &D_X
    fixed, ok = [], True
    for e in ex:
        if "?" in e:
            syms = re.findall(r'\bD_\w+', e)
            if len(syms) != 1: ok = False; break
            if re.search(r'(?<!&)\b%s\b' % re.escape(syms[0]), body): ok = False; break
            fixed.append("extern u8 %s;" % syms[0])
        else: fixed.append(e)
    if not ok: continue
    declared = set()
    for e in fixed: declared.update(re.findall(r'\bD_\w+', e))
    if set(re.findall(r'\bD_\w+', body)) - declared: continue
    cands[fn] = body
    for e in fixed:
        sym = re.findall(r'\bD_\w+', e); externs[sym[0] if sym else e] = e

spec = {"module": MOD, "externs": list(externs.values()), "funcs": cands}
json.dump(spec, open(OUT, "w"), indent=1)
print(f"{MOD}: {len(cands)}/{len(funcs)} call-accepting candidates")
