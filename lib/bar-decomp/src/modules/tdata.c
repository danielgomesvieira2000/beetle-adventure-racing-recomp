// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern u8 D_tdata_004029E0;
/*__SEEDEXTERNS__*/
extern u8 D_tdata_004029D4;
extern s32 D_tdata_004029DC;
void func_tdata_00400194();
void func_tdata_00400234();
void func_tdata_00400738();
void func_tdata_0040087C();
void func_tdata_004008B8();
void func_tdata_004008D4();
void func_tdata_004009B0();
void func_tdata_00400DB4();
s32 func_tdata_00401254(void);
void func_tdata_00401294();
void func_tdata_00401344();
s32 func_tdata_00401354(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void func_tdata_004013EC();
void func_tdata_00401524();
void func_tdata_00401588();
void func_tdata_004015F0();
void func_tdata_00401638();
f32 func_tdata_00401860(s32 arg0);
void func_tdata_0040186C();
void func_tdata_00401904();
void func_tdata_00401998();
void func_tdata_00401A0C();
void func_tdata_00401A80();
void func_tdata_00401ADC();
void func_tdata_00401CBC();
void func_tdata_00401D38();
void func_tdata_00402388();
void func_tdata_004027CC();
void func_tdata_00400DB4(void);
extern s32 D_tdata_004029D8;
extern s32 D_tdata_004029F0;
extern s32 D_tdata_004029F4;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/__entrypoint_func_tdata_400000.s")

void func_tdata_0040018C(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00400194.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00400234.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00400738.s")

void func_tdata_0040087C(void *arg0) {
    *(s16 *)((u8 *)arg0 + 0x0) = 0;
    *(s16 *)((u8 *)arg0 + 0x2) = 0;
    *(s16 *)((u8 *)arg0 + 0xC) = 0;
    *(f32 *)((u8 *)arg0 + 0x8) = 0.0f;
    *(f32 *)((u8 *)arg0 + 0x4) = 0.0f;
}

void func_tdata_00400898(void) {
    func_tdata_00400DB4();
}

void func_tdata_004008B8(s32 a0, s32 a1) {
}

void func_tdata_004008C4(void) {
}

void func_tdata_004008CC(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_004008D4.s")

s32 func_tdata_004009A8(void) {
    return 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_004009B0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00400DB4.s")

s32 func_tdata_00401254(void)
{
  s32 temp_t6;
  s32 var_v1;
  u8 *new_var;
  u32 temp_a1;
  u32 var_v0;
  u8 *var_a2;
  var_v0 = (var_v1 = 0);
  new_var = (u8 *) D_tdata_004029DC;
  temp_a1 = *((u32 *) (new_var + 0x4));
  if (temp_a1 != 0)
  {
    var_a2 = *((u8 **) (new_var + 0xC));
    do
    {
      temp_t6 = (*((s32 *) (var_a2 + 0x4))) & 0xFFFFFFFFFFFFFFFFu;
      var_v0 += 1;
      var_a2 += 8;
      var_v1 += temp_t6;
    }
    while (var_v0 < temp_a1);
  }
 if (0) { }
  return var_v1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401294.s")

void func_tdata_00401344(s32 a0, s32 a1, s32 a2) {
}

s32 func_tdata_00401354(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
  s32 sp1C;
  s32 t0;
  s32 v0;
  s32 v1;
  s32 v0b;
  v0 = arg3 - (arg4 / 2);
  sp1C = v0;
  v1 = v0 + arg4;
  t0 = v1;
  if (v0 <= 0)
  {
    sp1C = 0;
  }
  v0b = (*((s32 *) (&D_tdata_004029E0))) - 1;
  if (v1 >= v0b)
  {
    t0 = v0b;
  }
  func_tdata_0040186C(arg0, sp1C);
  func_tdata_0040186C(arg1, arg3);
  func_tdata_0040186C(arg2, t0);
  return t0 - sp1C;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_004013EC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401524.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401588.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_004015F0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401638.s")

f32 func_tdata_00401860(s32 arg0) {
    return 0.0f;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_0040186C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401904.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401998.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401A0C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401A80.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401ADC.s")

void func_tdata_00401C58(void *a0) {
    *(s32*)((u8*)a0 + 0x18) = D_tdata_004029D8;
    D_tdata_004029D8 = (s32)a0;
}

void* func_tdata_00401C70(f32 arg0, f32 arg1) {
    void* temp_v0;
    temp_v0 = _uvMemAllocAlign8(0xC);
    *(f32*)((u8*)temp_v0 + 0x0) = arg0;
    *(f32*)((u8*)temp_v0 + 0x4) = arg1;
    *(s32*)((u8*)temp_v0 + 0x8) = *(s32*)((u8*)&D_tdata_004029D4 + 0x0);
    *(s32*)((u8*)&D_tdata_004029D4 + 0x0) = (s32)temp_v0;
    return temp_v0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401CBC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00401D38.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_00402388.s")

void func_tdata_004027B8(s32 a0, s32 a1) {
    D_tdata_004029F0 = a0;
    D_tdata_004029F4 = a1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/tdata/func_tdata_004027CC.s")

