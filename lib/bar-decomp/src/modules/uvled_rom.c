// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern f32 D_uvled_rom_00401664;
extern f32 D_uvled_rom_00401668;
extern s16 D_uvled_rom_0040166C;
extern s16 D_uvled_rom_0040166E;
void func_uvled_rom_004004C8(s32 arg0, s32 arg1);
extern s16 D_uvled_rom_00401660;
extern s16 D_uvled_rom_00401662;
void func_uvled_rom_00400118();
void func_uvled_rom_00400148();
void func_uvled_rom_004002E0();
void func_uvled_rom_00400300();
void func_uvled_rom_00400320();
void func_uvled_rom_00400340(s16 arg0, s16 arg1);
void func_uvled_rom_0040035C(f32 arg0, f32 arg1);
void func_uvled_rom_00400410();
void func_uvled_rom_004004C8();
void func_uvled_rom_004007A4();
void func_uvled_rom_00400B2C();
void func_uvled_rom_00400C04();
void func_uvled_rom_00400DFC();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvled_rom/__entrypoint_func_uvled_rom_400000.s")

void func_uvled_rom_00400118(void) {
    uvUnloadModule('FMTX');
    uvUnloadModule('GMGR');
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvled_rom/func_uvled_rom_00400148.s")

void func_uvled_rom_004002E0(s32 arg0) {
    func_uvled_rom_004004C8(arg0, 0xA);
}

void func_uvled_rom_00400300(s32 arg0) {
    func_uvled_rom_004004C8(arg0, 2);
}

void func_uvled_rom_00400320(s32 arg0) {
    func_uvled_rom_004004C8(arg0, 0x10);
}

void func_uvled_rom_00400340(s16 arg0, s16 arg1) {
    D_uvled_rom_00401660 = arg0;
    D_uvled_rom_00401662 = arg1;
}

void func_uvled_rom_0040035C(f32 arg0, f32 arg1)
{
  D_uvled_rom_00401664 = ((f64) arg0) / 10.0;
  D_uvled_rom_00401668 = (f32) (((f64) arg1) / 14.0);
  D_uvled_rom_0040166C = (s16) ((s32) ((((f64) D_uvled_rom_00401664) + 0.25) * ((float) 10.0)));
  D_uvled_rom_0040166E = (s16) ((s32) ((((f64) D_uvled_rom_00401668) + 0.25) * ((float) 14.0)));
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvled_rom/func_uvled_rom_00400410.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvled_rom/func_uvled_rom_004004C8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvled_rom/func_uvled_rom_004007A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvled_rom/func_uvled_rom_00400B2C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvled_rom/func_uvled_rom_00400C04.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvled_rom/func_uvled_rom_00400DFC.s")

