export const meta = {
  name: 'bar-decomp-wave',
  description: 'Parallel decomp wave: cheap models write clean matching C per module; one build verifies; failures reverted to keep repo byte-matching',
  phases: [
    { title: 'Decompile', detail: 'one writer agent per module (parallel, cheap model)' },
    { title: 'Integrate', detail: 'apply all, single build, verify per-module, revert failures' },
  ],
}

const modules = (args && args.modules) || []
const maxInsns = (args && args.maxInsns) || 6
const writerModel = (args && args.writerModel) || 'sonnet'
if (!modules.length) { log('no modules provided'); return { error: 'no modules' } }

const IDIOMS = [
  'MATCHING IDIOMS (IDO 7.1):',
  '- Parameters are register-width (s32/u32/void*) with narrow casts at point of use. A NARROW param (u8/s16) makes IDO emit an arg spill `sw aN,k(sp)` with no stack frame; only declare a narrow param if the asm actually shows that spill.',
  '- load/store width<->C cast: lw/sw=s32*, lh/sh=s16*, lhu=u16*, lb=s8*, lbu/sb=u8*.',
  '- empty leaf (jr ra; nop, size 0x8) = `void f(void){}`. (jr ra; move v0,zero) = `s32 f(void){return 0;}`.',
  '- global accessor: `lui+lw %hi/%lo(D_X)` => `return D_X;` (declare `extern <type> D_X;`). `lui+addiu` => `return &D_X;`.',
  '- struct field via pointer: `lw v0,0x2C(a0)` => `return *(s32*)((u8*)p + 0x2C);` (use a named struct if the file already has one).',
  '- module C files need `#include "common.h"` for s32/u8 types (add only if absent).',
  '- CLEAN CODE REQUIRED: typed externs / named structs over raw casts; simple readable bodies; keep D_/func_ names if meaning unknown.',
].join('\n')

const WSCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    module: { type: 'string' },
    addIncludes: { type: 'array', items: { type: 'string' } },
    addExterns: { type: 'array', items: { type: 'string' } },
    functions: { type: 'array', items: { type: 'object', additionalProperties: false,
      properties: { name: { type: 'string' }, code: { type: 'string' } }, required: ['name', 'code'] } },
    skipped: { type: 'array', items: { type: 'string' } },
  },
  required: ['module', 'functions'],
}

function writerPrompt(mod) {
  return [
    'You are a matching decompiler for Beetle Adventure Racing (N64, IDO 7.1). Target module: ' + mod + '.',
    'Operate READ-ONLY through WSL. Run simple shell commands, e.g.:',
    "  wsl.exe -d Ubuntu-24.04 -- bash -lc 'cd ~/projects/bar-decomp && grep GLOBAL_ASM src/modules/" + mod + ".c'",
    'Steps:',
    '1. Find unmatched functions: each `#pragma GLOBAL_ASM(\"asm/us/nonmatchings/modules/' + mod + '/<func>.s\")` in src/modules/' + mod + '.c is one. Skip the `__entrypoint_*` one.',
    '2. cat each .s. SELECT ONLY simple functions: <= ' + maxInsns + ' real instructions, NO floating point (no .s/.d/lwc1/swc1/ldc1/sdc1/mtc1/mfc1/cvt/c.), and no loops/multiple branches. Put the names you skip in "skipped".',
    '3. cat src/modules/' + mod + '.c to see existing includes / extern declarations / typedef structs. Do NOT redeclare anything already present.',
    '4. Translate each selected function to CLEAN, byte-exact C using these idioms:',
    IDIOMS,
    'Return the structured result: addIncludes (e.g. ["common.h"] only if not already in the file), addExterns (typed `extern <type> D_...;` lines, deduped, only NEW ones), and each function as {name, full C definition}. Do NOT edit files. Do NOT build. Be precise: wrong width/sign or a needless narrow param will fail to match.',
  ].join('\n')
}

const ISCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    matched: { type: 'array', items: { type: 'string' } },
    failed: { type: 'array', items: { type: 'string' } },
    functionsLanded: { type: 'number' },
    finalMatch: { type: 'boolean' },
    notes: { type: 'string' },
  },
  required: ['matched', 'failed', 'finalMatch'],
}

function integratePrompt(dataJson, mods) {
  return [
    'You integrate decompiled C into the Beetle Adventure Racing repo and verify a byte-matching build. Work via WSL: `wsl.exe -d Ubuntu-24.04 -- bash -s < /path/script.sh` for any multi-line script (avoid bash -lc for complex scripts - it mangles quotes).',
    'You are given writer results (JSON array, one per module): each has module, addIncludes, addExterns, functions[{name,code}].',
    'ALGORITHM (do exactly this):',
    'A. Write the JSON to a file on the Windows side and have a python script (run inside WSL) apply it: for each module result, open src/modules/<module>.c; if an addInclude is not already present, insert `#include \"<inc>\"` right after the first line (the SPDX line); append each addExtern that is not already present right after the includes; then for each function replace the line `#pragma GLOBAL_ASM(\"asm/us/nonmatchings/modules/<module>/<name>.s\")` with its code. Save.',
    'B. cd ~/projects/bar-decomp && source .venv/bin/activate && make -j6 > /tmp/wave_build.log 2>&1 .',
    'C. If the build FAILS to compile (grep -i \"error:\" /tmp/wave_build.log) or a module mismatches (grep -i \"MISMATCH\\|checkModuleHash\" /tmp/wave_build.log), identify which module(s) are at fault from the file paths in the errors. For EACH faulty module run `git checkout -- src/modules/<module>.c` to revert it (this puts back its GLOBAL_ASM pragmas; the .s files still exist so it will build). Add it to "failed". Re-run make. Repeat until the build is green.',
    'D. Once `make` is green, verify the ROM: `sha1sum build/beetleadventurerac.us.z64` must equal `e5ab4d226c08d22f68a2edcc48870203e67454b8`.',
    'E. For each module that survived (in "matched"), delete its now-matched .s files: for every function you landed, `rm -f asm/us/nonmatchings/modules/<module>/<name>.s`. Then run make once more and re-check the SHA-1 is still matching.',
    'F. Count functions actually landed (in matched modules) into functionsLanded.',
    'Target modules: ' + JSON.stringify(mods) + '.',
    'Writer results JSON:',
    dataJson,
    'Return: matched (module names that compiled+matched and were kept), failed (reverted modules), functionsLanded, finalMatch (true only if the final ROM SHA-1 equals e5ab4d22...), and notes (what failed and why, briefly). Do NOT commit.',
  ].join('\n')
}

phase('Decompile')
const results = (await parallel(modules.map(function (m) {
  return function () { return agent(writerPrompt(m), { label: 'write:' + m, model: writerModel, phase: 'Decompile', schema: WSCHEMA }) }
}))).filter(Boolean)
const totalFns = results.reduce(function (a, r) { return a + ((r.functions && r.functions.length) || 0) }, 0)
log('writers returned ' + results.length + '/' + modules.length + ' modules, ' + totalFns + ' candidate functions')

phase('Integrate')
const integ = await agent(integratePrompt(JSON.stringify(results), modules), { label: 'integrate', model: 'sonnet', phase: 'Integrate', schema: ISCHEMA })
log('integrate: matched=' + JSON.stringify(integ.matched) + ' failed=' + JSON.stringify(integ.failed) + ' landed=' + (integ.functionsLanded || 0) + ' finalMatch=' + integ.finalMatch)

return { matched: integ.matched, failed: integ.failed, functionsLanded: integ.functionsLanded || 0, finalMatch: integ.finalMatch, notes: integ.notes }