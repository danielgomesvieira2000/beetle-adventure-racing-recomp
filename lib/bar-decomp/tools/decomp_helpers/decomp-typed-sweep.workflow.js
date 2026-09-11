export const meta = {
  name: 'decomp-typed-pilot',
  description: 'Typed/named/commented decomp seeds for pilot modules uvpfx_rom/uvdobj_rom/caraudio (75 funcs)',
  phases: [{ title: 'Seed', detail: 'parallel agents write typed/commented seeds from asm cards' }],
}

const work = Array.isArray(args) ? args : []
const funcs = work.map(w => String(w).trim().split(/\s+/)[1]).filter(Boolean)
const N = 8
const chunks = []
for (let i = 0; i < N; i++) chunks.push([])
funcs.forEach((fn, i) => { chunks[i % N].push(fn) })

const CARD = 'C:/Users/Bryan/Projects/BeetleAdventureDecomp/p2cards'
const OUT = 'C:/Users/Bryan/Projects/BeetleAdventureDecomp/seeds_out'

function prompt(list) {
  return [
    'You are decompiling Beetle Adventure Racing! (N64), compiler IDO 7.1 at -O2. Produce C that IDO compiles byte-identical to a target AND reads cleanly. A byte-exact gate verifies each function later.',
    '',
    'For EACH function listed at the end:',
    '1. Read its card with the Read tool: ' + CARD + '/<FUNC>.txt  (authoritative target MIPS asm + an m2c hint that is OFTEN WRONG on types/widths).',
    '2. Reference named types from ' + CARD + '/headers/structs.h and ' + CARD + '/headers/global_exports.h. Use a named struct/field when the access pattern matches. Key ones: UnkStruct_80025CF0 = game settings/state (optionsMusicVol@0x18, optionsSfxVol@0x1C, optionsSpeechVol@0x20, numPlayers@0x24, numAiCars@0x28, currentTrack@0x44, initFlag@0x84, pauseFlag@0x86, raceState@0x88, currentGameState@0xA4); UnkDobjExports = dobj export table (unk18/unk24/unk40/unk58); CarInfo = {s16 currentCar; s16 currentColor; u8 transmissionType}.',
    '2b. ENGINE CALLS -- use the EXACT prototype arity, never guess. Read ' + CARD + '/headers/file_manager.h, mem_allocator.h, uv_filesystem.h, functions.h. Known: void *uvGetLoadedFile(s32 formFileTag, s32 fileId); void *uvLoadFile(s32 formFileTag, s32 fileId); s32 uvGetFilesCount(s32 tag); void *_uvMemAllocAlign8(u32 size); void _uvMemFree(void *ptr); s32 uvUnloadModule(s32 tag). For any other uv*/_uv*/sibling-func call, Read its prototype/definition first. If a function returns void, NEVER use its result as a value.',
    '3. Write the result with the Write tool to: ' + OUT + '/<FUNC>.c  (PURE C only: D_ externs then the function definition; no markdown fences, no prose).',
    '',
    'MATCHING (required for byte-match):',
    '- Params register-width (s32/u32/void*); narrow-cast at the point of use (declaring u8/s16 params spills and mismatches).',
    '- Load/store width+sign dictates the cast: lw->s32* lh->s16* lhu->u16* lb->s8* lbu->u8* sw->s32* sh->s16* sb->u8* lwc1->f32*.',
    '- Typed export globals (gSndExports, gUvEmitterExports, gUvFmtxExports, gUvFvecExports, gGameExports/gGameSettings, ...) are POINTERS used DIRECTLY (never &, never re-declare them extern). If structs.h types it, use the named field; otherwise raw-cast.',
    '- Commutative float ops (add.s/mul.s) emit operands in the REVERSE of C source order; if a vec add/FMA mismatches, swap the source operands.',
    '- Loops mirror the asm; a bottom-tested loop is: i = 0; do { ... } while (cond);  (keep i=0; and do{ on their own lines).',
    '- Calls go directly to the named target; declare externs ONLY for D_ DATA symbols, NEVER function prototypes.',
    '- Pure ASCII.',
    '',
    'READABILITY (required -- a real decompilation, not just bytes):',
    '- Name locals by purpose; do NOT leave var_v0/temp_a1 names. Use struct field names where a type applies.',
    '- One single-line comment directly above the function summarizing what it does.',
    '- For any raw-cast export call, add a short inline comment, e.g.  (*(void (**)(s32))((u8 *)gSndExports + 0x40))(vol); // sound: set music volume',
    '- Do not reformat in a way that breaks matching (keep loop init+head adjacency).',
    '',
    'EFFICIENCY: do NOT run builds, WSL, shell, or any verification commands -- just Read the card + the headers and Write the seed file (a downstream byte-exact gate verifies). OVERWRITE any existing file at the target path; never skip a function because its file already exists.',
    '',
    'After writing all your files, reply ONLY with the count and list of filenames written (do NOT paste the C).',
    '',
    'Your functions (' + list.length + '):',
    list.join('\n'),
  ].join('\n')
}

phase('Seed')
const results = await parallel(
  chunks.filter(c => c.length).map((c, i) => () =>
    agent(prompt(c), { label: 'seed:chunk' + (i + 1), phase: 'Seed' })
  )
)
return {
  chunks: chunks.filter(c => c.length).length,
  funcs: funcs.length,
  summaries: results.map((r, i) => ({ chunk: i + 1, ok: !!r, head: (r || '(null)').slice(0, 160) })),
}