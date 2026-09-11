// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
void func_splash_00400000();
void func_splash_0040020C();
void func_splash_004005F8();
void func_splash_00400828();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/splash/func_splash_00400000.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/splash/func_splash_0040020C.s")

void func_splash_004005F8(void) {
    uvUnloadModule('ripl');
    uvUnloadFile('UVTX', 0x79);
    uvUnloadFile('UVTX', 0x7A);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/splash/__entrypoint_func_splash_40063c.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/splash/func_splash_00400828.s")
