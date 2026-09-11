// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "global_exports.h"

typedef struct UnkStruct_80025BD8_s {
    char pad0[0x14];
    void (*unk14)(f32);
} UnkStruct_80025BD8;

typedef struct UnkStruct_80025C54_s {
    char pad0[0xC];
    void (*unkC)(s32, f32);
    u8 pad10[0x120 - 0x10];
    s32 (*unk120)(s32);
    s32 (*unk124)(s32);
    s32 (*unk128)(s32, s32, f32);
} UnkStruct_80025C54;

typedef struct UnkStruct_80025C64_s {
    char pad00[8];
    void (*unk8)(s32, s32);
    char pad68[0x64];
    s32 (*unk70)(void);
    s32 (*unk74)(s32);
    void (*unk78)(s32, s32);
} UnkStruct_80025C64;

typedef struct UnkStruct_80025C84_s {
    s32 pad0;
    s32 (*unk4)(s32);
} UnkStruct_80025C84;

typedef struct UnkStruct_80025C90_s {
    /* 0x00 */ char pad0[0x10];
    /* 0x10 */ s32 (*unk10)(void);
    /* 0x18 */ char pad14[0xC];                       /* maybe part of unk20[3]? */
    /* 0x20 */ s32 (*unk20)(s32);                     /* inferred */
    /* 0x24 */ char pad24[0x10];                    /* maybe part of unk20[5]? */
    /* 0x34 */ void (*unk34)(s32);
    /* 0x38 */ void (*unk38)(s32, s32);
    /* 0x3C */ void (*unk3C)(s32);
} UnkStruct_80025C90;                               /* size = 0x40 */

typedef struct UnkStruct_8002CCB0_s {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
} UnkStruct_8002CCB0;

typedef struct UnkStruct_intro_004009C0_s {
    s32 pad0;
} UnkStruct_intro_004009C0;

typedef struct UnkStruct_intro_00400A88_s {
    char pad0[0x8 - 0x0];
    void (*unk8)(s32,s32);
    char padC[0x14 - 0xC];
    void (*unk14)(s32,s32);
    char pad18[0x48 - 0x18];
    void (*unk48)(s32);
    void (*unk4C)(void);
} UnkStruct_intro_00400A88;

typedef struct UnkStruct_intro_00400A90_s {
    /* 0x00 */ struct UnkPlayer* unk0;
    /* 0x04 */ s32 (*unk4)(void*);
    /* 0x08 */ s32 (*unk8[2])(void*);                       /* maybe part of unk4[3]? */
    /* 0x0C */ void (*unkC)(s32);
    /* 0x10 */ void (*unk10)(s32);
    /* 0x14 */ s32 (*unk14)(s32);                   /* inferred */
    /* 0x18 */ char pad18[4];
    /* 0x1C */ void (*unk1C)(s32, ...);
} UnkStruct_intro_00400A90;

typedef struct UnkStruct_intro_00400A84_s {
    char pad0[0x4];
    s32 (*unk4)(s32);
    void (*unk8)(struct UnkPlayer*);
    void (*unkC)(struct UnkPlayer*);
    char pad24[0x10];
} UnkStruct_intro_00400A84;

typedef struct UnkStruct_80025C88_s {
    /* 0x00 */ void (*unk0)(void);                      /* inferred */
    /* 0x04 */ void (*unk4)(s32, s32);
} UnkStruct_80025C88;

typedef struct UnkStruct_80025C78_s {
    /* 0x00 */ void (*unk0)(void);                      /* inferred */
    /* 0x04 */ void (*unk4)(s32, s32);
               char pad8[0x34-0x8];
               void (*unk34)(s32);
               void (*unk38)(s32);
               void (*unk3C)(s32);
} UnkStruct_80025C78;

typedef struct UnkStruct_80025C10_s {
    /* 0x00 */ void (*unk0)(void);                      /* inferred */
    /* 0x04 */ void (*unk4)(s32, s32);
               char pad8[0x4C-0x8];
               void (*unk4C)(s32, s32);
} UnkStruct_80025C10;

typedef struct UnkStruct_80025BE4_s {
    char pad8[0x2C];
    void (*unk2C)(void);                      /* inferred */
} UnkStruct_80025BE4;

typedef struct UnkPlayer_Unk0 {
    u8 pad0[0x118];
    s32 unk118;
} UnkPlayer_Unk0;
typedef struct UnkPlayer_Unk4 {
    u8 pad0[0x404];
    s32 unk404;
    s32 unk408;
} UnkPlayer_Unk4;
typedef struct UnkPlayer_UnkC {
    u8 pad0[0x140];
    s32 unk140;
} UnkPlayer_UnkC;
typedef struct UnkPlayer {
    UnkPlayer_Unk0* unk0;
    UnkPlayer_Unk4* unk4;
    s32 pad8[0x1];
    UnkPlayer_UnkC* unkC;
} UnkPlayer;

void __entrypoint_func_intro_400000(Intro_Exports *arg0);
void func_intro_004004F0(void);
void func_intro_004005CC(void);
void func_intro_00400820(void);
void func_intro_0040087C(void);
s32 func_intro_00400960(s32* arg0, void *arg1, void *arg2, s32* arg3);

extern UnkStruct_80025BE4* gUvCmidiExports;
extern UnkStruct_80025C10* gUvModelExports;
extern UnkStruct_80025C14* gUvSprtExports;
extern UnkStruct_80025C54* gUvDynExports;
extern UnkStruct_80025C74* gGameGuiExports;
extern UnkStruct_80025C78* gSndExports;
extern UnkStruct_80025C88* gScrnExports;
extern UnkStruct_80025C90* gReplayExports;
extern UnkStruct_8002CCB0 D_8002CCB0[];
extern u8 D_intro_00400A04[];
extern u8 D_intro_00400A0C[];
extern u16 D_intro_00400A14[];
extern s32 D_intro_00400A20[][2];
extern UnkStruct_8002CCB0 D_intro_00400A50;
extern UnkStruct_intro_00400A84* D_intro_00400A84;
extern UnkStruct_intro_00400A88* D_intro_00400A88;
extern UnkStruct_8002D1A4* D_intro_00400A8C;
extern UnkStruct_intro_00400A90 D_intro_00400A90;
extern s32 D_intro_00400A98;
extern s32 D_intro_00400AA0[];
extern UnkStruct_8002CCB0 D_intro_00400AA8[];
extern s32 gCurrentReplayEvent;
extern UnkStruct_80025CF0 gGameSettings[];
extern UnkStruct_80025BD8* gUvAudiomgrExports;
extern UnkStruct_80025C54* gUvDynExports;
extern f32 D_intro_004009C8;
extern f32 D_intro_004009D4[];
extern s32 D_intro_004009EC[];
extern UnkStruct_intro_00400A84* D_intro_00400A84;
extern UnkStruct_80025C90* gReplayExports;
extern u8 D_intro_004009D0;
extern s32 D_intro_00400A94;
extern UnkStruct_80025C84* gSceneExports;
extern UnkStruct_80025C64* gUvGuiExports;
extern s32 D_intro_00400A9C;
extern UnkStruct_intro_004009C0 D_intro_004009C0;
extern UnkStruct_80025C84* gSceneExports;

// init "intro" game state
void __entrypoint_func_intro_400000(Intro_Exports *arg0) {
    s32 pad[3];
    s32 sp70;
    s32 temp_a2;
    s32 i;
    s32 j;

    uvUpdateFileAllocPtr(arg0);
    arg0->func_intro_004004F0 = func_intro_004004F0;
    arg0->func_intro_004005CC = &func_intro_004005CC;
    arg0->func_intro_00400820 = func_intro_00400820;

    D_intro_00400A90.unk4  = gGameSettings->numAiCars;
    gGameSettings->numAiCars = 0;
    for (j = 0; j < 2; j++) {
        D_intro_00400AA8[j] = D_8002CCB0[j];
        D_intro_00400AA0[j] = gGameSettings->unk704C[j];
        D_8002CCB0[j] = D_intro_00400A50;
        gGameSettings->unk704C[j] = 0;
    }
    D_intro_00400A84 = uvLoadModule('plyr');
    D_intro_00400A88 = uvLoadModule('motn');
    D_intro_00400A8C = uvGetLoadedModule('cara');
    if (gGameSettings->unkPtr90 != 0) {
        gUvDynExports->unk128(gGameSettings->unkPtr90, 0, 1.0f);
    }
    D_intro_00400A90.unk0 = D_intro_00400A84->unk4(0);
    sp70 = D_intro_00400A90.unk0->unkC;
    gScrnExports->unk4(6, &sp70);
    D_intro_00400A90.unk0->unkC->unk140 = 3;
    D_intro_00400A90.unk0->unk4->unk408 = D_intro_00400A04[gGameSettings->finishedIntroCount];
    D_intro_00400A90.unk0->unk4->unk404 = D_intro_00400A0C[gGameSettings->finishedIntroCount];
    D_intro_00400A90.unk0->unk0->unk118 = 1;
    D_intro_00400A88->unk8(D_intro_00400A90.unk0->unk0, 0xA);
    D_intro_00400A88->unk14(D_intro_00400A90.unk0->unk0, 0x42569446);
    D_intro_00400A8C->unk8(D_intro_00400A90.unk0->unk4);
    if (gGameSettings->unkPtr90 != 0) {
        gUvDynExports->unk120(gGameSettings->unkPtr90);
    }

    for (i = 0; i < 2; i++) {
        uvLoadFile('UVBT', D_intro_00400A20[gGameSettings->finishedIntroCount][i]);
        D_intro_00400A90.unk8[i] = gUvSprtExports->unk4();
        gUvSprtExports->unk1C(D_intro_00400A90.unk8[i], 9, D_intro_00400A20[gGameSettings->finishedIntroCount][i], 3, 1, 0xB, 1, 0xC, 0, 6, 0, 0);
    }

    temp_a2 = 0xA0 - (gUvSprtExports->unk14(D_intro_00400A90.unk8[0]) / 2); // This broke the entire function lol
    gUvSprtExports->unk1C(D_intro_00400A90.unk8[0], 2, temp_a2, 0x64, 0);
    gUvSprtExports->unk1C(D_intro_00400A90.unk8[1], 2, temp_a2, 0x64, 0);
    uvLoadFile('UVMD', 0x116);
    gUvModelExports->unk4C(0x116, func_intro_00400960);
    gGameGuiExports->unk1C((s32) func_intro_0040087C);
    gGameGuiExports->unk10(0);
    gReplayExports->unk10();
    if (gGameSettings->dbgOptsRecordIntro == 0) {
        gReplayExports->unk3C(&gCurrentReplayEvent);
        gGameSettings->introReplayState = 1;
    }
    uvClkReset(1);
    gSndExports->unk3C(gGameSettings->optionsMusicVol);
    gUvCmidiExports->unk2C();
    gSndExports->unk34(D_intro_00400A14[gGameSettings->finishedIntroCount]);
    gSndExports->unk38(0);
}

// exit "intro" game state
void func_intro_004004F0(void) {
    int i;

    gGameSettings->numAiCars = (s32) D_intro_00400A94;
    if (gGameSettings->gameStateFlag != 2) {
        gGameSettings->currentTrack = gSceneExports->unk4(0);
        gGameSettings->finishedIntroCount = 0;
        gGameSettings->dbgOptsRecordIntro = 0;
        gGameSettings->introReplayState = 0;
    }

    for (i = 0; i < 2; i++) {
        D_8002CCB0[i] = D_intro_00400AA8[i]; // M2C_MEMCPY_ALIGNED indicates a struct copy
        gGameSettings->unk704C[i] = D_intro_00400AA0[i];
    }

}

// check if player pressed button, otherwise proceed to next track
void func_intro_004005CC(void) {
    s32 sp1C;
    f32 temp_fv1;

    sp1C = gReplayExports->unk20(0);
    temp_fv1 = D_intro_004009D4[gGameSettings->finishedIntroCount] - uvClkGetSec(1);
    if (((sp1C & 0x1000) || (sp1C & 0x8000)) && (gGameSettings->dbgOptsRecordIntro == 0)) {
        gGameSettings->gameStateFlag = 0xE;
    } else {
        if ((gGameSettings->dbgOptsRecordIntro != 0) && (temp_fv1 < -2.0f)) {
            gReplayExports->unk38((s32) &gCurrentReplayEvent, sp1C);
            gGameSettings->finishedIntroCount++;
            if (gGameSettings->finishedIntroCount >= 6) {
                gGameSettings->gameStateFlag = 0xE;
                return;
            }
            gGameSettings->gameStateFlag = 2;
            gGameSettings->currentTrack = D_intro_004009EC[gGameSettings->finishedIntroCount];
            return;
        }
        if (temp_fv1 < 0.0f) {
            if (gGameSettings->dbgOptsRecordIntro == 0) {
                gGameSettings->finishedIntroCount++;
                if (gGameSettings->finishedIntroCount >= 6) {
                    gGameSettings->gameStateFlag = 0xE;
                } else {
                    gGameSettings->gameStateFlag = 2;
                    gGameSettings->currentTrack = D_intro_004009EC[gGameSettings->finishedIntroCount];
                }
                return;
            }
        }
        if ((temp_fv1 < D_intro_004009C8) && (gGameSettings->dbgOptsRecordIntro == 0)) {
            D_intro_004009D0 = 1;
            return;
        }
    }

    D_intro_00400A84->unk8(D_intro_00400A90.unk0);
    gUvAudiomgrExports->unk14(0.0f);
    if (gGameSettings->unkPtr90 != 0) {
        gUvDynExports->unkC(0, gGameSettings->unk80);
        gUvDynExports->unk124(gGameSettings->unkPtr90);
    }
    D_intro_00400A88->unk4C();
    D_intro_00400A84->unkC(D_intro_00400A90.unk0);
}


void func_intro_00400820(void) {
    if (D_intro_004009D0 != 0) {
        gUvSprtExports->unk10(D_intro_00400A98);
        gUvSprtExports->unk10(D_intro_00400A9C);
    }
}

void func_intro_0040087C(void) {
    s32 sp1C;
    s32 temp_v0;
    s16 a0;

    sp1C = gGameGuiExports->unk28 + 0x10;
    gGameGuiExports->unk14();
    a0 = gUvGuiExports->unk70(); // (sll a0,v0,0x10 and sra t0,a0,0x10) means there is a conversion from s16 to s32
    temp_v0 = gUvGuiExports->unk74(a0);
    gUvGuiExports->unk78(temp_v0, &D_intro_004009C0);
    gUvGuiExports->unk8(sp1C, temp_v0);
    gReplayExports->unk34(temp_v0);
    if (gAiExports != 0) {
        gAiExports->unk14(temp_v0);
    }
}

s32 func_intro_00400960(s32* arg0, void *arg1, void *arg2, s32* arg3) {
    if (arg3 == NULL) {
        return 1;
    }
    if (arg0[/*unk34*/13] == 0x116) {
        D_intro_00400A88->unk48(*arg3);
    }
    return 0;
}
