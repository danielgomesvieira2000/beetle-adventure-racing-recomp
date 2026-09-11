// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern void *D_sparks_00400454;
void func_sparks_00400160();
void func_sparks_00400194();
void func_sparks_004001EC();
void func_sparks_0040027C();
void func_sparks_00400360();
void func_sparks_00400400();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/sparks/__entrypoint_func_sparks_400000.s")

void func_sparks_00400160(void) {
    uvUnloadFile('UVMD', 0x47);
    _uvMemFree(D_sparks_00400454);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/sparks/func_sparks_00400194.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/sparks/func_sparks_004001EC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/sparks/func_sparks_0040027C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/sparks/func_sparks_00400360.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/sparks/func_sparks_00400400.s")

