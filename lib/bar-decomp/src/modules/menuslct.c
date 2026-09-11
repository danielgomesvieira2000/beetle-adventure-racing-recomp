// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
extern void *gUvFontExports;
extern void *gGameGuiExports;
extern void *gReplayExports;
void func_menuslct_00400BCC(s32, s32, s32, s32, s32, s32, s32, s32);
void func_menuslct_004000F8();
void func_menuslct_0040043C();
void func_menuslct_0040056C();
void func_menuslct_0040062C();
s32 func_menuslct_004009FC(void *arg0);
void func_menuslct_00400A5C();
void func_menuslct_00400B14();
void func_menuslct_00400BCC();
void func_menuslct_00400E48();
void func_menuslct_004010C4();
void func_menuslct_004011B0();
void func_menuslct_0040126C();
void func_menuslct_00401320();
void func_menuslct_004013F8();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/__entrypoint_func_menuslct_400000.s")

void func_menuslct_004000F0(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_004000F8.s")

s32 func_menuslct_00400330(s32 arg0, s32 arg1) {
    if ((*(s32 (**)(void))((u8 *)gGameGuiExports + 0x24))() != 0) {
        return 0;
    }
    if ((*(s32 (**)(s32, s32))((u8 *)gReplayExports + 0x28))(arg0, 0x4000) != 0) {
        return 7;
    }
    if (*(s16 *)((u8 *)arg1 + (arg0 * 0xC) + 0x4) == -1) {
        return 0;
    }
    if ((*(s32 (**)(s32, s32))((u8 *)gReplayExports + 0x28))(arg0, 0x8000) != 0) {
        return 8;
    }
    if (((*(s32 (**)(s32, s32))((u8 *)gReplayExports + 0x28))(arg0, 0x1000) != 0) || ((*(s32 (**)(s32, s32))((u8 *)gReplayExports + 0x28))(arg0, 0x1000) != 0)) {
        return 8;
    }
    return 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_0040043C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_0040056C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_0040062C.s")

s32 func_menuslct_004009FC(void *arg0)
{
  s16 count;
  s16 i;
  void *base;
  count = *((s16 *) (((u8 *) arg0) + 0x0));
  i = 0;
  if (count > 0)
  {
    base = (void *) (*((s32 *) (((u8 *) arg0) + 0x8)));
    do
    {
      if ((*((s16 *) ((((u8 *) base) + (i * 0x14)) + 0x2))) == 1)
      {
        return i;
      }
      i = i + 1;
    }
    while (i < count);
  }
  return -1;
  return -1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_00400A5C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_00400B14.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_00400BCC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_00400E48.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_004010C4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_004011B0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_0040126C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/menuslct/func_menuslct_00401320.s")

void func_menuslct_004013F8(s32 arg0) {
    func_menuslct_00400BCC(0x16, 0xE1, 0x12A, 0x10, 0, 0, 0, arg0);
}

void func_menuslct_0040143C(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8) {
    s32 sp24;

    sp24 = 2;
    (*(void (*)(s32))(*(s32 *)((u8 *)gUvFontExports + 0x4)))(a2);
    (*(void (*)(f64, f64))(*(s32 *)((u8 *)gUvFontExports + 0x8)))(1.0, 1.0);
    if (a2 == 0) {
        (*(void (*)(s32))(*(s32 *)((u8 *)gUvFontExports + 0x4)))(2);
        sp24 = 4;
    }
    if (a8 != 0) {
        if (a2 != 3) {
            (*(void (*)(s32, s32, s32, s32))(*(s32 *)((u8 *)gUvFontExports + 0xC)))(0, 0, 0, 0xAF);
            (*(void (*)(s32, s32, s32))(*(s32 *)((u8 *)gUvFontExports + 0x24)))(a0 + sp24, a1 - sp24, a7);
            (*(void (*)(s32))(*(s32 *)((u8 *)gUvFontExports + 0x4)))(a2);
            (*(void (*)(s32, s32, s32, s32))(*(s32 *)((u8 *)gUvFontExports + 0xC)))(0, 0, 0, 0xFF);
            (*(void (*)(s32, s32, s32))(*(s32 *)((u8 *)gUvFontExports + 0x24)))(a0 + 1, a1 - 1, a7);
        }
        (*(void (*)(s32, s32, s32, s32))(*(s32 *)((u8 *)gUvFontExports + 0xC)))((u8)a3, (u8)a4, (u8)a5, (u8)a6);
        (*(void (*)(s32, s32, s32))(*(s32 *)((u8 *)gUvFontExports + 0x24)))(a0, a1, a7);
        return;
    }
    (*(void (*)(s32))(*(s32 *)((u8 *)gUvFontExports + 0x4)))(a2);
    (*(void (*)(s32, s32, s32, s32))(*(s32 *)((u8 *)gUvFontExports + 0xC)))(0, 0, 0, 0xFF);
    (*(void (*)(s32, s32, s32))(*(s32 *)((u8 *)gUvFontExports + 0x24)))(a0 + 1, a1 - 1, a7);
    (*(void (*)(s32, s32, s32, s32))(*(s32 *)((u8 *)gUvFontExports + 0xC)))(0x78, 0x78, 0xBE, 0xE1);
    (*(void (*)(s32, s32, s32))(*(s32 *)((u8 *)gUvFontExports + 0x24)))(a0, a1, a7);
}

