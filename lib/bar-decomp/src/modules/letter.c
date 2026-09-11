// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
void _uvMemFree(void *ptr);
void uvUnloadFile(s32 formFileTag, s32 fileId);
extern UnkDobjExports *gUvDobjExports;
extern void *D_letter_00400504;
typedef struct {
    /* 0x00 */ char pad[0x3C];
    /* 0x3C */ void (*unk3C)(f32, f32, f32, f32);
} UvGfxMgrExp_00400420;
extern UvGfxMgrExp_00400420 *gUvGfxMgrExports;
typedef struct {
    /* 0x00 */ char unk0[0x8];
    /* 0x08 */ f32 unk8;
    /* 0x0C */ f32 unkC;
    /* 0x10 */ f32 unk10;
} TableEntry_00400420; /* stride 0x14 */

typedef struct {
    /* 0x00 */ char unk0[0x8];
    /* 0x08 */ u8 unk8;
    /* 0x09 */ char unk9[0x17];
    /* 0x20 */ f32 unk20;
} LetterObj_00400420;
extern char D_letter_00400480[];
void func_letter_00400230();
void * func_letter_004003E8();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/letter/__entrypoint_func_letter_400000.s")

void func_letter_00400140(void)
{
  void *temp_a1;
  s32 temp_a0;
  s32 var_s0;
 temp_a1 = D_letter_00400504; var_s0 = 0; do { temp_a0 = *((s32 *) ((((u8 *) temp_a1) + var_s0) + 0x4)); if (temp_a0 >= 0) { gUvDobjExports->unk40(temp_a0);
      temp_a1 = D_letter_00400504;
    }
    temp_a1 = D_letter_00400504;
    var_s0 += 0x24;
  }
  while (var_s0 != 0x2D0);
  _uvMemFree(temp_a1);
  uvUnloadFile(0x55564D44, 0x101);
  uvUnloadFile(0x55564D44, 0x102);
  uvUnloadFile(0x55564D44, 0x104);
  uvUnloadFile(0x55564D44, 0x103);
  uvUnloadFile(0x55565458, 0x3E8);
  uvUnloadFile(0x55565458, 0x3E9);
  uvUnloadFile(0x55565458, 0x3FE);
  uvUnloadFile(0x55565458, 0x3FC);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/letter/func_letter_00400230.s")

void *func_letter_004003E8(s32 arg0) {
    if (arg0 < 0 || arg0 >= 6) {
        return D_letter_00400480;
    }
    return D_letter_00400480 + arg0 * 0x14;
}

/* Looks up a viewport-parameter table entry by byte index and calls the gfx scissor/viewport setter. */
void func_letter_00400420(LetterObj_00400420 *arg0) {
    TableEntry_00400420 *temp_v0;

    temp_v0 = (TableEntry_00400420 *)(D_letter_00400480 + arg0->unk8 * 0x14);
    gUvGfxMgrExports->unk3C(temp_v0->unk8, temp_v0->unkC, temp_v0->unk10, arg0->unk20);
}

