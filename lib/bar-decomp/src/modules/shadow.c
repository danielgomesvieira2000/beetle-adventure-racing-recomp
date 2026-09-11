// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 gDebugOptsCarShadow;
void func_shadow_00400108();
void func_shadow_00400168();
void func_shadow_00400180();
void func_shadow_00400460();
void func_shadow_00400498();
void func_shadow_00400750();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/shadow/__entrypoint_func_shadow_400000.s")

void func_shadow_00400100(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/shadow/func_shadow_00400108.s")

void func_shadow_00400168(void *a0) {
    if (a0 != NULL) {
        *(s32 *)((u8 *)a0 + 0x84) = -1;
        *(u8 *)((u8 *)a0 + 0xD4) = 0;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/shadow/func_shadow_00400180.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/shadow/func_shadow_00400460.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/shadow/func_shadow_00400498.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/shadow/func_shadow_00400750.s")

