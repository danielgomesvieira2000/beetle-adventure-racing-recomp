// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
u8 * func_dledit_00400098();
void func_dledit_004000C8();
void func_dledit_00400100();
void func_dledit_00400174();
void func_dledit_004001F8();
void func_dledit_0040030C();
void func_dledit_004003D4();
s32 * func_dledit_004004FC();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/dledit/__entrypoint_func_dledit_400000.s")

void func_dledit_00400090(void) {
}

u8 *func_dledit_00400098(u8 *a0, s32 a1) {
    s32 temp;

loop:
    temp = *a0;
    if (temp == a1) {
        return a0;
    }
    a0 += 8;
    if (((temp << 24) >> 24) == 0xDF) {
        return NULL;
    }
    goto loop;
}

void func_dledit_004000C8(u8 *a0, s32 a1) {
    u8 *temp;

    temp = func_dledit_00400098(a0, 0xFD);
    if (temp != NULL) {
        *(s32 *)(temp + 4) = (s32)((u32)a1 + 0x80000000U);
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/dledit/func_dledit_00400100.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/dledit/func_dledit_00400174.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/dledit/func_dledit_004001F8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/dledit/func_dledit_0040030C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/dledit/func_dledit_004003D4.s")

s32 *func_dledit_004004FC(s32 *a0, s32 a1) {
    s32 temp;
    s32 word;

loop:
    temp = *(u8 *)a0;
    word = *a0;
    if (word == a1) {
        return a0;
    }
    a0 += 2;
    if (((temp << 24) >> 24) == 0xDF) {
        return NULL;
    }
    goto loop;
}

