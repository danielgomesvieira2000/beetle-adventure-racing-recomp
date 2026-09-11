export const meta = {
  name: 'bar-decomp-fix-wave-opus',
  description: 'Fix compile errors in byte-match seeds (given the exact compiler output)',
  phases: [{ title: 'Fix', detail: 'one writer per BUILDERR function' }],
}
const items = [{"mod": "pause", "fn": "func_pause_00402470"}, {"mod": "pause", "fn": "func_pause_00402854"}, {"mod": "med", "fn": "func_med_004000DC"}, {"mod": "med", "fn": "func_med_004052C8"}, {"mod": "med", "fn": "func_med_00405338"}, {"mod": "menuslct", "fn": "func_menuslct_004010C4"}, {"mod": "menuslct", "fn": "func_menuslct_004011B0"}, {"mod": "menuslct", "fn": "func_menuslct_0040126C"}, {"mod": "menuslct", "fn": "func_menuslct_00401320"}, {"mod": "scene", "fn": "func_scene_00400188"}, {"mod": "letter", "fn": "func_letter_00400140"}, {"mod": "scrn", "fn": "func_scrn_00400398"}, {"mod": "cam", "fn": "func_cam_00402158"}, {"mod": "battle", "fn": "func_battle_004006A4"}, {"mod": "battle", "fn": "func_battle_00402750"}, {"mod": "battle", "fn": "func_battle_00404438"}, {"mod": "battle", "fn": "func_battle_00404580"}, {"mod": "battle", "fn": "func_battle_00405A1C"}, {"mod": "battle", "fn": "func_battle_00405C3C"}, {"mod": "battle", "fn": "func_battle_00405D34"}, {"mod": "battle", "fn": "func_battle_00406784"}, {"mod": "battle", "fn": "func_battle_00406804"}, {"mod": "battle", "fn": "func_battle_00407254"}, {"mod": "battle", "fn": "func_battle_00407DC4"}, {"mod": "battle", "fn": "func_battle_00407FF4"}, {"mod": "battle", "fn": "func_battle_0040807C"}, {"mod": "battle", "fn": "func_battle_004090F0"}, {"mod": "battle", "fn": "func_battle_00409314"}, {"mod": "battle", "fn": "func_battle_00409838"}, {"mod": "battle", "fn": "func_battle_00409910"}, {"mod": "selection", "fn": "func_selection_004015D4"}, {"mod": "selection", "fn": "func_selection_00401BF4"}, {"mod": "selection", "fn": "func_selection_00403938"}, {"mod": "selection", "fn": "func_selection_00404888"}, {"mod": "selection", "fn": "func_selection_00405E38"}, {"mod": "selection", "fn": "func_selection_0040611C"}, {"mod": "selection", "fn": "func_selection_00408B24"}, {"mod": "selection", "fn": "func_selection_0040A778"}, {"mod": "selection", "fn": "func_selection_0040BEC8"}, {"mod": "selection", "fn": "func_selection_0040F64C"}, {"mod": "selection", "fn": "func_selection_00410AA8"}, {"mod": "selection", "fn": "func_selection_00413418"}, {"mod": "selection", "fn": "func_selection_00413594"}, {"mod": "selection", "fn": "func_selection_00413E48"}, {"mod": "selection", "fn": "func_selection_00415CA0"}, {"mod": "selection", "fn": "func_selection_00415D48"}, {"mod": "selection", "fn": "func_selection_00415DEC"}, {"mod": "selection", "fn": "func_selection_00415E80"}, {"mod": "selection", "fn": "func_selection_00415EF4"}, {"mod": "selection", "fn": "func_selection_00415FB4"}, {"mod": "selection", "fn": "func_selection_00416A50"}, {"mod": "selection", "fn": "func_selection_00419038"}, {"mod": "selection", "fn": "func_selection_00419084"}, {"mod": "selection", "fn": "func_selection_00419FB8"}, {"mod": "gamegui", "fn": "func_gamegui_004000B0"}, {"mod": "gamegui", "fn": "func_gamegui_00401D34"}, {"mod": "gamegui", "fn": "func_gamegui_004023BC"}]
const FIX = "C:/Users/Bryan/Projects/BeetleRecomp/.grind/fixcards"
const HDRS = "C:/Users/Bryan/Projects/BeetleRecomp/.grind/headers"
const SEEDS = "C:/Users/Bryan/Projects/BeetleRecomp/.grind/seeds"

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
    { label: `fixO:${it.mod}/${addr}`, phase: 'Fix', model: 'opus', effort: 'high',
      schema: { type: 'object', additionalProperties: false, required: ['fn','fixed','note'],
        properties: { fn:{type:'string'}, fixed:{type:'boolean'}, note:{type:'string'} } } }
  ).then(r => r || { fn: it.fn, fixed: false, note: 'died' })
}))
const fixed = results.filter(r => r && r.fixed).length
log(`fix wave: ${fixed}/${items.length} rewritten`)
return { total: items.length, fixed }
