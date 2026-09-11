// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern void *D_uvbill_rom_0040202C;
extern void *D_uvbill_rom_00402040;
extern void *D_uvbill_rom_00402028;
extern s32 D_uvbill_rom_00402020;
extern s32 D_uvbill_rom_00402000;
extern s32 D_uvbill_rom_00402004;
extern s32 D_uvbill_rom_00402008;
extern s32 D_uvbill_rom_0040200C;
extern void func_uvbill_rom_00400DEC(void);
void func_uvbill_rom_004002C0();
void func_uvbill_rom_00400318();
void func_uvbill_rom_004003B4();
void func_uvbill_rom_00400D9C();
void func_uvbill_rom_00400DEC();
void func_uvbill_rom_004010B8();
void func_uvbill_rom_00401388();
void func_uvbill_rom_00401418();
void func_uvbill_rom_0040154C();
void func_uvbill_rom_004015A4();
void func_uvbill_rom_00401E40();
void func_uvbill_rom_00401F5C();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvbill_rom/__entrypoint_func_uvbill_rom_400000.s")

void func_uvbill_rom_004002C0(s32 arg0) {
    s32 sp1C;

    (*(void (*)(s32, s32, s32 *, s32))(*(s32 *)((u8 *)D_uvbill_rom_0040202C + 0x4)))(arg0, 6, &sp1C, 0);
    (*(void (*)(s32, void *, s32, s32))(*(s32 *)((u8 *)D_uvbill_rom_00402040 + 0x10)))(sp1C, func_uvbill_rom_00400DEC, 0, 0x32);
}

void func_uvbill_rom_00400318(void) {
    _uvMemFree(D_uvbill_rom_0040200C);
    _uvMemFree(D_uvbill_rom_00402008);
    _uvMemFree(D_uvbill_rom_00402004);
    _uvMemFree(D_uvbill_rom_00402000);
    uvUnloadModule('STAT');
    uvUnloadModule('CHAN');
    uvUnloadModule('FMTX');
    uvUnloadModule('MATH');
    uvUnloadModule('TSEQ');
    uvUnloadModule('DGEO');
    uvUnloadModule('CBCK');
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvbill_rom/func_uvbill_rom_004003B4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvbill_rom/func_uvbill_rom_00400D9C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvbill_rom/func_uvbill_rom_00400DEC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvbill_rom/func_uvbill_rom_004010B8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvbill_rom/func_uvbill_rom_00401388.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvbill_rom/func_uvbill_rom_00401418.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvbill_rom/func_uvbill_rom_0040154C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvbill_rom/func_uvbill_rom_004015A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvbill_rom/func_uvbill_rom_00401E40.s")

void func_uvbill_rom_00401F5C(s32 arg0) {
    s32 temp_v0;
    u8 *temp_v1;

    if (arg0 >= 0 && arg0 < D_uvbill_rom_00402020) {
        temp_v0 = arg0 * 0x78;
        temp_v1 = (u8 *)D_uvbill_rom_00402000 + temp_v0;
        if (*(u8 *)(temp_v1 + 1) != 0) {
            *(u8 *)(temp_v1 + 3) = 0;
            *(u8 *)((u8 *)D_uvbill_rom_00402000 + temp_v0 + 1) = 0;
        }
    }
}
