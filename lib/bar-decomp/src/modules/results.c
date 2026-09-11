// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
typedef struct {
    /* 0x00 */ char pad0[0x08];
    /* 0x08 */ void (*unk8)(void *, s16, s32, f32, f32);
    /* 0x0C */ void (*unkC)(s16, s32, f32, s32);
    /* 0x10 */ char pad10[0x34];
    /* 0x44 */ void (*unk44)(s32);
    /* 0x48 */ char pad48[0x04];
    /* 0x4C */ f32 (*unk4C)(void);
    /* 0x50 */ char pad50[0x7C];
    /* 0xCC */ void (*unkCC)(void *);
} SndExp_0040659C;
extern f32 D_80025D6C;
extern f32 D_results_004072AC;
extern f32 D_results_004076F0;
extern s32 D_results_004075B8;
extern s32 gOptionsSpeechVol;
extern void *gSndExports;
extern void *gUvEmitterExports;
extern u8 D_results_00407720;
extern u8 D_results_00407728;
extern u8 D_results_00407730;
extern u8 D_results_00407738;
extern u8 D_results_00407740;
extern u8 D_results_00407748;
extern u8 D_results_00407750;
extern u8 D_results_00407751;
extern u8 D_8002CB9A;
extern u8 D_8002CD98;
extern void *D_results_00407718;
extern s16 D_results_00407568[];
extern s32 D_results_00407508[4][6];
extern s32 D_results_004075A8;
extern s32 D_results_004075AC;
extern s32 D_results_004075BC;
extern s32 D_results_004075C0;
void func_results_004001B8();
void func_results_004002BC();
void func_results_00400AC8();
void func_results_00401390();
void func_results_004013BC();
void func_results_00401B00();
void func_results_00402B9C();
void func_results_00403284();
void func_results_00404474();
void func_results_00404D20();
void func_results_00405608();
void func_results_00405DE8();
void func_results_00405E18();
void func_results_00405ED8();
void func_results_00405F94();
void func_results_004060F0();
void func_results_00406184();
void func_results_0040620C();
void func_results_00406338();
void func_results_004064C4(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s16 func_results_00406528();
s32 func_results_00406550();
void func_results_004066E8();
void func_results_00406710();
void func_results_00406738();
void func_results_004067CC();
void func_results_004067F4();
extern s32 D_results_004074F8[];
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/__entrypoint_func_results_400000.s")

void func_results_004001B8(void) {
    (*(void (**)(void *))((u8 *)gSndExports + 0x70))((u8 *)&D_results_00407720);
    (*(void (**)(void *))((u8 *)gSndExports + 0x70))((u8 *)&D_results_00407728);
    (*(void (**)(void *))((u8 *)gSndExports + 0x70))((u8 *)&D_results_00407730);
    (*(void (**)(void *))((u8 *)gSndExports + 0x70))((u8 *)&D_results_00407738);
    (*(void (**)(void *))((u8 *)gSndExports + 0x70))((u8 *)&D_results_00407740);
    (*(void (**)(void *))((u8 *)gSndExports + 0x70))((u8 *)&D_results_00407748);
    (*(void (**)(s32))((u8 *)gUvEmitterExports + 0x5C))(*(u8 *)&D_results_00407750);
    (*(void (**)(s32))((u8 *)gUvEmitterExports + 0x5C))(*(u8 *)&D_results_00407751);
}

s32 func_results_004002A8(s32 a0) {
    return D_results_004074F8[a0];
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_004002BC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00400AC8.s")

void func_results_00401390(void) {
    if (gGameSettings->introReplayState == 0) {
        gGameSettings->introReplayState = 1;
        gGameSettings->gameStateFlag = 5;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_004013BC.s")

void func_results_00401AF8(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00401B00.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00402B9C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00403284.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00404474.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00404D20.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00405608.s")

void func_results_00405DE8(void) {
    (*(void (**)(s32))((u8 *)D_results_00407718 + 0x3C))(0x64);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00405E18.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00405ED8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00405F94.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_004060F0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00406184.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_0040620C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/results/func_results_00406338.s")

void func_results_004064C4(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (*(s32 *)((u8 *)&D_8002CD98 + 0x258) < arg0) {
        *(s32 *)((u8 *)&D_8002CD98 + 0x258) = arg0;
    }
    if (*(s32 *)((u8 *)&D_8002CD98 + 0x25C) < arg1) {
        D_8002CB9A = 0;
        *(s32 *)((u8 *)&D_8002CD98 + 0x25C) = arg1;
    }
    if (*(s32 *)((u8 *)&D_8002CD98 + 0x260) < arg2) {
        *(s32 *)((u8 *)&D_8002CD98 + 0x260) = arg2;
    }
    if (*(s32 *)((u8 *)&D_8002CD98 + 0x264) < arg3) {
        *(s32 *)((u8 *)&D_8002CD98 + 0x264) = arg3;
    }
}

s16 func_results_00406528(s32 a0) {
    if (a0 < 0 || a0 >= 8) {
        a0 = 7;
    }
    return D_results_00407568[a0];
}

s32 func_results_00406550(s32 a0, s32 a1) {
    if (a0 < 0 || a0 >= 4) {
        a0 = 3;
    }
    if (a1 < 0 || a1 >= 6) {
        a1 = 5;
    }
    return D_results_00407508[a0][a1];
}

/* Plays a results speech sample with volume/pan derived from a per-frame delta timer. */
void func_results_0040659C(s16 arg0, s32 *arg1) {
    s64 sp40;
    f32 var_fs0;

    ((SndExp_0040659C *)gSndExports)->unkCC(&sp40);
    if (gOptionsSpeechVol == 0) {
        return;
    }
    ((SndExp_0040659C *)gSndExports)->unk44(gOptionsSpeechVol);
    var_fs0 = 2.0f - (D_80025D6C - D_results_004076F0);
    if (*arg1 != 0) {
        if (var_fs0 < 0.0f) {
            var_fs0 = 0.0f;
            ((SndExp_0040659C *)gSndExports)->unk8(&sp40, arg0, 0x7FFF, ((SndExp_0040659C *)gSndExports)->unk4C(), 0.5f);
        } else {
            ((SndExp_0040659C *)gSndExports)->unkC(arg0, 0x7FFF, ((SndExp_0040659C *)gSndExports)->unk4C(), (s32) (var_fs0 * D_results_004072AC));
        }
        D_results_004076F0 = D_80025D6C + var_fs0;
        *arg1 = 0;
    }
}

void func_results_004066E8(void) {
    func_results_0040659C(0xEB, &D_results_004075A8);
}

void func_results_00406710(void) {
    func_results_0040659C(0xEC, &D_results_004075AC);
}

void func_results_00406738(void) {
    if (gOptionsSpeechVol != 0) {
        (*(void (*)(s32))(*(s32 *)((u8 *)gSndExports + 0x44)))(gOptionsSpeechVol);
        if (D_results_004075B8 != 0) {
            (*(void (*)(void *, s32, s32, f32, f32))(*(s32 *)((u8 *)gSndExports + 0x8)))(
                (u8 *)&D_results_00407748, 0xCF, 0x7FFF,
                (*(f32 (*)(void))(*(s32 *)((u8 *)gSndExports + 0x4C)))(),
                0.5f);
            D_results_004075B8 = 0;
        }
    }
}

void func_results_004067CC(void) {
    func_results_0040659C(0xF4, &D_results_004075BC);
}

void func_results_004067F4(void) {
    func_results_0040659C(0xF5, &D_results_004075C0);
}

