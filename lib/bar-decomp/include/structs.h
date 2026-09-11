// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef BAR_STRUCTS_H
#define BAR_STRUCTS_H
typedef struct UnkStruct_80025C00_s {
    s32 pad0;
    void (*unk4)(s32); // fontSet
    char pad[0x4];
    void (*unkC)(s32, s32, s32, s32); // fontColor
    char pad10[0x8];
    s32 (*unk18)(s32);
    char pad1C[0x8];
    void (*unk24)(s32, s32, s32);
    void (*unk28)(void);

} UnkStruct_80025C00;

typedef struct UnkStruct_80025C08_s {
    s32 pad0;
    void (*unk4)(void);
    char pad8[0x18];
    void (*unk20)(void);
    char padC[0x2C];
    void (*unk50)(s32, s32, s32); // bg solid color
    char pad54[0x4];
    void (*unk58)(s32, s32, s32, s32);
    char pad5C[0x18];
    void (*unk74)();
    char pad78[0xC];
    s32 (*unk84)(void);
    s32 (*unk88)(void);
} UnkStruct_80025C08;

// logo and intro
typedef struct UnkStruct_80025C14_s {
    s32 unk0;
    s32 (*unk4)(void);
    char pad8[8];
    void (*unk10)(s32);
    s32 (*unk14)(s32);
    char pad18[4];
    void (*unk1C)(s32, ...);
} UnkStruct_80025C14;

typedef struct UnkStruct_80025C74_s {
    s32 pad0;
    s32 unk4;
    u8 pad8[0x10 - 0x8];
    void (*unk10)(s32);
    void (*unk14)(void);
    s32 pad18;
    void (*unk1C)(s32);
    u8 pad20[0x28 - 0x20];
    s32 unk28;
} UnkStruct_80025C74;

typedef struct UnkStruct_80025CAC_s {
    char pad14[0x14];
    void (*unk14)(s32);
} UnkStruct_80025CAC;

typedef struct CarInfo_s {
    s16 currentCar;
    s16 currentColor;
    u8 transmissionType;
} CarInfo;

typedef struct UnkStruct_80025CF0_s {
    /* 0x0000 */ s32 debugState;
    // 80025CF0
    /* 0x0004 */ s32 numMaxTxts;
    /* 0x0008 */ s32 gameStateFlag;
    /* 0x000C */ s32 unkC;
    // 80025D00
    /* 0x0010 */ s32 unk10;
    /* 0x0014 */ s32 finishedIntroCount;
    /* 0x0018 */ s32 optionsMusicVol;
    /* 0x001C */ s32 optionsSfxVol; // 80025D10
    /* 0x0020 */ s32 optionsSpeechVol;
    /* 0x0024 */ s32 numPlayers;
    /* 0x0028 */ s32 numAiCars;
    /* 0x002C */ f32 dbgPlayerLead;
    /* 0x0030 */ f32 dbgMxSpeed; // 80025D20
    /* 0x0034 */ s32 dbgDefaultMaterialFlag;
    /* 0x0038 */ s32 dbgAiDbgState;
    /* 0x003C */ s32 dbgRaceAiRecord;
    /* 0x0040 */ s32 dbgSteerAssist; // 80025D30
    /* 0x0044 */ s32 currentTrack;
    /* 0x0048 */ s32 unk48;
    /* 0x004C */ s32 unk4C;
    /* 0x0050 */ s32 unk50; // 80025D40
    /* 0x0054 */ s32 dbgDispMasks;
    /* 0x0058 */ s32 dbgDispViGamma;
    /* 0x005C */ s32 dbgDispViGamDith;
    /* 0x0060 */ s32 dbgDispViDivot; // 80025D50
    /* 0x0064 */ s32 dbgDispViDither;
    /* 0x0068 */ s32 dbgDispAntialias;
    /* 0x006C */ s32 dbgDispTxtFilter;
    /* 0x0070 */ s32 dbgDispTxtPersp; // 80025D60
    /* 0x0074 */ s32 dbgDispTxtDetail;
    /* 0x0078 */ s32 dbgDispMipmap;
    /* 0x007C */ f32 unk7C;
    /* 0x0080 */ f32 unk80; // 80025D70
    /* 0x0084 */ s16 initFlag;
    /* 0x0086 */ s16 pauseFlag;
    /* 0x0088 */ s32 raceState;
    /* 0x008C */ s32 unk8C;
    /* 0x0090 */ s32 unkPtr90; // 80025D80
    /* 0x0094 */ s32 unk94;
    /* 0x0098 */ s32 unk98;
    /* 0x009C */ s16 introReplayState;
    /* 0x009E */ s16 unk9E;
    /* 0x00A0 */ s32 dbgOptsRecordIntro; // 80025D90
    /* 0x00A4 */ s32 currentGameState;
    /* 0x00A8 */ s32 dbgOptsGlare;
    /* 0x00AC */ s32 dbgOptsFrameRate;
    /* 0x00B0 */ s32 dbgAbortMode; // 80025DA0
    /* 0x00B4 */ s32 dbgInfLapsFlag;
    /* 0x00B8 */ s32 dbgHudState;
    /* 0x00BC */ s32 dbgFrateDisp;
    /* 0x00C0 */ s32 dbgSuspensionFlag; // 80025DB0
    /* 0x00C4 */ s32 dbgTileSort;
    /* 0x00C8 */ s32 dbgTrackWeather;
    /* 0x00CC */ s32 dbgOptsCarShadow;
    /* 0x00D0 */ s32 dbgDisableGoodFramera; // 80025DC0
    /* 0x00D4 */ s32 dbgOptsLosCheckOff;
    /* 0x00D8 */ s32 dbgOptsVelCam;
    /* 0x00DC */ s32 dbgOptsDrawingOff;
    /* 0x00E0 */ u8 padE0[0x138 - 0xE0];
    /* 0x0138 */ CarInfo unk138[4];
    /* 0x0150 */ u8 unk150[16];
    /* 0x0160 */ u8 unk160;
    /* 0x0161 */ u8 pad161[0x170 - 0x161];
    /* 0x0170 */ s32 unk170;
    /* 0x0174 */ s32 unk174;
    /* 0x0178 */ u8 pad178[0x180 - 0x178];
    /* 0x0180 */ s32 unk180;
    /* 0x0184 */ s32 unk184;
    /* 0x0188 */ u8 pad188[0x18C - 0x188];
    /* 0x018C */ s32 unk18C;
    /* 0x0190 */ u8 pad190[0x6E9C - 0x190];
    /* 0x6E9C */ s32 unk6E9C;
    /* 0x6EA0 */ u8 pad6EA0[0x6EAA - 0x6EA0];
    /* 0x6EAA */ u8 unk6EAA;
    /* 0x6EAC */ s16 unk6EAC;
    /* 0x6EAE */ s16 unk6EAE;
    /* 0x6EB0 */ u8 unk6EB0[16];
    /* 0x6EC0 */ s16 unk6EC0[4];
    /* 0x6EC8 */ u8 pad6EC8[0x6F74 - 0x6EC8];
    /* 0x6F74 */ s32 unk6F74;
    /* 0x6F78 */ s32 unk6F78[4];
    /* 0x6F88 */ u8 pad6F88[0x6FA0 - 0x6F88];
    /* 0x6FA0 */ f32 unk6FA0;
    /* 0x6FA4 */ u8 pad6FA4[0x6FB0 - 0x6FA4];
    /* 0x6FB0 */ s32 unk6FB0;
    /* 0x6FB4 */ s32 unk6FB4;
    /* 0x6FB8 */ s16 unk6FB8;
    /* 0x6FBC */ s32 unk6FBC;
    /* 0x6FC0 */ u8 pad6FC0[0x7050 - 0x6FC0];
    /* 0x704C */ s32 unk704C[2];
    /* 0x7054 */ s32 unk7058;
    /* 0x705C */ s32 unk705C;
    /* 0x7060 */ u8 unk7060;
    /* 0x7061 */ u8 pad7061[0x7064 - 0x7061];
    /* 0x7064 */ s32 unk7064[4];
    /* 0x7074 */ s32 unk7074[4];
    /* 0x7084 */ s32 unk7084[8];
} UnkStruct_80025CF0;

typedef struct UnkStruct_80025BE8_s {
    /* 0x00 */ char pad0[4];
    /* 0x04 */ s32 (*unk4)(void); /* inferred */
    /* 0x08 */ s32 (*unk8)(s32);  /* inferred */
    /* 0x0C */ char padC[0x60];   /* maybe part of unk8[0x19]? */
    /* 0x6C */ void (*unk6C)();
} UnkStruct_80025BE8; /* size = 0x70 */

typedef struct UnkStruct_8002D1A4_s {
    char pad[0x4];
    void (*unk4)(void);
    void (*unk8)(s32);
    void (*unkC)(s32);
    char pad10[0x58];
    void (*unk68)(void);
} UnkStruct_8002D1A4;

typedef struct UnkStruct_80025CB0_s {
    char pad[0x24];
    void (*unk24)(void);
} UnkStruct_80025CB0;

typedef struct UnkDobjExports_s {
    /* 0x00 */ char pad0[0x18];
    /* 0x18 */ s32 (*unk18)(void);              /* inferred */
    /* 0x1C */ char pad1C[8];                   /* maybe part of unk18[3]? */
    /* 0x24 */ void (*unk24)(s32, u8, Mtx4F *); /* inferred */
    /* 0x28 */ char pad28[0x18];                /* maybe part of unk24[7]? */
    /* 0x40 */ void (*unk40)(s32);
    /* 0x44 */ char pad[0x14];
    /* 0x58 */ s32 (*unk58)(s32, s32, s32);
} UnkDobjExports; /* size = 0x44 */

#endif /* BAR_STRUCTS_H */
