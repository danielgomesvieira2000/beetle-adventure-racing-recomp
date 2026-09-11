export const meta = {
  name: 'bar-decomp-seed-wave',
  description: 'Fan out Sonnet writers to produce byte-matching C seeds for HUD/menu functions',
  phases: [{ title: 'Seed', detail: 'one Sonnet writer per target function' }],
}

// args: { items:[{mod,fn,card,size}], cardsDir, headersDir, seedsDir }
const items = args.items
const CARDS = args.cardsDir
const HDRS = args.headersDir
const SEEDS = args.seedsDir

const GUIDE = `You are matching a single MIPS function from Beetle Adventure Racing (N64) back to C.
Success is OBJECTIVE: your C, compiled with IDO 5.3 -O2, must assemble to the TARGET ASM byte-for-byte.

INPUTS to READ:
- The card: ${CARDS}/<card>  -> has the TARGET ASM and an m2c first-pass seed (types are GUESSES).
- Headers in ${HDRS}/ -> structs.h, global_exports.h, functions.h, variables.h, mem_allocator.h,
  file_manager.h, snd.h, etc. USE the real types/prototypes/struct-field names from these instead of
  inventing. Export tables are pointers like gUvGfxMgrExports/gUvSprtExports/gUvFontExports/gSndExports
  (structs of function pointers, often typed with 'char pad[..]; ret (*unkNN)(args)').

HOW TO WRITE MATCHING IDO C:
- Start from the m2c seed's control flow, but FIX every type: replace '?'/void*/raw casts with the real
  struct types and signatures from the headers. Register-width params (s32/u32), typed pointers.
- Preserve statement ORDER and structure that reproduces the asm. Do not add temporaries or reorder ops.
- Match immediates EXACTLY (keep hex like 0x2D0, 0x55564D44). Float/double constants become .rodata.
- IDO regalloc cues: reading a GLOBAL directly (not caching in a local) forces a reload after a call
  (globals live in caller-saved regs across calls); caching in a local tends to use a callee-saved reg.
  Match whichever the asm shows (count saved-reg push/pops in the prologue). branch-likely (bltzl/bnel/
  beql) comes from specific loop/if shapes m2c usually captures.
- A struct field at +0xN accessed as s32 => 'sN' int field; lbu => u8; lwc1 => f32; etc.

OUTPUT: Write ONLY C to ${SEEDS}/<fn>.c  (no markdown fences, no prose). The file must contain, in order:
  1. any 'extern' declarations and 'typedef' struct definitions your function needs that are NOT already
     in the headers (declare a struct if you had to infer its layout; give real field types/offsets),
  2. the single function definition, exactly one, named <fn>, layout-faithful.
Keep it clean and idiomatic (named locals, a one-line purpose comment). ASCII only. Do NOT include headers
(#include) — the consolidator injects the function into a module that already includes common.h.`

phase('Seed')

const results = await parallel(items.map((it) => () =>
  agent(
    `${GUIDE}\n\nTARGET: module="${it.mod}" function="${it.fn}" (size ${it.size} bytes).\n` +
    `Read the card ${CARDS}/${it.card} and the headers, then Write your matching C to ${SEEDS}/${it.fn}.c.`,
    {
      label: `seed:${it.mod}/${it.fn.replace('func_' + it.mod + '_', '')}`,
      phase: 'Seed',
      model: 'sonnet',
      effort: 'medium',
      schema: {
        type: 'object', additionalProperties: false,
        required: ['fn', 'wrote', 'note'],
        properties: {
          fn: { type: 'string' },
          wrote: { type: 'boolean', description: 'true if you wrote the seed file' },
          note: { type: 'string', description: 'one line: approach / uncertainty / inferred structs' },
        },
      },
    }
  ).then(r => r || { fn: it.fn, wrote: false, note: 'agent died' })
))

const wrote = results.filter(r => r && r.wrote).length
log(`seed wave: ${wrote}/${items.length} seeds written`)
return { total: items.length, wrote, results }
