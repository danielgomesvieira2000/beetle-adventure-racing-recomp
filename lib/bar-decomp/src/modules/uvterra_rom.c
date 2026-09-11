// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 D_uvterra_rom_0040FBF0;
extern s32 D_uvterra_rom_0040FBF4;
extern u8 D_uvterra_rom_0040F7A0[];
#include "module.h"
#include "uvasset_types.h"
extern f32 D_uvterra_rom_0040D6E4;
extern s32 D_uvterra_rom_0040FBA4;
extern u16 D_uvterra_rom_0040D718;
// uvchan_rom exports
typedef struct UnkStruct_uvterra_rom_0040F760_s {
    /* 0x00 */ char pad0[4];
    /* 0x04 */ void (*unk4)(s32, s32, f32 *, s32); /* inferred */
    /* 0x08 */ char pad8[0x24];                    /* maybe part of unk4[0xA]? */
    /* 0x2C */ s32 (*unk2C)(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4); /* inferred */
} UnkStruct_uvterra_rom_0040F760;                                              /* size = 0x30 */

typedef struct UnkStruct_uvterra_rom_0040F768_s {
    /* 0x00 */ char pad0[0x18];
    /* 0x18 */ s32 (*unk18)(Vtx*, u16, u16, u16, void*, f32); /* inferred */
} UnkStruct_uvterra_rom_0040F768;                   /* size = 0x1C */


/*
 * D_uvterra_rom_0040F760 = uvLoadModule('CHAN');
 * D_uvterra_rom_0040F770 = uvLoadModule(0x534F5254);
 * D_uvterra_rom_0040F74C = uvLoadModule(0x494D5458);
 * D_uvterra_rom_0040F744 = uvLoadModule(0x464D5458);
 * D_uvterra_rom_0040F748 = uvLoadModule(0x46564543);
 * D_uvterra_rom_0040F750 = uvLoadModule(0x53544154);
 * D_uvterra_rom_0040F754 = uvLoadModule(0x474D4752);
 * D_uvterra_rom_0040F75C = uvLoadModule('MATH');
 * D_uvterra_rom_0040F758 = uvLoadModule(0x4D4F444C);
 * D_uvterra_rom_0040F764 = uvLoadModule('CBCK');
 * D_uvterra_rom_0040F768 = uvLoadModule('ISCT');
 * D_uvterra_rom_0040F76C = uvLoadModule('QERY');
 */

u16 func_uvterra_rom_0040B0C4(s16 arg0, s16 arg1, s32 arg2);
void func_uvterra_rom_0040B26C(s16 *arg0, s16 *arg1, s16 *arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6,
                               f32 arg7, f32 arg8, f32 arg9, u8 arg10, f32 arg11, f32 arg12);
void *func_uvterra_rom_0040C0F8(s32);
void func_uvterra_rom_0040C218(s32);
s32 func_uvterra_rom_0040B8C0(void *arg0, f32 arg1, f32 arg2, f32 arg3, f32 *arg4, f32 *arg5, f32 *arg6, s16 *arg7, s16 *arg8, u8 arg9);

extern UvCback_Exports *D_uvterra_rom_0040F764;
extern s32 *D_uvterra_rom_0040F77C;
extern UvMath_Exports *D_uvterra_rom_0040F75C;
extern f32 D_uvterra_rom_0040D6A8;
extern s32 D_uvterra_rom_0040F740;
extern s32 D_uvterra_rom_0040FBEC;
extern UnkStruct_uvterra_rom_0040F768* D_uvterra_rom_0040F768;

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/__entrypoint_func_uvterra_rom_400000.s")

void func_uvterra_rom_004005A4(void) {
    uvUnloadModule('IMTX');
    uvUnloadModule('FMTX');
    uvUnloadModule('FVEC');
    uvUnloadModule('STAT');
    uvUnloadModule('GMGR');
    uvUnloadModule('MATH');
    uvUnloadModule('MODL');
    uvUnloadModule('CHAN');
    uvUnloadModule('CBCK');
    uvUnloadModule('ISCT');
    uvUnloadModule('SORT');
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00400640.s")

void func_uvterra_rom_004006E0(s32 arg0, s32 arg1) {
    if ((arg1 == 0xFFFF) || (arg1 == -1)) {
        D_uvterra_rom_0040F77C[arg0] = 0xFFFF;
    } else if (uvGetLoadedFile('UVTR', arg1) == NULL) {
        D_uvterra_rom_0040F77C[arg0] = 0xFFFF;
    } else {
        D_uvterra_rom_0040F77C[arg0] = arg1;
    }
}

s32 func_uvterra_rom_00400774(s32 arg0) {
    return D_uvterra_rom_0040F77C[arg0];
}

// _uvTerraDraw
#ifdef NEEDS_RODATA
void func_uvterra_rom_0040078C(s32 arg0) {
    s32 var_v0;
    s32 sp1A8;
    s32 sp1A4;
    s32 var_v1;
    s32 sp19C;
    s32 var_t2;
    s32 var_t3;
    s32 var_t4;
    s32 var_t5;
    f32 temp_fs2;
    f32 temp_fs3;
    f32 x;
    f32 y;
    f32 z;
    f32 temp_fs0;
    f32 temp_fs1;
    f32 temp_fa0;
    f32 temp_fv1;
    f32 temp_fv0;
    Vec3F sp158;
    Vec3F sp14C;
    f32 temp_fs2_2;
    f32 temp_fs3_2;
    f32 temp_fs4;
    f32 temp_fs5;
    f32 temp_fa1;
    f32 var_ft4;
    f32 var_fv0;
    u16 temp_s0;
    u16 temp_v0_6;
    u16 var_s6;
    u16 temp_v0;
    s32 temp_t0;
    f32 sp110[5];
    f32 spFC[5];
    f32 spE8[5];
    u8 spE0[5];
    u16 spDE;
    ParsedUVTR_inner *temp_fp;
    UnkStruct_uvterra_rom_0040FBE8 *temp_s0_2;
    UnkStruct_uvterra_rom_0040078C *temp_v0_2;
    s16 spCE;
    s16 spCC;
    ParsedUVTR *spC8;
    s32 i;
    f32 temp_ft0;
    UnkStruct_uvterra_rom_0040F774 *spBC;

    spBC = &D_uvterra_rom_0040F774[arg0];
    D_uvterra_rom_0040F740 = 0;
    if (D_uvterra_rom_0040F77C[arg0] == 0xFFFF) {
        return;
    }

    spC8 = uvGetLoadedFile('UVTR', D_uvterra_rom_0040F77C[arg0]);
    if (spC8 == NULL) {
        return;
    }

    func_uvterra_rom_0040C218(arg0);
    temp_v0_2 = func_uvterra_rom_0040C0F8(arg0);
    temp_fs2 = spC8->unk1C;
    temp_fs3 = spC8->unk20;
    var_t2 = (s32) ((temp_v0_2->unk0 - spC8->unk0.unk0) / temp_fs2);
    var_t3 = (s32) ((temp_v0_2->unkC - spC8->unk0.unk0) / temp_fs2);
    var_t4 = (s32) ((temp_v0_2->unk4 - spC8->unk0.unk4) / temp_fs3);
    var_t5 = (s32) ((temp_v0_2->unk10 - spC8->unk0.unk4) / temp_fs3);

    if (var_t2 < 0) {
        var_t2 = 0;
    }

    if ((spC8->unk18 - 1) < var_t3) {
        var_t3 = spC8->unk18 - 1;
    }
    if (var_t4 < 0) {
        var_t4 = 0;
    }

    if ((spC8->unk19 - 1) < var_t5) {
        var_t5 = spC8->unk19 - 1;
    }

    for (i = 0; i < spBC->unk48; i++) {
        temp_fv1 = spBC->unk0[i].x;
        temp_fa1 = spBC->unk0[i].y;
        if (i < (spBC->unk48 - 1)) {
            var_fv0 = spBC->unk0[i + 1].x;
            var_ft4 = spBC->unk0[i + 1].y;
        } else {
            var_fv0 = spBC->unk0[0].x;
            var_ft4 = spBC->unk0[0].y;
        }

        temp_fa0 = var_fv0 - temp_fv1;
        temp_ft0 = var_ft4 - temp_fa1;

        if (temp_fa0 == 0.0f) {
            sp110[i] = 2140143600.0f;
            spE8[i] = 0.0f;
            spFC[i] = temp_fv1;
            if (temp_ft0 > 0.0f) {
                spE0[i] = 1;
            } else {
                spE0[i] = 0;
            }
        } else {
            temp_fv0 = (var_ft4 - temp_fa1) / temp_fa0;
            if (temp_fa0 > 0.0f) {
                spE0[i] = 1;
            } else {
                spE0[i] = 0;
            }
            sp110[i] = temp_fv0;
            spE8[i] = 1.0f / temp_fv0;
            spFC[i] = temp_fa1 - (temp_fv0 * temp_fv1);
        }
    }

    D_uvterra_rom_0040F760->unk4(arg0, 7, &sp158, 0);
    D_uvterra_rom_0040F760->unk4(arg0, 8, &sp14C, 0);

    for (sp1A8 = var_t2; sp1A8 <= var_t3; sp1A8++) {
        for (sp1A4 = var_t4; sp1A4 <= var_t5; sp1A4++) {
            if ((D_uvterra_rom_0040FBEC != 0) && (sp14C.x > 0.0f) ^ (D_uvterra_rom_0040FBEC == 2)) {
                var_v1 = (var_t3 - sp1A8) + var_t2;
            } else {
                var_v1 = sp1A8;
            }
            if ((D_uvterra_rom_0040FBEC != 0) && (sp14C.y > 0.0f) ^ (D_uvterra_rom_0040FBEC == 2)) {
                var_v0 = (var_t5 - sp1A4) + var_t4;
            } else {
                var_v0 = sp1A4;
            }
            sp19C = (spC8->unk18 * var_v0) + var_v1;
            temp_fp = spC8->unk28 + sp19C;
            if (temp_fp == NULL) {
                continue;
            }

            if (temp_fp->unk40 == NULL) {
                continue;
            }

            x = (temp_fp->unk30 + temp_fp->unk40->unk18);
            y = (temp_fp->unk34 + temp_fp->unk40->unk1C);
            z = (temp_fp->unk38 + temp_fp->unk40->unk20);
            if (!D_uvterra_rom_0040F760->unk2C(arg0, x - sp158.x, y - sp158.y, z - sp158.z, temp_fp->unk40->unk24)) {
                continue;
            }

            temp_fs0 = 15.0f / temp_fs2;
            temp_fs1 = 15.0f / temp_fs3;
            temp_fs2_2 = temp_fp->unk30 - (0.5f * temp_fs2);
            temp_fs3_2 = temp_fp->unk30 + (0.5f * temp_fs2);
            temp_fs4 = temp_fp->unk34 + (0.5f * temp_fs3);
            temp_fs5 = temp_fp->unk34 - (0.5f * temp_fs3);

            var_s6 = 0;
            spDE = 0xFFFF;
            for (i = 0; i < spBC->unk48; i++) {
                func_uvterra_rom_0040B26C(&spCE, &spCC, &spDE, temp_fs2_2, temp_fs3_2, temp_fs4,
                                          temp_fs5, spFC[i], sp110[i], spE8[i], (s32) spE0[i],
                                          temp_fs0, temp_fs1);
                if (spCE >= 0) {
                    temp_s0 = func_uvterra_rom_0040B0C4(spCE, spCC, 0);
                    temp_v0_6 = func_uvterra_rom_0040B0C4(spCE, spCC, 1);
                    var_s6 |= temp_s0 & temp_v0_6;
                    if (spE0[i] != 0) {
                        spDE &= temp_v0_6;
                    } else {
                        spDE &= temp_s0;
                    }
                }

                if (spDE == 0) {
                    break;
                }
            }

            temp_v0 = spDE & ~var_s6;
            if (spDE) {
                temp_s0_2 = &D_uvterra_rom_0040FBE8[D_uvterra_rom_0040F740];
                temp_s0_2->unk0 = temp_fp;
                temp_s0_2->unk6 = spDE;
                temp_s0_2->unk4 = temp_v0;
                temp_s0_2->unkC = sp19C;

                temp_s0_2->unk8 =
                    D_uvterra_rom_0040F75C->uvSqrtf(SQ(sp158.z - temp_fp->unk38) + (SQ(sp158.x - temp_fp->unk30) + SQ(sp158.y - temp_fp->unk34)));
                D_uvterra_rom_0040F740++;
            }
        }
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040078C.s")
#endif

// uvTerraGetPlane
void func_uvterra_rom_00400E70(s32 arg0, u32 arg1, f32 arg2, f32 arg3, f32* arg4, Vec3F* arg5) {
    f32 temp_ft5;
    uvUnkTileStruct* sp60;
    ParsedUVTR* temp_v0;
    Unk80225FBC_0x28_UnkC* sp58;
    Vtx* sp54;
    f32 sp50;
    f32 sp4C;
    f32 sp48;
    s16 sp46;
    s16 sp44;
    Unk80225FBC_0x28* v1;
    f32 temp_fa0;
    f32 temp_fa1;
    f32 temp_fv0;

    if (arg1 == -1) {
        return;
    }

    temp_v0 = uvGetLoadedFile('UVTR', arg0);
    if (temp_v0 == NULL) {
        return;
    }

    sp60 = &temp_v0->unk28[(arg1 >> 0x16) & 0x3FF];
    v1 = &sp60->unk40->unk8[(arg1 >> 0xC) & 0x3FF];
    sp58 = &v1->unk18[arg1 & 0xFFF];
    sp54 = sp60->unk40->vtxTable;

    func_uvterra_rom_0040B8C0(temp_v0, arg2, arg3, 0.0f, &sp50, &sp4C, &sp48, &sp46, &sp44, 0U);
    sp50 *= sp60->unk40->unk2C;
    sp4C *= sp60->unk40->unk2C;
    sp48 *= sp60->unk40->unk30;
    temp_fv0 = (sp60->unk40->unk2C / sp60->unk40->unk30);
    if (D_uvterra_rom_0040F768->unk18(sp54, sp58->unk0, sp58->unk2, sp58->unk4, arg5, temp_fv0) == 0) {
        arg5->x = 0.0f;
        arg5->y = 0.0f;
        arg5->z = 1.0f;
    }

    temp_ft5 = sp54[sp58->unk0].v.ob[0];
    temp_fa0 = sp54[sp58->unk0].v.ob[1];
    temp_fa1 = sp54[sp58->unk0].v.ob[2];
    if (arg5->z != 0.0f) {
        *arg4 = (((arg5->x * (temp_ft5 - sp50)) + (arg5->y * (temp_fa0 - sp4C))) /
             ( arg5->z * temp_fv0)) + temp_fa1;
        *arg4 /= sp60->unk40->unk30;
        *arg4 += sp60->unk0.m[3][2];
    }
}


void func_uvterra_rom_004010B4(s32 arg0, u32 arg1, Vec3F* arg2) {
    ParsedUVCT* temp_v0_2;
    ParsedUVTR* temp_v0;
    Unk80225FBC_0x28 *t;
    uvUnkTileStruct* t2;
    Unk80225FBC_0x28_UnkC* temp_v1;

    if (arg1 == -1U) {
        return;
    }

    temp_v0 = uvGetLoadedFile(0x55565452, arg0);
    if (temp_v0 == NULL) {
        return;
    }

    t2 = &temp_v0->unk28[(arg1 >> 0x16) & 0x3FF];
    temp_v0_2 = t2->unk40;
    t =  &temp_v0_2->unk8[(arg1 >> 0xC) & 0x3FF];
    temp_v1 = &t->unk18[arg1 & 0xFFF];

    if (D_uvterra_rom_0040F768->unk18(temp_v0_2->vtxTable, temp_v1->unk0, temp_v1->unk2, temp_v1->unk4, arg2, temp_v0_2->unk2C / temp_v0_2->unk30) == 0) {
        arg2->x = 0.0f;
        arg2->y = 0.0f;
        arg2->z = 0.0f;
    }
}

s32 func_uvterra_rom_004011A4(s32 arg0, u32 arg1) {
    ParsedUVCT* temp_a1;
    ParsedUVTR* temp_v0;
    s32 temp_t6;
    uvUnkTileStruct* temp_a0;
    Unk80225FBC_0x28 *t;

    if (arg1 == -1U) {
        return 0xFFF;
    }
    temp_v0 = uvGetLoadedFile(0x55565452, arg0);
    if (temp_v0 == NULL) {
        return 0xFFF;
    }
    temp_t6 = (arg1 >> 0x16) & 0x3FF;
    if (temp_t6 >= (temp_v0->unk18 * temp_v0->unk19)) {
        return 0xFFF;
    }
    temp_a0 = &temp_v0->unk28[temp_t6];
    if (temp_a0 == NULL) {
        return 0xFFF;
    }
    temp_a1 = temp_a0->unk40;
    if (temp_a1 == NULL) {
        return 0xFFF;
    }
    t = &temp_a1->unk8[(arg1 >> 0xC) & 0x3FF];
    return t->unk0.state;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00401284.s")

void *func_uvterra_rom_00401E28(s32 arg0) {
    void *temp_v0;

    temp_v0 = uvGetLoadedFile('UVTR', arg0);
    if (temp_v0 == NULL) {
        return NULL;
    }
    return temp_v0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00401E64.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00402244.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00403270.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00403464.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00403630.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_004037B8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040385C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00404E14.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_004060A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00406160.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00406324.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00406648.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00406C60.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_004071A4.s")

void func_uvterra_rom_004072BC(s32 arg0) {
    D_uvterra_rom_0040FBF0 = arg0;
}

s32 func_uvterra_rom_004072C8(void) {
    return D_uvterra_rom_0040FBF0;
}

void func_uvterra_rom_004072D4(s32 arg0) {
    D_uvterra_rom_0040FBF4 = arg0;
}

s32 func_uvterra_rom_004072E0(void) {
    return D_uvterra_rom_0040FBF4;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_004072EC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_004074EC.s")

void func_uvterra_rom_00407680(s32 arg0, f32 arg1)
{
  s32 base = D_uvterra_rom_0040FBA4;
  *((f32 *) (((u8 *) D_uvterra_rom_0040FBA4) + (arg0 * 4))) = 1.0f / arg1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_004076A8.s")

s32 func_uvterra_rom_00407A80(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp0;
    s32 sp4;
    s32 sp8;
    sp0 = arg0;
    sp4 = arg1;
    sp8 = arg2;
    return 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00407A94.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00407B0C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00407E5C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00407ED4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00408300.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00408534.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00408614.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_004087D8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040880C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040888C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00408AD8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_00409394.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040A08C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040A43C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040A79C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040AB14.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040ABB4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040ABE0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040AC14.s")

u8 func_uvterra_rom_0040AC8C(s32 arg0) {
    return D_uvterra_rom_0040F7A0[arg0];
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040AC9C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040AE4C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040AFD8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040B0C4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040B26C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040B8C0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040BBC0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040BD38.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040BF74.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040C08C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040C0F8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040C218.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040C490.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040C50C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040C998.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040CA64.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040CEA8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040CF98.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040D050.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvterra_rom/func_uvterra_rom_0040D128.s")
