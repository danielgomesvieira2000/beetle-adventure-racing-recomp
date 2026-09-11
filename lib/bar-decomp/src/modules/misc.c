// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
void func_misc_00400AD0();
void func_misc_00401080();
void func_misc_004012E4();
f32 func_misc_0040142C(f32 arg0, f32 arg1, f32 arg2);
void func_misc_0040146C();
void func_misc_00401528();
void func_misc_004015A0();
void func_misc_00401614();
void func_misc_004016CC();
void func_misc_004017A8();
void func_misc_004017C4();
void func_misc_004017FC();
void func_misc_0040187C(void *arg0, void *arg1, f32 arg2, void *arg3);
f32 func_misc_004018AC(void *arg0, void *arg1);
void func_misc_004018D0(void *arg0, void *arg1, void *arg2);
void func_misc_004018F4(void *arg0, void *arg1, void *arg2);
void func_misc_00401918(void *arg0, f32 arg1, void *arg2);
void func_misc_00401938();
void func_misc_0040197C();
void func_misc_00401990();
void func_misc_004019FC();
void func_misc_00401E14(void *arg0, void *arg1, void *arg2);
void func_misc_00401E48(void *arg0, void *arg1, void *arg2);
void func_misc_00401E7C(void *arg0, f32 arg1, void *arg2);
void func_misc_00401EA8();
void func_misc_00401F2C();
void func_misc_00401F48();
void func_misc_00401FA0();
void func_misc_0040213C();
void func_misc_0040234C();
void func_misc_00402528();
void func_misc_00402698();
void func_misc_00402730();
void func_misc_00402874();
void func_misc_004029DC();
void func_misc_00402A5C();
void func_misc_00402BFC();
void func_misc_00402D48();
void func_misc_00402E94();
void func_misc_00402EFC();
void func_misc_00403000();
void func_misc_00403110();
void func_misc_00403300();
void func_misc_00403324();
void func_misc_00403348();
void func_misc_00403650();
void func_misc_00403748();
void func_misc_004038D8();
void func_misc_00403A5C();
void func_misc_00403BEC();
void func_misc_00403CA4();
void func_misc_00403CD8();
void func_misc_00403D8C();
void func_misc_004041B4();
extern s32 D_misc_00404300;
#include "gbi.h"
#include "module.h"
#include "global_exports.h"

#define ABS_2(x) ((x) > 0 ? (x) : -(x))

typedef struct UnkStruct_misc_004006A0_s {
    /* 0x00 */ f32 unk0;                            /* inferred */
    /* 0x04 */ f32 unk4;                            /* inferred */
    /* 0x08 */ f32 unk8;                            /* inferred */
    /* 0x0C */ f32 unkC;                            /* inferred */
    /* 0x10 */ f32 unk10;                           /* inferred */
    /* 0x14 */ f32 unk14;                           /* inferred */
    /* 0x18 */ f32 unk18;                           /* inferred */
    /* 0x1C */ f32 unk1C;                           /* inferred */
    /* 0x20 */ f32 unk20;                           /* inferred */
    /* 0x24 */ f32 unk24;                           /* inferred */
    /* 0x28 */ f32 unk28;                           /* inferred */
    /* 0x2C */ f32 unk2C;                           /* inferred */
    /* 0x30 */ f32 unk30;
    /* 0x34 */ f32 unk34;
    /* 0x38 */ f32 unk38;
} UnkStruct_misc_004006A0;                          /* size = 0x3C */

extern f32 D_80025D70;


void func_misc_0040070C(Vec3F *arg0, UnkStruct_misc_004006A0 *arg1);
void func_misc_004008B4(f32 arg0, f32 arg1, f32 arg2, f32* arg3, f32* arg4, f32* arg5);
void func_misc_004009B0(f32 arg0, f32 arg1, f32 arg2, f32* arg3, f32* arg4, f32* arg5);
void func_misc_00400CA8(Mtx4F *mtx, f32 arg1, f32 arg2, f32 arg3);
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/__entrypoint_func_misc_400000.s")

void func_misc_00400390(void) {

}

void func_misc_00400398(f32 arg0) {
    f64 sec;

    sec = uvClkGetSec(0) + arg0;
    while (uvClkGetSec(0) < sec) {

    }
}

// .rodata
extern f32 D_misc_00404220;
f32 func_misc_00400400(f32 arg0, f32 arg1, f32 arg2) {
    f32 temp_ft4;
    f32 var_fa1;
    f32 var_ft5;
    f32 var_fv0;
    f32 var_fv1;
    f32 var_fv1_2;

    if (arg1 < arg0) {
        var_fv0 = arg0 - arg1;
        var_fv1 = var_fv0;
    } else {
        var_fv0 = arg0 - arg1;
        var_fv1 = -var_fv0;
    }
    if (var_fv1 < D_misc_00404220) {
        return arg0;
    }
    temp_ft4 = arg2 * var_fv0 * D_80025D70;
    if (var_fv0 > 0.0f) {
        var_ft5 = var_fv0;
    } else {
        var_ft5 = -var_fv0;
    }
    if (temp_ft4 > 0.0f) {
        var_fv1 = temp_ft4;
    } else {
        var_fv1 = -temp_ft4;
    }
    if (var_ft5 < var_fv1) {
        arg1 = arg0;
    } else {
        arg1 = arg1 + temp_ft4;
    }
    return arg1;
}

// .rodata
extern f32 D_misc_00404224;
f32 func_misc_004004B8(f32 arg0, f32 arg1, f32 arg2) {
    f32 var_fa1;
    f32 var_ft4;
    f32 var_ft5;
    f32 var_fv0;
    f32 var_fv1;
    f32 var_fv1_2;

    if (arg1 < arg0) {
        var_fv0 = arg0 - arg1;
        var_fv1 = var_fv0;
    } else {
        var_fv0 = arg0 - arg1;
        var_fv1 = -var_fv0;
    }
    if (var_fv1 < D_misc_00404224) {
        return arg0;
    }
    if (var_fv0 > 0) {
        var_ft4 = arg2 * D_80025D70;
    } else {
        var_ft4 = -arg2 * D_80025D70;
    }
    if (var_fv0 > 0.0f) {
        var_ft5 = var_fv0;
    } else {
        var_ft5 = -var_fv0;
    }
    if (var_ft4 > 0.0f) {
        var_fv1 = var_ft4;
    } else {
        var_fv1 = -var_ft4;
    }
    if (var_ft5 < var_fv1) {
        arg1 = arg0;
    } else {
        arg1 = arg1 + var_ft4;
    }
    return arg1;
}

void func_misc_004005A0(Vec3F* arg0, Vec3F* arg1, f32 arg2) {
    f32 temp_fv0;
    f32 temp_fv0_2;
    f32 temp_fv1;

    temp_fv0 = gUvFvecExports->uvVec3FLen(arg1);
    if (temp_fv0 == 0.0f) {
        return;
    }

    arg1->x = func_misc_00400400(arg0->x, arg1->x, arg2);
    arg1->y = func_misc_00400400(arg0->y, arg1->y, arg2);
    arg1->z = func_misc_00400400(arg0->z, arg1->z, arg2);

    temp_fv0_2 = gUvFvecExports->uvVec3FLen(arg1);
    if (temp_fv0_2 == 0.0f) {
        return;
    }

    temp_fv1 = temp_fv0 / temp_fv0_2;
    arg1->x *= temp_fv1;
    arg1->y *= temp_fv1;
    arg1->z *= temp_fv1;
}


void func_misc_004006A0(UnkStruct_misc_004006A0* arg0, UnkStruct_misc_004006A0* arg1, f32 arg2) {
    arg1->unk30 = func_misc_00400400(arg0->unk30, arg1->unk30, arg2);
    arg1->unk34 = func_misc_00400400(arg0->unk34, arg1->unk34, arg2);
    arg1->unk38 = func_misc_00400400(arg0->unk38, arg1->unk38, arg2);
}

void func_misc_0040070C(Vec3F* arg0, UnkStruct_misc_004006A0* arg1) {
    Vec3F sp3C;
    Vec3F sp30;
    Vec3F sp24;

    gUvFvecExports->uvVec3FNormalize(arg0, arg0);
    gUvFvecExports->uvVec3FCopy(&sp3C, arg0);
    sp30.x = arg1->unk0;
    sp30.y = arg1->unk4;
    sp30.z = arg1->unk8;
    gUvFvecExports->uvVec3FCross(&sp24, &sp3C, (Vec3F* ) &sp30);
    gUvFvecExports->uvVec3FNormalize(&sp24, &sp24);
    gUvFvecExports->uvVec3FCross((Vec3F* ) &sp30, &sp24, &sp3C);
    gUvFvecExports->uvVec3FNormalize((Vec3F* ) &sp30, (Vec3F* ) &sp30);
    arg1->unk0 = sp30.x;
    arg1->unk4 = sp30.y;
    arg1->unk8 = sp30.z;
    arg1->unk10 = sp24.x;
    arg1->unk14 = sp24.y;
    arg1->unk18 = sp24.z;
    arg1->unk20 = sp3C.x;
    arg1->unk24 = sp3C.y;
    arg1->unk28 = sp3C.z;
    arg1->unkC = 0.0f;
    arg1->unk1C = 0.0f;
    arg1->unk2C = 0.0f;
}

void func_misc_00400844(Vec3F arg0, f32* arg3, f32* arg4, f32* arg5) {
    func_misc_004008B4(arg0.x, arg0.y, arg0.z, arg3, arg4, arg5);
}

void func_misc_00400884(f32 arg0, f32 arg1, f32 arg2, f32* arg3) {
    func_misc_004009B0(arg0, arg1, arg2, arg3, arg3 + 1, arg3 + 2);
}

void func_misc_004008B4(f32 arg0, f32 arg1, f32 arg2, f32* arg3, f32* arg4, f32* arg5) {
    f32 sp1C;
    f32 temp_fa0;
    s32 pad;
    f32 sp20;

    sp20 = gUvMathExports->uvSqrtf(SQ(arg0) + SQ(arg1));
    *arg3 = gUvMathExports->uvSqrtf(SQ(arg0) + SQ(arg1) + SQ(arg2));
    if (*arg3 == 0.0f) {
        *arg4 = 0.0f;
        *arg5 = 0.0f;
        return;
    }
    *arg4 = gUvMathExports->uvAtan2F(arg1 / *arg3, arg0 / *arg3);
    *arg5 = gUvMathExports->uvAtan2F(arg2 / *arg3, sp20 / *arg3);
}

void func_misc_004009B0(f32 arg0, f32 arg1, f32 arg2, f32* arg3, f32* arg4, f32* arg5) {
    f32 cosf;

    cosf =  gUvMathExports->uvCosF(arg2);
    *arg3 = gUvMathExports->uvCosF(arg1) * (arg0 * cosf);
    *arg4 = gUvMathExports->uvSinF(arg1) * (arg0 * cosf);
    *arg5 = gUvMathExports->uvSinF(arg2) * arg0;
}

f32 func_misc_00400A7C(f32 arg0) {
    s32 pad;
    f32 sp18;

    sp18 = gUvMathExports->uvSinF(arg0);
    return sp18 / gUvMathExports->uvCosF(arg0);
}


#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00400AD0.s")

extern f32 D_misc_00404234;

f32 func_misc_00400BF0(f32 arg0) {
    if (arg0 == 0.0f) {
        return D_misc_00404234;
    }

    if (arg0 < -1.0f) {
        arg0 = -1.0f;
    } else if (arg0 > 1.0f) {
        arg0 = 1.0f;
    }

    return gUvMathExports->uvAtan2F(gUvMathExports->uvSqrtf(1 - (arg0 * arg0)), arg0);
}

// Mtx rotate
void func_misc_00400CA8(Mtx4F* mtx, f32 x, f32 y, f32 z) {
    f32 temp_fs0;
    f32 temp_fs1;
    Vec3F sp34;
    f32 temp_ft4;
    f32 temp_ft5;
    f32 temp_fv0;

    temp_fs0 = gUvMathExports->uvSinF(z);
    temp_fs1 = gUvMathExports->uvCosF(z);
    sp34.z = gUvMathExports->uvSinF(y);
    sp34.y = gUvMathExports->uvCosF(y);
    sp34.x = gUvMathExports->uvSinF(x);
    temp_fv0 = gUvMathExports->uvCosF(x);
    temp_ft4 = sp34.z * sp34.x;
    mtx->m[0][0] = (temp_fs1 * temp_fv0) - (temp_fs0 * temp_ft4);
    temp_ft5 = sp34.z * temp_fv0;
    mtx->m[0][1] = (temp_fs1 * sp34.x) + (temp_fs0 * temp_ft5);
    mtx->m[0][3] = 0.0f;
    mtx->m[0][2] = -temp_fs0 * sp34.y;
    mtx->m[1][0] = -sp34.y * sp34.x;
    mtx->m[1][1] = sp34.y * temp_fv0;
    mtx->m[1][2] = sp34.z;
    mtx->m[2][0] = (temp_fs0 * temp_fv0) + (temp_fs1 * temp_ft4);
    mtx->m[2][1] = (temp_fs0 * sp34.x) - (temp_fs1 * temp_ft5);
    mtx->m[2][2] = temp_fs1 * sp34.y;
    mtx->m[3][3] = 1.0f;
    mtx->m[1][3] = 0.0f;
    mtx->m[2][3] = 0.0f;
    mtx->m[3][0] = 0.0f;
    mtx->m[3][1] = 0.0f;
    mtx->m[3][2] = 0.0f;
}

extern f32 D_misc_00404238;
extern f32 D_misc_0040423C;
extern f32 D_misc_00404240;

void func_misc_00400E38(UnkStruct_misc_004006A0* arg0, f32* arg1, f32* arg2, f32* arg3, f32* arg4, f32* arg5, f32* arg6) {
    f32 len;
    f32 var_fv1;
    f32 sp2C;

    sp2C = arg0->unk18;
    len = gUvMathExports->uvSqrtf(SQ(arg0->unk14) + SQ(arg0->unk10));
    if (ABS_2(len) < D_misc_00404238) {
        if (sp2C < 0.0f) {
            *arg5 = D_misc_0040423C;
        } else {
            *arg5 = D_misc_00404240;
        }
        *arg4 = 0.0f;
        *arg6 = 0.0f;
    } else {
        f32 fa2;
        f32 fa1;

        *arg5 = gUvMathExports->uvAtan2F(sp2C, len);
        fa2 = arg0->unk14 / len;
        fa1 = -arg0->unk10 / len;
        *arg4 = gUvMathExports->uvAtan2F(fa1, fa2);
        fa2 = arg0->unk28 / len;
        fa1 = -arg0->unk8 / len;
        *arg6 = gUvMathExports->uvAtan2F(fa1, fa2);
    }
    *arg1 = arg0->unk30;
    *arg2 = arg0->unk34;
    *arg3 = arg0->unk38;
}

void func_misc_00400FB8(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, Mtx4F* arg6) {
    gUvFmtxExports->func_00400B68(arg6);
    gUvFmtxExports->func_00401000(arg6, arg3, 0x7A);
    gUvFmtxExports->func_00401000(arg6, arg4, 0x78);
    gUvFmtxExports->func_00401000(arg6, arg5, 0x79);
    arg6->m[3][0] = arg0;
    arg6->m[3][1] = arg1;
    arg6->m[3][2] = arg2;
}

void func_misc_00401078(void) {
    *(s32*)0 = 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00401080.s")

s32 func_misc_004012A4(s32 arg0, s32 arg1, s32 arg2) {
    if ((arg1 & arg2) && !(arg0 & arg2)) {
        return 1;
    }
    if (!(arg1 & arg2) && (arg0 & arg2)) {
        return -1;
    }
    return 0;
}


#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_004012E4.s")

f32 func_misc_0040142C(f32 arg0, f32 arg1, f32 arg2) {
    if (arg1 < arg0) {
        return arg1;
    }
    if (arg0 < arg2) {
        return arg2;
    }
    return arg0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_0040146C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00401528.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_004015A0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00401614.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_004016CC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_004017A8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_004017C4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_004017FC.s")

void func_misc_0040187C(void *arg0, void *arg1, f32 arg2, void *arg3)
{
  u8 *new_var;
  *((f32 *) (((u8 *) arg0) + 0x0)) = (f32) ((*((f32 *) (((u8 *) arg1) + 0x0))) + ((*((f32 *) (((u8 *) arg3) + 0x0))) * arg2));
  new_var = ((u8 *) arg0) + 0x4;
  *((f32 *) new_var) = (f32) ((*((f32 *) (((u8 *) arg1) + 0x4))) + ((*((f32 *) (((u8 *) arg3) + 0x4))) * arg2));
}

f32 func_misc_004018AC(void *arg0, void *arg1)
{
  float new_var;
  return (((*((f32 *) (((u8 *) arg1) + 0x4))) * (*((f32 *) (((u8 *) arg0) + 0x4)))) + ((*((f32 *) (((u8 *) arg0) + 0x0))) * (*((f32 *) (((u8 *) arg1) + 0x0))))) * (new_var = 1.f);
}

void func_misc_004018D0(void *arg0, void *arg1, void *arg2)
{
  *((f32 *) (((u8 *) arg0) + 0x0)) = (f32) ((*((f32 *) (((u8 *) arg1) + 0x0))) + (*((f32 *) (((u8 *) arg2) + 0x0))));
  *((f32 *) (((u8 *) arg0) + 0x4)) = (f32) ((*((f32 *) (((u8 *) arg1) + 0x4))) + (*((f32 *) (((u8 *) arg2) + 0x4))));
}

void func_misc_004018F4(void *arg0, void *arg1, void *arg2) {
    (*(f32 *)((u8 *)arg0 + 0x0)) = (f32) ((*(f32 *)((u8 *)arg1 + 0x0)) - (*(f32 *)((u8 *)arg2 + 0x0)));
    (*(f32 *)((u8 *)arg0 + 0x4)) = (f32) ((*(f32 *)((u8 *)arg1 + 0x4)) - (*(f32 *)((u8 *)arg2 + 0x4)));
}

void func_misc_00401918(void *arg0, f32 arg1, void *arg2) {
    (*(f32 *)((u8 *)arg0 + 0x0)) = (f32) ((*(f32 *)((u8 *)arg2 + 0x0)) * arg1);
    (*(f32 *)((u8 *)arg0 + 0x4)) = (f32) ((*(f32 *)((u8 *)arg2 + 0x4)) * arg1);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00401938.s")

void func_misc_0040197C(void *arg0, s32 arg1) {
    *(f32 *)((u8 *)arg0 + 0x0) = *(f32 *)((u8 *)arg1 + 0x0);
    *(f32 *)((u8 *)arg0 + 0x4) = *(f32 *)((u8 *)arg1 + 0x4);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00401990.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_004019FC.s")

void func_misc_00401E14(void *arg0, void *arg1, void *arg2) {
    (*(f32 *)((u8 *)arg0 + 0x0)) = (f32) ((*(f32 *)((u8 *)arg1 + 0x0)) + (*(f32 *)((u8 *)arg2 + 0x0)));
    (*(f32 *)((u8 *)arg0 + 0x4)) = (f32) ((*(f32 *)((u8 *)arg1 + 0x4)) + (*(f32 *)((u8 *)arg2 + 0x4)));
    (*(f32 *)((u8 *)arg0 + 0x8)) = (f32) ((*(f32 *)((u8 *)arg1 + 0x8)) + (*(f32 *)((u8 *)arg2 + 0x8)));
}

void func_misc_00401E48(void *arg0, void *arg1, void *arg2) {
    (*(f32 *)((u8 *)arg0 + 0x0)) = (f32) ((*(f32 *)((u8 *)arg1 + 0x0)) - (*(f32 *)((u8 *)arg2 + 0x0)));
    (*(f32 *)((u8 *)arg0 + 0x4)) = (f32) ((*(f32 *)((u8 *)arg1 + 0x4)) - (*(f32 *)((u8 *)arg2 + 0x4)));
    (*(f32 *)((u8 *)arg0 + 0x8)) = (f32) ((*(f32 *)((u8 *)arg1 + 0x8)) - (*(f32 *)((u8 *)arg2 + 0x8)));
}

void func_misc_00401E7C(void *arg0, f32 arg1, void *arg2) {
    (*(f32 *)((u8 *)arg0 + 0x0)) = (f32) ((*(f32 *)((u8 *)arg2 + 0x0)) * arg1);
    (*(f32 *)((u8 *)arg0 + 0x4)) = (f32) ((*(f32 *)((u8 *)arg2 + 0x4)) * arg1);
    (*(f32 *)((u8 *)arg0 + 0x8)) = (f32) ((*(f32 *)((u8 *)arg2 + 0x8)) * arg1);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00401EA8.s")

void func_misc_00401F2C(void *arg0, s32 arg1) {
    *(f32 *)((u8 *)arg0 + 0x0) = *(f32 *)((u8 *)arg1 + 0x0);
    *(f32 *)((u8 *)arg0 + 0x4) = *(f32 *)((u8 *)arg1 + 0x4);
    *(f32 *)((u8 *)arg0 + 0x8) = *(f32 *)((u8 *)arg1 + 0x8);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00401F48.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00401FA0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_0040213C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_0040234C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00402528.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00402698.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00402730.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00402874.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_004029DC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00402A5C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00402BFC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00402D48.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00402E94.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00402EFC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00403000.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00403110.s")

void func_misc_00403300(void) {
    uvLoadModule('filr');
}

void func_misc_00403324(void) {
    uvUnloadModule('filr');
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00403348.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00403650.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00403748.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_004038D8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00403A5C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00403BEC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00403CA4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00403CD8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_00403D8C.s")

void func_misc_004041A8(s32 arg0) {
    D_misc_00404300 = arg0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/misc/func_misc_004041B4.s")
