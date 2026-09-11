// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
extern void *D_uvdobj_rom_00403A1C;
extern void *D_uvdobj_rom_00403A20;
extern s32 D_uvdobj_rom_004039E8;
extern s32 D_uvdobj_rom_004039C4;
extern s32 D_uvdobj_rom_004039B0;
void func_uvdobj_rom_00400770(s32, s32);
void func_uvdobj_rom_00401240(s32, s32);
void func_uvdobj_rom_004005C4();
void func_uvdobj_rom_00400704();
void func_uvdobj_rom_00400770();
void func_uvdobj_rom_00400A20();
void func_uvdobj_rom_00400C7C();
void func_uvdobj_rom_00400D5C();
void func_uvdobj_rom_00400EC0();
void func_uvdobj_rom_00400FF0();
void func_uvdobj_rom_00401028();
u8 func_uvdobj_rom_00401074();
void func_uvdobj_rom_004010B4();
void func_uvdobj_rom_00401104();
void func_uvdobj_rom_004011CC();
void func_uvdobj_rom_00401210();
void func_uvdobj_rom_00401240();
void func_uvdobj_rom_00401598(s32 arg0, f32 arg1);
void func_uvdobj_rom_004015C0();
void func_uvdobj_rom_00401ABC();
void func_uvdobj_rom_00401F14();
void func_uvdobj_rom_00402138();
void func_uvdobj_rom_00402420();
void func_uvdobj_rom_00402870();
void func_uvdobj_rom_00402D68();
void func_uvdobj_rom_00403054();
void func_uvdobj_rom_004032DC();
void func_uvdobj_rom_0040359C();
void func_uvdobj_rom_0040368C();
void func_uvdobj_rom_004038C4();
extern s32 D_uvdobj_rom_004039EC;
extern s32 D_uvdobj_rom_004039F0;
extern void *D_uvdobj_rom_004039B4;
extern s32 D_uvdobj_rom_004039B8;
extern u16 D_uvdobj_rom_004039BC;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/__entrypoint_func_uvdobj_rom_400000.s")

void func_uvdobj_rom_00400544(s32 a0, s32 a1) {
    D_uvdobj_rom_004039EC = a0;
    D_uvdobj_rom_004039F0 = a1;
}

s32 func_uvdobj_rom_00400558(s16 a0, s16 a1, s16 a2) {
    return 0;
}

void func_uvdobj_rom_0040056C(s32 arg0) {
    s32 handle;

    (*(void (**)(s32, s32, s32 *, s32))((u8 *)D_uvdobj_rom_00403A1C + 0x4))(arg0, 6, &handle, 0); // alloc handle into &handle
    (*(void (**)(s32, void *, s32, s32))((u8 *)D_uvdobj_rom_00403A20 + 0x10))(handle, func_uvdobj_rom_00401210, 0, 0x3C); // register callback
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_004005C4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00400704.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00400770.s")

u16 func_uvdobj_rom_00400A00(s32 a0) {
    return *(u16 *)((u8 *)D_uvdobj_rom_004039B4 + a0 * 0x30);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00400A20.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00400C7C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00400D5C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00400EC0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00400FF0.s")

void func_uvdobj_rom_00401028(s32 a0, s32 a1) {
    if (D_uvdobj_rom_004039C4 >= a0) {
        func_uvdobj_rom_00400FF0(a0, *(u8 *)((u8 *)D_uvdobj_rom_004039B4 + a0 * 0x30 + 4) | a1);
    }
}

u8 func_uvdobj_rom_00401074(s32 a0) {
    if (D_uvdobj_rom_004039C4 < a0) {
        return 0;
    }
    return *(u8 *)((u8 *)D_uvdobj_rom_004039B4 + a0 * 0x30 + 4);
}

void func_uvdobj_rom_004010B4(s32 a0, s32 a1) {
    if (D_uvdobj_rom_004039C4 >= a0) {
        func_uvdobj_rom_00400FF0(a0, *(u8 *)((u8 *)D_uvdobj_rom_004039B4 + a0 * 0x30 + 4) & ~a1);
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00401104.s")

void func_uvdobj_rom_004011CC(s32 a0) {
    func_uvdobj_rom_00400770(a0, 0xFFFF);
    *(u8 *)((u8 *)D_uvdobj_rom_004039B4 + a0 * 0x30 + 5) = 0;
}

void func_uvdobj_rom_00401210(s32 a0) {
    if (D_uvdobj_rom_004039B0 == 0) {
        func_uvdobj_rom_00401240(a0, 0);
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00401240.s")

void func_uvdobj_rom_00401598(s32 arg0, f32 arg1)
{
  s32 base;
  *((f32 *) (((u8 *) D_uvdobj_rom_004039E8) + (arg0 * 4))) = 1.0f / arg1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_004015C0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00401ABC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00401F14.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00402138.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00402420.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00402870.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00402D68.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_00403054.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_004032DC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_0040359C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_0040368C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvdobj_rom/func_uvdobj_rom_004038C4.s")

u16 func_uvdobj_rom_00403954(s32 *a0) {
    *a0 = D_uvdobj_rom_004039B8;
    return D_uvdobj_rom_004039BC;
}

