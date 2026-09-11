// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
void func_smack_004000E8();
void func_smack_00400290();
void func_smack_0040038C();
void func_smack_0040062C();
void func_smack_00400780();
void func_smack_00400824();
void func_smack_0040095C();
void func_smack_0040097C();
void func_smack_00400E00();
void func_smack_00400FB4();
void func_smack_00401240();
void func_smack_00401540();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/__entrypoint_func_smack_400000.s")

void func_smack_004000E0(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_004000E8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_00400290.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_0040038C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_0040062C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_00400780.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_00400824.s")

void func_smack_0040095C(void *arg0) {
    if (*(u8 *)((u8 *)arg0 + 0x39) != 0) {
        *(s32 *)((u8 *)arg0 + 0x2C) = -1;
        *(u8 *)((u8 *)arg0 + 0x39) = 0;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_0040097C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_00400E00.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_00400FB4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_00401240.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/smack/func_smack_00401540.s")

