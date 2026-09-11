// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
void func_uvquery_rom_00400294();
s32 func_uvquery_rom_004004CC();
void func_uvquery_rom_004004F4();
void func_uvquery_rom_00400558();

// TODO(cleanup): func_uvquery_rom_00400200/0040020C/00400218/00400288 return s32 but the
// globals (D_..630/634/638/644) are void* pointers freed via _uvMemFree - retype these getters
// void* in the polish pass so callers aren't misled.

extern void *D_uvquery_rom_00400630;
extern void *D_uvquery_rom_00400634;
extern void *D_uvquery_rom_00400638;
extern void *D_uvquery_rom_00400644;
extern void *D_uvquery_rom_0040064C;
extern s32 D_uvquery_rom_0040063C;
extern s32 D_uvquery_rom_00400640;
extern s32 D_uvquery_rom_00400620;
extern s32 D_uvquery_rom_00400650;
extern s32 D_uvquery_rom_00400654;
extern s32 D_uvquery_rom_0040065C;
extern s32 D_uvquery_rom_00400658;
extern u16 D_uvquery_rom_00400660;

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvquery_rom/__entrypoint_func_uvquery_rom_400000.s")

void func_uvquery_rom_004001AC(void) {
    _uvMemFree(D_uvquery_rom_00400630);
    _uvMemFree(D_uvquery_rom_00400634);
    _uvMemFree(D_uvquery_rom_00400638);
    _uvMemFree(D_uvquery_rom_00400644);
    _uvMemFree(D_uvquery_rom_0040064C);
}

s32 func_uvquery_rom_00400200(void) {
    return D_uvquery_rom_00400634;
}

s32 func_uvquery_rom_0040020C(void) {
    return D_uvquery_rom_00400630;
}

s32 func_uvquery_rom_00400218(void) {
    return D_uvquery_rom_00400638;
}

s32 func_uvquery_rom_00400224(void) {
    if (D_uvquery_rom_00400650 != 0) {
        return (D_uvquery_rom_00400654 < D_uvquery_rom_00400620) ? D_uvquery_rom_00400654 : D_uvquery_rom_00400620;
    }
    return D_uvquery_rom_00400620;
}


s32 *func_uvquery_rom_00400270(void) {
    return &D_uvquery_rom_0040063C;
}

s32 *func_uvquery_rom_0040027C(void) {
    return &D_uvquery_rom_00400640;
}

s32 func_uvquery_rom_00400288(void) {
    return D_uvquery_rom_00400644;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvquery_rom/func_uvquery_rom_00400294.s")

s32 func_uvquery_rom_004004CC(void) {
    if (D_uvquery_rom_00400650 != 0) {
        return D_uvquery_rom_00400650;
    }
    return (s32)D_uvquery_rom_0040064C;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvquery_rom/func_uvquery_rom_004004F4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvquery_rom/func_uvquery_rom_00400558.s")

void func_uvquery_rom_004005C0(s32 a0, s32 a1) {
    D_uvquery_rom_00400650 = a0;
    D_uvquery_rom_00400654 = a1;
}

void func_uvquery_rom_004005D4(s32 a0, s16 a1) {
    D_uvquery_rom_0040065C = a0;
    D_uvquery_rom_00400660 = a1;
}

s32 func_uvquery_rom_004005EC(void) {
    return D_uvquery_rom_0040065C;
}

s32 *func_uvquery_rom_004005F8(void) {
    return &D_uvquery_rom_00400658;
}

s32 func_uvquery_rom_00400604(void) {
    return D_uvquery_rom_00400658;
}

u16 func_uvquery_rom_00400610(void) {
    return D_uvquery_rom_00400660;
}

