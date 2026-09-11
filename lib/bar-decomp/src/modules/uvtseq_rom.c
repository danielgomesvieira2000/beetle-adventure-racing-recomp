// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "uvasset_types.h"
#include "stdarg.h"

typedef struct UnkStruct_uvtseq_rom_00400A80_s {
    /* 0x00 */ u8 unk0;
    /* 0x01 */ u8 unk1;
    /* 0x02 */ u8 unk2;
    /* 0x03 */ char pad3[1];
    /* 0x04 */ f32 unk4;
    /* 0x08 */ ParsedUVTS uvts;
} UnkStruct_uvtseq_rom_00400A80; /* size = 0x18 */

typedef struct TexSequenceSettings_s {
    s32 unk0;
    s32 unk4;
} TexSequenceSettings;

extern UnkStruct_uvtseq_rom_00400A80 *D_uvtseq_rom_00400A80;
extern UvGfxMgr_Exports *D_uvtseq_rom_00400A90;
extern UvCback_Exports *D_uvtseq_rom_00400A94;
extern s32 D_uvtseq_rom_00400A84;
extern u8 D_uvtseq_rom_00400A8C;

void func_uvtseq_rom_0040026C(void);
void func_uvtseq_rom_00400378(s32 arg0, s32 arg1);
void func_uvtseq_rom_00400454(s32 arg0, ...);
s32 func_uvtseq_rom_004005BC(void);
void func_uvtseq_rom_00400620(s32 arg0);
void func_uvtseq_rom_00400668(s32 arg0);
void func_uvtseq_rom_00400688(void);
void func_uvtseq_rom_00400714(s32 arg0);
u16 func_uvtseq_rom_0040093C(s32 arg0);
u8 func_uvtseq_rom_0040096C(s32 arg0);
u16 func_uvtseq_rom_0040098C(s32 arg0, s32 arg1);
u8 func_uvtseq_rom_004009F0(s32 arg0);
f32 func_uvtseq_rom_00400A10(s32 arg0);
void __entrypoint_func_uvtseq_rom_400000(UvTSeq_Exports *exports);

extern s32 D_uvtseq_rom_00400A88;

void __entrypoint_func_uvtseq_rom_400000(UvTSeq_Exports *exports) {
    UnkStruct_uvtseq_rom_00400A80 *temp_v0_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_a0;
    s32 var_v0;
    s32 i;
    TexSequenceSettings *temp_v0;

    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvtseq_rom_0040026C = func_uvtseq_rom_0040026C;
    exports->func_uvtseq_rom_00400378 = func_uvtseq_rom_00400378;
    exports->func_uvtseq_rom_00400454 = func_uvtseq_rom_00400454;
    exports->func_uvtseq_rom_004005BC = func_uvtseq_rom_004005BC;
    exports->func_uvtseq_rom_00400620 = func_uvtseq_rom_00400620;
    exports->func_uvtseq_rom_0040093C = func_uvtseq_rom_0040093C;
    exports->func_uvtseq_rom_0040096C = func_uvtseq_rom_0040096C;
    exports->func_uvtseq_rom_0040098C = func_uvtseq_rom_0040098C;
    exports->func_uvtseq_rom_004009F0 = func_uvtseq_rom_004009F0;
    exports->func_uvtseq_rom_00400A10 = func_uvtseq_rom_00400A10;
    temp_v0 = uvGetSystemProp(12);
    if (temp_v0 == NULL) {
        D_uvtseq_rom_00400A84 = 0x14;
        D_uvtseq_rom_00400A88 = 0x10;
    } else {
        if (temp_v0->unk0 != 0) {
            D_uvtseq_rom_00400A84 = temp_v0->unk0;
        } else {
            D_uvtseq_rom_00400A84 = 0x14;
        }
        if (temp_v0->unk4 != 0) {
            D_uvtseq_rom_00400A88 = temp_v0->unk4;
        } else {
            D_uvtseq_rom_00400A88 = 0x10;
        }
    }
    D_uvtseq_rom_00400A90 = uvLoadModule('GMGR');
    D_uvtseq_rom_00400A94 = uvLoadModule('CBCK');
    D_uvtseq_rom_00400A80 =
        _uvMemAllocAlign8(D_uvtseq_rom_00400A84 * sizeof(UnkStruct_uvtseq_rom_00400A80));
    uvMemSet(D_uvtseq_rom_00400A80, 0, D_uvtseq_rom_00400A84 * sizeof(UnkStruct_uvtseq_rom_00400A80));

    for (i = 0; i < D_uvtseq_rom_00400A84; i++) {
        D_uvtseq_rom_00400A80[i].unk0 = 0;
        D_uvtseq_rom_00400A80[i].unk1 = 0;
        D_uvtseq_rom_00400A80[i].unk2 = 0;
        D_uvtseq_rom_00400A80[i].unk4 = 0.0f;
        D_uvtseq_rom_00400A80[i].uvts.unk0 = 0;
        D_uvtseq_rom_00400A80[i].uvts.unk4 = 0;
        D_uvtseq_rom_00400A80[i].uvts.unk8 = 1;
        D_uvtseq_rom_00400A80[i].uvts.unk9 = 0;
        D_uvtseq_rom_00400A80[i].uvts.unkC = 1.0f;
    }

    D_uvtseq_rom_00400A94->func_uvcback_rom_0040016C(
        D_uvtseq_rom_00400A90->func_uvgfxmgr_rom_00400AB8(1), (s32) func_uvtseq_rom_00400668, 0, 0);
    D_uvtseq_rom_00400A8C = 0;
}

void func_uvtseq_rom_0040026C(void) {
    s32 var_s0;
    s32 var_s1;
    s32 i;

    D_uvtseq_rom_00400A94->func_uvcback_rom_00400320(
        D_uvtseq_rom_00400A90->func_uvgfxmgr_rom_00400AB8(1), (s32) func_uvtseq_rom_00400668);

    for (i = 0; i < D_uvtseq_rom_00400A84; i++) {
        _uvMemFree(D_uvtseq_rom_00400A80[i].uvts.unk4);
    }

    _uvMemFree(D_uvtseq_rom_00400A80);
    uvUnloadModule('STAT');
    uvUnloadModule('GMGR');
    uvUnloadModule('MATH');
    uvUnloadModule('MODL');
    uvUnloadModule('CHAN');
    uvUnloadModule('CBCK');
}

void func_uvtseq_rom_00400378(s32 arg0, s32 arg1) {
    UnkStruct_uvtseq_rom_00400A80 *temp_v1;
    ParsedUVTS *vts;

    temp_v1 = &D_uvtseq_rom_00400A80[arg0];
    if (arg1 == 0xFF) {
        temp_v1->unk0 = 0;
        return;
    }
    vts = uvGetLoadedFile('UVTS', arg1);
    if ((vts != NULL) || (((vts = uvLoadFile('UVTS', arg1)) != NULL))) {
        temp_v1->unk0 = 1;

        if (vts->unk9 == 1) {
            temp_v1->unk2 = vts->unk0 - 1;
        } else {
            temp_v1->unk2 = 0;
        }

        if (vts->unk0 != 0) {
            temp_v1->unk4 = vts->unk4[temp_v1->unk2].unk4;
        }

        _uvMediaCopy(&temp_v1->uvts, vts, sizeof(ParsedUVTS));
    }
}

void func_uvtseq_rom_00400454(s32 arg0, ...) {
    ParsedUVTS *temp_v1;
    UnkStruct_uvtseq_rom_00400A80 *temp_v0;
    int prop;
    va_list args;

    temp_v0 = &D_uvtseq_rom_00400A80[arg0];
    temp_v1 = &temp_v0->uvts;
    if (temp_v0 == NULL) {
        return;
    }
    va_start(args, arg0);
    if (temp_v1 == NULL) {
        return;
    }

    while (TRUE) {
        prop = va_arg(args, s32);
        switch (prop) {
            case 1:
                temp_v0->unk0 = va_arg(args, s32);
                break;
            case 3:
                prop = va_arg(args, s32);
                if (prop < temp_v1->unk0) {
                    temp_v0->unk2 = prop;
                }
                break;
            case 2:
                temp_v1->unk8 = va_arg(args, s32);
                break;
            case 5:
                temp_v1->unk9 = va_arg(args, s32);
                temp_v0->unk2 = temp_v1->unk0 - 1;
                break;
            case 4:
                temp_v1->unkC = va_arg(args, f64);
                break;
            case 6:
                D_uvtseq_rom_00400A8C = va_arg(args, s32);
                break;
            case 0:
                return;
        }
    }
}

s32 func_uvtseq_rom_004005BC(void) {
    s32 i;

    for (i = 0; i < D_uvtseq_rom_00400A84; i++) {
        if (D_uvtseq_rom_00400A80[i].unk1 == 0) {
            D_uvtseq_rom_00400A80[i].unk1 = 1;
            D_uvtseq_rom_00400A80[i].unk0 = 1;
            return i;
        }
    }
    return 0xFF;
}

void func_uvtseq_rom_00400620(s32 arg0) {
    if ((arg0 >= 0) && (arg0 < D_uvtseq_rom_00400A84)) {
        D_uvtseq_rom_00400A80[arg0].unk0 = 0;
        D_uvtseq_rom_00400A80[arg0].unk1 = 0;
    }
}

void func_uvtseq_rom_00400668(s32 arg0) {
    func_uvtseq_rom_00400688();
}

void func_uvtseq_rom_00400688(void) {
    s32 var_s0;
    s32 var_s1;
    s32 i;
    if (D_uvtseq_rom_00400A8C == 1) {
        return;
    }

    for (i = 0; i < D_uvtseq_rom_00400A84; i++) {
        if (D_uvtseq_rom_00400A80[i].unk0) {
            func_uvtseq_rom_00400714(i);
        }
    }
}

void func_uvtseq_rom_00400714(s32 arg0) {
    UnkStruct_uvtseq_rom_00400A80 *temp_a1;
    ParsedUVTS *uvts;

    temp_a1 = &D_uvtseq_rom_00400A80[arg0];
    uvts = &temp_a1->uvts;
    temp_a1->unk4 -= uvts->unkC * D_uvtseq_rom_00400A90->func_uvgfxmgr_rom_00401004();

    if (!(temp_a1->unk4 > 0.0f) && (temp_a1->unk4 <= 0.0f)) {
        while (TRUE) {
            switch (uvts->unk8) { /* irregular */
                case 1:
                    if (uvts->unk9 == 0) {
                        temp_a1->unk2 = ((temp_a1->unk2 + 1) % uvts->unk0);
                    } else {
                        temp_a1->unk2 =
                            (u8) ((s32) ((temp_a1->unk2 + uvts->unk0) - 1) % (s32) uvts->unk0);
                    }
                    break;
                case 0:
                    if (uvts->unk9 == 0) {
                        temp_a1->unk2++;
                        if ((temp_a1->unk2 + 1) == uvts->unk0) {
                            temp_a1->unk0 = 0;
                            return;
                        }
                    } else {
                        if (temp_a1->unk2 == 0) {
                            temp_a1->unk0 = 0;
                            return;
                        }
                        temp_a1->unk2--;
                    }
                    break;
                case 2:
                    if (uvts->unk9 == 0) {
                        temp_a1->unk2++;
                        if ((temp_a1->unk2 + 1) == uvts->unk0) {
                            uvts->unk9 = 1;
                        }
                    } else {
                        temp_a1->unk2--;
                        if (temp_a1->unk2 == 0) {
                            uvts->unk9 = 0;
                        }
                    }
                    break;
            }

            temp_a1->unk4 += uvts->unk4[temp_a1->unk2].unk4;
            if (!(temp_a1->unk4 <= 0.0f)) {
                return;
            }
        }
    }
}

u16 func_uvtseq_rom_0040093C(s32 arg0) {
    UnkStruct_uvtseq_rom_00400A80 *temp_v1;

    temp_v1 = &D_uvtseq_rom_00400A80[arg0];
    return temp_v1->uvts.unk4[temp_v1->unk2].unk0;
}

u8 func_uvtseq_rom_0040096C(s32 arg0) {
    UnkStruct_uvtseq_rom_00400A80 *temp_v1;

    temp_v1 = &D_uvtseq_rom_00400A80[arg0];
    return temp_v1->uvts.unk0;
}

u16 func_uvtseq_rom_0040098C(s32 arg0, s32 arg1) {
    UnkStruct_uvtseq_rom_00400A80 *temp_v1;

    temp_v1 = &D_uvtseq_rom_00400A80[arg0];
    arg1 = (s32) (temp_v1->unk2 + arg1) % (s32) temp_v1->uvts.unk0;
    return temp_v1->uvts.unk4[arg1].unk0;
}

u8 func_uvtseq_rom_004009F0(s32 arg0) {
    UnkStruct_uvtseq_rom_00400A80 *temp_v1;

    temp_v1 = &D_uvtseq_rom_00400A80[arg0];
    return temp_v1->unk0;
}

f32 func_uvtseq_rom_00400A10(s32 arg0) {
    UnkStruct_uvtseq_rom_00400A80 *temp_v0;

    temp_v0 = &D_uvtseq_rom_00400A80[arg0];
    return temp_v0->unk4 / temp_v0->uvts.unk4[temp_v0->unk2].unk4;
}
