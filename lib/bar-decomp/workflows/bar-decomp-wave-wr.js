export const meta = {
  name: 'bar-decomp-wave-wr',
  description: 'Decomp wave write+review: Sonnet writes clean C per module, Opus reviews/corrects against the asm; returns verified C for the orchestrator to integrate',
  phases: [
    { title: 'Write', detail: 'Sonnet writer per module (parallel)' },
    { title: 'Review', detail: 'Opus reviews/corrects each module against the assembly (parallel)' },
  ],
}

const modules = (args && args.modules) || []
const maxInsns = (args && args.maxInsns) || 6
const writerModel = (args && args.writerModel) || 'sonnet'
if (!modules.length) { log('no modules'); return { error: 'no modules' } }

const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    module: { type: 'string' },
    addIncludes: { type: 'array', items: { type: 'string' } },
    addExterns: { type: 'array', items: { type: 'string' } },
    addStructs: { type: 'array', items: { type: 'string' } },
    addPrototypes: { type: 'array', items: { type: 'string' } },
    functions: { type: 'array', items: { type: 'object', additionalProperties: false,
      properties: { name: { type: 'string' }, code: { type: 'string' } }, required: ['name', 'code'] } },
    skipped: { type: 'array', items: { type: 'string' } },
    reviewNotes: { type: 'string' },
  },
  required: ['module', 'functions'],
}

const IDIOMS = [
  'IDO 7.1 matching idioms:',
  '- Parameters are register-width (s32/u32/void*); a NARROW param (u8/s16) makes IDO emit an arg spill `sw aN,k(sp)` with no stack frame. Only use a narrow param if the asm shows that exact spill; otherwise s32 with a narrow cast at the store.',
  '- load/store width<->cast: lw/sw=s32, lh/sh=s16, lhu=u16, lb=s8, lbu/sb=u8. The RETURN type and the CAST type must match the load/store instruction exactly.',
  '- empty leaf (jr ra; nop) = `void f(void){}`. (jr ra; move v0,zero) = `s32 f(void){return 0;}`.',
  '- global accessor: `lui+lw %hi/%lo(D_X)` => `return D_X;` with `extern <type> D_X;` (type from the load: lw=s32, lhu=u16...). `lui+addiu` => `return &D_X;`.',
  '- struct field on a pointer: `lw v0,0x2C(a0)` => `*(s32*)((u8*)p + 0x2C)` (use void* param). DO NOT invent a named struct.',
  '- module C files: add `#include "common.h"` only if absent. Source must be PURE ASCII (no em-dashes etc).',
  '- EXPORTS (critical): first cat include/global_exports.h. If a function uses an export global declared THERE (gUvFmtxExports, gUvContExports, gAiExports, gTdataExports, gUvDobjExports, gUvFontExports, gGameExports, gUvDGeomExports, gUvFvecExports, gUvMathExports, gUvGfxMgrExports, etc.), add #include "global_exports.h" and use it as-is. Do NOT redeclare it (declaring extern void* gXExports CONFLICTS with the typed declaration there and breaks ->member access).',
  '- For an export global NOT in global_exports.h (e.g. gSndExports, gUvGuiExports, gUvChanExports, gUvSprtExports, gUvEmitterExports), declare a LOCAL typedef whose called member sits at its offset, e.g.  typedef struct { char pad0[0x58]; void (*unk58)(void); } SndExports;  extern SndExports* gSndExports;  -- get the offset from the asm: lw $t,0xNN($export); jalr $t means the function pointer is at 0xNN. If a function references an export you cannot resolve this way, SKIP it.',
]

function writerPrompt(mod) {
  return [
    'Matching decompiler for Beetle Adventure Racing (N64, IDO 7.1). Module: ' + mod + '. STRICTLY READ-ONLY: do NOT use Edit/Write, do NOT modify any repo file, do NOT run make. ONLY read (cat/grep/m2c) and RETURN the C in the structured result. Read via WSL, e.g.:',
    "  wsl.exe -d Ubuntu-24.04 -- bash -lc 'cd ~/projects/bar-decomp && grep -n GLOBAL_ASM src/modules/" + mod + ".c'",
    'Then for the .s files: `cat asm/us/nonmatchings/modules/' + mod + '/<func>.s` (you can cat several at once).',
    '1. Each `#pragma GLOBAL_ASM(".../' + mod + '/<func>.s")` (skip `__entrypoint_*`) is unmatched.',
    '2. SELECT functions with <= ' + maxInsns + ' real instructions and NO floating point (.s/.d/lwc1/swc1/ldc1/sdc1/mtc1/mfc1/cvt/c.). Control flow (if/while/for/branches) is FINE. Skip float-using or oversized ones (list them in skipped).',
    '   Tip for non-trivial functions: seed with m2c then refine: `wsl.exe -d Ubuntu-24.04 -- bash -lc \"cd ~/projects/bar-decomp && python3 tools/m2c/m2c.py -t mips-ido-c asm/us/nonmatchings/modules/' + mod + '/<func>.s\"`',
    '3. READ THE SHARED KNOWLEDGE BASE and USE it: cat include/structs.h (shared/export struct types with named members), include/functions.h (real prototypes), include/global_exports.h (typed export globals), include/' + mod + '.h (if present), and src/modules/' + mod + '.c (existing decls). #include the headers you need. Do NOT invent a typedef or redeclare a global/function that already exists there (it conflicts and breaks the match).',
    '4. Translate each to CLEAN, byte-exact C:',
    IDIOMS.join('\n'),
    'COMMENT every function: a brief // purpose line above it, plus inline comments for non-obvious offsets/magic-numbers/inferred fields (ASCII only -- no em-dashes). Return: addIncludes (e.g. the structs.h/functions.h/global_exports.h/<module>.h you used), addExterns (only NEW), addStructs (NEW struct typedefs you CONFIRMED, for structs.h), addPrototypes (NEW function signatures you confirmed, for functions.h), functions [{name, commented C}], skipped. Precision matters: wrong width/sign or a needless narrow param will not match.',
  ].join('\n')
}

function reviewPrompt(mod, cand) {
  return [
    'You are an EXPERT IDO-7.1 matching reviewer (Beetle Adventure Racing, N64). A junior decompiler produced candidate C for module ' + mod + '. Your job: verify EACH function against its assembly and CORRECT any error, then return the corrected result. These candidates are NOT yet build-verified, so be rigorous.',
    'Read each function0s assembly: wsl.exe -d Ubuntu-24.04 -- bash -lc \'cd ~/projects/bar-decomp && cat asm/us/nonmatchings/modules/' + mod + '/<name>.s\'',
    'Check for the COMMON failure modes that break the byte-match:',
    '- Load/store WIDTH+SIGN vs C type: lw=s32, lh=s16, lhu=u16, lb=s8, lbu/sb=u8. A getter returning u16 must read a u16 global/field (lhu); if the asm is lhu but the type is s16, IDO emits lh and it FAILS. Fix the type.',
    '- Narrow PARAM spill: a param typed u8/s16 makes IDO spill it (`sw aN,k(sp)`). If the asm has NO such spill, the param MUST be register-width (s32) with a narrow cast at the store. If the asm DOES spill, the param must be the narrow type. Match exactly.',
    '- Return type vs the load that sets $v0. addiu (no load) = address-of => pointer return.',
    '- extern global types consistent with how every function uses them (a global read by lhu in one func and written by sh in another is u16, not s16).',
    '- Source must be PURE ASCII.',
    IDIOMS.join('\n'),
    'Candidate (JSON): ' + JSON.stringify(cand),
    'Also: ensure each kept function has a brief ASCII // purpose comment; verify any addStructs/addPrototypes are REAL (match the asm) and not duplicates of include/structs.h or include/functions.h. Return the SAME schema with corrected addExterns/addStructs/addPrototypes/functions (fix types/params/returns; drop any function you cannot make correct and add its name to skipped). One-line-per-fix summary in reviewNotes.',
  ].join('\n')
}

const reviewed = await pipeline(modules,
  function (mod) { return agent(writerPrompt(mod), { label: 'write:' + mod, model: writerModel, phase: 'Write', schema: SCHEMA }) },
  function (w, mod) {
    if (w && w.functions && w.functions.length) {
      return agent(reviewPrompt(mod, w), { label: 'review:' + mod, model: 'opus', phase: 'Review', schema: SCHEMA })
    }
    return w
  }
)
const out = reviewed.filter(function (r) { return r && r.functions && r.functions.length })
const totalFns = out.reduce(function (a, r) { return a + r.functions.length }, 0)
log('Wave write+review done: ' + out.length + ' modules with C, ' + totalFns + ' functions verified by Opus')
return { modules: out, totalFunctions: totalFns }