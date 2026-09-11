// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "uvfmtx_rom.h"
#include "uvgfxmgr_rom.h"
#include "uvstring_rom.h"

typedef struct {
    f64 unk0;
    s32 unk8;
    u8 padC[4];
} Entry;

typedef struct {
    Entry entry[30];
} Unk802B92A0;

typedef struct UnkStruct_00402FB8_s {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
} UnkStruct_00402FB8;

typedef struct UnusedDebugStrings1_s {
    const char *dbgStrings[6];
} UnusedDebugStrings1;

typedef struct KernelDbgInfo_s {
    u8 *code;
    s32 codeSize;
    s32 form0Size;
    s32 formFilesSize;
    s32 unk10;
    s32 unk14;
} KernelDbgInfo;

// UvChannel exports
typedef struct UnkStruct_uvdbg_rom_00402FF8_s {
    char pad[0x4];
    s32 (*unk4)(s32, s32, s32 *, s32);
} UnkStruct_uvdbg_rom_00402FF8;

// UvCbck exports
typedef struct UnkStruct_uvdbg_rom_00402FFC_s {
    char pad[0x10];
    s32 (*unk10)(s32, void (*)(s32), s32, s32);
} UnkStruct_uvdbg_rom_00402FFC;

// UvGeom Exports
typedef struct UnkExports1_s {
    /* 0x00 */ char pad0[0x10];
    /* 0x10 */ void (*unk10)(s32, s32, s32, s32); /* inferred */
} UnkExports1;                                    /* size = 0x14 */

// uvFont exports
typedef struct UnkStruct_00402FD8_s {
    /* 0x00 */ char pad0[4];
    /* 0x04 */ void (*unk4)(s32);
    /* 0x08 */ char pad8[4];
    /* 0x0C */ void (*unkC)(s32, s32, s32, s32);
    /* 0x10 */ char pad10[0xC];    /* maybe part of unkC[4]? */
    /* 0x1C */ s32 (*unk1C)(void); /* inferred */
    /* 0x20 */ char pad20[4];
    /* 0x24 */ void (*unk24)(s32, s32, s8 *);
} UnkStruct_00402FD8; /* size = 0x28 */

// uvGfxState exports
typedef struct UnkStruct_00402FE4_s {
    char pad0[0xC];
    void (*unkC)(s32);
    void (*unk10)(s32);
    char pad14[0x3C];
    void (*unk50)(void);
    void (*unk54)(void);
    s32 pad58[2];
    s32 (*unk60)(s32);
} UnkStruct_00402FE4;

// UvCont exports
typedef struct UnkStruct_00402FF0_s {
    /* 0x00 */ char pad0[4];
    /* 0x04 */ s32 (*unk4)(void); /* inferred */
    /* 0x08 */ char pad8[4];
    /* 0x0C */ f32 (*unkC)(s32, s32);  /* inferred */
    /* 0x10 */ s32 (*unk10)(s32, s32); /* inferred */
    /* 0x14 */ char pad14[4];
    /* 0x18 */ s32 (*unk18)(s32, s32); /* inferred */
} UnkStruct_00402FF0;                  /* size = 0x1C */

// UvAudioMgr exports
typedef struct UnkStruct_00402FE0_s {
    s32 pad0;
    s32 (*unk4)(void);
} UnkStruct_00402FE0;

typedef struct UnkStruct_00402F34_s {
    const char *unk0;
    const char *unk4;
    const char *unk8;
    const char *unkC[2];
    const char *unk14;
    const char *unk18;
} UnkStruct_00402F34;

typedef struct UnkStruct_UvDbg_Rom_00402128_s {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC[2];
    s32 unk14;
    s32 unk18;
} UnkStruct_UvDbg_Rom_00402128;

// .bss
extern UvFMtx_Rom_Exports *D_uvdbg_rom_00402FF4;
extern UvGfxMgr_Exports *D_uvdbg_rom_00402FE8;
extern UvString_Exports *D_uvdbg_rom_00402FDC;
extern UnkExports1 *D_uvdbg_rom_00402FEC;
extern UnkStruct_uvdbg_rom_00402FF8 *D_uvdbg_rom_00402FF8;
extern UnkStruct_uvdbg_rom_00402FFC *D_uvdbg_rom_00402FFC;
extern f32 D_uvdbg_rom_00402F70;
extern f32 D_uvdbg_rom_00402F74;
extern f32 D_uvdbg_rom_00402F78;
extern f32 D_uvdbg_rom_00402F7C;
extern f32 D_uvdbg_rom_00402F80;
extern f32 D_uvdbg_rom_00402F84;
extern f32 D_uvdbg_rom_00402F88;
extern f32 D_uvdbg_rom_00402F8C;
extern f32 D_uvdbg_rom_00402F90;
extern f32 D_uvdbg_rom_00402F94;
extern f32 D_uvdbg_rom_00402F98;
extern f32 D_uvdbg_rom_00402F9C;
extern f32 D_uvdbg_rom_00402FA0;
extern f32 D_uvdbg_rom_00402FA4;
extern f32 D_uvdbg_rom_00402FA8;
extern f32 D_uvdbg_rom_00402FAC;
extern f32 D_uvdbg_rom_00402FB0;
extern f32 D_uvdbg_rom_00402FB4;
extern UnkStruct_00402FD8 *D_uvdbg_rom_00402FD8;
extern UnkStruct_00402FE4 *D_uvdbg_rom_00402FE4;
extern UnkStruct_00402FE0 *D_uvdbg_rom_00402FE0;
extern UnkStruct_00402FB8 D_uvdbg_rom_00402FB8[2];
extern UnkStruct_00402FF0 *D_uvdbg_rom_00402FF0;

// External variables
extern u8 gMemBlock[];
extern Unk802B92A0 D_8002E440[];
extern Unk802B92A0 D_8002DAE0[];
extern s32 D_8002EDB8[];
extern f64 D_8002EDD0[];
extern s32 D_8002EDA0[];
extern s32 *gModuleNameTags;
extern s32 D_80000318; // could also be an array

void __entrypoint_func_uvdbg_rom_400318(UvDbgRom_Exports *);
void func_uvdbg_rom_004014E4(void);
void func_uvdbg_rom_00400000(s32 arg0);
void func_uvdbg_rom_0040002C(s32 arg0);
void func_uvdbg_rom_00400058(s32 arg0);
void func_uvdbg_rom_00400084(s32 arg0);
void func_uvdbg_rom_004000B0(s32 arg0);
void func_uvdbg_rom_004000DC(s32 arg0);
void func_uvdbg_rom_00400108(s32 arg0);
void func_uvdbg_rom_00400134(s32 arg0);
void func_uvdbg_rom_00400160(s32 arg0);
void func_uvdbg_rom_0040018C(s32 arg0);
void func_uvdbg_rom_004001B8(s32 arg0);
void func_uvdbg_rom_004001E4(s32 arg0);
void func_uvdbg_rom_00400210(s32 arg0);
void func_uvdbg_rom_0040023C(s32 arg0);
void func_uvdbg_rom_00400268(s32 arg0);
void func_uvdbg_rom_00400294(s32 arg0);
void func_uvdbg_rom_004002C0(s32 arg0);
void func_uvdbg_rom_004002EC(s32 arg0);
void func_uvdbg_rom_00400490(void);
f64 func_uvdbg_rom_00400520(void);
f64 func_uvdbg_rom_004005B8(void);
f64 func_uvdbg_rom_0040075C(void);
f64 func_uvdbg_rom_004008E4(void);
f64 func_uvdbg_rom_00400A2C(void);
void func_uvdbg_rom_00400B74(f32 arg0, f32 arg1, s16 arg2, u8 arg3, u8 arg4, u8 arg5);
void func_uvdbg_rom_00400D74(s32 arg0);
void func_uvdbg_rom_00400E74(u8 arg0, u8 arg1);
void func_uvdbg_rom_004011BC(u8 arg0);
void func_uvdbg_rom_004011C4(u8 arg0, u8 arg1);
void func_uvdbg_rom_004011D0(const char *arg0, s32 arg1, s32 *arg2, s32 arg3);
void func_uvdbg_rom_00401318(s32 arg0);
void func_uvdbg_rom_004014DC(void);
void func_uvdbg_rom_004014E4(void);
void func_uvdbg_rom_004017A0(void);
void func_uvdbg_rom_004017A8(s32 arg0, s32 arg1, u16 arg2, u8 arg3, u8 arg4, u8 arg5);
void func_uvdbg_rom_00401AFC(void);
void func_uvdbg_rom_00401B90(u8 arg0);
void func_uvdbg_rom_00401D20(u8 arg0, u8 arg1, u8 arg2);
void func_uvdbg_rom_00401D94(s32 arg0, s32 arg1);
void func_uvdbg_rom_00401DE8(s32 arg0);
void func_uvdbg_rom_00402128(void);
void func_uvdbg_rom_00402490(void);
void func_uvdbg_rom_00402590(s32 arg0);
void func_uvdbg_rom_004025B8(const char *arg0, f32 arg1, s32 *arg2);
void func_uvdbg_rom_0040262C(void);
void func_uvdbg_rom_004029D4(s32 arg0);
void func_uvdbg_rom_004029E0(void);
void func_uvdbg_rom_00402A14(s32 arg0);
void func_uvdbg_rom_00402CB8(u8 arg0, u8 arg1);
void func_uvdbg_rom_00402D00(void);

//.data
s32 D_uvdbg_rom_00402F30 = 0;

static const char sDevStr[] = { "start: %f  (%d)   finish: %f (%d)\n" };

void func_uvdbg_rom_004017A8(s32 arg0, s32 arg1, u16 arg2, u8 arg3, u8 arg4, u8 arg5);

void func_uvdbg_rom_00400000(s32 arg0) {
    D_uvdbg_rom_00402F70 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_0040002C(s32 arg0) {
    D_uvdbg_rom_00402F74 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_00400058(s32 arg0) {
    D_uvdbg_rom_00402F78 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_00400084(s32 arg0) {
    D_uvdbg_rom_00402F7C = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_004000B0(s32 arg0) {
    D_uvdbg_rom_00402F80 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_004000DC(s32 arg0) {
    D_uvdbg_rom_00402F84 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_00400108(s32 arg0) {
    D_uvdbg_rom_00402F88 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_00400134(s32 arg0) {
    D_uvdbg_rom_00402F8C = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_00400160(s32 arg0) {
    D_uvdbg_rom_00402F90 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_0040018C(s32 arg0) {
    D_uvdbg_rom_00402F94 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_004001B8(s32 arg0) {
    D_uvdbg_rom_00402F98 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_004001E4(s32 arg0) {
    D_uvdbg_rom_00402F9C = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_00400210(s32 arg0) {
    D_uvdbg_rom_00402FA0 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_0040023C(s32 arg0) {
    D_uvdbg_rom_00402FA4 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_00400268(s32 arg0) {
    D_uvdbg_rom_00402FA8 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_00400294(s32 arg0) {
    D_uvdbg_rom_00402FAC = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_004002C0(s32 arg0) {
    D_uvdbg_rom_00402FB0 = (f32) uvClkGetSec(0);
}

void func_uvdbg_rom_004002EC(s32 arg0) {
    D_uvdbg_rom_00402FB4 = (f32) uvClkGetSec(0);
}

void __entrypoint_func_uvdbg_rom_400318(UvDbgRom_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvdbg_rom_00400490 = func_uvdbg_rom_00400490;
    exports->func_uvdbg_rom_00402D00 = func_uvdbg_rom_00402D00;
    exports->func_uvdbg_rom_00401D20 = func_uvdbg_rom_00401D20;
    exports->func_uvdbg_rom_00401D94 = func_uvdbg_rom_00401D94;
    exports->func_uvdbg_rom_00401DE8 = func_uvdbg_rom_00401DE8;
    exports->func_uvdbg_rom_00402128 = func_uvdbg_rom_00402128;
    exports->func_uvdbg_rom_00402490 = func_uvdbg_rom_00402490;
    exports->func_uvdbg_rom_004029D4 = func_uvdbg_rom_004029D4;
    exports->func_uvdbg_rom_004029E0 = func_uvdbg_rom_004029E0;
    exports->func_uvdbg_rom_00402A14 = func_uvdbg_rom_00402A14;
    exports->func_uvdbg_rom_00402CB8 = func_uvdbg_rom_00402CB8;
    #line 1
    D_uvdbg_rom_00402FD8 = uvLoadModule('FONT');
    D_uvdbg_rom_00402FDC = uvLoadModule('STRG');
    D_uvdbg_rom_00402FE0 = uvLoadModule('AMGR');
    D_uvdbg_rom_00402FE4 = uvLoadModule('STAT');
    D_uvdbg_rom_00402FE8 = uvLoadModule('GMGR');
    D_uvdbg_rom_00402FEC = uvLoadModule('GEOM');
    D_uvdbg_rom_00402FF0 = uvLoadModule('CONT');
    D_uvdbg_rom_00402FF4 = uvLoadModule('FMTX');
    D_uvdbg_rom_00402FF8 = uvLoadModule('CHAN');
    D_uvdbg_rom_00402FFC = uvLoadModule('CBCK');
    uvClkReset(0);
}

void func_uvdbg_rom_00400490(void) {
    uvUnloadModule('FONT');
    uvUnloadModule('STRG');
    uvUnloadModule('AMGR');
    uvUnloadModule('STAT');
    uvUnloadModule('GMGR');
    uvUnloadModule('DGEO');
    uvUnloadModule('CONT');
    uvUnloadModule('FMTX');
    uvUnloadModule('CHAN');
    uvUnloadModule('CBCK');
}

f64 func_uvdbg_rom_00400520(void) {
    s32 temp_hi;

    temp_hi = (s32) (gSchedRingIdx + 1) % 5;
    return D_8002EDD0[(s32) (temp_hi + 1) % 5] - D_8002EDD0[temp_hi];
}

f64 func_uvdbg_rom_004005B8(void) {
    s32 new_var;
    s32 temp_hi;
    s32 j;
    s32 var_a1;
    s32 var_a2;
    s32 i;
    f64 var_fv1;
    new_var = gSchedRingIdx;
    for (i = ((s32) (new_var + 1)) % 5, var_a1 = 0, var_a2 = 0; i != new_var; i = (i + 1) % 5) {
        for (j = 0; j < D_8002EDB8[i]; j++) {
            Entry *t0 = &(D_8002E440 + i)->entry[j];
            if (!var_a1) {
                if (t0->unk8 == 0x2A) {
                    var_a1 = 1;
                    var_fv1 = t0->unk0;
                }
                continue;
            }
            if (var_a1) {
                if (t0->unk8 == 0x2B) {
                    var_a2 |= 1;
                }
                if (t0->unk8 == 0x30) {
                    var_a2 |= 2;
                }
            }
            if (var_a2 == 3) {
                return t0->unk0 - var_fv1;
            }
        }
    }

    return 0;
}

f64 func_uvdbg_rom_0040075C(void) {
    s32 new_var;
    s32 temp_hi;
    s32 j;
    s32 var_a1;
    s32 var_a2;
    s32 i;
    f64 var_fv1;
    new_var = gSchedRingIdx;
    for (i = ((s32) (new_var + 1)) % 5, var_a1 = 0, var_a2 = 0; i != new_var; i = (i + 1) % 5) {
        for (j = 0; j < D_8002EDB8[i]; j++) {
            Entry *t0 = &(D_8002E440 + i)->entry[j];
            if (!var_a1) {
                if (t0->unk8 == 0x29) {
                    var_a1 = 1;
                    var_fv1 = t0->unk0;
                }
                continue;
            }

            if (var_a1 && t0->unk8 == 0x2C) {
                var_a2 |= 1;
            }

            if (var_a2 == 1) {
                return t0->unk0 - var_fv1;
            }
        }
    }

    return 0;
}

f64 func_uvdbg_rom_004008E4(void) {
    s32 new_var;
    s32 temp_hi;
    s32 j;
    s32 var_a1;
    s32 var_a2;
    s32 i;
    f64 var_fv1;

    new_var = gSchedRingIdx;
    i = ((s32) (new_var + 1)) % 5;
    var_fv1 = 0;
    var_a1 = 0;

    for (; i != new_var; i = (i + 1) % 5) {
        for (j = 0; j < D_8002EDA0[i]; j++) {
            Entry *t0 = &(D_8002DAE0 + i)->entry[j];
            if (!var_a1) {
                if (t0->unk8 == 0x29) {
                    var_a1 = 1;
                    var_fv1 = t0->unk0;
                }
                continue;
            }

            if (var_a1) {
                if (t0->unk8 == 0x2C) {
                    return t0->unk0 - var_fv1;
                }
            }
        }
    }

    return 0.0;
}

f64 func_uvdbg_rom_00400A2C(void) {
    s32 new_var;
    s32 temp_hi;
    s32 j;
    s32 var_a1;
    s32 var_a2;
    s32 i;
    f64 var_fv1;

    new_var = gSchedRingIdx;
    i = ((s32) (new_var + 1)) % 5;
    var_fv1 = 0;
    var_a1 = 0;

    for (; i != new_var; i = (i + 1) % 5) {
        for (j = 0; j < D_8002EDA0[i]; j++) {
            Entry *t0 = &(D_8002DAE0 + i)->entry[j];
            if (!var_a1) {
                if (t0->unk8 == 0x2A) {
                    var_a1 = 1;
                    var_fv1 = t0->unk0;
                }
                continue;
            }

            if (var_a1) {
                if (t0->unk8 == 0x2B) {
                    return t0->unk0 - var_fv1;
                }
            }
        }
    }

    return 0.0;
}

void func_uvdbg_rom_00400B74(f32 arg0, f32 arg1, s16 arg2, u8 arg3, u8 arg4, u8 arg5) {
    s16 sp36;
    s16 screenWidth;
    s16 temp_ft3_2;

    sp36 = arg2 + 2;
    screenWidth = (s32) ((arg1 * 1000.0f * 2) + 26.0f);
    if (arg0 < 0.0f) {
        return;
    }

    temp_ft3_2 = (s32) ((arg0 * 1000.0f * 2) + 26.0f);
    if (screenWidth < temp_ft3_2) {
        return;
    }

    if (D_uvdbg_rom_00402FE8->uvGetScreenWidth() < temp_ft3_2) {
        return;
    }

    if (screenWidth >= D_uvdbg_rom_00402FE8->uvGetScreenWidth()) {
        screenWidth = D_uvdbg_rom_00402FE8->uvGetScreenWidth() - 1;
    }

    if (temp_ft3_2 == screenWidth) {
        screenWidth = temp_ft3_2 + 1;
    }

    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_0040107C(arg3 / 255.0f, arg4 / 255.0f, arg5 / 255.0f, 1.0f);
    D_uvdbg_rom_00402FEC->unk10(temp_ft3_2, arg2, screenWidth, sp36);
}

void func_uvdbg_rom_00400D74(s32 arg0) {
    UNUSED char pad[40];
    char dest[40];

    D_uvdbg_rom_00402FD8->unk4(D_uvdbg_rom_00402F30);
    D_uvdbg_rom_00402FD8->unkC(0xFF, 0, 0, 0xFF);
    D_uvdbg_rom_00402FE4->unk50();
    D_uvdbg_rom_00402FE4->unkC(0xFFF);
    D_uvdbg_rom_00402FE4->unk10(0x80F00000);

    D_uvdbg_rom_00402FDC->uvSprintf(dest - 8, "MEM  %d", _uvMemGetBlocksSize());
    D_uvdbg_rom_00402FD8->unk24(0x28, 0xC8, dest - 8);
    D_uvdbg_rom_00402FE4->unk54();
}

void func_uvdbg_rom_00400E74(u8 arg0, u8 arg1) {
    s32 pad;
    s16 sp8A;
    s16 sp88;
    f64 var_fs0;
    char sp30[70];

    sp88 = 0x46;
    if (arg1 == 1) {
        var_fs0 = 1000.0;
    } else {
        var_fs0 = 1000000.0;
        ;
    }
    D_uvdbg_rom_00402FD8->unk4(D_uvdbg_rom_00402F30);
    D_uvdbg_rom_00402FD8->unkC(0xFF, 0xFF, 0, 0xFF);
    D_uvdbg_rom_00402FE4->unk50();
    D_uvdbg_rom_00402FE4->unkC(0xFFF);
    D_uvdbg_rom_00402FE4->unk10(0x80F00000);

    if (arg0 & 1) {
        D_uvdbg_rom_00402FDC->uvSprintf(sp30 - 8, "RET  %d",
                                        (s32) (func_uvdbg_rom_00400520() * var_fs0));
        D_uvdbg_rom_00402FD8->unk24(0x19, 0x46, sp30 - 8);
        sp88 = D_uvdbg_rom_00402FD8->unk1C() + 0x49;
    }
    if (arg0 & 2) {
        D_uvdbg_rom_00402FDC->uvSprintf(sp30 - 8, "GFX  %d",
                                        (s32) (func_uvdbg_rom_004005B8() * var_fs0));
        D_uvdbg_rom_00402FD8->unk24(0x19, sp88, sp30 - 8);
        sp88 += D_uvdbg_rom_00402FD8->unk1C() + 3;
    }
    if (arg0 & 4) {
        D_uvdbg_rom_00402FDC->uvSprintf(sp30 - 8, "CPU  %d",
                                        (s32) (func_uvdbg_rom_00400A2C() * var_fs0));
        D_uvdbg_rom_00402FD8->unk24(0x19, sp88, sp30 - 8);
        D_uvdbg_rom_00402FD8->unk1C();
    }
    sp8A = 0x5F;
    sp88 = 0x46;
    if (arg0 & 8) {
        D_uvdbg_rom_00402FDC->uvSprintf(sp30 - 8, "AUD  %d",
                                        (s32) (func_uvdbg_rom_0040075C() * var_fs0));
        D_uvdbg_rom_00402FD8->unk24(sp8A, 0x46, sp30 - 8);
        sp88 = D_uvdbg_rom_00402FD8->unk1C() + 0x49;
    }
    if (arg0 & 0x10) {
        D_uvdbg_rom_00402FDC->uvSprintf(sp30 - 8, "ACPU %d",
                                        (s32) (func_uvdbg_rom_004008E4() * var_fs0));
        D_uvdbg_rom_00402FD8->unk24(sp8A, sp88, sp30 - 8);
        D_uvdbg_rom_00402FD8->unk1C();
    }
    if (&sp8A)
        ; // Fake: Put sp8A on the stack
    D_uvdbg_rom_00402FE4->unk54();
}

void func_uvdbg_rom_004011BC(u8 arg0) {
}

void func_uvdbg_rom_004011C4(u8 arg0, u8 arg1) {
}

void func_uvdbg_rom_004011D0(const char *arg0, s32 arg1, s32 *arg2, s32 arg3) {
    char sp30[80];
    f64 sp28;
    s32 sp20;
    s32 temp_v0;
    s32 sp1C;

    sp28 = func_uvdbg_rom_004005B8();
    temp_v0 = D_uvdbg_rom_00402FE4->unk60(arg1);
    D_uvdbg_rom_00402FDC->uvSprintf(sp30, "%4.4s %+6.0d", arg0, temp_v0);
    D_uvdbg_rom_00402FD8->unk24(0x19, *arg2, sp30);
    if ((sp28 != 0.0) && (arg3 != 0)) {
        sp1C = 0x7D;
        D_uvdbg_rom_00402FDC->uvSprintf(sp30, "%+6.0d", (s32) ((f64) temp_v0 / sp28));
        D_uvdbg_rom_00402FD8->unk24(sp1C, *arg2, sp30);
    }
    if (&sp1C)
        ;
    *arg2 = (D_uvdbg_rom_00402FD8->unk1C() + 3) + *arg2;
}

void func_uvdbg_rom_00401318(s32 arg0) {
    s32 sp24;

    sp24 = 0x6E;
    D_uvdbg_rom_00402FD8->unkC(0xFF, 0xFF, 0, 0xFF);
    D_uvdbg_rom_00402FE4->unk50();
    D_uvdbg_rom_00402FE4->unkC(0xFFF);
    D_uvdbg_rom_00402FE4->unk10(0x80F00000);
    if (arg0 & 1) {
        func_uvdbg_rom_004011D0("VXFM", 0, &sp24, arg0 & 0x200);
    }
    if (arg0 & 2) {
        func_uvdbg_rom_004011D0("TRI ", 1, &sp24, arg0 & 0x200);
    }
    if (arg0 & 4) {
        func_uvdbg_rom_004011D0("MLD ", 2, &sp24, arg0 & 0x200);
    }
    if (arg0 & 8) {
        func_uvdbg_rom_004011D0("MMU ", 3, &sp24, arg0 & 0x200);
    }
    if (arg0 & 0x10) {
        func_uvdbg_rom_004011D0("TXL ", 4, &sp24, arg0 & 0x200);
    }
    if (arg0 & 0x20) {
        func_uvdbg_rom_004011D0("GFX ", 5, &sp24, arg0 & 0x200);
    }
    if (arg0 & 0x40) {
        func_uvdbg_rom_004011D0("VCL ", 6, &sp24, arg0 & 0x200);
    }
    if (arg0 & 0x80) {
        func_uvdbg_rom_004011D0("VTX ", 7, &sp24, arg0 & 0x200);
    }
    if (arg0 & 0x100) {
        func_uvdbg_rom_004011D0("SYNC", 8, &sp24, arg0 & 0x200);
    }
    D_uvdbg_rom_00402FE4->unk54();
}

static const char unused0[] = "frame time %f\n";
static const char unused1[] = "started at %f\n";
static const char unused2[] = "finished at %f\n";
static const char unused3[] = "\n\n";

// Individual string globals
const char gStr_StaticCode[] = "static code";
const char gStr_FrameBuffers[] = "frame buffers";
const char gStr_AudioHeap[] = "audio heap";
const char gStr_Modules[] = "modules";
const char gStr_Data[] = "data";
const char gStr_Unknown[] = "unknown";
const char gStr_Free[] = "free";

const char gStr_Unknown1[] = "unknown";
const char gStr_StaticCode1[] = "static code";
const char gStr_Filemap[] = "filemap";
const char gStr_Filesys[] = "filesys";
const char gStr_Free8[] = "free 8";
const char gStr_Free16[] = "free 16";

void func_uvdbg_rom_004014DC(void) {
}

void func_uvdbg_rom_004014E4(void) {
    s32 s0;
    s32 temp;

    D_uvdbg_rom_00402FE4->unk50();
    D_uvdbg_rom_00402FE4->unkC(0xFFF);
    D_uvdbg_rom_00402FE4->unk10(0x80F00000);

    s0 = 0x1A;
    temp = 0x3A;
    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_0040107C(0.0f, 0.0f, 0.0f, 1.0f);
    D_uvdbg_rom_00402FEC->unk10(s0, 0x2D, temp, 0x3F);

    s0 = temp;
    temp = 0x5C;
    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_0040107C(0.2f, 0.2f, 0.2f, 1.0f);
    D_uvdbg_rom_00402FEC->unk10(s0, 0x2D, temp, 0x3F);

    s0 = temp;
    temp = 0x7E;
    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_0040107C(0.0f, 0.0f, 0.0f, 1.0f);
    D_uvdbg_rom_00402FEC->unk10(s0, 0x2D, temp, 0x3F);

    s0 = temp;
    temp = 0x9E;
    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_0040107C(0.2f, 0.2f, 0.2f, 1.0f);
    D_uvdbg_rom_00402FEC->unk10(s0, 0x2D, temp, 0x3F);

    s0 = temp;
    temp = 0xC0;
    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_0040107C(0.0f, 0.0f, 0.0f, 1.0f);
    D_uvdbg_rom_00402FEC->unk10(s0, 0x2D, temp, 0x3F);

    s0 = temp;
    temp = 0xE2;
    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_0040107C(0.2f, 0.2f, 0.2f, 1.0f);
    D_uvdbg_rom_00402FEC->unk10(s0, 0x2D, temp, 0x3F);

    s0 = temp;
    temp = 0x102;
    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_0040107C(0.0f, 0.0f, 0.0f, 1.0f);
    D_uvdbg_rom_00402FEC->unk10(s0, 0x2D, temp, 0x3F);

    s0 = temp;
    temp = 0x124;
    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_0040107C(0.2f, 0.2f, 0.2f, 1.0f);
    D_uvdbg_rom_00402FEC->unk10(s0, 0x2D, temp, 0x3F);

    D_uvdbg_rom_00402FE4->unk54();
}

void func_uvdbg_rom_004017A0(void) {
}

void func_uvdbg_rom_004017A8(s32 arg0, s32 arg1, u16 arg2, u8 arg3, u8 arg4, u8 arg5) {
    int idx;
    int hi;
    Entry *sp4C;
    s32 sp48;
    s32 sp44;
    s32 *sp38;
    s32 sp40;
    s32 sp3C;
    s32 sp34;
    s32 sp30;
    f64 sp28;
    f64 sp20;

    idx = gSchedRingIdx;
    hi = (idx + 1) % 5;
    sp44 = hi;
    sp28 = D_8002EDD0[sp44];
    if (arg0 == 0) {
        sp38 = D_8002EDA0;
    } else {
        sp38 = D_8002EDB8;
    }

    if (arg1 == 0x29) {
        sp34 = 0x29;
        sp30 = 0x2C;
    } else {
        sp34 = 0x2A;
        sp30 = 0x2B;
    }

    sp48 = -1;
    sp3C = 0;
    for (;;) {
        sp48 += 1;
        if (sp48 > sp38[sp44]) {
            sp3C += 1;
            sp48 = 0;
            sp44 = (sp44 + 1) % 5;
        }
        if (sp3C >= 5) {
            break;
        }
        if (arg0 == 0) {
            sp4C = &D_8002DAE0[sp44].entry[sp48];
        } else {
            sp4C = &D_8002E440[sp44].entry[sp48];
        }
        if (sp4C->unk8 == sp34) {
            break;
        }
        if (sp4C->unk8 == sp30) {
            func_uvdbg_rom_00400B74(0.0f, (f32) (sp4C->unk0 - sp28), arg2, arg3, arg4, arg5);
            break;
        }
    }

    sp48 = -1;
    sp3C = 0;
    sp40 = -1;
    sp44 = hi;

    for (;;) {
        sp48 += 1;
        if (sp48 > sp38[sp44]) {
            sp3C += 1;
            sp48 = 0;
            sp44 = (sp44 + 1) % 5;
        }
        if (sp3C >= 4) {
            break;
        }
        if (arg0 == 0) {
            sp4C = &D_8002DAE0[sp44].entry[sp48];
        } else {
            sp4C = &D_8002E440[sp44].entry[sp48];
        }
        if (sp40 != sp34) {
            if (sp4C->unk8 == sp34) {
                sp40 = sp34;
                sp20 = sp4C->unk0;
            }
        } else if (sp4C->unk8 != sp30) {
        } else {
            func_uvdbg_rom_00400B74((f32) (sp20 - sp28), (f32) (sp4C->unk0 - sp28), arg2, arg3, arg4,
                                    arg5);
            sp40 = -1;
        }
    }
}

void func_uvdbg_rom_00401AFC(void) {
    func_uvdbg_rom_004017A8(0, 0x2A, 0x3C, 0, 0, 0xFF);
    func_uvdbg_rom_004017A8(0, 0x29, 0x37, 0xFF, 0, 0xFF);
    func_uvdbg_rom_004017A8(1, 0x2A, 0x32, 0, 0xFF, 0);
    func_uvdbg_rom_004017A8(1, 0x29, 0x2D, 0xFF, 0, 0);
}

void func_uvdbg_rom_00401B90(u8 arg0) {
    Mtx4F sp68;
    Mtx4F sp28;

    D_uvdbg_rom_00402FF4->func_00402110(&sp28, 0.0f, D_uvdbg_rom_00402FE8->uvGetScreenWidth(), 0.0f,
                                        D_uvdbg_rom_00402FE8->uvGetScreenHeight());
    D_uvdbg_rom_00402FF4->func_00402908(&sp28);
    D_uvdbg_rom_00402FF4->func_00400B68(&sp68);
    D_uvdbg_rom_00402FF4->func_004029DC(&sp68);
    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_00401BD4(0, D_uvdbg_rom_00402FE8->uvGetScreenWidth() - 1, 0,
                                                     D_uvdbg_rom_00402FE8->uvGetScreenHeight() - 1);
    func_uvdbg_rom_004014E4();
    D_uvdbg_rom_00402FE4->unk50();
    D_uvdbg_rom_00402FE4->unkC(0xFFF);
    D_uvdbg_rom_00402FE4->unk10(0xF00000);
    func_uvdbg_rom_004017A0();
    func_uvdbg_rom_00401AFC();
    D_uvdbg_rom_00402FE4->unk54();
}

void func_uvdbg_rom_00401D20(u8 arg0, u8 arg1, u8 arg2) {
    if (arg1 == 1) {
        func_uvdbg_rom_00401B90(arg0);
    } else if (arg1 == 2) {
        func_uvdbg_rom_00400E74(arg0, arg2);
    } else {
        func_uvdbg_rom_004011C4(arg0, arg2);
    }
}

void func_uvdbg_rom_00401D94(s32 arg0, s32 arg1) {
    if ((arg0 != 0) && (arg1 != 1)) {
        if (arg1 == 2) {
            func_uvdbg_rom_00401318(arg0);
            return;
        }
        if (arg1 == 3) {
            func_uvdbg_rom_004014DC();
        }
    }
}

void func_uvdbg_rom_00401DE8(s32 arg0) {
    f32 temp_fs0;
    f32 temp_fv0;
    f32 var_fs1;
    f32 var_fs2;
    s32 vram;
    s32 tagPtr;
    s32 formFileId;
    s32 formFileEntryId;
    s32 var_v0;

    // Why IDO?
    // 0 is fs4
    // 0.0f is fs3
    var_fs2 = 0;
    var_fs1 = 0;
    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_00400B24();
    while (D_uvdbg_rom_00402FF0->unk4() != 0) {
        temp_fs0 = D_uvdbg_rom_00402FF0->unkC(arg0, 0);
        temp_fv0 = D_uvdbg_rom_00402FF0->unkC(arg0, 1);
        var_fs2 += temp_fs0 * FABS(temp_fs0) * 0.05f;
        var_fs1 += temp_fv0 * FABS(temp_fv0) * 0.2f;
        if ((2.0f * (f32) D_uvdbg_rom_00402FE8->uvGetScreenWidth() * 0.0625f) < var_fs2) {
            var_fs2 = 0;
            var_fs1++;
        }
        if (var_fs2 < 0) {
            var_fs2 = 2.0f * D_uvdbg_rom_00402FE8->uvGetScreenWidth() * 0.0625f;
            var_fs1 -= 1.0f;
        }
        if (var_fs1 < 0.0f) {
            var_fs1 = 0;
        }
        if ((4194304 / (2.0f * D_uvdbg_rom_00402FE8->uvGetScreenWidth())) < var_fs1) {
            var_fs1 = 4194304.0f / (2.0f * D_uvdbg_rom_00402FE8->uvGetScreenWidth());
        }
        vram = ((D_uvdbg_rom_00402FE8->uvGetScreenWidth() * (s32) var_fs1 * 2) + ((s32) var_fs2 * 0x10)
                + 0x8000003F)
               & ~0x3F;
        osViSwapBuffer(vram + 0xFFFECDC0);
        if ((D_uvdbg_rom_00402FF0->unk18(arg0, 0x2000) != 0)
            || (D_uvdbg_rom_00402FF0->unk18(arg0, 0x1000) != 0)) {
            break;
        }

        if (D_uvdbg_rom_00402FF0->unk10(arg0, 0x8000) != 0) {
            func_8000226C(&tagPtr, &formFileEntryId, &formFileId, vram); // Get form file by vram
            var_v0 = 0;
            while (var_v0 != 2) {
                var_v0++;
            }
        }
    }

    D_uvdbg_rom_00402FE8->func_uvgfxmgr_rom_004007F8();
}

UnkStruct_00402F34 D_uvdbg_rom_00402F34 = {
    gStr_StaticCode, gStr_FrameBuffers, gStr_AudioHeap, { gStr_Modules, gStr_Data },
    gStr_Unknown,    gStr_Free,
};

UnusedDebugStrings1 D_uvdbg_rom_00402F50 = { { gStr_Unknown1, gStr_StaticCode1, gStr_Filemap,
                                               gStr_Filesys, gStr_Free8, gStr_Free16 } };

s32 unused[] = { 0x002C0000, &__entrypoint_func_uvdbg_rom_400318 };

void func_uvdbg_rom_00402128(void) {
    int v0;
    UnkStruct_00402F34 sp1F8;
    UnkStruct_UvDbg_Rom_00402128 sp1DC;
    s32 sp114[0x32];
    s32 sp4C[0x32];
    s32 i; // var_s0
    s32 sp44;
    s32 sp40;

    sp1F8 = D_uvdbg_rom_00402F34;
    sp1DC.unk0 = gMemBlock - main_VRAM;
    sp1DC.unk4 =
        D_uvdbg_rom_00402FE8->uvGetScreenWidth() * D_uvdbg_rom_00402FE8->uvGetScreenHeight() * 6;
    sp1DC.unk8 = D_uvdbg_rom_00402FE0->unk4();
    sp1DC.unkC[0] = sp1DC.unkC[1] = sp1DC.unk14 = 0;

    sp1DC.unk18 = _uvMemGetBlocksSize();
    func_8000218C(sp114, sp4C, &sp44, 0x32);

    for (i = 0; i < sp44; i++) {
        if (sp114[i] == 'UVMO') {
            sp1DC.unkC[0] += sp4C[i];
        } else {
            sp1DC.unkC[1] += sp4C[i];
        }
    }

    sp1DC.unkC[0] = (sp1DC.unkC[0] - sp1DC.unk8) - sp1DC.unk4;
    sp1DC.unk14 =
        D_80000318 - sp1DC.unk0 - sp1DC.unk4 - sp1DC.unk8 - sp1DC.unkC[0] - sp1DC.unkC[1] - sp1DC.unk18;

    for (i = 0; i < 7; i++) {
    }

    for (i = 0; i < sp44; i++) {
        if (sp114[i]) {
        }
    }

    sp40 = uvGetFilesCount('UVMO');
    for (i = 0; i < sp40; i++) {
        v0 = uvGetUnusedFileInfo('UVMO', i);
        if (gModuleNameTags[i] == 'AMGR') {
            v0 -= sp1DC.unk8;
        }
        if (gModuleNameTags[i] == 'GMGR') {
            v0 -= sp1DC.unk8;
        }
        if (v0) {
        }
    }
}

void func_uvdbg_rom_00402490(void) {
    s32 pad2;
    UnusedDebugStrings1 dbgStrings;
    KernelDbgInfo info;
    char spD0[0xC4];
    s32 spEC;
    char pad[0xBC];
    s32 i;
    s32 pad24;
    s32 sp24;
    s32 sp20;

    dbgStrings = D_uvdbg_rom_00402F50;
    info.code = main_ROM_START;
    info.codeSize = FORM0_ROM_START - main_ROM_START;
    info.form0Size = gModuleFilesSize;
    info.formFilesSize = gFormFilesSize;
    info.unk10 = (u8 *) 0x800000 - gRomEnd;
    info.unk14 = (u8 *) 0x01000000 - gRomEnd;

    i = 0;
    while (i < 6) {
        i++;
    }

    func_80002088(&spEC, &sp24, &sp20, 0x32);
    for (i = 0; i < sp20; i++) {
    }
    if (&info)
        ; // FAKE: force the compiler to use this var
}

void func_uvdbg_rom_00402590(s32 arg0) {
    s32 *temp_v0;

    temp_v0 = arg0 + (s32 *) D_uvdbg_rom_00402FB8;
    if (*temp_v0 != 0) {
        *temp_v0 = 0;
    }
}

void func_uvdbg_rom_004025B8(const char *arg0, f32 arg1, s32 *arg2) {
    s32 pad;
    char sp20[250];

    sprintf(sp20, "%12.12s : %5.2f", arg0, (f64) arg1);
    D_uvdbg_rom_00402FD8->unk24(0x1E, *arg2, sp20);
    *arg2 -= 0xA;
}

void func_uvdbg_rom_0040262C(void) {
    s32 sp2C;

    sp2C = 0xC8;
    D_uvdbg_rom_00402FD8->unk4(D_uvdbg_rom_00402F30);
    D_uvdbg_rom_00402FD8->unkC(0xFF, 0xFF, 0, 0xFF);
    func_uvdbg_rom_004025B8("PRE", (D_uvdbg_rom_00402F74 - D_uvdbg_rom_00402F70) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("ENV", (D_uvdbg_rom_00402F78 - D_uvdbg_rom_00402F74) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("TERRA", (D_uvdbg_rom_00402F7C - D_uvdbg_rom_00402F78) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("TILEFAR", (D_uvdbg_rom_00402F80 - D_uvdbg_rom_00402F7C) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("DOBJ", (D_uvdbg_rom_00402F84 - D_uvdbg_rom_00402F80) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("FX", (D_uvdbg_rom_00402F88 - D_uvdbg_rom_00402F84) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("DOBJ1", (D_uvdbg_rom_00402F8C - D_uvdbg_rom_00402F88) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("FOG", (D_uvdbg_rom_00402F90 - D_uvdbg_rom_00402F8C) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("SORT1", (D_uvdbg_rom_00402F94 - D_uvdbg_rom_00402F90) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("FOGOVER", (D_uvdbg_rom_00402F98 - D_uvdbg_rom_00402F94) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("PFX", (D_uvdbg_rom_00402F9C - D_uvdbg_rom_00402F98) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("TILENEAR", (D_uvdbg_rom_00402FA0 - D_uvdbg_rom_00402F9C) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("DOBJ2", (D_uvdbg_rom_00402FA4 - D_uvdbg_rom_00402FA0) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("SORT2", (D_uvdbg_rom_00402FA8 - D_uvdbg_rom_00402FA4) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("SHADOW", (D_uvdbg_rom_00402FAC - D_uvdbg_rom_00402FA8) * 1000.0f, &sp2C);
    func_uvdbg_rom_004025B8("DOBJCLOSE", (D_uvdbg_rom_00402FB0 - D_uvdbg_rom_00402FAC) * 1000.0f,
                            &sp2C);
    func_uvdbg_rom_004025B8("POST", (D_uvdbg_rom_00402FB4 - D_uvdbg_rom_00402FB0) * 1000.0f, &sp2C);
}

void func_uvdbg_rom_004029D4(s32 arg0) {
    D_uvdbg_rom_00402F30 = arg0;
}

void func_uvdbg_rom_004029E0(void) {
    int i;
    for (i = 0; i < 2; i++) {
        D_uvdbg_rom_00402FB8[i].unk4 = 1;
        D_uvdbg_rom_00402FB8[i].unk8 = 1;
        D_uvdbg_rom_00402FB8[i].unkC = 1;
        D_uvdbg_rom_00402FB8[i].unk0 = 1;
    }
}

void func_uvdbg_rom_00402A14(s32 arg0) {
    s32 sp24;

    D_uvdbg_rom_00402FF8->unk4(arg0, 6, &sp24, 0);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_00400000, 0, 1);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_0040002C, 0, 0x13);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_00400058, 0, 0x15);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_00400084, 0, 0x29);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_004000B0, 0, 0x2E);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_004000DC, 0, 0x3D);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_00400108, 0, 0x42);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_00400134, 0, 0x51);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_00400160, 0, 0x53);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_0040018C, 0, 0x56);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_004001B8, 0, 0x58);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_004001E4, 0, 0x5B);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_00400210, 0, 0x65);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_0040023C, 0, 0x6F);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_00400268, 0, 0x74);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_00400294, 0, 0x83);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_004002C0, 0, 0x97);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_004002EC, 0, 0xFE);
    D_uvdbg_rom_00402FFC->unk10(sp24, func_uvdbg_rom_00402590, 0, 0xFF);
}

void func_uvdbg_rom_00402CB8(u8 arg0, u8 arg1) {
    if (arg1 == 2) {
        func_uvdbg_rom_00400D74(arg0);
    } else {
        func_uvdbg_rom_004011BC(arg0);
    }
}

void func_uvdbg_rom_00402D00(void) {
    func_uvdbg_rom_0040262C();
}
