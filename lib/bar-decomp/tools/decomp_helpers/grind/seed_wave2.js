export const meta = {
  name: 'bar-decomp-seed-wave2',
  description: 'Refined Sonnet writers: byte-matching C seeds with correct export shims + unique struct names',
  phases: [{ title: 'Seed', detail: 'one Sonnet writer per unmatched function' }],
}

const items = args.items
const CARDS = args.cardsDir
const HDRS = args.headersDir
const SEEDS = args.seedsDir

const GUIDE = `You are matching ONE MIPS function from Beetle Adventure Racing (N64) back to C. Success is OBJECTIVE:
your C compiled with IDO 5.3 -O2 must assemble to the TARGET ASM byte-for-byte (a DaisyBox module-hash gate).

READ FIRST:
- The card ${CARDS}/<card> -> TARGET ASM + an m2c first-pass seed (types are GUESSES; fix them).
- Headers in ${HDRS}/ : structs.h, functions.h, variables.h, global_exports.h, file_manager.h,
  mem_allocator.h, and the engine export structs uv*_rom.h. USE real types/field names/prototypes from these.

THE MODULE ONLY INCLUDES common.h (= ultra64 + structs.h + functions.h + variables.h + file_manager.h +
mem_allocator.h + macros.h). Anything else you reference MUST be declared in your seed's preamble.

=== ENGINE EXPORT CALLS (this is where most attempts fail — do it EXACTLY like this) ===
In the asm an export call is three ops:
    lw  $t7, %lo(gXxxExports)($..)   # load the export-table pointer (global from global_exports.h)
    lw  $t9, 0xNN($t7)               # load the function pointer at byte offset 0xNN
    jalr $t9                          # call
Reproduce it with an opaque shim + the REAL global name (from global_exports.h), member named unk<offset>:
    typedef struct { char pad[0xNN]; RET (*unkNN)(ARGTYPES); } XxxExp_<ADDR>;
    extern XxxExp_<ADDR> *gXxxExports;
    ...
    gXxxExports->unkNN(ARGS);
- Compute NN from the asm (the 0xNN in 'lw $t9,0xNN($t7)'). Use the EXACT global name that global_exports.h
  declares (e.g. gUvGfxMgrExports, gUvSprtExports, gUvFontExports, gSndExports, gUvContExports, gUvFvecExports,
  gUvEmitterExports, gUvCbckExports, gUvChanExports, gUvCmidiExports, gUvAudiomgrExports, gUvColorExports...).
- NEVER invent a member name like 'func_uvgfxmgr_rom_XXXX'. The member is always unk<hexoffset>. Args/ret from
  the asm register setup. (You may instead reuse the real uv*_rom.h struct if it already has that exact member.)

=== INFERRED STRUCTS: NAME THEM UNIQUELY ===
If you must define a struct whose layout you inferred from offsets, suffix its name with the function's hex
address so it never collides when many functions are combined into one file:
    typedef struct { /* 0x00 */ s32 unk0; /* 0x04 */ s16 unk4; ... } Node_<ADDR>;
(<ADDR> = the hex in the function name, e.g. 402F30.) Prefer real header types (UnkStruct_80025CF0 gGameSettings
fields, etc.) when they fit — only invent a struct when nothing in the headers matches.

=== MATCHING IDO C ===
- Start from the m2c control flow; fix every type (s32/u32/s16/u8/f32, typed pointers). Keep statement ORDER.
- Match immediates exactly (hex). Reading a global directly forces reload-after-call; caching in a local uses a
  saved reg — match whatever the prologue's saved-reg push/pops imply. branch-likely (bltzl/bnel/beql) comes
  from the loop/if shape m2c captured.
- Declare each sibling callee not in headers: 'extern RET name(ARGTYPES);' (arg types from the asm arg regs).

OUTPUT: Write ONLY C (no markdown, no #include) to ${SEEDS}/<fn>.c, in order: (1) your extern/typedef preamble,
(2) exactly one function definition named <fn>, layout-faithful, with a one-line purpose comment. ASCII only.`

phase('Seed')

const results = await parallel(items.map((it) => () => {
  const addr = it.fn.replace('func_' + it.mod + '_', '')
  return agent(
    `${GUIDE}\n\nTARGET: module="${it.mod}" function="${it.fn}" (ADDR=${addr}, size ${it.size} bytes).\n` +
    `Read ${CARDS}/${it.card} and the headers, then Write matching C to ${SEEDS}/${it.fn}.c.`,
    {
      label: `seed2:${it.mod}/${addr}`,
      phase: 'Seed', model: 'sonnet', effort: 'medium',
      schema: {
        type: 'object', additionalProperties: false,
        required: ['fn', 'wrote', 'note'],
        properties: {
          fn: { type: 'string' }, wrote: { type: 'boolean' }, note: { type: 'string' },
        },
      },
    }
  ).then(r => r || { fn: it.fn, wrote: false, note: 'agent died' })
}))

const wrote = results.filter(r => r && r.wrote).length
log(`seed wave 2: ${wrote}/${items.length} seeds written`)
return { total: items.length, wrote }
