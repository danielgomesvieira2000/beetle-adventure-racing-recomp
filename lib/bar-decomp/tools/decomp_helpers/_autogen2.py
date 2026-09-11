"""Auto-generate C for pure load/store/const/copy functions (no arithmetic, no
branches, no calls). Uses a tiny register value model. Bails (leaves a pragma)
if ANY instruction isn't in the understood set, so it only ever emits for a
well-defined safe class. The byte-exact build + bisect filter wrong guesses.

Usage: _autogen2.py <module> [--dry]
"""
import os, re, sys, json

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)

LOAD_CAST = {'lw': 's32', 'lh': 's16', 'lhu': 'u16', 'lb': 's8', 'lbu': 'u8'}
STORE_CAST = {'sw': 's32', 'sh': 's16', 'sb': 'u8'}

def parse_asm(path):
    insns = []
    for line in open(path):
        m = re.search(r'\*/\s+(\S+)\s*(.*?)\s*$', line)
        if m:
            insns.append((m.group(1), m.group(2).strip()))
    return insns

def argidx(r):
    m = re.match(r'\$a([0-3])$', r)
    return int(m.group(1)) if m else None

def split_mem(op):
    m = re.match(r'(-?0x[0-9A-Fa-f]+|0)\((\$\w+)\)$', op)
    if not m:
        return None
    off = int(m.group(1), 16) if m.group(1) != '0' else 0
    return off, m.group(2)

class Bail(Exception):
    pass

def gen(fn, insns):
    core = [(m, o) for m, o in insns if m != 'nop']
    if not core or 'jr' not in [m for m, o in core]:
        return None
    jri = [m for m, o in core].index('jr')
    if core[jri][1] != '$ra':
        return None
    work = [(m, o) for k, (m, o) in enumerate(core) if k != jri]

    val = {}            # reg -> C expr string
    stmts = []          # emitted statements in order
    nargs = -1
    ret = None          # ('int'|'float', expr)
    fv0_expr = None

    def base_expr(reg):
        ai = argidx(reg)
        if ai is None:
            raise Bail()
        return f"(u8 *)arg{ai} + "

    def memexpr(off, reg, cast):
        ai = argidx(reg)
        if ai is None:
            raise Bail()
        nonlocal nargs
        nargs = max(nargs, ai)
        sign = '-' if off < 0 else ''
        return f"*({cast} *)((u8 *)arg{ai} {('-' if off<0 else '+')} 0x{abs(off):X})"

    try:
        for mn, ops in work:
            if mn in ('lui', 'ori', 'addiu') and '%hi' in ops or '%lo' in ops:
                raise Bail()  # global symbol -> needs external decl, skip
            if mn == 'move':
                d, s = [x.strip() for x in ops.split(',')]
                if s == '$zero':
                    val[d] = '0'
                elif argidx(s) is not None:
                    nargs = max(nargs, argidx(s))
                    val[d] = f'arg{argidx(s)}'
                else:
                    val[d] = val.get(s)
                    if val[d] is None: raise Bail()
            elif mn == 'mtc1':
                s, d = [x.strip() for x in ops.split(',')]
                if s == '$zero':
                    val[d] = '0.0f'
                elif argidx(s) is not None:
                    nargs = max(nargs, argidx(s))
                    val[d] = f'arg{argidx(s)}'  # int bits -> float reg (rare)
                else:
                    raise Bail()
            elif mn == 'addiu':
                m = re.match(r'(\$\w+), \$zero, (-?0x[0-9A-Fa-f]+)$', ops)
                if not m: raise Bail()
                val[m.group(1)] = m.group(2)
            elif mn == 'li':
                m = re.match(r'(\$\w+), (-?0x[0-9A-Fa-f]+|-?\d+)$', ops)
                if not m: raise Bail()
                val[m.group(1)] = m.group(2)
            elif mn in LOAD_CAST:
                d, mem = [x.strip() for x in ops.split(',', 1)]
                pm = split_mem(mem.strip())
                if not pm: raise Bail()
                val[d] = memexpr(pm[0], pm[1], LOAD_CAST[mn])
            elif mn == 'lwc1':
                d, mem = [x.strip() for x in ops.split(',', 1)]
                pm = split_mem(mem.strip())
                if not pm: raise Bail()
                val[d] = memexpr(pm[0], pm[1], 'f32')
            elif mn in STORE_CAST:
                s, mem = [x.strip() for x in ops.split(',', 1)]
                pm = split_mem(mem.strip())
                if not pm: raise Bail()
                if argidx(s) is not None:
                    nargs = max(nargs, argidx(s))
                    sval = f'arg{argidx(s)}'
                elif s == '$zero':
                    sval = '0'
                else:
                    sval = val.get(s)
                    if sval is None: raise Bail()
                stmts.append(f"{memexpr(pm[0], pm[1], STORE_CAST[mn])} = {sval};")
            elif mn == 'swc1':
                s, mem = [x.strip() for x in ops.split(',', 1)]
                pm = split_mem(mem.strip())
                if not pm: raise Bail()
                sval = val.get(s)
                if sval is None: raise Bail()
                stmts.append(f"{memexpr(pm[0], pm[1], 'f32')} = {sval};")
            else:
                raise Bail()
    except Bail:
        return None

    # determine return
    intret = '$v0' in val
    floatret = any(r in val for r in ('$fv0',)) and not any('$fv0' in s for s in stmts)
    nparams = nargs + 1

    def paramlist():
        if nparams <= 0:
            return "void"
        ps = []
        for i in range(nparams):
            ps.append(f"void *arg{i}" if i == 0 else f"s32 arg{i}")
        return ", ".join(ps)

    if intret and not stmts:
        return f"s32 {fn}({paramlist()}) {{\n    return {val['$v0']};\n}}"
    if floatret and not stmts:
        return f"f32 {fn}({paramlist()}) {{\n    return {val['$fv0']};\n}}"
    if stmts and not intret:
        body = "\n".join("    " + s for s in stmts)
        return f"void {fn}({paramlist()}) {{\n{body}\n}}"
    if not stmts and not intret and not floatret:
        return f"void {fn}({paramlist()}) {{\n}}"
    return None  # mixed store+return: ambiguous, skip

def main():
    mod = sys.argv[1]
    c = f"src/modules/{mod}.c"
    txt = open(c).read()
    funcs = re.findall(r'GLOBAL_ASM\("asm/us/nonmatchings/modules/%s/(func_%s_[0-9A-Fa-f]+)\.s"\)' % (mod, mod), txt)
    out = {}
    for fn in funcs:
        p = f"asm/us/nonmatchings/modules/{mod}/{fn}.s"
        if not os.path.exists(p):
            continue
        code = gen(fn, parse_asm(p))
        if code:
            out[fn] = code
    if "--dry" in sys.argv:
        print(f"# {mod}: {len(out)}/{len(funcs)} recognized")
        for fn, code in out.items():
            print("\n" + code)
    else:
        print(json.dumps(out))

if __name__ == "__main__":
    main()
