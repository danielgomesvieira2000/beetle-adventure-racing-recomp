// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "uvquat_rom.h"

#define alloca(size) __builtin_alloca(size)

/*
 * D_uvjanim_rom_00401760 = uvLoadModule(0x4D415448, arg0);
 * D_uvjanim_rom_00401764 = uvLoadModule(0x4D4F444C);
 * D_uvjanim_rom_00401768 = uvLoadModule(0x444F424A);
 * D_uvjanim_rom_0040176C = uvLoadModule(0x464D5458);
 * D_uvjanim_rom_00401770 = uvLoadModule(0x51554154)
 */

typedef struct UnkModelExports_s {
    char pad[0x14];
    void (*unk14)(s32, s32, Mtx4F *);
} UnkModelExports;

extern UvQuat_Exports *D_uvjanim_rom_00401770;
extern UnkDobjExports *D_uvjanim_rom_00401768;
extern UnkModelExports *D_uvjanim_rom_00401764;
extern UvFMtx_Rom_Exports *D_uvjanim_rom_0040176C;
extern UvMath_Exports *D_uvjanim_rom_00401760;

void func_uvjanim_rom_004000F0(void);
void func_uvjanim_rom_00400144(UnkStruct_UvJanim_rom_00400144 *arg0,
                               UnkStruct_UvJanim_rom_00400144 *arg1);
void func_uvjanim_rom_00400278(s32 arg0, UnkStruct_UvJanim_rom_00400144 *arg1);
void func_uvjanim_rom_00400C0C(s32 arg0, s32 arg1, f32 arg2);
void func_uvjanim_rom_00400CFC(UnkStruct_UvJanim_rom_00400144 *arg0, s32 arg1, f32 arg2);
void func_uvjanim_rom_00401288(s32 arg0, s32 arg1, f32 arg2, f32 arg3);
void func_uvjanim_rom_00401380(UnkStruct_UvJanim_rom_00400144 *arg0, s32 arg1, f32 arg2, f32 arg3);
void func_uvjanim_rom_00401704(s32 arg0, UnkStruct_uvjanim_rom_00401704 *arg1);
void func_uvjanim_rom_00400CFC(UnkStruct_UvJanim_rom_00400144 *, s32, f32); /* extern */
void func_uvjanim_rom_00400434(UnkStruct_UvJanim_rom_00400144 *arg0,
                               UnkStruct_UvJanim_rom_00400144 *arg1,
                               UnkStruct_UvJanim_rom_00400144 *arg2, f32 arg3);

void __entrypoint_func_uvjanim_rom_400000(UvJanim_Exports *exports);

void __entrypoint_func_uvjanim_rom_400000(UvJanim_Exports *exports) {
    uvUpdateFileAllocPtr(exports);
    exports->func_uvjanim_rom_004000F0 = func_uvjanim_rom_004000F0;
    exports->func_uvjanim_rom_00400144 = func_uvjanim_rom_00400144;
    exports->func_uvjanim_rom_00400278 = func_uvjanim_rom_00400278;
    exports->func_uvjanim_rom_00400434 = func_uvjanim_rom_00400434;
    exports->func_uvjanim_rom_00400C0C = func_uvjanim_rom_00400C0C;
    exports->func_uvjanim_rom_00400CFC = func_uvjanim_rom_00400CFC;
    exports->func_uvjanim_rom_00401288 = func_uvjanim_rom_00401288;
    exports->func_uvjanim_rom_00401380 = func_uvjanim_rom_00401380;
    exports->func_uvjanim_rom_00401704 = func_uvjanim_rom_00401704;
#line 45
    D_uvjanim_rom_00401760 = uvLoadModule('MATH');
    D_uvjanim_rom_00401764 = uvLoadModule('MODL');
    D_uvjanim_rom_00401768 = uvLoadModule('DOBJ');
    D_uvjanim_rom_0040176C = uvLoadModule('FMTX');
    D_uvjanim_rom_00401770 = uvLoadModule('QUAT');
}

void func_uvjanim_rom_004000F0(void) {
    uvUnloadModule('MATH');
    uvUnloadModule('MODL');
    uvUnloadModule('DOBJ');
    uvUnloadModule('FMTX');
    uvUnloadModule('QUAT');
}

void func_uvjanim_rom_00400144(UnkStruct_UvJanim_rom_00400144 *arg0,
                               UnkStruct_UvJanim_rom_00400144 *arg1) {
    if (arg0->unk12 >= arg1->unk10) {
        if (arg1->unk0 != NULL) {
            if ((arg0->unk0 != NULL) && (arg1->unk11 & 1)) {
                _uvMediaCopy(arg0->unk0, arg1->unk0, arg1->unk10 * 0x10);
            }
        }
        if (arg1->unk4 != NULL) {
            if ((arg0->unk4 != NULL) && (arg1->unk11 & 2)) {
                _uvMediaCopy(arg0->unk4, arg1->unk4, arg1->unk10 * 0xC);
            }
        }
        if (arg1->unk8 != NULL) {
            if ((arg0->unk8 != NULL) && (arg1->unk11 & 4)) {
                _uvMediaCopy(arg0->unk8, arg1->unk8, arg1->unk10 * 0xC);
            }
        }
        if (arg1->unkC != NULL) {
            if (arg0->unkC != NULL) {
                _uvMediaCopy(arg0->unkC, arg1->unkC, (u32) arg1->unk10);
            }
        }
        arg0->unk11 = arg1->unk11;
        arg0->unk10 = arg1->unk10;
    }
}

void func_uvjanim_rom_00400278(s32 arg0, UnkStruct_UvJanim_rom_00400144 *arg1) {
    Mtx4F sp68;
    s32 i; // s4
    Quat *s0;
    s32 temp_v0;
    Vec3F *s1;
    Vec3F *s2;
    s32 s7;

    temp_v0 = D_uvjanim_rom_00401768->unk18();
    s0 = arg1->unk0;
    s1 = arg1->unk4;
    s2 = arg1->unk8;
    s7 = arg1->unk10;

    for (i = 0; i < s7; i++) {
        D_uvjanim_rom_00401764->unk14(temp_v0, arg1->unkC[i], &sp68);
        if ((arg1->unk11 & 1) && (s0 != NULL)) {
            D_uvjanim_rom_0040176C->func_0040246C(&sp68, s0->x, s0->y, s0->z, s0->w);
            s0++;
        }

        if ((arg1->unk11 & 2) && (s1 != NULL)) {
            sp68.m[3][0] += s1->x;
            sp68.m[3][1] += s1->y;
            sp68.m[3][2] += s1->z;
            s1++;
        }
        if ((arg1->unk11 & 4) && (s2 != NULL)) {
            D_uvjanim_rom_0040176C->func_004014D0(&sp68, s2->x, s2->y, s2->z);
            s2++;
            // FAKE
            if (s7)
                ;
        }
        // FAKE
        if (&D_uvjanim_rom_00401768)
            ;

        D_uvjanim_rom_00401768->unk24(arg0, arg1->unkC[i], &sp68);
    }
}

void func_uvjanim_rom_00400434(UnkStruct_UvJanim_rom_00400144 *arg0,
                               UnkStruct_UvJanim_rom_00400144 *arg1,
                               UnkStruct_UvJanim_rom_00400144 *arg2, f32 arg3) {
    Vec3F *temp_a1;
    f32 var_fv0;
    s32 var_t4;
    s32 var_t0;
    s32 var_t3;
    s32 var_t1;
    s32 var_t2;
    Vec3F *temp_a2;
    Vec3F *temp_a3;
    Vec3F *var_a0;
    Vec3F *var_v0;
    Vec3F *var_v1;
    f32 var_fa1;
    Quat *var_a0_3;
    Quat *var_a1;
    Quat *var_a2;

    var_t4 = 0;
    if (arg3 > 1.0f) {
        var_fv0 = 1.0f;
    } else {
        var_fv0 = arg3;
    }
    if (var_fv0 < 0.0f) {
        var_fv0 = 0.0f;
    }
    var_fa1 = 1.0f - var_fv0;
    arg0->unk11 = 0;
    if (arg0->unk4 != NULL) {
        if (arg1->unk4 != NULL) {
            if (arg2->unk4 != NULL) {
                var_v0 = arg0->unk4;
                var_v1 = arg1->unk4;
                var_a0 = arg2->unk4;
                if (arg1->unk11 & arg2->unk11 & 2) {
                    var_t1 = arg1->unk10;
                    var_t3 = 0;
                    var_t0 = 0;
                    var_t2 = arg2->unk10;
                    while (((var_t1 != 0) || (var_t2 != 0)) && (var_t4 < arg0->unk12)) {
                        if ((var_t1 != 0) && (var_t2 != 0)
                            && (arg1->unkC[var_t0] == arg2->unkC[var_t3])) {
                            var_v0->x = ((var_a0->x * var_fv0) + (var_v1->x * var_fa1));
                            var_v0->y = ((var_a0->y * var_fv0) + (var_v1->y * var_fa1));
                            var_v0->z = ((var_a0->z * var_fv0) + (var_v1->z * var_fa1));
                            var_v0++;
                            var_t1--;
                            var_v1++;
                            var_t3++;
                            var_t2--;
                            var_a0++;
                            arg0->unkC[var_t4++] = arg1->unkC[var_t0];
                            var_t0++;
                        } else if ((var_t2 != 0)
                                   && ((var_t1 == 0)
                                       || ((s32) arg2->unkC[var_t3] < (s32) arg1->unkC[var_t0]))) {
                            var_t2--;
                            if (var_fv0 > 0.0f) {
                                var_v0->x = var_a0->x;
                                var_v0->y = var_a0->y;
                                var_v0->z = var_a0->z;
                                var_v0++;
                                arg0->unkC[var_t4++] = arg2->unkC[var_t3];
                            }
                            var_t3++;
                            var_a0++;
                        } else if ((var_t1 != 0)
                                   && ((var_t2 == 0)
                                       || ((s32) arg1->unkC[var_t0] < (s32) arg2->unkC[var_t3]))) {
                            var_t1--;
                            if (var_fa1 > 0.0f) {
                                var_v0->x = var_v1->x;
                                var_v0->y = var_v1->y;
                                var_v0->z = var_v1->z;
                                var_v0++;
                                arg0->unkC[var_t4++] = arg1->unkC[var_t0];
                            }

                            var_t0++;
                            var_v1++;
                        }
                    }
                    arg0->unk11 |= 2;
                }
            }
        }
    }
    if (arg0->unk8 != NULL) {
        if (arg1->unk8 != NULL) {
            if (arg2->unk8 != NULL) {
                var_v0 = arg0->unk8;
                var_v1 = arg1->unk8;
                var_a0 = arg2->unk8;
                if (arg1->unk11 & arg2->unk11 & 4) {
                    var_t1 = arg1->unk10;
                    var_t3 = 0;
                    var_t0 = 0;
                    var_t4 = 0;
                    var_t2 = arg2->unk10;
                    while (((var_t1 != 0) || (var_t2 != 0)) && (var_t4 < arg0->unk12)) {
                        if ((var_t1 != 0) && (var_t2 != 0)
                            && (arg1->unkC[var_t0] == arg2->unkC[var_t3])) {
                            var_v0->x = ((var_a0->x * var_fv0) + (var_v1->x * var_fa1));
                            var_v0->y = ((var_a0->y * var_fv0) + (var_v1->y * var_fa1));
                            var_v0->z = ((var_a0->z * var_fv0) + (var_v1->z * var_fa1));
                            var_v0++;
                            var_t1--;
                            var_v1++;
                            var_t3++;
                            var_t2--;
                            var_a0++;
                            arg0->unkC[var_t4++] = arg1->unkC[var_t0];
                            var_t0++;
                        } else if ((var_t2 != 0)
                                   && ((var_t1 == 0) || (arg2->unkC[var_t3] < arg1->unkC[var_t0]))) {
                            var_t2--;
                            if (var_fv0 > 0.0f) {
                                var_v0->x = var_a0->x;
                                var_v0->y = var_a0->y;
                                var_v0->z = var_a0->z;
                                var_v0++;
                                arg0->unkC[var_t4++] = arg2->unkC[var_t3];
                            }
                            var_t3++;
                            var_a0++;
                        } else if ((var_t1 != 0)
                                   && ((var_t2 == 0) || (arg1->unkC[var_t0] < arg2->unkC[var_t3]))) {
                            var_t1 -= 1;
                            if (var_fa1 > 0.0f) {
                                var_v0->x = var_v1->x;
                                var_v0->y = var_v1->y;
                                var_v0->z = var_v1->z;
                                var_v0++;
                                arg0->unkC[var_t4++] = arg1->unkC[var_t0];
                            }
                            var_t0++;
                            var_v1++;
                        }
                    }
                    arg0->unk11 |= 4;
                }
            }
        }
    }
    if (arg0->unk0 != NULL) {
        if (arg1->unk0 != NULL) {
            if (arg2->unk0 != NULL) {
                var_a0_3 = arg0->unk0;
                var_a1 = arg1->unk0;
                var_a2 = arg2->unk0;
                if (arg1->unk11 & arg2->unk11 & 1) {
                    var_t1 = arg1->unk10;
                    var_t3 = 0;
                    var_t0 = 0;
                    var_t4 = 0;
                    var_t2 = arg2->unk10;
                    while (((var_t1 != 0) || (var_t2 != 0)) && (var_t4 < arg0->unk12)) {
                        if ((var_t1 != 0) && (var_t2 != 0)
                            && (arg1->unkC[var_t0] == arg2->unkC[var_t3])) {
                            D_uvjanim_rom_00401770->func_uvquat_rom_004009BC(var_a0_3, var_a1, var_a2,
                                                                             var_fv0);
                            var_a0_3++;
                            var_a1++;
                            var_a2++;
                            var_t1--;
                            var_t2--;
                            var_t3++;
                            arg0->unkC[var_t4++] = arg1->unkC[var_t0];
                            var_t0++;
                        } else if ((var_t2 != 0)
                                   && ((var_t1 == 0)
                                       || ((s32) arg2->unkC[var_t3] < (s32) arg1->unkC[var_t0]))) {
                            var_t2 -= 1;
                            if (var_fv0 > 0.0f) {
                                var_a0_3->x = var_a2->x;
                                var_a0_3->y = var_a2->y;
                                var_a0_3->z = var_a2->z;
                                var_a0_3->w = var_a2->w;
                                var_a0_3++;
                                arg0->unkC[var_t4++] = arg2->unkC[var_t3];
                            }
                            var_t3++;
                            var_a2++;
                        } else if ((var_t1 != 0)
                                   && ((var_t2 == 0)
                                       || ((s32) arg1->unkC[var_t0] < (s32) arg2->unkC[var_t3]))) {
                            var_t1 -= 1;
                            if (var_fa1 > 0.0f) {
                                var_a0_3->x = var_a1->x;
                                var_a0_3->y = var_a1->y;
                                var_a0_3->z = var_a1->z;
                                var_a0_3->w = var_a1->w;
                                var_a0_3++;
                                arg0->unkC[var_t4++] = arg1->unkC[var_t0];
                            }
                            var_t0++;
                            var_a1++;
                        }
                    }
                    arg0->unk11 |= 1;
                }
            }
        }
    }
    arg0->unk10 = var_t4;
}

void func_uvjanim_rom_00400C0C(s32 arg0, s32 arg1, f32 arg2) {
    UnkStruct_UvJanim_rom_00400144 sp34;
    UvParsedAN *temp_v0;

    temp_v0 = uvGetLoadedFile('UVAN', arg1);
    if (temp_v0 != NULL) {
        sp34.unk0 = alloca(sizeof(Quat) * temp_v0->unk9);
        sp34.unk4 = alloca(sizeof(Vec3F) * temp_v0->unk9);
        sp34.unk8 = alloca(sizeof(Vec3F) * temp_v0->unk9);
        sp34.unkC = alloca(sizeof(u8) * temp_v0->unk9);
        sp34.unk12 = temp_v0->unk9;
        sp34.unk10 = 0;
        sp34.unk11 = 7;
        func_uvjanim_rom_00400CFC(&sp34, arg1, arg2);
        func_uvjanim_rom_00400278(arg0, (UnkStruct_UvJanim_rom_00400144 *) &sp34);
    }
}

void func_uvjanim_rom_00400CFC(UnkStruct_UvJanim_rom_00400144 *arg0, s32 arg1, f32 arg2) {
    Quat *temp_s4;
    Quat *var_s0;
    Vec3F *var_s1;
    Vec3F *var_s2;
    UvParsedAN_2 *var_t4;
    s32 v0;
    UvParsedAN *parsedAnimationData;
    s32 var_a1;
    s32 var_a2;
    f32 sqrtLen;
    s32 a2;
    f32 temp_fv1_2;
    f32 var_fa0;
    f32 var_ft5;
    f32 var_fs0;
    f32 var_fv1;
    s32 pad;
    s32 temp_ra;
    s32 var_a3;
    s32 var_t0;
    s32 var_t3;
    s32 var_t1;
    u16 temp_t5;
    u8 *var_s6;
    Vec3F *v1;
    Vec3F *v2;
    Quat *q1;
    Quat *q2;

    parsedAnimationData = uvGetLoadedFile('UVAN', arg1);
    if (parsedAnimationData == NULL) {
        return;
    }

    arg0->unk11 = 0;
    sqrtLen = (parsedAnimationData->unk2 - 1);
    if (arg2 < 0.0f) {
        arg2 = (arg2 + 1.0f) - (f32) (s32) arg2;
    }
    if (arg2 > 1.0f) {
        arg2 -= (f32) (s32) arg2;
    }
    arg0->unk11 = parsedAnimationData->unk8[0];
    arg2 *= sqrtLen;
    arg0->unk10 = 0;
    var_t4 = parsedAnimationData->unkC;

    var_s6 = arg0->unkC;
    var_s0 = arg0->unk0;

    var_s1 = arg0->unk4;
    var_s2 = arg0->unk8;
    while (var_t4 != NULL) {
        temp_t5 = var_t4->unk6;
        temp_s4 = arg0->unk0;
        if (parsedAnimationData->unk2 == temp_t5) {
            var_a1 = (s32) arg2;
        } else {
            var_t1 = 0;
            var_a3 = 0;
            temp_ra = temp_t5 - 2;
            if (var_t4->unk8 != NULL) {
                var_a1 = var_t4->unk8[(s32) arg2];
            } else {
                var_t0 = temp_ra;
                while ((var_t0 >= var_a3) && (!var_t1)) {
                    var_t3 = (s32) (var_a3 + var_t0) / 2;
                    a2 = var_t4->unkC[var_t3 + 1];
                    if (((s32) arg2 >= var_t4->unkC[var_t3]) && ((s32) arg2 < a2)) {
                        var_t1 = TRUE;
                    } else if ((s32) arg2 < var_t4->unkC[var_t3]) {
                        var_t0 = var_t3 - 1;
                    } else {
                        var_a3 = var_t3 + 1;
                    }
                }

                if (var_t1) {
                    var_a1 = var_t3;
                } else {
                    var_a1 = temp_ra;
                }
            }
        }
        v0 = var_a1 + 1;
        if (temp_t5 == (0, v0 ^ 0)) { // FAKE: required to flip this comparission
            var_a2 = temp_t5 - 1;
        } else {
            var_a2 = v0;
        }
        var_fa0 = var_t4->unkC[var_a1];
        var_fv1 = var_t4->unkC[var_a2];
        if (var_fv1 == var_fa0) {
            var_ft5 = 1.0f;
            var_fs0 = 0.0f;
        } else {
            var_ft5 = (var_fv1 - arg2) * (1.0f / (var_fv1 - var_fa0));
            var_fs0 = 1.0f - var_ft5;
        }
        if ((var_t4->unk18 != 0) && (temp_s4 != NULL)) {
            q2 = &var_t4->unk18[var_a1];
            q1 = &var_t4->unk18[var_a2];
            var_s0->x = (q1->x * var_fs0) + (q2->x * var_ft5);
            var_s0->y = (q1->y * var_fs0) + (q2->y * var_ft5);
            var_s0->z = (q1->z * var_fs0) + (q2->z * var_ft5);
            var_s0->w = (q1->w * var_fs0) + (q2->w * var_ft5);
            if (var_t4->unk1C[var_a1] != 0) {
                sqrtLen = D_uvjanim_rom_00401760->uvSqrtf(
                    SQ(var_s0->w) + (SQ(var_s0->x) + SQ(var_s0->y) + SQ(var_s0->z)));
                if (sqrtLen != 0.0f) {
                    temp_fv1_2 = 1.0f / sqrtLen;
                    var_s0->x *= temp_fv1_2;
                    var_s0->y *= temp_fv1_2;
                    var_s0->z *= temp_fv1_2;
                    var_s0->w *= temp_fv1_2;
                }
            }
        } else if (temp_s4 != NULL) {
            var_s0->x = 0.0f;
            var_s0->y = 0.0f;
            var_s0->z = 0.0f;
            var_s0->w = 1.0f;
        }
        if ((var_t4->unk10 != 0) && (arg0->unk4 != NULL)) {
            v2 = &var_t4->unk10[var_a1];
            v1 = &var_t4->unk10[var_a2];
            var_s1->x = (v1->x * var_fs0) + (v2->x * var_ft5);
            var_s1->y = (v1->y * var_fs0) + (v2->y * var_ft5);
            var_s1->z = (v1->z * var_fs0) + (v2->z * var_ft5);
        } else if (arg0->unk4 != NULL) {
            var_s1->x = 0.0f;
            var_s1->y = 0.0f;
            var_s1->z = 0.0f;
        }
        if ((var_t4->unk14 != 0) && (arg0->unk8 != NULL)) {
            v2 = &var_t4->unk14[var_a1];
            v1 = &var_t4->unk14[var_a2];
            var_s2->x = (v1->x * var_fs0) + (v2->x * var_ft5);
            var_s2->y = (v1->y * var_fs0) + (v2->y * var_ft5);
            var_s2->z = (v1->z * var_fs0) + (v2->z * var_ft5);
        } else if (arg0->unk8 != NULL) {
            var_s2->x = 1.0f;
            var_s2->y = 1.0f;
            var_s2->z = 1.0f;
        }
        *var_s6++ = var_t4->unk4;
        var_s0++;
        var_s1++;
        var_s2++;
        arg0->unk10++;
        var_t4 = var_t4->unk0;
    }
}

void func_uvjanim_rom_00401288(s32 arg0, s32 arg1, f32 arg2, f32 arg3) {
    UnkStruct_UvJanim_rom_00400144 sp34;
    UvParsedAN *temp_v0;

    temp_v0 = uvGetLoadedFile('UVAN', arg1);
    if (temp_v0 == NULL) {
        return;
    }

    sp34.unk0 = __builtin_alloca(sizeof(Quat) * temp_v0->unk9);
    sp34.unk4 = __builtin_alloca(sizeof(Vec3F) * temp_v0->unk9);
    sp34.unk8 = __builtin_alloca(sizeof(Vec3F) * temp_v0->unk9);
    sp34.unkC = __builtin_alloca(sizeof(u8) * temp_v0->unk9);
    sp34.unk12 = temp_v0->unk9;
    sp34.unk10 = 0;
    sp34.unk11 = 7;
    func_uvjanim_rom_00401380(&sp34, arg1, arg2, arg3);
    func_uvjanim_rom_00400278(arg0, (UnkStruct_UvJanim_rom_00400144 *) &sp34);
}

// uvJanimPoseGrid
void func_uvjanim_rom_00401380(UnkStruct_UvJanim_rom_00400144 *arg0, s32 arg1, f32 arg2, f32 arg3) {
    UnkStruct_UvJanim_rom_00400144 sp6C;
    UnkStruct_UvJanim_rom_00400144 sp58;
    f32 sp54;
    f32 sp50;
    UvParsedAN *temp_v0;
    s32 pad;
    s32 var_a0;
    s32 var_a1;

    temp_v0 = uvGetLoadedFile('UVAN', arg1);
    if (temp_v0 == NULL) {
        return;
    }
    sp6C.unk0 = alloca(sizeof(Quat) * temp_v0->unk9);
    sp6C.unk4 = alloca(sizeof(Vec3F) * temp_v0->unk9);
    sp6C.unk8 = alloca(sizeof(Vec3F) * temp_v0->unk9);
    sp6C.unkC = alloca(sizeof(u8) * temp_v0->unk9);
    sp6C.unk12 = temp_v0->unk9;
    sp6C.unk10 = 0;
    sp6C.unk11 = 7;
    sp58.unk0 = alloca(sizeof(Quat) * temp_v0->unk9);
    sp58.unk4 = alloca(sizeof(Vec3F) * temp_v0->unk9);
    sp58.unk8 = alloca(sizeof(Vec3F) * temp_v0->unk9);
    sp58.unkC = alloca(sizeof(u8) * temp_v0->unk9);
    sp58.unk12 = temp_v0->unk9;
    sp58.unk10 = 0;
    sp58.unk11 = 7;

    if (arg2 < 0.0f) {
        arg2 = (arg2 + 1.0f) - (s32) arg2;
    }
    if (arg2 > 1.0f) {
        arg2 -= (s32) arg2;
    }
    if (arg3 < 0.0f) {
        arg3 = (arg3 + 1.0f) - (s32) arg3;
    }
    if (arg3 > 1.0f) {
        arg3 -= (s32) arg3;
    }

    var_a1 = temp_v0->unk6;
    arg2 *= var_a1 - 1;
    arg3 *= temp_v0->unk4 - 1;
    var_a1 = (s32) arg3;
    if (var_a1 > temp_v0->unk4 - 1) {
        var_a1 = temp_v0->unk4 - 1;
        var_a0 = temp_v0->unk4 - 1;
    } else {
        if (var_a1 < temp_v0->unk4 - 1) {
            var_a0 = var_a1 + 1;
        } else {
            var_a0 = var_a1;
        }
    }
    sp54 = (f32) (temp_v0->unk6 * var_a1) + arg2;
    sp50 = (f32) (temp_v0->unk6 * var_a0) + arg2;

    if (temp_v0->unk2 < 2) {
        func_uvjanim_rom_00400CFC(arg0, arg1, 0.0f);
        return;
    }

    func_uvjanim_rom_00400CFC((UnkStruct_UvJanim_rom_00400144 *) &sp6C, arg1,
                              sp54 / (temp_v0->unk2 - 1));
    func_uvjanim_rom_00400CFC((UnkStruct_UvJanim_rom_00400144 *) &sp58, arg1,
                              sp50 / (temp_v0->unk2 - 1));
    func_uvjanim_rom_00400434(arg0, &sp6C, &sp58, arg3 - (s32) arg3);
}

void func_uvjanim_rom_00401704(s32 arg0, UnkStruct_uvjanim_rom_00401704 *arg1) {
    s32 a1;
    s32 a2;

    a1 = arg1->unk0;
    a2 = arg1->unkC;
    arg1->unk10 = D_uvjanim_rom_00401768->unk58(arg0, a1, a2);
}

