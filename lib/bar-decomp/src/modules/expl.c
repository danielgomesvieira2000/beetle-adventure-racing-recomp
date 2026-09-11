// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
typedef struct UnkStruct_80025C78_s {
    char pad0[0x70];
    void (*unk70)(void *);
} UnkStruct_80025C78;
extern UnkStruct_80025C78 *gSndExports;
extern s32 D_expl_00400648;
void func_expl_004000AC();
void func_expl_004000E0();
void func_expl_0040011C();
void func_expl_004002D0();
void func_expl_00400370();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/expl/__entrypoint_func_expl_400000.s")

void func_expl_004000AC(void) {
    gSndExports->unk70(&D_expl_00400648);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/expl/func_expl_004000E0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/expl/func_expl_0040011C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/expl/func_expl_004002D0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/expl/func_expl_00400370.s")

