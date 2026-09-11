// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 gNmiAsserted;
extern u8 D_uvcont_rom_00401AB8;
extern u8 D_uvcont_rom_00401AC2;
extern u8 D_uvcont_rom_00401AC6;
extern f32 D_uvcont_rom_004018A4;
extern u8 D_uvcont_rom_00401880;
extern u8 D_uvcont_rom_004018A0;
extern u8 D_uvcont_rom_00401918;
extern s32 func_8000FA00(void *);
extern s32 func_8000F670(void *);
void func_uvcont_rom_004002D8();
f32 func_uvcont_rom_0040061C(s32 arg0, s32 arg1);
s32 func_uvcont_rom_00400640();
s32 func_uvcont_rom_00400694();
s32 func_uvcont_rom_004006E0();
void func_uvcont_rom_0040072C();
void func_uvcont_rom_00400768();
void func_uvcont_rom_0040079C();
void func_uvcont_rom_00400834();
s32 func_uvcont_rom_00400974(char arg0);
void func_uvcont_rom_004009F4();
void func_uvcont_rom_00400A98();
void func_uvcont_rom_00400B5C();
void func_uvcont_rom_00400C90();
s32 func_uvcont_rom_00400D5C(s32 arg0, s32 arg1, void *arg2);
void func_uvcont_rom_00400E4C();
void func_uvcont_rom_00400EF0();
s32 func_uvcont_rom_00400F94();
s32 func_uvcont_rom_00400FDC();
void func_uvcont_rom_00401024();
void func_uvcont_rom_00401148();
void func_uvcont_rom_004011FC();
void func_uvcont_rom_0040128C();
void func_uvcont_rom_004013E8();
void func_uvcont_rom_00401478();
void func_uvcont_rom_00401520();
s32 func_uvcont_rom_004015D8(s32 arg0);
void func_uvcont_rom_00401658();
void func_uvcont_rom_00401720();
void func_uvcont_rom_00401760();
extern void func_uvcont_rom_00401720(void);
extern u8 D_uvcont_rom_00401AE4;
extern u16 D_uvcont_rom_004018B0;
extern u8 D_uvcont_rom_004018B6;
extern s32 D_uvcont_rom_00401AE8;
extern s32 D_uvcont_rom_00401B04;
extern u16 D_uvcont_rom_00401B08;
extern s32 D_uvcont_rom_00401B0C;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/__entrypoint_func_uvcont_rom_400000.s")

void func_uvcont_rom_004002B8(void) {
    func_uvcont_rom_00401720();
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_004002D8.s")

s32 func_uvcont_rom_00400604(s32 a0) {
    return ((s32)D_uvcont_rom_00401AE4 >> a0) & 1;
}

f32 func_uvcont_rom_0040061C(s32 arg0, s32 arg1) {
    return *(f32 *)((u8 *)&D_uvcont_rom_004018A4 + (arg0 * 0x18) + (arg1 * 4));
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00400640.s")

u16 func_uvcont_rom_00400678(s32 a0) {
    return *(u16 *)((u8 *)&D_uvcont_rom_004018B0 + a0 * 24);
}

s32 func_uvcont_rom_00400694(s32 a0, s32 a1) {
    u8 *base;

    base = (u8 *)&D_uvcont_rom_004018A0 + a0 * 24;
    if (*(u16 *)(base + 0x10) & a1 && !(*(u16 *)(base + 0x12) & a1)) {
        return 1;
    }
    return 0;
}

s32 func_uvcont_rom_004006E0(s32 a0, s32 a1) {
    u8 *base;

    base = (u8 *)&D_uvcont_rom_004018A0 + a0 * 24;
    if (!(*(u16 *)(base + 0x10) & a1) && *(u16 *)(base + 0x12) & a1) {
        return 1;
    }
    return 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_0040072C.s")

void func_uvcont_rom_00400768(s32 a0, u16 *a1, u8 *a2, u8 *a3) {
    u8 *base;

    base = (u8 *)&D_uvcont_rom_004018A0 + a0 * 24;
    *a2 = *(u8 *)base;
    *a3 = *(u8 *)(base + 1);
    *a1 = *(u16 *)(base + 0x10);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_0040079C.s")

u8 func_uvcont_rom_00400800(s32 a0) {
    return (&D_uvcont_rom_004018B6)[a0 * 24];
}

void func_uvcont_rom_0040081C(s32 a0) {
    D_uvcont_rom_00401B04 = a0;
}

void func_uvcont_rom_00400828(s32 a0) {
    D_uvcont_rom_00401AE8 = a0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00400834.s")

s32 func_uvcont_rom_00400974(char arg0)
{
  s32 v1 = arg0 & 0xFF;
  if (v1 < 0xF)
  {
    return 0x2D;
  }
  if (v1 == 0xF)
  {
    return 0x20;
  }
  if (v1 < 0x1A)
  {
    return (v1 + 0x20) & 0xFF;
  }
  if (v1 < 0x34)
  {
    return (v1 + 0x27) & 0xFF;
  }
  if (v1 < 0x42)
  {
    return ((u8 *) (&D_uvcont_rom_00401880))[v1 - 0x30];
  }
  return 0x2D;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_004009F4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00400A98.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00400B5C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00400C90.s")

void func_uvcont_rom_00400D48(s32 a0, s32 a1) {
    D_uvcont_rom_00401B08 = a0;
    D_uvcont_rom_00401B0C = a1;
}

s32 func_uvcont_rom_00400D5C(s32 arg0, s32 arg1, void *arg2)
{
  u8 *new_var;
  unsigned int flags;
  flags = *((u8 *) (((u8 *) (&D_uvcont_rom_004018B6)) + (arg0 * 0x18)));
  if (!(flags & 1))
  {
    return 0;
  }
  if (flags & 8)
  {
    return 0;
  }
  if (func_8000F2E0(((u8 *) (&D_uvcont_rom_00401918)) + ((8 * arg0) * 13), arg1, &D_uvcont_rom_00401AB8) == 0)
  {
    *((s32 *) (((u8 *) arg2) + 0x8)) = *((u16 *) (((u8 *) (&D_uvcont_rom_00401AB8)) + 0x8));
    *((s32 *) (((u8 *) arg2) + 0x4)) = *((s32 *) (((u8 *) (&D_uvcont_rom_00401AB8)) + 0x4));
    new_var = ((u8 *) arg2) + 0xC;
    *((s32 *) (((u8 *) arg2) + 0x0)) = *((s32 *) (((u8 *) (&D_uvcont_rom_00401AB8)) + 0x0));
    func_uvcont_rom_00401478(((u8 *) arg2) + 0x11, &D_uvcont_rom_00401AC6, 0x10);
    func_uvcont_rom_00401478(new_var, &D_uvcont_rom_00401AC2, 0x4);
 *((u8 *) (((u8 *) arg2) + 0x21)) = 0; *((u8 *) (((u8 *) arg2) + 0x10)) = 0;
    return 1;
  }
  return 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00400E4C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00400EF0.s")

s32 func_uvcont_rom_00400F94(s32 a0) {
    if (func_8000FA00((u8 *)&D_uvcont_rom_00401918 + a0 * 0x68) == 0) {
        return 1;
    }
    return 0;
}

s32 func_uvcont_rom_00400FDC(s32 a0) {
    if (func_8000F670((u8 *)&D_uvcont_rom_00401918 + a0 * 0x68) == 0) {
        return 1;
    }
    return 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00401024.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00401148.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_004011FC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_0040128C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_004013E8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00401478.s")

void func_uvcont_rom_00401518(s16 a0) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00401520.s")

s32 func_uvcont_rom_004015D8(s32 arg0)
{
  u8 v0;
  if (!(((u8 *) (&D_uvcont_rom_004018B6))[arg0 * 0x18] & 2))
  {
    return 0;
  }
  if (((u8 *) (&D_uvcont_rom_004018B6))[arg0 * 0x18] & 8)
  {
    return 0;
  }
  func_8000E820(((u8 *) (&D_uvcont_rom_00401918)) + (arg0 * 0x68), 0, arg0);
  return 1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00401658.s")

void func_uvcont_rom_00401720(void) {
    s32 i;

    i = 0;
    do {
        func_uvcont_rom_00401658(i);
        i++;
    } while (i != 4);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvcont_rom/func_uvcont_rom_00401760.s")

void func_uvcont_rom_0040183C(s16 a0) {
}

