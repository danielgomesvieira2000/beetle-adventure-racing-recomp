export const meta = {
  name: 'bar-decomp-fix-wave',
  description: 'Fix compile errors in byte-match seeds (given the exact compiler output)',
  phases: [{ title: 'Fix', detail: 'one writer per BUILDERR function' }],
}
const items = args.items      // [{mod, fn}]
const FIX = args.fixDir
const HDRS = args.headersDir
const SEEDS = args.seedsDir

const GUIDE = `You previously wrote C for a Beetle Adventure Racing function that FAILS TO COMPILE. Fix it so it
(a) compiles under IDO 5.3 and (b) STILL assembles to the target asm byte-for-byte (DaisyBox hash gate).

READ: the fix-card ${FIX}/<fn>.txt = your broken C + the EXACT compiler errors + the target asm. Also the
headers in ${HDRS}/ (structs.h, global_exports.h, functions.h, variables.h, uv*_rom.h, ...).

The module only includes common.h (structs.h/functions.h/variables.h/file_manager.h/mem_allocator.h/macros.h).
FIX EACH compiler error. The usual causes and their fixes:
- "'gXxxExports' undeclared": you used an engine export table without declaring it. Add
    typedef struct { char pad[0xNN]; RET (*unkNN)(ARGS); } XxxExp_<ADDR>;  extern XxxExp_<ADDR> *gXxxExports;
  where 0xNN is the offset from the asm 'lw $t9,0xNN($t7)', member is unk<offset>, and gXxxExports is the
  EXACT name from global_exports.h. NEVER name a member 'func_uvgfxmgr_rom_XXXX' — always unk<hexoffset>.
- "conflicting types for 'gGameSettings'" (or any symbol already in a header): DELETE your redeclaration of
  it; common.h already declares it. Use the header's type (e.g. gGameSettings is UnkStruct_80025CF0[]).
- "has no member named 'unkXX'": add that member at the right offset to your struct, or fix the offset.
- "conflicting types for 'func_...'": make your extern proto match how it's really called (arg widths).
- "unknown type name 's32'": you must NOT emit #include lines; put decls after (they inject into common.h scope).
Keep statement order/immediates identical so it still byte-matches. Give inferred structs unique names
(suffix the function's hex address). ASCII, no markdown, no #include.

OUTPUT: Write the corrected full C (preamble decls + the one function) to ${SEEDS}/<fn>.c (overwrite).`

phase('Fix')
const results = await parallel(items.map((it) => () => {
  const addr = it.fn.replace('func_' + it.mod + '_', '')
  return agent(
    `${GUIDE}\n\nFIX TARGET: module="${it.mod}" function="${it.fn}" (ADDR=${addr}).\n` +
    `Read ${FIX}/${it.fn}.txt and the headers, then Write corrected C to ${SEEDS}/${it.fn}.c.`,
    { label: `fix:${it.mod}/${addr}`, phase: 'Fix', model: 'sonnet', effort: 'medium',
      schema: { type: 'object', additionalProperties: false, required: ['fn','fixed','note'],
        properties: { fn:{type:'string'}, fixed:{type:'boolean'}, note:{type:'string'} } } }
  ).then(r => r || { fn: it.fn, fixed: false, note: 'died' })
}))
const fixed = results.filter(r => r && r.fixed).length
log(`fix wave: ${fixed}/${items.length} rewritten`)
return { total: items.length, fixed }
