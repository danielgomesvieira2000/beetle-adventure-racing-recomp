// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 D_uvmodel_rom_00403130;
extern s32 D_uvmodel_rom_00403160;
void func_uvmodel_rom_004002BC();
void func_uvmodel_rom_0040031C(s32 arg0);
void func_uvmodel_rom_00400324(s32 arg0, s32 arg1);
void func_uvmodel_rom_00400330(s32 arg0, s32 arg1, s32 arg2);
s32 * func_uvmodel_rom_00400340();
void func_uvmodel_rom_004003A4();
void func_uvmodel_rom_0040045C();
void func_uvmodel_rom_00400608();
void func_uvmodel_rom_004006B4();
void func_uvmodel_rom_0040199C();
void func_uvmodel_rom_0040215C();
s32 func_uvmodel_rom_00402224(f32 arg0, f32 arg1, f32 arg2, void *arg3);
void func_uvmodel_rom_004022E4();
void func_uvmodel_rom_00402AD0();
s32 func_uvmodel_rom_00402AE0();
void func_uvmodel_rom_00402AFC();
void func_uvmodel_rom_00402B98();
void func_uvmodel_rom_00402BB8();
void func_uvmodel_rom_00402CEC();
void func_uvmodel_rom_00402E50();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/__entrypoint_func_uvmodel_rom_400000.s")

void func_uvmodel_rom_004002BC(void) {
    _uvMemFree((void *)D_uvmodel_rom_00403160);
    uvUnloadModule(0x464D5458);
    uvUnloadModule(0x46564543);
    uvUnloadModule(0x51455259);
    uvUnloadModule(0x49534354);
    uvUnloadModule(0x4D415448);
}

void func_uvmodel_rom_0040031C(s32 arg0) {

}

void func_uvmodel_rom_00400324(s32 arg0, s32 arg1) {
    s32 sp0;
    s32 sp4;
    sp0 = arg0;
    sp4 = arg1;
}

void func_uvmodel_rom_00400330(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp0;
    s32 sp4;
    s32 sp8;
    sp0 = arg0;
    sp4 = arg1;
    sp8 = arg2;
}

s32 *func_uvmodel_rom_00400340(s32 arg0) {
    void *temp_v0;

    temp_v0 = uvGetLoadedFile(0x55564D44, arg0);
    if (temp_v0 == NULL) {
        return NULL;
    }
    if (*(u8 *)(*(s32 *)*(s32 *)temp_v0 + 4) == 0) {
        return NULL;
    }
    return (s32 *)(*(s32 *)*(s32 *)*(s32 *)temp_v0 + 4);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/func_uvmodel_rom_004003A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/func_uvmodel_rom_0040045C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/func_uvmodel_rom_00400608.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/func_uvmodel_rom_004006B4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/func_uvmodel_rom_0040199C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/func_uvmodel_rom_0040215C.s")

s32 func_uvmodel_rom_00402224(f32 arg0, f32 arg1, f32 arg2, void *arg3) {
    if (arg0 < (*(f32 *)((u8 *)arg3 + 0x0))) {
        return 0;
    }
    if ((*(f32 *)((u8 *)arg3 + 0xC)) < arg0) {
        return 0;
    }
    if (arg1 < (*(f32 *)((u8 *)arg3 + 0x4))) {
        return 0;
    }
    if ((*(f32 *)((u8 *)arg3 + 0x10)) < arg1) {
        return 0;
    }
    if (arg2 < (*(f32 *)((u8 *)arg3 + 0x8))) {
        return 0;
    }
    if ((*(f32 *)((u8 *)arg3 + 0x14)) < arg2) {
        return 0;
    }
    return 1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/func_uvmodel_rom_004022E4.s")

void func_uvmodel_rom_00402AD0(void) {
    D_uvmodel_rom_00403130 = -1;
}

s32 func_uvmodel_rom_00402AE0(void) {
    return (D_uvmodel_rom_00403130 << 6) + D_uvmodel_rom_00403160;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/func_uvmodel_rom_00402AFC.s")

void func_uvmodel_rom_00402B98(void) {
    if (D_uvmodel_rom_00403130 >= 0) {
        D_uvmodel_rom_00403130 -= 1;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/func_uvmodel_rom_00402BB8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmodel_rom/func_uvmodel_rom_00402CEC.s")

void func_uvmodel_rom_00402E50(s32 arg0, s32 arg1) {
    void *temp_v0;
    void *temp_v1;

    temp_v0 = uvGetLoadedFile(0x55564D44, arg0);
    if (temp_v0 == NULL) {
        return;
    }
    temp_v1 = *(void **)((u8 *)temp_v0 + 0x20);
    if (temp_v1 == NULL) {
        return;
    }
    *(s32 *)((u8 *)temp_v1 + 0x38) = arg1;
}

