"""Run m2c per remaining function in a module and auto-accept clean candidates,
including struct-field accessors: m2c's `argN->unkOFF` is rewritten to a
width-correct raw cast resolved from the asm load/store at off(aN). Rejects
anything with control flow, calls, unknown types, or unresolved fields.
Emits a handwave spec JSON.

Usage: _m2cwave.py <module> <out.json>
"""
import os, re, sys, json, subprocess

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)
MOD, OUT = sys.argv[1], sys.argv[2]
SRC = f"src/modules/{MOD}.c"
txt = open(SRC).read()
funcs = re.findall(r'GLOBAL_ASM\("asm/us/nonmatchings/modules/%s/(func_%s_[0-9A-Fa-f]+)\.s"\)' % (MOD, MOD), txt)

LOAD = {'lw': 's32', 'lh': 's16', 'lhu': 'u16', 'lb': 's8', 'lbu': 'u8', 'lwc1': 'f32'}
STORE = {'sw': 's32', 'sh': 's16', 'sb': 'u8', 'swc1': 'f32'}

# blockers (note: '->' handled separately by the field resolver)
BAD = ["?", "MIPS2C", "goto", "while", "for ", "do {", "do{", "loop",
       "phi_", "temp_", "(sp", "__"]

def parse_asm(path):
    out = []
    for line in open(path):
        m = re.search(r'\*/\s+(\S+)\s*(.*?)\s*$', line)
        if m:
            out.append((m.group(1), m.group(2).strip()))
    return out

def width_map(insns):
    wm = {}
    for mn, ops in insns:
        mm = re.match(r'\$\w+,\s*(-?0x[0-9A-Fa-f]+|0)\((\$a[0-3])\)$', ops)
        if mm and (mn in LOAD or mn in STORE):
            off = int(mm.group(1), 16) if mm.group(1) != '0' else 0
            ai = int(mm.group(2)[2])
            wm[(ai, off)] = LOAD.get(mn) or STORE.get(mn)
    return wm

def resolve_fields(body, wm):
    """rewrite argN->unkOFF to width-correct casts; return None if any unresolved."""
    failed = [False]
    def rep(m):
        var, offs = m.group(1), m.group(2)
        am = re.match(r'arg([0-3])$', var)
        if not am:
            failed[0] = True
            return m.group(0)
        ai = int(am.group(1))
        off = int(offs, 16)
        cast = wm.get((ai, off))
        if cast is None:
            failed[0] = True
            return m.group(0)
        return f"(*({cast} *)((u8 *)arg{ai} + 0x{off:X}))"
    new = re.sub(r'\b(arg[0-3])->unk([0-9A-Fa-f]+)', rep, body)
    if failed[0] or "->" in new:
        return None
    return new

externs = {}
cands = {}
for fn in funcs:
    p = f"asm/us/nonmatchings/modules/{MOD}/{fn}.s"
    if not os.path.exists(p):
        continue
    r = subprocess.run(["python3", "tools/m2c/m2c.py", "-t", "mips-ido-c", p],
                       capture_output=True, text=True)
    out = r.stdout.strip()
    if not out or fn not in out:
        continue
    lines = out.split("\n")
    ex = [l.strip() for l in lines if l.strip().startswith("extern ")]
    body = "\n".join(l for l in lines if not l.strip().startswith("extern ")).strip()
    if not re.search(r'\b%s\s*\(' % re.escape(fn), body):
        continue
    # signature must be fully typed
    sig = body.split("{", 1)[0]
    if "?" in sig:
        continue
    # resolve struct fields if present
    if "->" in body:
        nb = resolve_fields(body, width_map(parse_asm(p)))
        if nb is None:
            continue
        body = nb
    if any(b in body for b in BAD):
        continue
    if [m for m in re.findall(r'\b(func_\w+)\s*\(', body) if m != fn]:
        continue
    # handle '?' externs: retype to u8 if used only as &D_X, else reject
    fixed, ok = [], True
    for e in ex:
        if "?" in e:
            syms = re.findall(r'\bD_\w+', e)
            if len(syms) != 1:
                ok = False; break
            sym = syms[0]
            if re.search(r'(?<!&)\b%s\b' % re.escape(sym), body):
                ok = False; break
            fixed.append("extern u8 %s;" % sym)
        else:
            fixed.append(e)
    if not ok:
        continue
    # any remaining undeclared D_ in body?
    declared = set()
    for e in fixed:
        declared.update(re.findall(r'\bD_\w+', e))
    if set(re.findall(r'\bD_\w+', body)) - declared:
        continue
    cands[fn] = body
    for e in fixed:
        sym = re.findall(r'\bD_\w+', e)
        externs[sym[0] if sym else e] = e

spec = {"module": MOD, "externs": list(externs.values()), "funcs": cands}
json.dump(spec, open(OUT, "w"), indent=1)
print(f"{MOD}: {len(cands)}/{len(funcs)} candidates")
