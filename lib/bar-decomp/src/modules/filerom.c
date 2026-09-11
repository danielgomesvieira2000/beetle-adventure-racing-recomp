// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 D_filerom_00400624;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/filerom/__entrypoint_func_filerom_400000.s")

void func_filerom_00400060(void) {
}

s32 func_filerom_00400068(u8 a0, u8 a1, u8 a2) {
    return 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/filerom/func_filerom_0040007C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/filerom/func_filerom_00400230.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/filerom/func_filerom_00400270.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/filerom/func_filerom_00400308.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/filerom/func_filerom_00400364.s")

s32 func_filerom_00400494(s32 arg0) {
    if (D_filerom_00400624 == 0) {
        return -1;
    }
    D_filerom_00400624 = 0;
    return 0;
}

