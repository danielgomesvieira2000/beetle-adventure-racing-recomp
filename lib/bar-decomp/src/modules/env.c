// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 D_env_00400848[];
extern s32 D_env_00400870;
extern void *D_env_0040087C;
void func_env_004000B4();
void func_env_00400100();
void func_env_00400190();
void func_env_004001F4();
void func_env_0040033C();
void func_env_00400380();
void func_env_00400758();
void func_env_004007A4();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/env/__entrypoint_func_env_400000.s")

void func_env_004000AC(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/env/func_env_004000B4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/env/func_env_00400100.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/env/func_env_00400190.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/env/func_env_004001F4.s")

void func_env_0040033C(s32 arg0) {
    if (D_env_00400870 < 0xA && arg0 != 0) {
        D_env_00400848[D_env_00400870] = arg0;
        D_env_00400870 += 1;
    }
}

void func_env_00400378(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/env/func_env_00400380.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/env/func_env_00400758.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/env/func_env_004007A4.s")

