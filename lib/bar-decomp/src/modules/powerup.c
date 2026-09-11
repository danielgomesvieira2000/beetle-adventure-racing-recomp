// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 D_powerup_004003C0[][5];
void func_powerup_004000F0();
void func_powerup_00400180();
void * func_powerup_00400330();
void func_powerup_00400368();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/powerup/__entrypoint_func_powerup_400000.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/powerup/func_powerup_004000F0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/powerup/func_powerup_00400180.s")

void *func_powerup_00400330(s32 arg0)
{
    if (arg0 < 0 || arg0 >= 0xD) {
        return (void *)D_powerup_004003C0;
    }
    return (void *)D_powerup_004003C0[arg0];
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/powerup/func_powerup_00400368.s")

