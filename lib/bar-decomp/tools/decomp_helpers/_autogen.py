"""Auto-generate C for mechanically-recognizable IDO asm patterns.
Self-contained patterns only (need just common.h): empty leaves, constant
returns, simple pointer-param getters/setters (int + float). Anything not
recognized is left as a pragma. Generation need not be perfect -- the byte-exact
build gate + bisect filter out wrong guesses.

Usage: _autogen.py <module> [--dry]   (prints recognized candidates)
"""
import os, re, glob, sys, json

REPO = os.path.expanduser("~/projects/bar-decomp")
os.chdir(REPO)

def parse_asm(path):
    insns = []
    for line in open(path):
        m = re.search(r'\*/\s+(\S+)\s*(.*?)\s*$', line)
        if not m:
            continue
        mn, ops = m.group(1), m.group(2).strip()
        insns.append((mn, ops))
    return insns

# width -> (c cast type, return type for getter)
LOADS = {
    'lw':  ('s32', 's32'),
    'lh':  ('s16', 's32'),
    'lhu': ('u16', 's32'),
    'lb':  ('s8',  's32'),
    'lbu': ('u8',  's32'),
}
STORES = {
    'sw': 's32', 'sh': 's16', 'sb': 'u8',
}

def reg_argindex(r):
    m = re.match(r'\$a([0-3])$', r)
    return int(m.group(1)) if m else None

def split_mem(op):
    # "0x2C($a0)" -> (0x2C, "$a0")
    m = re.match(r'(-?0x[0-9A-Fa-f]+|0)\((\$\w+)\)$', op)
    if not m:
        return None
    off = int(m.group(1), 16) if m.group(1) != '0' else 0
    return off, m.group(2)

def args_used(insns):
    """highest a-register index referenced (to size the param list)."""
    hi = -1
    for mn, ops in insns:
        for r in re.findall(r'\$a([0-3])', ops):
            hi = max(hi, int(r))
    return hi

def gen(fn, insns):
    # strip leading nops; keep order. Work with (mn,ops).
    body = [(m, o) for (m, o) in insns]
    # remove standalone nops for pattern logic but track presence
    core = [(m, o) for (m, o) in body if m != 'nop']
    if not core:
        return None
    mns = [m for m, o in core]

    # locate jr $ra
    if 'jr' not in mns:
        return None
    jri = mns.index('jr')
    if core[jri][1] != '$ra':
        return None
    # instructions excluding the jr itself (delay slot insn may be after jr)
    work = [(m, o) for k, (m, o) in enumerate(core) if k != jri]

    nargs = args_used(core) + 1  # number of a-regs referenced (0..4)

    def params(kind_first_ptr=True, valtype=None):
        ps = []
        for i in range(nargs):
            if i == 0 and kind_first_ptr:
                ps.append("void *arg0")
            elif valtype and i == 1:
                ps.append(f"{valtype} arg1")
            else:
                ps.append(f"s32 arg{i}")
        return ", ".join(ps) if ps else "void"

    # ---- empty leaf ----
    if not work:
        return f"void {fn}({params(kind_first_ptr=False)}) {{\n}}"

    # ---- single meaningful instruction patterns ----
    if len(work) == 1:
        mn, ops = work[0]
        # const 0 return
        if mn == 'move' and ops == '$v0, $zero':
            return f"s32 {fn}({params(False)}) {{\n    return 0;\n}}"
        if mn in ('addiu', 'li') and re.match(r'\$v0, \$zero, (0x[0-9A-Fa-f]+)$', ops):
            v = re.match(r'\$v0, \$zero, (0x[0-9A-Fa-f]+)$', ops).group(1)
            return f"s32 {fn}({params(False)}) {{\n    return {v};\n}}"
        if mn == 'li' and re.match(r'\$v0, (0x[0-9A-Fa-f]+|-?\d+)$', ops):
            v = ops.split(', ')[1]
            return f"s32 {fn}({params(False)}) {{\n    return {v};\n}}"
        # float zero return
        if mn == 'mtc1' and ops == '$zero, $fv0':
            return f"f32 {fn}({params(False)}) {{\n    return 0.0f;\n}}"
        # int getter: load v0 from off(a0)
        if mn in LOADS:
            mm = re.match(r'\$v0, (.+)$', ops)
            if mm:
                mem = split_mem(mm.group(1))
                if mem and reg_argindex(mem[1]) is not None:
                    cast, ret = LOADS[mn]
                    off, reg = mem
                    ai = reg_argindex(reg)
                    return (f"{ret} {fn}({params()}) {{\n"
                            f"    return *({cast} *)((u8 *)arg{ai} + 0x{off:X});\n}}")
        # float getter
        if mn == 'lwc1':
            mm = re.match(r'\$\w+, (.+)$', ops)
            if mm:
                mem = split_mem(mm.group(1))
                if mem and reg_argindex(mem[1]) is not None:
                    off, reg = mem
                    ai = reg_argindex(reg)
                    return (f"f32 {fn}({params()}) {{\n"
                            f"    return *(f32 *)((u8 *)arg{ai} + 0x{off:X});\n}}")
        # int setter: store aN to off(a0)
        if mn in STORES:
            mm = re.match(r'(\$\w+), (.+)$', ops)
            if mm:
                src, memstr = mm.group(1), mm.group(2)
                mem = split_mem(memstr)
                si = reg_argindex(src)
                if mem and reg_argindex(mem[1]) is not None and si is not None:
                    cast = STORES[mn]
                    off, reg = mem
                    ai = reg_argindex(reg)
                    return (f"void {fn}({params(valtype='s32')}) {{\n"
                            f"    *({cast} *)((u8 *)arg{ai} + 0x{off:X}) = arg{si};\n}}")
    return None

def main():
    mod = sys.argv[1]
    c = f"src/modules/{mod}.c"
    txt = open(c).read()
    funcs = re.findall(r'GLOBAL_ASM\("asm/us/nonmatchings/modules/%s/(func_%s_[0-9A-Fa-f]+)\.s"\)' % (mod, mod), txt)
    out = {}
    for fn in funcs:
        path = f"asm/us/nonmatchings/modules/{mod}/{fn}.s"
        if not os.path.exists(path):
            continue
        insns = parse_asm(path)
        code = gen(fn, insns)
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
