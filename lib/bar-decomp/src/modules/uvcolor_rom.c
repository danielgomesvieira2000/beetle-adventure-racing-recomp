// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "uvasset_types.h"

extern f32 D_uvcolor_rom_00408820;
extern f32 D_uvcolor_rom_00408824;
extern f32 D_uvcolor_rom_00408828;
extern f32 D_uvcolor_rom_0040882C;
extern f32 D_uvcolor_rom_00408830;
extern f32 D_uvcolor_rom_00408834;
extern s32 D_uvcolor_rom_00408810;
extern f32 D_uvcolor_rom_00408838;
extern f32 D_uvcolor_rom_0040883C;
extern f32 D_uvcolor_rom_00408840;
extern f32 D_uvcolor_rom_00408844;
extern f32 D_uvcolor_rom_00408848;
extern f32 D_uvcolor_rom_0040884C;
extern UvGfxMgr_Exports *D_uvcolor_rom_00408850;

void func_uvcolor_rom_00400070(void);
void func_uvcolor_rom_00400094(s32 arg0, f32 arg1, f32 arg2, f32 arg3);
void func_uvcolor_rom_00400120(s32 arg0);
void func_uvcolor_rom_00400520(s32 arg0, f32 arg1, f32 arg2, f32 arg3);
void func_uvcolor_rom_00400578(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4);
void func_uvcolor_rom_004005FC(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6);
void func_uvcolor_rom_00400950(Vtx *arg0, s32 arg1, f32 arg2, f32 arg3, f32 arg4);
void func_uvcolor_rom_00401110(Vtx *arg0, s32 arg1, f32 arg2, f32 arg3, f32 arg4);
void func_uvcolor_rom_00401488(s32 arg0, s32 arg1, s32 arg2, void *arg3, f32 arg4, f32 arg5, f32 arg6);
void func_uvcolor_rom_00402B10(s32 arg0, s32 arg1, s32 arg2, void *arg3, f32 arg4, f32 arg5, f32 arg6);
void func_uvcolor_rom_00404354(Vtx *arg0, s32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6,
                               f32 arg7);
void func_uvcolor_rom_00404C54(s32 arg0, s32 arg1, s32 arg2, void *arg3, f32 arg4, f32 arg5, f32 arg6,
                               f32 arg7, f32 arg8, f32 arg9);
void func_uvcolor_rom_004065E4(s32 arg0, s32 arg1, s32 arg2, void *arg3, f32 arg4, f32 arg5, f32 arg6,
                               f32 arg7, f32 arg8, f32 arg9);
void func_uvcolor_rom_00408044(f32 arg0, f32 arg1, f32 arg2, f32 *arg3, f32 *arg4, f32 *arg5);
void func_uvcolor_rom_004081D0(f32 arg0, f32 arg1, f32 arg2, f32 *arg3, f32 *arg4, f32 *arg5);
void func_uvcolor_rom_004083F0(Vtx *arg0, s32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6,
                               f32 arg7);

void __entrypoint_func_uvcolor_rom_400000(UvColor_Exports *exports);

void __entrypoint_func_uvcolor_rom_400000(UvColor_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvcolor_rom_00400070 = func_uvcolor_rom_00400070;
    exports->func_uvcolor_rom_00400094 = func_uvcolor_rom_00400094;
    exports->func_uvcolor_rom_00400120 = func_uvcolor_rom_00400120;
    exports->func_uvcolor_rom_00400520 = func_uvcolor_rom_00400520;
    exports->func_uvcolor_rom_00400578 = func_uvcolor_rom_00400578;
    #line 46
    D_uvcolor_rom_00408850 = uvLoadModule('GMGR');
}

void func_uvcolor_rom_00400070(void) {
    uvUnloadModule('GMGR');
}

void func_uvcolor_rom_00400094(s32 arg0, f32 arg1, f32 arg2, f32 arg3) {
    if (arg0 == 1) {
        func_uvcolor_rom_004005FC(arg0, D_uvcolor_rom_0040882C, D_uvcolor_rom_00408830,
                                  D_uvcolor_rom_00408834, arg1, arg2, arg3);
        return;
    }
    func_uvcolor_rom_004005FC(arg0, D_uvcolor_rom_00408820, D_uvcolor_rom_00408824,
                              D_uvcolor_rom_00408828, arg1, arg2, arg3);
}

void func_uvcolor_rom_00400120(s32 arg0) {
    f32 temp_fv0;
    s32 filesCount;
    f32 a0;
    f32 a1;
    f32 a2;
    ParsedUVCT *temp_v0_2;
    ParsedUVMD *temp_v0_4;
    ParsedUVTX *temp_v0_6;
    s32 i;

    D_uvcolor_rom_00408850->func_uvgfxmgr_rom_00402090();
    filesCount = uvGetFilesCount('UVCT');
    for (i = 0; i < filesCount; i++) {
        temp_v0_2 = uvGetLoadedFile('UVCT', i);
        if ((temp_v0_2 != NULL) && (temp_v0_2->unk28 != 0.0f)) {
            if (arg0 == 0) {
                a0 = 1.0f - ((1.0f - D_uvcolor_rom_00408820) * temp_v0_2->unk28);
                a1 = 1.0f - ((1.0f - D_uvcolor_rom_00408824) * temp_v0_2->unk28);
                a2 = 1.0f - ((1.0f - D_uvcolor_rom_00408828) * temp_v0_2->unk28);

                func_uvcolor_rom_00400950(temp_v0_2->vtxTable, (s32) temp_v0_2->vtxCount, a0, a1, a2);
            } else {
                a0 = 1.0f - ((1.0f - D_uvcolor_rom_0040882C) * temp_v0_2->unk28);
                a1 = 1.0f - ((1.0f - D_uvcolor_rom_00408830) * temp_v0_2->unk28);
                a2 = 1.0f - ((1.0f - D_uvcolor_rom_00408834) * temp_v0_2->unk28);
                func_uvcolor_rom_00401110((s32) temp_v0_2->vtxTable, (s32) temp_v0_2->vtxCount, a0, a1,
                                          a2);
            }
        }
    }

    filesCount = uvGetFilesCount('UVMD');
    for (i = 0; i < filesCount; i++) {
        temp_v0_4 = uvGetLoadedFile('UVMD', i);
        if ((temp_v0_4 != NULL) && (temp_v0_4->unk14 != 0.0f)) {
            if (arg0 == 0) {
                a0 = 1.0f - ((1.0f - D_uvcolor_rom_00408820) * temp_v0_4->unk14);
                a1 = 1.0f - ((1.0f - D_uvcolor_rom_00408824) * temp_v0_4->unk14);
                a2 = 1.0f - ((1.0f - D_uvcolor_rom_00408828) * temp_v0_4->unk14);
                func_uvcolor_rom_00400950(temp_v0_4->vtxTable, (s32) temp_v0_4->vtxCount, a0, a1, a2);
            } else {
                a0 = 1.0f - ((1.0f - D_uvcolor_rom_0040882C) * temp_v0_4->unk14);
                a1 = 1.0f - ((1.0f - D_uvcolor_rom_00408830) * temp_v0_4->unk14);
                a2 = 1.0f - ((1.0f - D_uvcolor_rom_00408834) * temp_v0_4->unk14);
                func_uvcolor_rom_00401110((s32) temp_v0_4->vtxTable, (s32) temp_v0_4->vtxCount, a0, a1,
                                          a2);
            }
        }
    }

    filesCount = uvGetFilesCount('UVTX');
    for (i = 0; i < filesCount; i++) {
        temp_v0_6 = uvGetLoadedFile('UVTX', i);
        if ((temp_v0_6 != NULL) && (temp_v0_6->unk28 != 0.0f)) {
            if (arg0 == 0) {
                a0 = 1.0f - ((1.0f - D_uvcolor_rom_00408820) * temp_v0_6->unk28);
                a1 = 1.0f - ((1.0f - D_uvcolor_rom_00408824) * temp_v0_6->unk28);
                a2 = 1.0f - ((1.0f - D_uvcolor_rom_00408828) * temp_v0_6->unk28);
                func_uvcolor_rom_00402B10(temp_v0_6->unk22, temp_v0_6->unkD, temp_v0_6->unk14,
                                          temp_v0_6->unk2C, a0, a1, a2);
            } else {
                temp_fv0 = temp_v0_6->unk28;
                a0 = 1.0f - ((1.0f - D_uvcolor_rom_0040882C) * temp_v0_6->unk28);
                a1 = 1.0f - ((1.0f - D_uvcolor_rom_00408830) * temp_v0_6->unk28);
                a2 = 1.0f - ((1.0f - D_uvcolor_rom_00408834) * temp_v0_6->unk28);
                func_uvcolor_rom_00401488(temp_v0_6->unk22, temp_v0_6->unkD, temp_v0_6->unk14,
                                          temp_v0_6->unk2C, a0, a1, a2);
            }
        }
    }
}

void func_uvcolor_rom_00400520(s32 arg0, f32 arg1, f32 arg2, f32 arg3) {
    if (arg0 == 1) {
        D_uvcolor_rom_0040882C = arg1;
        D_uvcolor_rom_00408830 = arg2;
        D_uvcolor_rom_00408834 = arg3;
        return;
    }
    D_uvcolor_rom_00408820 = arg1;
    D_uvcolor_rom_00408824 = arg2;
    D_uvcolor_rom_00408828 = arg3;
}

void func_uvcolor_rom_00400578(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    ParsedUVMD *temp_v0;

    temp_v0 = uvGetLoadedFile('UVMD', arg0);
    if (temp_v0 != NULL) {
        D_uvcolor_rom_00408850->func_uvgfxmgr_rom_00402090();
        func_uvcolor_rom_00404354(temp_v0->vtxTable, (s32) temp_v0->vtxCount, arg1, arg2, arg3, arg4,
                                  arg4, arg4);
    }
}

void func_uvcolor_rom_004005FC(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6) {
    f32 temp_fa0;
    f32 temp_fa1;
    f32 temp_fv1;
    ParsedUVCT *uvct;
    ParsedUVMD *uvmd;
    ParsedUVTX *uvtx;
    s32 filesCount;
    s32 i;

    D_uvcolor_rom_00408838 = arg1;
    D_uvcolor_rom_0040883C = arg2;
    D_uvcolor_rom_00408840 = arg3;
    D_uvcolor_rom_00408844 = arg4;
    D_uvcolor_rom_00408848 = arg5;
    D_uvcolor_rom_0040884C = arg6;
    D_uvcolor_rom_00408810 = 0;
    D_uvcolor_rom_00408850->func_uvgfxmgr_rom_00402090();

    filesCount = uvGetFilesCount('UVCT');
    for (i = 0; i < filesCount; i++) {
        uvct = uvGetLoadedFile('UVCT', i);
        if (uvct != NULL) {
            if (uvct->unk28 != 0.0f) {
                temp_fv1 = 1.0f - (uvct->unk28 * arg4);
                temp_fa0 = 1.0f - (uvct->unk28 * arg5);
                temp_fa1 = 1.0f - (uvct->unk28 * arg6);
                if (arg0 == 0) {
                    func_uvcolor_rom_00404354(uvct->vtxTable, (s32) uvct->vtxCount, arg1, arg2, arg3,
                                              temp_fv1, temp_fa0, temp_fa1);
                } else {
                    func_uvcolor_rom_004083F0(uvct->vtxTable, (s32) uvct->vtxCount, arg1, arg2, arg3,
                                              temp_fv1, temp_fa0, temp_fa1);
                }
            }
        }
    }

    filesCount = uvGetFilesCount('UVMD');
    for (i = 0; i < filesCount; i++) {
        uvmd = uvGetLoadedFile('UVMD', i);
        if (uvmd != NULL) {
            if (uvmd->unk14 != 0.0f) {
                temp_fv1 = 1.0f - (uvmd->unk14 * arg4);
                temp_fa0 = 1.0f - (uvmd->unk14 * arg5);
                temp_fa1 = 1.0f - (uvmd->unk14 * arg6);
                if (arg0 == 0) {
                    func_uvcolor_rom_00404354(uvmd->vtxTable, (s32) uvmd->vtxCount, arg1, arg2, arg3,
                                              temp_fv1, temp_fa0, temp_fa1);
                } else {
                    func_uvcolor_rom_004083F0(uvmd->vtxTable, (s32) uvmd->vtxCount, arg1, arg2, arg3,
                                              temp_fv1, temp_fa0, temp_fa1);
                }
            }
        }
    }

    filesCount = uvGetFilesCount('UVTX');
    for (i = 0; i < filesCount; i++) {
        uvtx = uvGetLoadedFile('UVTX', i);
        if (uvtx != NULL) {
            if (uvtx->unk28 != 0.0f) {
                temp_fv1 = 1.0f - (uvtx->unk28 * arg4);
                temp_fa0 = 1.0f - (uvtx->unk28 * arg5);
                temp_fa1 = 1.0f - (uvtx->unk28 * arg6);
                if (arg0 == 0) {
                    func_uvcolor_rom_004065E4((s32) uvtx->unk22, (s32) uvtx->unkD, (s32) uvtx->unk14,
                                              uvtx->unk2C, arg1, arg2, arg3, temp_fv1, temp_fa0,
                                              temp_fa1);
                } else {
                    func_uvcolor_rom_00404C54((s32) uvtx->unk22, (s32) uvtx->unkD, (s32) uvtx->unk14,
                                              uvtx->unk2C, arg1, arg2, arg3, temp_fv1, temp_fa0,
                                              temp_fa1);
                }
            }
        }
    }
}

void func_uvcolor_rom_00400950(Vtx *arg0, s32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    s32 i;
    f32 temp_ft4;
    f32 fv0;

    for (i = 0; i < arg1; i++) {
        Vtx *v = &arg0[i];
        temp_ft4 = v->v.cn[0];
        fv0 = v->v.flag ? arg2 * temp_ft4 : CUBE(arg2) * temp_ft4;
        v->v.cn[0] = fv0;
        temp_ft4 = v->v.cn[1];
        fv0 = v->v.flag ? arg3 * temp_ft4 : CUBE(arg3) * temp_ft4;
        v->v.cn[1] = fv0;
        temp_ft4 = v->v.cn[2];
        fv0 = v->v.flag ? arg4 * temp_ft4 : CUBE(arg4) * temp_ft4;
        v->v.cn[2] = fv0;
    }
}

void func_uvcolor_rom_00401110(Vtx *arg0, s32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 fv0;

    f32 fa0;

    f32 sp8C;
    f32 sp88;
    f32 sp84;
    f32 fa1;
    s32 i;
    Vtx *s0;
    f32 sp74;
    f32 sp70;
    f32 sp6C;

    for (i = 0; i < arg1; i++) {
        s0 = arg0 + i;

        func_uvcolor_rom_00408044((f32) (u32) s0->v.cn[0] / 255.0f, (f32) (u32) s0->v.cn[1] / 255.0f,
                                  (f32) (u32) s0->v.cn[2] / 255.0f, &sp8C, &sp88, &sp84);

        if (s0->v.flag != 0) {
            fa0 = arg2 * sp8C;
            fa1 = arg3 * sp88;
            fv0 = arg4 * sp84;
        } else {
            fa0 = arg2 * sp8C;
            fa1 = arg3 * sp88;
            fv0 = arg4 * arg4 * arg4 * sp84;
        }

        func_uvcolor_rom_004081D0(fa0, fa1, fv0, &sp74, &sp70, &sp6C);

        s0->v.cn[0] = (u8) (u32) (sp74 * 255.0f);
        s0->v.cn[1] = (u8) (u32) (sp70 * 255.0f);
        s0->v.cn[2] = (u8) (u32) (sp6C * 255.0f);
    }
}

void func_uvcolor_rom_00401488(s32 arg0, s32 arg1, s32 arg2, void *arg3, f32 arg4, f32 arg5, f32 arg6) {
    s32 i;
    f32 sp90;
    f32 sp8C;
    f32 sp88;
    u16 *var_s0;
    u8 *var_v0;
    f32 temp_fv1;
    f32 temp_fa0;
    s32 pad;
    f32 sp70;
    f32 sp6C;
    f32 sp68;
    s32 pad2[2];

    switch (arg0) {
        case 4:
            var_s0 = arg3;
            for (i = 0; i < arg2 / 2; i++) {
                func_uvcolor_rom_00408044(((s32) (var_s0[i] & 0xF800) >> 0xB) / 31.0f,
                                          ((s32) (var_s0[i] & 0x7C0) >> 6) / 31.0f,
                                          ((s32) (var_s0[i] & 0x3E) >> 1) / 31.0f, &sp90, &sp8C, &sp88);
                func_uvcolor_rom_004081D0(arg4 * sp90, arg5 * sp8C, arg6 * sp88, &sp70, &sp6C, &sp68);
                sp70 *= 31.0f;
                sp6C *= 31.0f;
                sp68 *= 31.0f;
                var_s0[i] =
                    ((s32) sp70 << 11) | ((s32) sp6C << 6) | ((s32) sp68 << 1) | (var_s0[i] & 1);
            }
            break;
        case 2:
            switch (arg1) {
                case 4:
                    var_v0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = (var_v0[i] & 0xE) >> 1;
                        var_v0[i] = (var_v0[i] & 0xF1) | (((s32) (arg6 * temp_fa0)) << 1);
                        temp_fa0 = (var_v0[i] & 0xE0) >> 5;
                        var_v0[i] = (((s32) (arg6 * temp_fa0)) << 5) | (var_v0[i] & 0x1F);
                    }
                    break;
                case 8:
                    var_v0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = (var_v0[i] & 0xF0) >> 4;
                        var_v0[i] = ((((s32) (arg6 * temp_fa0)) & 0xF) << 4) | (var_v0[i] & 0xF);
                    }
                    break;
                case 16:
                    var_s0 = arg3;
                    for (i = 0; i < arg2 / 2; i++) {
                        temp_fa0 = (var_s0[i] & 0xFF00) >> 8;
                        var_s0[i] = ((((s32) (arg6 * temp_fa0)) & 0xFF) << 8) | (var_s0[i] & 0xFF);
                    }
                    break;
            }
            break;
        case 1:
            switch (arg1) {
                case 4:
                    var_v0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = var_v0[i] & 0xF;
                        var_v0[i] = (var_v0[i] & 0xF0) | (((s32) (arg6 * temp_fa0)) & 0xF);
                        temp_fa0 = (var_v0[i] & 0xF0) >> 4;
                        var_v0[i] = ((((s32) (arg6 * temp_fa0)) & 0xF) << 4) | (var_v0[i] & 0xF);
                    }
                    break;
                case 8:
                    var_v0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = var_v0[i];
                        var_v0[i] = arg6 * temp_fa0;
                    }
                    break;
                case 16:
                    var_s0 = arg3;
                    for (i = 0; i < arg2 / 2; i++) {
                        temp_fa0 = var_s0[i];
                        var_s0[i] = arg6 * temp_fa0;
                    }
                    break;
            }
            break;
    }
}

void func_uvcolor_rom_00402B10(s32 arg0, s32 arg1, s32 arg2, void *arg3, f32 arg4, f32 arg5, f32 arg6) {
    f32 var_fv1;
    u16 *var_a1;
    u8 *var_a0;
    s32 i;
    f32 r;
    f32 g;
    f32 b;
    f32 temp_fa0;

    if (arg5 < arg4) {
        if (arg6 < arg4) {
            var_fv1 = arg4;
        } else {
            var_fv1 = arg6;
        }
    } else {
        if (arg6 < arg5) {
            var_fv1 = arg5;
        } else {
            var_fv1 = arg6;
        }
    }
    switch (arg0) {
        case 4:
            var_a1 = arg3;
            for (i = 0; i < arg2 / 2; i++) {
                r = (var_a1[i] & 0xF800) >> 11;
                g = (var_a1[i] & 0x7C0) >> 6;
                b = (var_a1[i] & 0x3E) >> 1;
                var_a1[i] = ((s32) (r * arg4) << 11) | ((s32) (g * arg5) << 6) | ((s32) (b * arg6) << 1)
                            | (var_a1[i] & 1);
            }
            break;
        case 2:
            switch (arg1) {
                case 4:
                    var_a0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = (var_a0[i] & 0xE) >> 1;
                        var_a0[i] = (var_a0[i] & 0xF1) | (((s32) (var_fv1 * temp_fa0)) << 1);
                        temp_fa0 = (var_a0[i] & 0xE0) >> 5;
                        var_a0[i] = (((s32) (var_fv1 * temp_fa0)) << 5) | (var_a0[i] & 0x1F);
                    }
                    break;
                case 8:
                    var_a0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = (var_a0[i] & 0xF0) >> 4;
                        var_a0[i] = ((((s32) (var_fv1 * temp_fa0)) & 0xF) << 4) | (var_a0[i] & 0xF);
                    }
                    break;
                case 16:
                    var_a1 = arg3;
                    for (i = 0; i < arg2 / 2; i++) {
                        temp_fa0 = (var_a1[i] & 0xFF00) >> 8;
                        var_a1[i] = ((((s32) (var_fv1 * temp_fa0)) & 0xFF) << 8) | (var_a1[i] & 0xFF);
                    }
                    break;
            }
            break;
        case 1:
            switch (arg1) {
                case 4:
                    var_a0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = var_a0[i] & 0xF;
                        var_a0[i] = (var_a0[i] & 0xF0) | (((s32) (var_fv1 * temp_fa0)) & 0xF);
                        temp_fa0 = (var_a0[i] & 0xF0) >> 4;
                        var_a0[i] = ((((s32) (var_fv1 * temp_fa0)) & 0xF) << 4) | (var_a0[i] & 0xF);
                    }
                    break;
                case 8:
                    var_a0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = var_a0[i];
                        var_a0[i] = var_fv1 * temp_fa0;
                    }
                    break;
                case 16:
                    var_a1 = arg3;
                    for (i = 0; i < arg2 / 2; i++) {
                        temp_fa0 = var_a1[i];
                        var_a1[i] = var_fv1 * temp_fa0;
                    }
                    break;
            }
            break;
    }
}

void func_uvcolor_rom_00404354(Vtx *arg0, s32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6,
                               f32 arg7) {
    s32 i;
    f32 temp_ft4;
    f32 temp_ft5;
    f32 temp_fs3;
    f32 temp_fs5;
    f32 sp40;
    f32 sp3C;
    Vtx *temp_a3;
    f32 temp_fa0;
    f32 var_fv0;

    temp_ft4 = arg5 * arg5 * arg5;
    temp_ft5 = arg6 * arg6 * arg6;
    temp_fs3 = arg7 * arg7 * arg7;
    temp_fs5 = (1.0f - temp_ft4) * 255.0f * arg2;
    sp40 = (1.0f - temp_ft5) * 255.0f * arg3;
    sp3C = (1.0f - temp_fs3) * 255.0f * arg4;
    arg2 = arg2 * ((1.0f - arg5) * 255.0f);
    arg3 = arg3 * ((1.0f - arg6) * 255.0f);
    arg4 = arg4 * ((1.0f - arg7) * 255.0f);

    for (i = 0; i < arg1; i++) {
        temp_a3 = &arg0[i];
        temp_fa0 = temp_a3->v.cn[0];
        if (temp_a3->v.flag != 0) {
            var_fv0 = (arg5 * temp_fa0) + arg2;
        } else {
            var_fv0 = (temp_ft4 * temp_fa0) + temp_fs5;
        }
        temp_a3->v.cn[0] = var_fv0;
        temp_fa0 = temp_a3->v.cn[1];
        if (temp_a3->v.flag != 0) {
            var_fv0 = (arg6 * temp_fa0) + arg3;
        } else {
            var_fv0 = (temp_ft5 * temp_fa0) + sp40;
        }
        temp_a3->v.cn[1] = var_fv0;
        temp_fa0 = temp_a3->v.cn[2];
        if (temp_a3->v.flag != 0) {
            var_fv0 = (arg7 * temp_fa0) + arg4;
        } else {
            var_fv0 = (temp_fs3 * temp_fa0) + sp3C;
        }
        temp_a3->v.cn[2] = var_fv0;
    }
}

void func_uvcolor_rom_00404C54(s32 arg0, s32 arg1, s32 arg2, void *arg3, f32 arg4, f32 arg5, f32 arg6,
                               f32 arg7, f32 arg8, f32 arg9) {
    s32 i;
    f32 spB0;
    f32 spAC;
    f32 spA8;
    f32 sp50;
    f32 sp4C;
    f32 sp48;
    u16 *var_s0;
    u8 *var_v0;
    f32 temp_fv1;
    f32 temp_fa0;
    s32 pad;
    f32 sp84;
    f32 sp80;
    f32 sp7C;
    f32 sp78;
    s32 pad2[2];

    switch (arg0) {
        case 4:
            var_s0 = arg3;
            sp50 = (1.0f - arg7) * arg4;
            sp4C = (1.0f - arg8) * arg5;
            sp48 = (1.0f - arg9) * arg6;
            for (i = 0; i < arg2 / 2; i++) {
                func_uvcolor_rom_00408044((f32) ((s32) (var_s0[i] & 0xF800) >> 0xB) / 31.0f,
                                          (f32) ((s32) (var_s0[i] & 0x7C0) >> 6) / 31.0f,
                                          (f32) ((s32) (var_s0[i] & 0x3E) >> 1) / 31.0f, &spB0, &spAC,
                                          &spA8);
                func_uvcolor_rom_004081D0((arg7 * spB0) + sp50, (arg8 * spAC) + sp4C,
                                          (arg9 * spA8) + sp48, &sp80, &sp7C, &sp78);
                sp80 *= 31.0f;
                sp7C *= 31.0f;
                sp78 *= 31.0f;
                var_s0[i] =
                    ((s32) sp80 << 11) | ((s32) sp7C << 6) | ((s32) sp78 << 1) | (var_s0[i] & 1);
            }
            break;
        case 2:
            switch (arg1) {
                case 4:
                    temp_fv1 = (1.0f - arg9) * arg6 * 7.0f;
                    var_v0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = (var_v0[i] & 0xE) >> 1;
                        var_v0[i] = (var_v0[i] & 0xF1) | (((s32) ((arg9 * temp_fa0) + temp_fv1)) << 1);
                        temp_fa0 = (var_v0[i] & 0xE0) >> 5;
                        //! @bug sp84 uninitialised!
                        var_v0[i] = (((s32) ((arg9 * temp_fa0) + sp84)) << 5) | (var_v0[i] & 0x1F);
                    }
                    break;
                case 8:
                    temp_fv1 = (1.0f - arg9) * arg6 * 15.0f;
                    var_v0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = (var_v0[i] & 0xF0) >> 4;
                        var_v0[i] =
                            ((((s32) ((arg9 * temp_fa0) + temp_fv1)) & 0xF) << 4) | (var_v0[i] & 0xF);
                    }
                    break;
                case 16:
                    temp_fv1 = (1.0f - arg9) * arg6 * 255.0f;
                    var_s0 = arg3;
                    for (i = 0; i < arg2 / 2; i++) {
                        temp_fa0 = (var_s0[i] & 0xFF00) >> 8;
                        var_s0[i] =
                            ((((s32) ((arg9 * temp_fa0) + temp_fv1)) & 0xFF) << 8) | (var_s0[i] & 0xFF);
                    }
                    break;

                case 1:
                    switch (arg1) {
                        case 4:
                            temp_fv1 = (1.0f - arg9) * arg6 * 15.0f;
                            var_v0 = arg3;
                            for (i = 0; i < arg2; i++) {
                                temp_fa0 = var_v0[i] & 0xF;
                                var_v0[i] =
                                    (var_v0[i] & 0xF0) | (((s32) ((arg9 * temp_fa0) + temp_fv1)) & 0xF);
                                temp_fa0 = (var_v0[i] & 0xF0) >> 4;
                                var_v0[i] = ((((s32) ((arg9 * temp_fa0) + temp_fv1)) & 0xF) << 4)
                                            | (var_v0[i] & 0xF);
                            }
                            break;
                        case 8:
                            temp_fv1 = (1.0f - arg9) * arg6 * 255.0f;
                            var_v0 = arg3;
                            for (i = 0; i < arg2; i++) {
                                temp_fa0 = var_v0[i];
                                var_v0[i] = (arg9 * temp_fa0) + temp_fv1;
                            }
                            break;
                        case 16:
                            temp_fv1 = (1.0f - arg9) * arg6 * 65535.0f;
                            var_s0 = arg3;
                            for (i = 0; i < arg2 / 2; i++) {
                                temp_fa0 = var_s0[i];
                                var_s0[i] = (arg9 * temp_fa0) + temp_fv1;
                            }
                            break;
                    }
            }
            break;
    }
}

void func_uvcolor_rom_004065E4(s32 arg0, s32 arg1, s32 arg2, void *arg3, f32 arg4, f32 arg5, f32 arg6,
                               f32 arg7, f32 arg8, f32 arg9) {
    u8 *var_a0;
    u16 *var_a1;
    f32 var_fv0;
    f32 var_fv1;
    s32 i;
    f32 temp_fa0;
    f32 r;
    f32 g;
    f32 b;

    if (arg5 < arg4) {
        if (arg6 < arg4) {
            var_fv0 = arg4;
            var_fv1 = arg7;
        } else {
            var_fv0 = arg6;
            var_fv1 = arg9;
        }
    } else if (arg6 < arg5) {
        var_fv0 = arg5;
        var_fv1 = arg8;
    } else {
        var_fv0 = arg6;
        var_fv1 = arg9;
    }

    if (arg0 != 1) {
        if (arg0 != 2) {
            if (arg0 == 4) {
                arg4 *= (1.0f - arg7) * 31.0f;
                arg5 *= (1.0f - arg8) * 31.0f;
                arg6 *= (1.0f - arg9) * 31.0f;
                var_a1 = arg3;
                for (i = 0; i < arg2 / 2; i++) {
                    r = ((var_a1[i] & 0xF800) >> 11);
                    g = ((var_a1[i] & 0x7C0) >> 6);
                    b = ((var_a1[i] & 0x3E) >> 1);
                    var_a1[i] = ((s32) ((arg7 * r) + arg4) << 11) | ((s32) ((arg8 * g) + arg5) << 6)
                                | ((s32) ((arg9 * b) + arg6) << 1) | (var_a1[i] & 1);
                }
            }
        } else {
            switch (arg1) {
                case 4:
                    var_fv0 *= (1.0f - var_fv1) * 7.0f;
                    var_a0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = (var_a0[i] & 0xE) >> 1;
                        var_a0[i] = (var_a0[i] & 0xF1) | ((s32) ((var_fv1 * temp_fa0) + var_fv0) << 1);
                        temp_fa0 = (var_a0[i] & 0xE0) >> 5;
                        var_a0[i] = ((s32) ((var_fv1 * temp_fa0) + var_fv0) << 5) | (var_a0[i] & 0x1F);
                    }
                    break;
                case 8:
                    var_fv0 *= (1.0f - var_fv1) * 15.0f;
                    var_a0 = arg3;
                    for (i = 0; i < arg2; i++) {
                        temp_fa0 = (var_a0[i] & 0xF0) >> 4;
                        var_a0[i] =
                            (((s32) ((var_fv1 * temp_fa0) + var_fv0) & 0xF) << 4) | (var_a0[i] & 0xF);
                    }
                    break;
                case 16:
                    var_fv0 *= (1.0f - var_fv1) * 255.0f;
                    var_a1 = arg3;
                    for (i = 0; i < arg2 / 2; i++) {
                        temp_fa0 = (var_a1[i] & 0xFF00) >> 8;
                        var_a1[i] =
                            (((s32) ((var_fv1 * temp_fa0) + var_fv0) & 0xFF) << 8) | (var_a1[i] & 0xFF);
                    }
                    break;
            }
        }
    } else {
        switch (arg1) {
            case 4:
                var_fv0 *= (1.0f - var_fv1) * 15.0f;
                var_a0 = arg3;
                for (i = 0; i < arg2; i++) {
                    temp_fa0 = (var_a0[i] & 0xF);
                    var_a0[i] = (var_a0[i] & 0xF0) | ((s32) ((var_fv1 * temp_fa0) + var_fv0) & 0xF);
                    temp_fa0 = (var_a0[i] & 0xF0) >> 4;
                    var_a0[i] =
                        (((s32) ((var_fv1 * temp_fa0) + var_fv0) & 0xF) << 4) | (var_a0[i] & 0xF);
                }
                break;
            case 8:
                var_fv0 *= (1.0f - var_fv1) * 255.0f;
                var_a0 = arg3;
                for (i = 0; i < arg2; i++) {
                    temp_fa0 = var_a0[i];
                    var_a0[i] = (var_fv1 * temp_fa0) + var_fv0;
                }
                break;
            case 16:
                var_fv0 *= (1.0f - var_fv1) * 65535.0f;
                var_a1 = arg3;
                for (i = 0; i < arg2 / 2; i++) {
                    temp_fa0 = var_a1[i];
                    var_a1[i] = (var_fv1 * temp_fa0) + var_fv0;
                }
                break;
        }
    }
}

void func_uvcolor_rom_00408044(f32 arg0, f32 arg1, f32 arg2, f32 *arg3, f32 *arg4, f32 *arg5) {
    f32 var_fv0;
    f32 var_fv1;

    var_fv0 = arg0;
    var_fv1 = arg0;
    if (arg0 < arg1) {
        var_fv0 = arg1;
    }
    if (arg1 < arg0) {
        var_fv1 = arg1;
    }
    if (var_fv0 < arg2) {
        var_fv0 = arg2;
    }
    if (arg2 < var_fv1) {
        var_fv1 = arg2;
    }
    *arg5 = var_fv0;
    if (var_fv0 == var_fv1) {
        *arg4 = 0.0f;
        *arg3 = -1.0f;
        return;
    }

    *arg4 = (var_fv0 - var_fv1) / var_fv0;
    if (arg0 == var_fv0) {
        *arg3 = ((var_fv0 - arg2) / (var_fv0 - var_fv1)) - ((var_fv0 - arg1) / (var_fv0 - var_fv1));
    } else if (arg1 == var_fv0) {
        *arg3 =
            ((var_fv0 - arg0) / (var_fv0 - var_fv1)) - ((var_fv0 - arg2) / (var_fv0 - var_fv1)) + 2.0f;
    } else if (arg2 == var_fv0) {
        *arg3 =
            ((var_fv0 - arg1) / (var_fv0 - var_fv1)) - ((var_fv0 - arg0) / (var_fv0 - var_fv1)) + 4.0f;
    }
    *arg3 *= 60.0f;

    if (*arg3 < 0.0f) {
        *arg3 += 360.0f;
    }
    *arg3 /= 360.0f;
}

void func_uvcolor_rom_004081D0(f32 arg0, f32 arg1, f32 arg2, f32 *arg3, f32 *arg4, f32 *arg5) {
    s32 temp_ft3;
    f32 temp_ft1;

    if (arg1 <= 0.0001f) {
        *arg3 = arg2;
        *arg4 = arg2;
        *arg5 = arg2;
        return;
    }
    if (arg0 < 0.0f) {
        arg0 += 1.0f;
    }
    if (arg0 == 1.0f) {
        arg0 = 0;
    }

    arg0 *= 6.0f;
    temp_ft3 = (s32) arg0;
    temp_ft1 = arg0 - temp_ft3;

    switch (temp_ft3) {
        case 0:
            *arg3 = arg2;
            *arg4 = (1.0f - (arg1 * (1.0f - temp_ft1))) * arg2;
            *arg5 = (1.0f - arg1) * arg2;
            break;
        case 1:
            *arg3 = (1.0f - (arg1 * temp_ft1)) * arg2;
            *arg4 = arg2;
            *arg5 = (1.0f - arg1) * arg2;
            break;
        case 2:
            *arg3 = (1.0f - arg1) * arg2;
            *arg4 = arg2;
            *arg5 = (1.0f - (arg1 * (1.0f - temp_ft1))) * arg2;
            break;
        case 3:
            *arg3 = (1.0f - arg1) * arg2;
            *arg4 = (1.0f - (arg1 * temp_ft1)) * arg2;
            *arg5 = arg2;
            break;
        case 4:
            *arg3 = (1.0f - (arg1 * (1.0f - temp_ft1))) * arg2;
            *arg4 = (1.0f - arg1) * arg2;
            *arg5 = arg2;
            break;
        case 5:
            *arg3 = arg2;
            *arg4 = (1.0f - arg1) * arg2;
            *arg5 = (1.0f - (arg1 * temp_ft1)) * arg2;
            break;
        default:
            break;
    }
}

void func_uvcolor_rom_004083F0(Vtx *arg0, s32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6,
                               f32 arg7) {
    s32 i;
    Vtx *temp_s0;
    f32 spBC;
    f32 spB8;
    f32 spB4;
    f32 var_fa0;
    f32 var_fa1;
    f32 var_fv0;
    f32 spA4;
    f32 spA0;
    f32 sp9C;
    f32 sp98;
    f32 sp94;
    f32 sp90;
    f32 temp_fs4;
    s32 pad;

    temp_fs4 = arg7 * arg7 * arg7;
    sp98 = (1.0f - arg5) * arg2;
    sp90 = (1.0f - arg6) * arg3;
    sp94 = (1.0f - temp_fs4) * arg4;
    arg2 = (1.0f - arg5) * arg2;
    arg3 = (1.0f - arg6) * arg3;
    arg4 = (1.0f - arg7) * arg4;
    for (i = 0; i < arg1; i++) {
        temp_s0 = &arg0[i];

        func_uvcolor_rom_00408044(temp_s0->v.cn[0] / 255.0f, temp_s0->v.cn[1] / 255.0f,
                                  temp_s0->v.cn[2] / 255.0f, &spBC, &spB8, &spB4);
        if (temp_s0->v.flag != 0) {
            var_fa0 = (arg5 * spBC) + arg2;
            var_fa1 = (arg6 * spB8) + arg3;
            var_fv0 = (arg7 * spB4) + arg4;
        } else {
            var_fa0 = (arg5 * spBC) + sp98;
            var_fa1 = (arg6 * spB8) + sp90;
            var_fv0 = (temp_fs4 * spB4) + sp94;
        }
        func_uvcolor_rom_004081D0(var_fa0, var_fa1, var_fv0, &spA4, &spA0, &sp9C);
        temp_s0->v.cn[0] = spA4 * 255.0f;
        temp_s0->v.cn[1] = spA0 * 255.0f;
        temp_s0->v.cn[2] = sp9C * 255.0f;
    }
}
