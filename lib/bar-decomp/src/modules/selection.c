// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
extern f32 D_selection_0041F4EC;
extern f32 D_selection_0041F4F0;
extern f32 D_selection_00420E20;
typedef struct {
    /* 0x00 */ char pad0[0xF8];
    /* 0xF8 */ f32 unkF8;
    /* 0xFC */ Mtx4F unkFC;
    /* 0x13C */ char pad13C[0x6C];
    /* 0x1A8 */ Mtx4F unk1A8;
} ObjNode_4168C4;
typedef struct {
    /* 0x00 */ char pad00[0x0C];
    /* 0x0C */ void (*unk0C)(Mtx4F *, Mtx4F *);
    /* 0x10 */ char pad10[0x18];
    /* 0x28 */ void (*unk28)(Mtx4F *, f32, s8);
    /* 0x2C */ void (*unk2C)(Mtx4F *, f32, f32, f32);
} UvFMtxExp_4168C4;
extern UvFMtxExp_4168C4 *gUvFmtxExports;
typedef struct {
    char pad[0x40];
    void (*unk40)(s32, s32, s32, s32, s32, s32, s32, s32, s32);
} SelPrinter_00411038;
typedef struct {
    void (*unk0)(void);
    void (*unk4)(s32);
    char pad8[0x14];
    s32 (*unk1C)(void);
    char pad20[0x8];
    void (*unk28)(void);
} UvFontExp_00411038;
extern UvFontExp_00411038 *gUvFontExports;
extern s32 D_selection_00421CE8;
extern s32 D_selection_00421E98;
extern s32 D_selection_00421E9C;
typedef struct {
    char pad0[0x3C];
    s32 (*unk3C)(u16, s32);
    char pad40[0x4];
    s32 (*unk44)(s32, s32, s32);
} UvContExp_413418;
extern s32 D_selection_00420E4C;
extern void func_selection_00411B9C(void);
typedef struct { char pad[0x74]; void (*unk74)(void); } UvGfxMgrExp_BEC8;
typedef struct { char pad[0x8]; void (*unk8)(void); } UvTexAnimExp_BEC8;
extern s16 D_selection_00420D24[];
extern s16 D_selection_00420D3C[];
extern s16 D_selection_00420D48[];
extern s32 D_selection_00420E04;
extern s16 D_selection_00420D6C;
extern s16 D_selection_00420D7E;
extern s16 D_selection_00421E3A;
extern s16 D_selection_00421E3C;
typedef struct { /* 0x00 */ char pad0[4]; /* 0x04 */ s16 unk4; /* 0x06 */ char pad6[0xE]; } InnerItem_0041B19C;
typedef struct { /* 0x00 */ char pad0[4]; /* 0x04 */ s16 unk4; /* 0x06 */ char pad6[2]; /* 0x08 */ InnerItem_0041B19C *unk8; } InnerObj_0041B19C;
typedef struct { /* 0x00 */ char pad0[8]; /* 0x08 */ InnerObj_0041B19C *unk8; } D1CTarget_0041B19C;
typedef struct { /* 0x00 */ char pad0[4]; /* 0x04 */ void (*unk4)(void *, s32, InnerObj_0041B19C *); /* 0x08 */ s32 (*unk8)(s32, InnerObj_0041B19C *); } ContExp_0041B19C;
typedef struct { char pad[0x74]; void (*unk74)(void); } UvGfxMgrExp_sel_0041A0CC;
typedef struct {
    char pad0[0x3C];
    void (*unk3C)(u16, s32);
    char pad40[0x4];
    s32  (*unk44)(s32, s32, s32);
} UvContExp_00419AA4;
extern void func_selection_00419084(void);
extern void func_selection_00419B9C(void);
extern s32 D_8002CC8C;
extern s16 D_selection_00421BA4;
extern s16 D_selection_00421E34;
extern s16 D_selection_00421E36;
typedef struct {
    /* 0x00 */ char pad00[0x0C];
    /* 0x0C */ void (*unkC)(s32);
    /* 0x10 */ void (*unk10)(s32);
    /* 0x14 */ char pad14[0x3C];
    /* 0x50 */ void (*unk50)(void);
    /* 0x54 */ void (*unk54)(void);
} GfxStateExp_00416D94;
typedef struct {
    /* 0x00 */ char pad00[0x10];
    /* 0x10 */ void (*unk10)(s32);
    /* 0x14 */ char pad14[0x08];
    /* 0x1C */ void (*unk1C)(s32, s32, s32, s32, s32, s32, s32, s32);
} SprtExp_00416D94;
typedef struct {
    char pad0[0x4];
    s16 (*unk4)(void);
    char pad8[0xC];
    s16 (*unk14)(s32);
    s16 (*unk18)(s32);
    void (*unk1C)(s32, ...);
} UvSprtFull_0040F0F8;
extern s32 D_selection_00421CE4;
extern void func_selection_00415EF4(void);
extern void func_selection_004017E0(void);
extern void func_selection_00415A34(s32);
extern void func_selection_00415C78(void);
extern s16 D_selection_00421E24;
typedef struct {
    char pad00[0x34];
    void (*unk34)(s32);
    void (*unk38)(s32);
    void (*unk3C)(s32);
    char pad3C[0xA0];
    void (*unkE0)(s32);
} SndExp_409FC8;
typedef struct {
    char pad00[0x2C];
    void (*unk2C)(void);
} UvCMidiExp_409FC8;
extern UvCMidiExp_409FC8 *gUvCmidiExports;
typedef struct UnkSelectionObj_004158DC_s {
    /* 0x000 */ s16 unk0;
    /* 0x002 */ u8 pad2[0x220 - 0x2];
    /* 0x220 */ s32 unk220;
} UnkSelectionObj_004158DC;
typedef struct CamExp_004158DC_s {
    /* 0x00 */ char pad0[0x20];
    /* 0x20 */ void (*unk20)(void *, s32, f64, s32);
} CamExp_004158DC;
typedef struct UvGfxMgrExp_004158DC_s {
    /* 0x00 */ char pad0[0x74];
    /* 0x74 */ void (*unk74)(void);
} UvGfxMgrExp_004158DC;
typedef struct UvTexAnimExp_004158DC_s {
    /* 0x00 */ char pad0[0x8];
    /* 0x08 */ void (*unk8)(void);
} UvTexAnimExp_004158DC;
typedef struct UvTerraExp_004158DC_s {
    /* 0x00 */ char pad0[0x8];
    /* 0x08 */ void (*unk8)(s16, s32);
} UvTerraExp_004158DC;
typedef struct UvEnvExp_004158DC_s {
    /* 0x00 */ char pad0[0x18];
    /* 0x18 */ void (*unk18)(s16, s32);
} UvEnvExp_004158DC;
extern f32 D_selection_0041F4C4;
extern CamExp_004158DC *gCamExports;
extern s32 D_selection_00421E84;
extern s32 D_selection_00421E88;
extern s32 D_selection_00421E8C;
extern s32 D_selection_00421E94;
typedef struct {
    char pad0[0xC];
    void (*unkC)(f32, f32, f32, f32);
    void (*unk10)(s32, s32, s32, s32);
} UvGeomExp_00410D20;
extern UvGeomExp_00410D20 *gUvGeomExports;
typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 unkC;
} Node_0040D8B4;
extern s32 D_8002CCB0;
extern s32 D_8002CCB4;
extern s32 D_8002CCB8;
extern Node_0040D8B4 D_8002CCBC;
typedef struct { char pad[0x74]; void (*unk74)(void); } UvGfxMgrExp_0040C96C;
typedef struct { char pad[0x8]; void (*unk8)(void); } UvTexAnimExp_0040C96C;
typedef struct { /* 0x000 */ s16 unk0; /* 0x002 */ char pad2[0x21E]; /* 0x220 */ s32 unk220; } Node_0040C96C;
typedef struct { char pad[0x8]; void (*unk8)(s16, s32); } UvTerraExp_0040C96C;
typedef struct { char pad[0x18]; void (*unk18)(s16, s32); } UvEnvExp_0040C96C;
extern s32 D_selection_00420E68;
typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s16 unk4;
    /* 0x06 */ u8  pad6[0x14 - 0x6];
} Node_0040D98C;
typedef struct {
    /* 0x00 */ s32          unk0;
    /* 0x04 */ s16          unk4;
    /* 0x06 */ u8           pad6[0x8 - 0x6];
    /* 0x08 */ Node_0040D98C *unk8;
} Inner_0040D98C;
typedef struct {
    /* 0x00 */ u8           pad0[0x8];
    /* 0x08 */ Inner_0040D98C *unk8;
} Outer_0040D98C;
void func_selection_00401FB0(void);
void func_selection_00404888(void);
void func_selection_004048D8(void);
extern s16 D_selection_00421E38;
typedef struct {
    /* 0x00 */ char pad[0x8];
    /* 0x08 */ s32 (*unk8)(s32, s32);
} Ctrl_0040CA50;
typedef struct {
    /* 0x00 */ char pad[0x8];
    /* 0x08 */ s32 unk8;
} Data_0040CA50;
extern void func_selection_00401964(s32 *, s32, s32);
extern void func_selection_004039B0(void);
extern s8 D_selection_00421CF0;
typedef struct {
    char pad0[0xC];
    void (*unk0C)(s32);
    void (*unk10)(s32);
    char pad14[0x3C];
    void (*unk50)(void);
    void (*unk54)(void);
} UvGfxStateExp_0040626C;
extern UvGfxStateExp_0040626C *gUvGfxStateExports;
typedef struct { char pad[0x10]; void (*unk10)(s16); } UvSprtLocal_0040626C;
extern s32 gOptionsSfxVol;
typedef struct {
    char pad0[0x34];
    void (*unk34)(s32 *, s32, s32, s32, void *);
} UnkStruct_00421B90_00403C2C;
typedef struct {
    char pad0[0x44];
    void (*unk44)(s32);
    char pad48[0x98];
    void (*unkE0)(s32);
    char padE4[0x10];
    void (*unkF4)(s32);
} UnkSndExports_00403C2C;
extern s32 gOptionsSpeechVol;
typedef struct UnkContExports_004030E4_s {
    /* 0x00 */ char pad0[0x8];
    /* 0x08 */ s32 (*unk8)(s32);
} UnkContExports_004030E4;
extern void func_selection_00415CA0(s32);
extern void func_selection_00419038(void);
extern s32 D_8002CC64;
extern s32 D_8002CD54;
extern s32 D_8002CD58;
extern s32 D_8002CD5C;
extern s32 D_8002CD60;
extern s32 D_8002CD64;
extern s32 D_8002CD68;
extern s32 D_8002CD6C;
extern s32 D_8002CD70;
extern s32 D_selection_0041FF9C;
typedef struct UnkSelectionObj_s {
    /* 0x000 */ s16 unk0;
    /* 0x002 */ u8 pad2[0x220 - 0x2];
    /* 0x220 */ s32 unk220;
} UnkSelectionObj;
typedef struct UvGfxMgrExp_0040372C_s {
    /* 0x00 */ char pad0[0x74];
    /* 0x74 */ void (*unk74)(void);
} UvGfxMgrExp_0040372C;
typedef struct UvTexAnim_Exports_s {
    /* 0x00 */ char pad0[0x8];
    /* 0x08 */ void (*unk8)(void);
} UvTexAnim_Exports;
typedef struct UvTerra_Exports_s {
    /* 0x00 */ char pad0[0x8];
    /* 0x08 */ void (*unk8)(s16, s32);
} UvTerra_Exports;
typedef struct UvEnv_Exports_s {
    /* 0x00 */ char pad0[0x18];
    /* 0x18 */ void (*unk18)(s16, s32);
} UvEnv_Exports;
extern void func_selection_00416A50(s32, s32);
extern UnkSelectionObj **D_selection_00421D18;
extern UvGfxMgrExp_0040372C *gUvGfxMgrExports;
extern UvTexAnim_Exports *gUvTexAnimExports;
extern UvTerra_Exports *gUvTerraExports;
extern UvEnv_Exports *gUvEnvExports;
extern s32 D_selection_00421E40;
typedef struct UnkContExports_s {
    /* 0x00 */ char pad0[0x5C];
    /* 0x5C */ void (*unk5C)(s32, s32);
} UnkContExports;
extern UnkContExports *gUvContExports;
typedef struct UvString_Exports4_s {
    char pad0[0x10];
    void (*unk10)(char *, char *, s32, s32);
} UvString_Exports4;
typedef struct D_selection_004217C8_entry_s {
    s32 unk0;
    s32 unk4;
    char pad8[0x2C - 0x8];
} D_selection_004217C8_entry;
extern UvString_Exports4 *gUvStringExports;
extern D_selection_004217C8_entry D_selection_004217C8[];
extern char D_selection_0041EEA0[];
void func_selection_00410E78(s32, s32, char *);
void func_selection_00410BD0(s32 arg0, s32 arg1, s32 arg2);
typedef struct {
    s32 text;
    s8  pad[0x28];
} SelectionLangEntry; /* sizeof == 0x2C */

extern SelectionLangEntry D_selection_004217F0[];
extern s32 gOptionsLanguage;
typedef struct UvSprt_Exports_s {
    char pad0[0x8];
    void (*unk8)(s16);
} UvSprt_Exports;
void func_selection_00404888(void);
typedef struct {
    char pad0[0xE0];
    void (*unkE0)(s32);
} SndExports_sel3D24;
typedef struct {
    char pad0[0x3C];
    void (*unk3C)(s32);
} SndExports_sel3D24b;
typedef struct {
    char pad0[0x34];
    void (*unk34)(s32 *, s32, s32, s32, s32);
} SelectionCtrl3D24;
extern SelectionCtrl3D24 *D_selection_00421B90;
extern s32 gOptionsMusicVol;
extern void func_selection_00415D48(s32);
extern s32 D_selection_0041FC38;
typedef struct SelLeaf_s {
    /* 0x00 */ s16 unk0;
    /* 0x02 */ s16 unk2;
    /* 0x04 */ char pad4[0x12];
    /* 0x16 */ s16 unk16;
} SelLeaf;
typedef struct SelNodeData_s {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ SelLeaf *unk8;
} SelNodeData;
typedef struct SelNode_s {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ struct SelNode_s *unk4;
    /* 0x08 */ SelNodeData *unk8;
} SelNode;
void func_selection_00415CA0(s32);
extern SelNode D_selection_0041FBB4;
extern SelNode D_selection_0041FCF0;
typedef struct {
    char pad0[0xE0];
    void (*unkE0)(s32);
} SndExports_sel2E98;
typedef struct SelectionState2E98_s {
    char pad0[0x14];
    s16 unk14;
} SelectionState2E98;
extern SelectionState2E98 D_selection_0041F808;
extern void (*D_selection_00420F18[])(void);
extern SelectionState2E98 *D_selection_00421D20;
extern s32 D_selection_00421240[];
extern s16 D_selection_00421E08;
extern s32 D_selection_00420E24;
extern s32 D_selection_004201BC;
extern s32 D_selection_00421768;
extern s32 D_selection_0042176C[];
extern s8 D_selection_00421CF3;
extern s8 D_selection_00421CFB;
extern s8 D_selection_00421D03;
extern s8 D_selection_00421D0B;
extern s8 D_selection_00421DF4;
extern s8 D_selection_00421DF5;
extern s8 D_selection_00421DF6;
extern s8 D_selection_00421DF7;
typedef struct { char pad0[0xF4]; void (*unkF4)(u8); } UnkSndExports_sel;
typedef struct { char pad0[8]; void (*unk8)(s16); } UnkUvSprtExports_sel;
typedef struct { char pad0[0x7C]; s16 (*unk7C)(void); } UnkUvEmitterExports_sel;
extern UnkSndExports_sel *gSndExports;
extern UnkUvSprtExports_sel *gUvSprtExports;
extern UnkUvEmitterExports_sel *gUvEmitterExports;
extern u8 gOptionsStereoMono;
extern u8 gOptionsSpeed;
extern u8 gOptionsMap;
extern u8 gOptionsDisplay;
extern u8 D_80025E6A;
extern s32 gNumPlayers;
extern s32 gNumAiCars;
extern u8 gTransmissionType;
extern s32 D_80025E70;
extern s16 D_selection_00420DF0;
extern s16 D_selection_00420DFC;
extern s16 D_selection_00420DF8;
extern s32 D_selection_00421CE0;
extern void *D_selection_00421D1C;
extern s32 D_selection_00421D28;
extern s32 D_selection_00420E18;
extern s32 D_selection_00420E1C;
extern s32 D_selection_0041FC80;
extern s32 D_selection_0041FF2C;
extern s32 D_selection_0041FD74;
extern s32 D_selection_0041F770;
extern s32 D_selection_00420104;
extern s32 D_selection_0041FED0;
extern s32 D_selection_0041F94C;
extern s32 D_selection_0041FB58;
extern s32 D_selection_0041F994;
extern s32 D_selection_0041F904;
extern s32 D_selection_0041FE04;
extern s32 D_selection_0041FFE4;
extern s32 D_selection_00420218;
extern s32 D_selection_00420074;
extern s32 D_selection_0041F728;
extern s32 D_selection_00420260;
extern s32 D_selection_00420E90;
extern s32 D_selection_00420E84;
extern s32 D_selection_00420E74;
extern s32 D_selection_00420E78;
extern s32 D_selection_00420E70;
extern s32 D_selection_0041F5B4;
extern s32 D_selection_00421BA0;
extern s8 D_selection_00420DE8;
extern s16 D_selection_00420DEC;
extern s16 D_8002CC84;
extern s32 gCheatBattleNumLadybugs;
extern s32 gCheatBattleLadybugColor;
extern s32 gCheatBattleHealth;
extern s32 gCheatBattlePowerups;
extern s32 gCheatBattleTimeLimit;
extern s32 gCheatBattleRadar;
extern s32 gCheatBattleDamage;
extern s32 gCheatBattleMysteryBoxMode;
extern s32 gCheatTimeAttackBonusBoxes;
extern s32 gCheatBreakables;
extern s32 gCheatEnvironment;
extern s32 gCheatCars;
extern s32 gCheatPlayer2Handicap;
extern s32 gCheatColorChange;
extern s32 gCheatFieldOfView;
extern s32 gCheatHornSfxId;
extern s32 gCheatTrackMusic;
extern s32 gCheatHandbreakPower;
void func_selection_004000E4();
void func_selection_004003DC();
void func_selection_00400580();
void func_selection_004008B0();
void func_selection_00401570();
void func_selection_004015D4();
void func_selection_004016CC();
void func_selection_004017E0();
void func_selection_0040189C();
void func_selection_00401964();
void func_selection_00401A38();
void func_selection_00401B1C();
void func_selection_00401BF4();
void func_selection_00401D04();
void func_selection_00401FB0();
void func_selection_00402254();
void func_selection_00402588();
void func_selection_00402994();
void func_selection_00403050();
void func_selection_004031E8();
void func_selection_00403220();
void func_selection_00403254();
void func_selection_00403284();
void func_selection_004032BC();
void func_selection_00403338();
void func_selection_004037F4();
void func_selection_0040382C();
void func_selection_00403868();
void func_selection_004038A4();
void func_selection_004038E0();
void func_selection_0040390C();
void func_selection_00403938();
void func_selection_004039B0();
void func_selection_00403A38();
void func_selection_00403A64();
void func_selection_00403A9C();
void func_selection_00403AC8();
void func_selection_00403B40();
void func_selection_00403B6C();
void func_selection_00403BAC();
void func_selection_00403BEC();
void func_selection_00403DA0();
void func_selection_00403DE0();
void func_selection_00403E04();
void func_selection_00403E28();
void func_selection_00403E54();
void func_selection_00403E78();
void func_selection_0040437C();
void func_selection_004045F4();
void func_selection_00404620();
void func_selection_0040464C();
void func_selection_00404678();
void func_selection_004046A4();
void func_selection_004046D0();
void func_selection_004046FC();
void func_selection_00404728();
void func_selection_00404754();
void func_selection_00404780();
void func_selection_004047AC();
void func_selection_004047D8();
void func_selection_00404804();
void func_selection_00404830();
void func_selection_0040485C();
void func_selection_00404888();
void func_selection_004048D8();
void func_selection_00404A8C();
void func_selection_00404ACC();
void func_selection_00404AF8();
void func_selection_00404B2C();
void func_selection_00404C24();
void func_selection_00405140();
void func_selection_00405644();
void func_selection_00405CB0();
void func_selection_00405E38();
void func_selection_00405F48();
void func_selection_004060D4();
void func_selection_0040611C();
void func_selection_004062F8();
void func_selection_00406924();
void func_selection_00406DC4();
void func_selection_00406F80();
void func_selection_0040705C();
void func_selection_004078D4();
void func_selection_00407AF8();
void func_selection_004080D0();
void func_selection_004081B4();
void func_selection_00408304();
void func_selection_004083DC();
void func_selection_0040884C();
void func_selection_00408BEC();
void func_selection_004092F0();
void func_selection_004099A0();
void func_selection_00409C94();
void func_selection_00409EA4();
void func_selection_0040A104();
void func_selection_0040A5E0();
void func_selection_0040A778();
void func_selection_0040A820();
void func_selection_0040B070();
void func_selection_0040B144();
void func_selection_0040B458();
void func_selection_0040B534();
void func_selection_0040B608();
void func_selection_0040B724();
void func_selection_0040B74C();
void func_selection_0040BB14();
void func_selection_0040BFD0();
void func_selection_0040BFFC();
void func_selection_0040C454();
void func_selection_0040CAB8();
void func_selection_0040CAE8();
void func_selection_0040CB18();
void func_selection_0040CB50();
void func_selection_0040CB74();
void func_selection_0040CB9C();
void func_selection_0040CBC4();
void func_selection_0040CBEC();
void func_selection_0040D234();
void func_selection_0040D25C();
void func_selection_0040D280();
void func_selection_0040D514();
void func_selection_0040D844();
void func_selection_0040D9F8();
void func_selection_0040F294();
void func_selection_0040F4BC();
void func_selection_0040F64C();
void func_selection_0040F6E0();
void func_selection_0040F704();
void func_selection_0040F9EC();
void func_selection_0040FDB4();
void func_selection_004103CC();
void func_selection_00410614();
void func_selection_0041088C();
void func_selection_00410AA8();
void func_selection_00410BD0();
void func_selection_00410E78();
void func_selection_0041129C();
void func_selection_004114A0();
void func_selection_004116E0();
void func_selection_00411870();
void func_selection_00411B9C();
void func_selection_00411F40();
void func_selection_004133E0();
void func_selection_004134F4();
void func_selection_00413568();
void func_selection_00413594();
void func_selection_004136CC();
void func_selection_0041378C();
void func_selection_004137D0();
void func_selection_00413A88();
void func_selection_00413DE0();
void func_selection_00413E1C();
void func_selection_00413E48();
void func_selection_00413FC4();
void func_selection_004146C8();
void func_selection_00414CF4();
void func_selection_00415040();
void func_selection_0041532C();
void func_selection_004155A0();
void func_selection_004159FC();
void func_selection_00415AA0();
void func_selection_00415AD0();
void func_selection_00415B90();
void func_selection_00415BB8();
void func_selection_00415C78();
void func_selection_00415CA0();
void func_selection_00415D48();
void func_selection_00415D9C();
void func_selection_00415DC4();
void func_selection_00415DEC();
void func_selection_00415E80();
void func_selection_00415EF4();
void func_selection_00415FB4();
void func_selection_00416028();
void func_selection_0041639C();
void func_selection_00416794();
void func_selection_00416A50();
void func_selection_00416ABC();
void func_selection_00416E78();
void func_selection_00416F28();
void func_selection_00416F54();
void func_selection_00416FF4();
void func_selection_00417240();
void func_selection_004172B4();
void func_selection_004177A8();
void func_selection_004181D0();
void func_selection_0041844C();
void func_selection_00418800();
void func_selection_00418E98();
void func_selection_00419038();
void func_selection_00419084();
void func_selection_00419194();
void func_selection_00419384();
void func_selection_00419B78();
void func_selection_00419B9C();
void func_selection_00419BC8();
void func_selection_00419D54();
void func_selection_00419FB8();
void func_selection_0041A210();
void func_selection_0041A244();
void func_selection_0041A5BC();
void func_selection_0041A7D0();
void func_selection_0041A960();
f32 func_selection_0041ABC8(s32 arg0, s32 arg1, s32 arg2);
void func_selection_0041ABFC();
void func_selection_0041B11C();
void func_selection_0041B15C();
void func_selection_0041B28C();
void func_selection_0041B370();
extern s32 D_selection_00420E28;
extern s32 D_selection_0041F794;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/__entrypoint_func_selection_400000.s")

void func_selection_004000DC(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004000E4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004003DC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00400580.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004008B0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00401570.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004015D4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004016CC.s")

void func_selection_004017C8(void) {
}

void func_selection_004017D0(void) {
}

void func_selection_004017D8(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004017E0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040189C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00401964.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00401A38.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00401B1C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00401BF4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00401D04.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00401FB0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00402254.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00402588.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00402994.s")

void func_selection_00402D7C(void) {
}

/* Advance linked-list state: call exit fn for old node, link to next, flush gfx+texanim, call entry fn for new node. */
void func_selection_00402D84(void) {
    if (*(void **)((u8 *)D_selection_00421D1C + 0x4) != NULL) {
        D_selection_00420F18[*(s16 *)((u8 *)D_selection_00421D1C + 0x16)]();
        D_selection_00421D20 = D_selection_00421D1C;
        D_selection_00421D1C = *(void **)((u8 *)D_selection_00421D1C + 0x4);
        gUvGfxMgrExports->unk74();
        gUvTexAnimExports->unk8();
        D_selection_00420F18[*(s16 *)((u8 *)D_selection_00421D1C + 0x14)]();
    }
}

// Dispatch: flush gfx, tick tex-anim, then call a per-state update fn from a jump table.
void func_selection_00402E34(void) {
    gUvGfxMgrExports->unk74();
    gUvTexAnimExports->unk8();
    D_selection_00420F18[*(s16 *)((u8 *)D_selection_00421D1C + 0x16)]();
}

// Switch the active selection-state block, play cancel sounds when returning to root, then dispatch.
void func_selection_00402E98(SelectionState2E98 *arg0) {
    SelectionState2E98 *var_v0;

    if (D_selection_00421D1C != NULL) {
        D_selection_00421D20 = D_selection_00421D1C;
    }
    D_selection_00421D1C = arg0;
    var_v0 = arg0;
    if (arg0 == &D_selection_0041F808) {
        ((SndExports_sel2E98 *)gSndExports)->unkE0(0xE);
        ((SndExports_sel2E98 *)gSndExports)->unkE0(0xDB);
        var_v0 = D_selection_00421D1C;
    }
    D_selection_00420F18[var_v0->unk14]();
}

void func_selection_00402F30(void) {
    if (gGameSettings[0].numPlayers == 2 && gGameSettings[0].pad178[3] == 1) {
        gGameSettings[0].pad178[3] = 2;
    }
    gGameSettings[0].gameStateFlag = 5;
    func_selection_00402E34();
}

void func_selection_00402F80(void) {
    gGameSettings[0].gameStateFlag = 6;
    func_selection_00402E34();
    if (gGameSettings[0].numPlayers < 2) {
        gGameSettings[0].numPlayers = 2;
    }
    if (gGameSettings[0].currentTrack < 0x11) {
        gGameSettings[0].currentTrack = 0x11;
    }
}

/* Initialize single-player selection state and wire up the active context node. */
void func_selection_00402FE4(void) {
    func_selection_00415CA0(0);
    func_selection_00402E34();
    D_selection_00421CE0 = 1;
    gGameSettings[0].numPlayers = 1;
    D_selection_0041FCF0.unk8->unk8->unk16 = 1;
    D_selection_0041FCF0.unk4 = &D_selection_0041FBB4;
    D_selection_0041FBB4.unk8->unk8->unk2 = 1;
    func_selection_00402E98(&D_selection_0041FBB4);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00403050.s")

/* Check controller presence, then either abort or initialize the selection screen. */
void func_selection_004030E4(void) {
    if (((UnkContExports_004030E4 *)gUvContExports)->unk8(1) == 0) {
        func_selection_00419038();
        return;
    }
    func_selection_00415CA0(2);
    D_8002CD54 = 0;
    D_8002CD64 = 0;
    D_8002CD58 = 0;
    D_8002CD68 = 0;
    D_8002CD5C = 0;
    D_8002CD6C = 0;
    D_8002CD60 = 0;
    D_8002CD70 = 0;
    func_selection_00402E34();
    D_selection_00421CE0 = 1;
    D_8002CC64 = 4;
    func_selection_00402E98(&D_selection_0041FF9C);
}

void func_selection_0040319C(void) {
    func_selection_00402E34();
    if (gGameSettings[0].numPlayers < 2) {
        gGameSettings[0].numPlayers = 2;
    }
    func_selection_00402E98(&D_selection_0041FFE4);
}

void func_selection_004031E8(void) {
    func_selection_00415CA0(3);
    D_selection_00421CE0 = 0;
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041F904);
}

void func_selection_00403220(void) {
    D_selection_00421CE0 = 1;
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041FED0);
}

void func_selection_00403254(void) {
    D_selection_00421CE0 = 0;
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041F94C);
}

void func_selection_00403284(void) {
    func_selection_00415EF4();
    D_selection_00421CE0 = 0;
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041F994);
}

void func_selection_004032BC(void) {
    D_selection_00421CE0 = 0;
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041FB58);
}

/* Set up num AI cars + unk6F74, then transition to D_selection_0041FC38 screen. */
void func_selection_004032EC(void) {
    func_selection_00402E34();
    gGameSettings[0].numAiCars = 7;
    gGameSettings[0].unk6F74 = 2;
    func_selection_00415D48(1);
    func_selection_00402E98(&D_selection_0041FC38);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00403338.s")

/* Tear down the selection screen: clear gfx/texanim, unload files, reset track object. */
void func_selection_0040372C(void) {
    func_selection_00416A50(0, 0);
    gUvGfxMgrExports->unk74();
    gUvTexAnimExports->unk8();
    uvUnloadFile(0x55565452, 9);
    uvUnloadFile(0x5556454E, 0xB);
    (*D_selection_00421D18)->unk220 = -1;
    gUvTerraExports->unk8((*D_selection_00421D18)->unk0, -1);
    gUvEnvExports->unk18((*D_selection_00421D18)->unk0, -1);
}

void func_selection_004037F4(void) {
    func_selection_00415DEC(0);
    gGameSettings->unk180 = 0;
    gGameSettings->unk184 = 3;
    func_selection_004038E0();
}

void func_selection_0040382C(void) {
    func_selection_00415DEC(1);
    gGameSettings->unk180 = 1;
    gGameSettings->unk184 = 4;
    func_selection_004038E0();
}

void func_selection_00403868(void) {
    func_selection_00415DEC(2);
    gGameSettings->unk180 = 2;
    gGameSettings->unk184 = 5;
    func_selection_004038E0();
}

void func_selection_004038A4(void) {
    func_selection_00415DEC(3);
    gGameSettings->unk180 = 3;
    gGameSettings->unk184 = 6;
    func_selection_004038E0();
}

void func_selection_004038E0(void) {
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041FC80);
}

void func_selection_0040390C(void) {
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041FF2C);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00403938.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004039B0.s")

void func_selection_00403A38(void) {
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041FD74);
}

void func_selection_00403A64(void) {
    func_selection_00415CA0(4);
    D_selection_00421CE0 = 0;
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041FE04);
}

void func_selection_00403A9C(void) {
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041F770);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00403AC8.s")

void func_selection_00403B40(void) {
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_00420104);
}

void func_selection_00403B6C(void) {
    gNumPlayers = 2;
    func_selection_00415FB4(0);
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041FFE4);
}

void func_selection_00403BAC(void) {
    gNumPlayers = 3;
    func_selection_00415FB4(1);
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041FFE4);
}

void func_selection_00403BEC(void) {
    gNumPlayers = 4;
    func_selection_00415FB4(2);
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041FFE4);
}

/* Play confirm sfx, register speech volume slider callback, then apply volume. */
void func_selection_00403C2C(void) {
    ((UnkSndExports_00403C2C *)gSndExports)->unkE0(0xAB);
    D_selection_00421B90->unk34(&gOptionsSpeechVol, 8, 1, 0, D_selection_00421D1C);
    ((UnkSndExports_00403C2C *)gSndExports)->unk44(gOptionsSpeechVol);
}

/* Apply SFX volume: play a UI confirm sound, configure the audio channel volume, then set engine SFX volume. */
void func_selection_00403CA8(void) {
    (*(void (**)(s32))((u8 *)gSndExports + 0xE0))(0xAB);
    (*(void (**)(s32 *, s32, s32, s32, void *))((u8 *)D_selection_00421B90 + 0x34))(&gOptionsSfxVol, 8, 1, 1, D_selection_00421D1C);
    (*(void (**)(s32))((u8 *)gSndExports + 0x40))(gOptionsSfxVol);
}

// Play S_CONFIRM sfx, register a spinner widget for music volume, then apply the current volume.
void func_selection_00403D24(void) {
    ((SndExports_sel3D24 *)gSndExports)->unkE0(0xAB);
    D_selection_00421B90->unk34(&gOptionsMusicVol, 8, 1, 2, D_selection_00421D1C);
    ((SndExports_sel3D24b *)gSndExports)->unk3C(gOptionsMusicVol);
}

void func_selection_00403DA0(void) {
    func_selection_0040189C(&gOptionsStereoMono);
    gSndExports->unkF4(gOptionsStereoMono);
}

void func_selection_00403DE0(void) {
    func_selection_0040189C(&gOptionsSpeed);
}

void func_selection_00403E04(void) {
    func_selection_0040189C(&D_80025E6A);
}

void func_selection_00403E28(void) {
    func_selection_00401B1C(&gOptionsMap, 0, 2);
}

void func_selection_00403E54(void) {
    func_selection_0040189C(&gOptionsDisplay);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00403E78.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040437C.s")

void func_selection_004045F4(void) {
    func_selection_00401964(&gCheatBattleNumLadybugs, 0, 7);
}

void func_selection_00404620(void) {
    func_selection_00401964(&gCheatBattleLadybugColor, 0, 3);
}

void func_selection_0040464C(void) {
    func_selection_00401964(&gCheatBattleHealth, 0, 3);
}

void func_selection_00404678(void) {
    func_selection_00401964(&gCheatBattlePowerups, 0, 8);
}

void func_selection_004046A4(void) {
    func_selection_00401964(&gCheatBattleTimeLimit, 0, 6);
}

void func_selection_004046D0(void) {
    func_selection_00401964(&gCheatBattleRadar, 0, 1);
}

void func_selection_004046FC(void) {
    func_selection_00401964(&gCheatBattleDamage, 0, 3);
}

void func_selection_00404728(void) {
    func_selection_00401964(&gCheatBattleMysteryBoxMode, 0, 3);
}

void func_selection_00404754(void) {
    func_selection_00401B1C(&gCheatTimeAttackBonusBoxes, 0, 1);
}

void func_selection_00404780(void) {
    func_selection_00401B1C(&gCheatBreakables, 0, 2);
}

void func_selection_004047AC(void) {
    func_selection_00401B1C(&gCheatEnvironment, 0, 2);
}

void func_selection_004047D8(void) {
    func_selection_00401B1C(&gCheatCars, 0, 6);
}

void func_selection_00404804(void) {
    func_selection_00401B1C(&gCheatPlayer2Handicap, 0, 4);
}

void func_selection_00404830(void) {
    func_selection_00401B1C(&gCheatColorChange, 0, 1);
}

void func_selection_0040485C(void) {
    func_selection_00401B1C(&gCheatFieldOfView, 0, 2);
}

typedef struct
{
  char pad[0x8];
  void (*unk8)(u8);
} UvEmitterExp_00404888;
extern s32 D_selection_00420D94;
extern s32 D_selection_00420D98;

void func_selection_00404888(void)
{
  s32 slot = D_selection_00420D94;
  if (slot != (-1))
  {
    ((UvEmitterExp_00404888 *) gUvEmitterExports)->unk8((u8) slot);
  }
 do { D_selection_00420D94 = -1; } while (0);
  D_selection_00420D98 = 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004048D8.s")

void func_selection_00404A8C(void) {
    func_selection_00401B1C(&gCheatHornSfxId, 0, gUvEmitterExports->unk7C());
}

void func_selection_00404ACC(void) {
    func_selection_00401B1C(&gCheatTrackMusic, 0, 1);
}

void func_selection_00404AF8(void) {
    func_selection_00401B1C(&gCheatHandbreakPower, 0, 2);
}

void func_selection_00404B24(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00404B2C.s")

void func_selection_00404C1C(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00404C24.s")

/* Tear down the selection screen (variant B): reset gfx/texanim, clear object flag, unload terra/env, then unload files 0xA and 0xB. */
void func_selection_00405078(void) {
    func_selection_00416A50(0, 0);
    gUvGfxMgrExports->unk74();
    gUvTexAnimExports->unk8();
    (*D_selection_00421D18)->unk220 = -1;
    gUvTerraExports->unk8((*D_selection_00421D18)->unk0, -1);
    gUvEnvExports->unk18((*D_selection_00421D18)->unk0, -1);
    uvUnloadFile(0x55565452, 0xA);
    uvUnloadFile(0x5556454E, 0xB);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00405140.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00405644.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00405CB0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00405E38.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00405F48.s")

void func_selection_004060D4(void) {
    func_selection_0040189C((u8*)gGameSettings + (s32)D_selection_00420DF0 * 6 + 0x13C);
}

void func_selection_00406114(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040611C.s")

void func_selection_0040626C(void) {
    gUvGfxStateExports->unk50();
    gUvGfxStateExports->unk0C(0x4800000);
    gUvGfxStateExports->unk10(0x600000);
    ((UvSprtLocal_0040626C *)gUvSprtExports)->unk10(D_selection_00420DF8);
    gUvGfxStateExports->unk54();
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004062F8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00406924.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00406DC4.s")

void func_selection_00406F78(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00406F80.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040705C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004078D4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00407AF8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004080D0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004081B4.s")

void func_selection_004082FC(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00408304.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004083DC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040884C.s")

void func_selection_00408B24(void)
{
  s32 var_s2;
  s16 *var_s0;
  s16 new_var;
  gUvGfxMgrExports->unk74();
  gUvTexAnimExports->unk8();
  var_s2 = 0x55564254;
  uvUnloadFile(var_s2, 0x26);
  gUvSprtExports->unk8(new_var = D_selection_00421E3A);
  gUvSprtExports->unk8(D_selection_00421E3C);
 var_s0 = &D_selection_00420D6C; do { uvUnloadFile(var_s2, *var_s0);
    var_s0 += 1;
  }
  while (var_s0 != (&D_selection_00420D7E));
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00408BEC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004092F0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004099A0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00409C94.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00409EA4.s")

void func_selection_00409FC8(void) {
    s16 temp_v0;

    func_selection_00415EF4();
    ((SndExp_409FC8 *)gSndExports)->unkE0(0xAB);
    ((SndExp_409FC8 *)gSndExports)->unkE0(0xE);
    ((SndExp_409FC8 *)gSndExports)->unkE0(0xC);
    ((SndExp_409FC8 *)gSndExports)->unkE0(0xDB);
    D_selection_00421E24 += 1;
    func_selection_004017E0();
    temp_v0 = D_selection_00421E24;
    if (temp_v0 == 4 || temp_v0 == 3) {
        func_selection_00415A34(1);
        temp_v0 = D_selection_00421E24;
    }
    if (temp_v0 < 5) {
        return;
    }
    func_selection_00402E34();
    func_selection_00415C78();
    ((SndExp_409FC8 *)gSndExports)->unk3C(gOptionsMusicVol);
    gUvCmidiExports->unk2C();
    ((SndExp_409FC8 *)gSndExports)->unk34(0);
    ((SndExp_409FC8 *)gSndExports)->unk38(0);
    func_selection_00402E98(&D_selection_0041F808);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040A104.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040A5E0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040A778.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040A820.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040B070.s")

void func_selection_0040B13C(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040B144.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040B458.s")

void func_selection_0040B52C(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040B534.s")

void func_selection_0040B600(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040B608.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040B724.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040B74C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040BB14.s")

void func_selection_0040BEC8(void) {
    gUvGfxMgrExports->unk74();
    ((UvTexAnimExp_BEC8 *)gUvTexAnimExports)->unk8();
    uvUnloadFile(0x55564254, D_selection_00420D3C[D_selection_00420E04]);
    uvUnloadFile(0x55564254, D_selection_00420D48[D_selection_00420E04]);
    uvUnloadFile(0x55564254, D_selection_00420D24[D_selection_00420E04]);
    gUvSprtExports->unk8(D_selection_00421E36);
    gUvSprtExports->unk8(D_selection_00421E34);
    gUvSprtExports->unk8(D_selection_00420DF8);
    D_selection_00420DF8 = -1;
}

void func_selection_0040BFD0(void) {
    func_selection_00401964(&D_80025E70, 0, 2);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040BFFC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040C454.s")

void func_selection_0040C96C(void) {
    s32 var_s0;

    var_s0 = 0;
    do {
        func_selection_00416A50(var_s0, 0);
        var_s0 += 1;
    } while (var_s0 != 8);
    gUvGfxMgrExports->unk74();
    gUvTexAnimExports->unk8();
    uvUnloadFile(0x55565452, 0xB);
    uvUnloadFile(0x5556454E, 0);
    (*D_selection_00421D18)->unk220 = -1;
    gUvTerraExports->unk8((*D_selection_00421D18)->unk0, -1);
    gUvEnvExports->unk18((*D_selection_00421D18)->unk0, -1);
}

/* Select AI car count via a spinner widget, then trigger next-state if value==8 */
void func_selection_0040CA50(void) {
    func_selection_00401964(&gNumAiCars, 0, 7);
    if (((Ctrl_0040CA50 *)D_selection_00421B90)->unk8(0, ((Data_0040CA50 *)D_selection_00421D1C)->unk8) == 8) {
        D_selection_00421CF0 = 1;
        func_selection_004039B0();
    }
}

void func_selection_0040CAB8(void) {
    func_selection_00415E80(0);
    gNumAiCars = 7;
    func_selection_00403A38();
}

void func_selection_0040CAE8(void) {
    func_selection_00415E80(1);
    gNumAiCars = 1;
    func_selection_00403A38();
}

void func_selection_0040CB18(void) {
    func_selection_00415E80(2);
    gGameSettings->numAiCars = 0;
    gGameSettings->unk6F74 = 5;
    func_selection_004039B0();
}

void func_selection_0040CB50(void) {
    D_80025E70 = 0;
    func_selection_004039B0();
}

void func_selection_0040CB74(void) {
    D_80025E70 = 1;
    func_selection_004039B0();
}

void func_selection_0040CB9C(void) {
    D_80025E70 = 2;
    func_selection_004039B0();
}

void func_selection_0040CBC4(void) {
    D_80025E70 = 3;
    func_selection_004039B0();
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040CBEC.s")

/* Teardown: destroy player instances, flush gfx/texanim, unload files, reset terra/env, cleanup. */
void func_selection_0040D134(void) {
    s32 var_s0;

    var_s0 = 0;
    if (gGameSettings->numPlayers > 0) {
        do {
            func_selection_00416A50(var_s0, 0);
            var_s0 += 1;
        } while (var_s0 < gGameSettings->numPlayers);
    }
    gUvGfxMgrExports->unk74();
    gUvTexAnimExports->unk8();
    uvUnloadFile(0x55565452, 0x3);
    uvUnloadFile(0x5556454E, 0x12);
    (*D_selection_00421D18)->unk220 = -1;
    gUvTerraExports->unk8((*D_selection_00421D18)->unk0, -1);
    gUvEnvExports->unk18((*D_selection_00421D18)->unk0, -1);
    func_selection_004017E0();
}

void func_selection_0040D234(void) {
    ((u8*)&gTransmissionType)[D_selection_00420DF0 * 6] = 1;
}

void func_selection_0040D25C(void) {
    ((u8*)&gTransmissionType)[D_selection_00420DF0 * 6] = 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040D280.s")

/* Unload selection screen resources: flush gfx/texanim state, unload VB/VTX files, free sprite. */
void func_selection_0040D488(void) {
    gUvGfxMgrExports->unk74();
    gUvTexAnimExports->unk8();
    uvUnloadFile(0x55564254, 0x1D);
    uvUnloadFile(0x55565458, 0x5B6);
    uvUnloadFile(0x55565458, 0x5B7);
    gUvSprtExports->unk8(D_selection_00421E38);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040D514.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040D844.s")

s32 func_selection_0040D8B4(s32 arg0) {
    Node_0040D8B4 *var_v0;
    s32 var_v1;

    if (D_8002CCB0 & arg0) {
        return 0;
    }
    if (D_8002CCB4 & arg0) {
        return 1;
    }
    var_v0 = &D_8002CCBC;
    var_v1 = 3;
    if (D_8002CCB8 & arg0) {
        return 2;
    }
loop_7:
    if (var_v0->unk0 & arg0) {
        return var_v1;
    }
    if (var_v0->unk4 & arg0) {
        return var_v1 + 1;
    }
    if (var_v0->unk8 & arg0) {
        return var_v1 + 2;
    }
    if (var_v0->unkC & arg0) {
        return var_v1 + 3;
    }
    var_v1 += 4;
    var_v0 += 1;
    if (var_v1 == 0xB) {
        return -1;
    }
    goto loop_7;
}

/* Dispatch to enter or exit selection based on current menu item type (0xB7 = confirm). */
void func_selection_0040D98C(void) {
    Inner_0040D98C *temp_v0;

    func_selection_00401FB0();
    temp_v0 = ((Outer_0040D98C *)D_selection_00421D1C)->unk8;
    if (temp_v0->unk8[temp_v0->unk4].unk4 == 0xB7) {
        func_selection_004048D8();
        return;
    }
    func_selection_00404888();
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040D9F8.s")

/* Initializes the selection screen: loads the VUB sprite file, creates a sprite, sets its blit mode and centered position. */
void func_selection_0040F0F8(void) {
    s32 sp2C;

    func_selection_00415EF4();
    (*(void (**)(s32))((u8 *)gSndExports + 0xE0))(0xAB);
    D_selection_00420DFC = 0;
    *(s16 *)((u8 *)*(void **)((u8 *)D_selection_00421D1C + 8) + 4) = 0;
    D_selection_00421CE4 = 0xC;
    uvLoadFile(0x55564254, 0x63);
    D_selection_00420DF8 = ((UvSprtFull_0040F0F8 *)gUvSprtExports)->unk4();
    ((UvSprtFull_0040F0F8 *)gUvSprtExports)->unk1C(D_selection_00420DF8, 9, 0x63, 0);
    sp2C = ((UvSprtFull_0040F0F8 *)gUvSprtExports)->unk14(D_selection_00420DF8) / 2;
    ((UvSprtFull_0040F0F8 *)gUvSprtExports)->unk1C(D_selection_00420DF8, 2, 0xA0 - sp2C, 0x78 - (((UvSprtFull_0040F0F8 *)gUvSprtExports)->unk18(D_selection_00420DF8) / 2), 3, 1, 0);
}

// Unloads the UVBT sprite file and resets the sprite manager, then tears down selection state.
void func_selection_0040F248(void) {
    uvUnloadFile(0x55564254, 0x63);
    gUvSprtExports->unk8(D_selection_00420DF8);
    func_selection_00404888();
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040F294.s")

s32 func_selection_0040F430(s32 arg0) {
    s32 *var_v0;
    s32 var_v1;

    var_v0 = &D_selection_0042176C[0];
    var_v1 = 1;
    if (arg0 == D_selection_00421768) {
        return 0;
    }
loop_3:
    if (arg0 == var_v0[0]) {
        return var_v1;
    }
    if (arg0 == var_v0[1]) {
        return var_v1 + 1;
    }
    if (arg0 == var_v0[2]) {
        return var_v1 + 2;
    }
    if (arg0 == var_v0[3]) {
        return var_v1 + 3;
    }
    var_v1 += 4;
    var_v0 += 4;
    if (var_v1 == 0xD) {
        return -1;
    }
    goto loop_3;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040F4BC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040F64C.s")

void func_selection_0040F6E0(void) {
    func_selection_0040F4BC(D_selection_00420DF0);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040F704.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040F9EC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0040FDB4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004103CC.s")

/* Teardown: flush GFX, stop tex-anim, unload files, clear sprite sheet. */
void func_selection_0041057C(void) {
    gUvGfxMgrExports->unk74();
    gUvTexAnimExports->unk8();
    uvUnloadFile(0x55564254, 0x57);
    uvUnloadFile(0x55565458, 0x5B6);
    uvUnloadFile(0x55565458, 0x5B7);
    D_selection_00421CE0 = 1;
    gUvSprtExports->unk8(D_selection_00421E38);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00410614.s")

void func_selection_00410864(s16 a0, s16 a1) {
}

void func_selection_00410870(s16 a0, s16 a1) {
}

void func_selection_0041087C(void) {
}

void func_selection_00410884(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041088C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00410AA8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00410BD0.s")

/* Draw a 3-color layered highlight box used by the selection-screen UI. */
void func_selection_00410D20(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp2C;
    s32 sp28;
    s32 sp24;
    s32 sp20;
    s32 temp_t6;

    temp_t6 = D_selection_00421E84 / 2;
    sp2C = (arg2 - temp_t6) + D_selection_00421E88;
    sp28 = arg0 + D_selection_00421E8C;
    sp24 = (temp_t6 + arg2) + D_selection_00421E88;
    sp20 = arg1 + D_selection_00421E94;
    gUvGeomExports->unkC(0.75f, 0.75f, 0.75f, 1.0f);
    gUvGeomExports->unk10(sp2C + 1, sp28 + 1, sp24 + 1, sp20);
    gUvGeomExports->unkC(0.25f, 0.25f, 0.25f, 1.0f);
    gUvGeomExports->unk10(sp2C - 1, sp28, sp24 - 1, sp20 - 1);
    gUvGeomExports->unkC(0.5f, 0.5f, 0.5f, 1.0f);
    gUvGeomExports->unk10(sp2C, sp28, sp24, sp20);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00410E78.s")

/* Draw a selection label at (arg0+xOff+5, arg1+yOff - fontH/2) in highlight or normal color. */
void func_selection_00411038(s32 arg0, s32 arg1, s32 arg2) {
    s32 fontH;

    ((UvFontExp_00411038 *)gUvFontExports)->unk4(0xB);
    if (D_selection_00421CE8 != 0) {
        fontH = ((UvFontExp_00411038 *)gUvFontExports)->unk1C();
        ((SelPrinter_00411038 *)D_selection_00421B90)->unk40(arg0 + D_selection_00421E98 + 5, (arg1 + D_selection_00421E9C) - (fontH / 2), 0xB, 0xFB, 0xCC, 0x1E, 0xFF, arg2, 1);
    } else {
        fontH = ((UvFontExp_00411038 *)gUvFontExports)->unk1C();
        ((SelPrinter_00411038 *)D_selection_00421B90)->unk40(arg0 + D_selection_00421E98 + 5, (arg1 + D_selection_00421E9C) - (fontH / 2), 0xB, 0xD2, 0xD2, 0xD2, 0xFF, arg2, 1);
    }
    ((UvFontExp_00411038 *)gUvFontExports)->unk28();
}

void func_selection_004111B8(s32 arg0) {
    func_selection_00410BD0(0x74, 0xA3, 0xBD);
    func_selection_00410D20(0x86, 0xBD, 0xA3);
    func_selection_00410E78(0x74, 0xBD, arg0);
}

void func_selection_00411204(s32 arg0) {
    func_selection_00410BD0(0x74, 0x8E, 0xB1);
    func_selection_00410D20(0x98, 0xB1, 0x8E);
    func_selection_00410E78(0x74, 0xB1, arg0);
}

void func_selection_00411250(s32 arg0) {
    func_selection_00410BD0(0x74, 0x88, 0x90);
    func_selection_00410D20(0x8B, 0x90, 0x88);
    func_selection_00410E78(0x74, 0x90, arg0);
}

void func_selection_0041129C(s32 arg0) {
    func_selection_00410BD0(0x74, 0x80, 0x81);
    func_selection_00410E78(0x74, 0x81, arg0);
}

void func_selection_004112D8(s32 arg0) {
    func_selection_00410BD0(0x74, 0x88, 0x72);
    func_selection_00410D20(0x72, 0x76, 0x88);
    func_selection_00410E78(0x74, 0x72, arg0);
}

void func_selection_00411324(s32 arg0) {
    func_selection_00410BD0(0x74, 0x91, 0x63);
    func_selection_00410D20(0x63, 0x7D, 0x91);
    func_selection_00410E78(0x74, 0x63, arg0);
}

void func_selection_00411370(s32 arg0) {
    func_selection_00410D20(0x57, 0x5C, 0x97);
    func_selection_00410BD0(0x74, 0x97, 0x57);
    func_selection_00410E78(0x74, 0x57, arg0);
}

void func_selection_004113BC(s32 arg0) {
    func_selection_00410BD0(0xB9, 0xCD, 0xAD);
    func_selection_00410D20(0x89, 0xAD, 0xB9);
    func_selection_00411038(0xCD, 0xAD, arg0);
}

void func_selection_00411408(s32 arg0) {
    func_selection_00410BD0(0xBE, 0xCD, 0x9F);
    func_selection_00410D20(0x96, 0x9F, 0xBE);
    func_selection_00411038(0xCD, 0x9F, arg0);
}

void func_selection_00411454(s32 arg0) {
    func_selection_00410D20(0x8C, 0x91, 0xC0);
    func_selection_00410BD0(0xC0, 0xCD, 0x91);
    func_selection_00411038(0xCD, 0x91, arg0);
}

void func_selection_004114A0(s32 arg0) {
    func_selection_00410BD0(0xCA, 0xCD, 0x82);
    func_selection_00411038(0xCD, 0x82, arg0);
}

void func_selection_004114DC(s32 arg0) {
    func_selection_00410D20(0x75, 0x79, 0xC0);
    func_selection_00410BD0(0xC0, 0xCD, 0x75);
    func_selection_00411038(0xCD, 0x75, arg0);
}

void func_selection_00411528(s32 arg0) {
    func_selection_00410D20(0x67, 0x71, 0xB9);
    func_selection_00410BD0(0xB9, 0xCD, 0x67);
    func_selection_00411038(0xCD, 0x67, arg0);
}

void func_selection_00411574(s32 arg0) {
    func_selection_00410D20(0x59, 0x77, 0xB3);
    func_selection_00410BD0(0xB3, 0xCD, 0x59);
    func_selection_00411038(0xCD, 0x59, arg0);
}

// Draw three UI elements using fixed coords, with the third element's text
// pointer selected from a per-language table at D_selection_004217F0.
void func_selection_004115C0(void) {
    func_selection_00410BD0(0xA3, 0xCD, 0x4A);
    func_selection_00410D20(0x4A, 0x64, 0xA3);
    func_selection_00411038(0xCD, 0x4A, D_selection_004217F0[gOptionsLanguage].text);
}

void func_selection_0041162C(void) {
    char sp20[0x50];
    s32 sp1C;
    D_selection_004217C8_entry *temp_v0;

    sp1C = 0x74;
    temp_v0 = &D_selection_004217C8[gOptionsLanguage];
    gUvStringExports->unk10(sp20, D_selection_0041EEA0, temp_v0->unk0, temp_v0->unk4);
    if (gOptionsLanguage == 2) {
        sp1C = 0x8C;
    }
    func_selection_00410BD0(sp1C, 0xA3, 0x4A);
    func_selection_00410D20(0x4A, 0x64, 0xA3);
    func_selection_00410E78(sp1C, 0x4A, sp20);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004116E0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00411870.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00411B9C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00411F40.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004133E0.s")

/* Controller export table view: unk3C(u16,s32) and unk44(s32,s32,s32) */
/* initialise selection state, register a controller pak slot, and set the next update callback */
void func_selection_00413418(void) {
    s32 temp_v0;

    D_selection_00420DFC = 1;
    D_selection_00420E24 = -1;
    D_selection_00420E4C = 0;
    func_selection_00419084();
    *(s16 *)((u8 *)gGameSettings + 0x6F9A) = D_selection_00421BA4;
    if (D_selection_00421BA4 & 1) {
        ((UvContExp_413418 *)gUvContExports)->unk3C((u16)gGameSettings->unk6FB8, gGameSettings->unk6FBC);
        temp_v0 = ((UvContExp_413418 *)gUvContExports)->unk44(0, gGameSettings->unk6FB0, gGameSettings->unk6FB4);
        if (temp_v0 >= 0) {
            D_selection_00420E24 = temp_v0;
            D_8002CC8C = temp_v0;
        } else {
            D_selection_00420E24 = -1;
            *(s32 *)((u8 *)gGameSettings + 0x6F9C) = -1;
        }
    }
    func_selection_00411B9C();
    D_selection_00421D28 = (s32)func_selection_00411B9C;
}

void func_selection_004134F4(void) {
    D_selection_00420DFC = 1;
    func_selection_004017E0();
}

void func_selection_0041351C(void) {
    if (D_selection_00420E24 >= 0) {
        func_selection_00402E34();
        func_selection_00402E98(&D_selection_004201BC);
        return;
    }
    func_selection_00413594();
}

void func_selection_00413568(void) {
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_00420218);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00413594.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004136CC.s")

void func_selection_0041378C(void) {
    D_selection_00420DE8 = 2;
    D_selection_00420DEC = 0;
    D_8002CC84 = 0;
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041F770);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004137D0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00413A88.s")

void func_selection_00413DE0(void) {
    gUvSprtExports->unk8(D_selection_00420DF8);
    D_selection_00420DF8 = -1;
}

void func_selection_00413E1C(void) {
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_00420074);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00413E48.s")

void func_selection_00413FA4(void) {
}

void func_selection_00413FAC(void) {
}

void func_selection_00413FB4(void) {
}

void func_selection_00413FBC(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00413FC4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004146C8.s")

void func_selection_00414CEC(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00414CF4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00415040.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041532C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004155A0.s")

/* Tear down selection screen: camera reset, clear gfx/texanim, unload files, reset track object. */
void func_selection_004158DC(void) {
    s32 var_s0;

    gCamExports->unk20(*D_selection_00421D18, 0x9, (f64) D_selection_0041F4C4, 0);
    var_s0 = 0;
    do {
        func_selection_00416A50(var_s0, 0);
        var_s0 += 1;
    } while (var_s0 != 8);
    gUvGfxMgrExports->unk74();
    gUvTexAnimExports->unk8();
    uvUnloadFile(0x55565452, 0x2);
    uvUnloadFile(0x5556454E, 0xB);
    (*D_selection_00421D18)->unk220 = -1;
    gUvTerraExports->unk8((*D_selection_00421D18)->unk0, -1);
    gUvEnvExports->unk18((*D_selection_00421D18)->unk0, -1);
}

void func_selection_004159FC(void) {
    func_selection_00402E34();
    D_selection_00420DFC = 1;
    func_selection_00402E98(&D_selection_00420074);
}

void func_selection_00415A34(s32 arg0) {
    if (arg0 == 1) {
        gGameSettings->currentTrack = 0x1B;
        gGameSettings->finishedIntroCount = 0;
    }
    D_selection_00421E08 = 8;
    gGameSettings->unkC = arg0;
    D_selection_00421D28 = D_selection_00421240[D_selection_00421E08];
    D_selection_00420DE8 = 2;
    D_selection_00420DEC = 0;
}

void func_selection_00415AA0(void) {
    if (gGameSettings->pad178[5] != 0) {
        gGameSettings->gameStateFlag = 2;
        return;
    }
    gGameSettings->gameStateFlag = 3;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00415AD0.s")

void func_selection_00415B90(void) {
    func_selection_00415AD0(0xBC, &D_selection_00420E90);
}

void func_selection_00415BB8(void) {
    func_selection_00415AD0(0xE4, &D_selection_00420E84);
}

/* Stop active sounds then play the circuit-announce sfx when arg0 == 3. */
void func_selection_00415BE0(s32 arg0) {
    (*(void (**)(s32))((u8 *)gSndExports + 0xE0))(0xE);
    (*(void (**)(s32))((u8 *)gSndExports + 0xE0))(0xC);
    (*(void (**)(s32))((u8 *)gSndExports + 0xE0))(0xDB);
    if (arg0 != 0 && arg0 != 1 && arg0 != 2 && arg0 == 3) {
        func_selection_00415AD0(0x7A, &D_selection_00420E68);
    }
}

void func_selection_00415C78(void) {
    func_selection_00415AD0(0x7B, &D_selection_00420E74);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00415CA0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00415D48.s")

void func_selection_00415D9C(void) {
    func_selection_00415AD0(0xB5, &D_selection_00420E78);
}

void func_selection_00415DC4(void) {
    func_selection_00415AD0(0xB6, &D_selection_00420E70);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00415DEC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00415E80.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00415EF4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00415FB4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00416028.s")

/* Tear down the selection screen: clear gfx/texanim, unload files, reset track object. */
void func_selection_004162D4(void) {
    func_selection_00416A50(0, 0);
    gUvGfxMgrExports->unk74();
    gUvTexAnimExports->unk8();
    uvUnloadFile(0x55565452, 0x8);
    uvUnloadFile(0x5556454E, 0x12);
    (*D_selection_00421D18)->unk220 = -1;
    gUvTerraExports->unk8((*D_selection_00421D18)->unk0, -1);
    gUvEnvExports->unk18((*D_selection_00421D18)->unk0, -1);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041639C.s")

/* Tear-down: resets selection render state, unloads track/env files, clears terrain and env. */
void func_selection_004166CC(void) {
    func_selection_00416A50(0, 0);
    gUvGfxMgrExports->unk74();
    gUvTexAnimExports->unk8();
    uvUnloadFile(0x55565452, 0x9);
    uvUnloadFile(0x5556454E, 0xB);
    (**D_selection_00421D18).unk220 = -1;
    gUvTerraExports->unk8((**D_selection_00421D18).unk0, -1);
    gUvEnvExports->unk18((**D_selection_00421D18).unk0, -1);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00416794.s")

/* Applies a rotation offset to a world matrix via identity-reset and two axis rotations */
void func_selection_004168C4(f32 arg0, f32 arg1, f32 arg2) {
    Mtx4F sp20;

    gUvFmtxExports->unk0C(&sp20, &((ObjNode_4168C4 *)(*D_selection_00421D18))->unkFC);
    gUvFmtxExports->unk2C(&sp20, -arg0, -((ObjNode_4168C4 *)(*D_selection_00421D18))->unkF8, -arg1);
    sp20.xx = 1.0f;
    sp20.yy = 1.0f;
    sp20.zz = 1.0f;
    sp20.yx = 0.0f;
    sp20.zx = 0.0f;
    sp20.xy = 0.0f;
    sp20.zy = 0.0f;
    sp20.xz = 0.0f;
    sp20.yz = 0.0f;
    gUvFmtxExports->unk28(&sp20, D_selection_00420E20 * D_selection_0041F4EC, 0x7A);
    gUvFmtxExports->unk28(&sp20, arg2 * D_selection_0041F4F0, 0x78);
    gUvFmtxExports->unk2C(&sp20, arg0, ((ObjNode_4168C4 *)(*D_selection_00421D18))->unkF8, arg1);
    gUvFmtxExports->unk0C(&((ObjNode_4168C4 *)(*D_selection_00421D18))->unkFC, &sp20);
    gUvFmtxExports->unk0C(&((ObjNode_4168C4 *)(*D_selection_00421D18))->unk1A8, &((ObjNode_4168C4 *)(*D_selection_00421D18))->unkFC);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00416A50.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00416ABC.s")

/* Set sprite props then render two sprites with gfx state push/pop */
void func_selection_00416D94(void) {
    ((SprtExp_00416D94 *)gUvSprtExports)->unk1C(D_selection_00421E36, 3, 1, 0xB, 1, 0xC, 0, 0);
    ((GfxStateExp_00416D94 *)gUvGfxStateExports)->unk50();
    ((GfxStateExp_00416D94 *)gUvGfxStateExports)->unkC(0x800000);
    ((GfxStateExp_00416D94 *)gUvGfxStateExports)->unk10((s32)0xBE7C0000);
    ((SprtExp_00416D94 *)gUvSprtExports)->unk10(D_selection_00421E36);
    ((SprtExp_00416D94 *)gUvSprtExports)->unk10(D_selection_00421E34);
    ((GfxStateExp_00416D94 *)gUvGfxStateExports)->unk54();
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00416E78.s")

void func_selection_00416F28(void) {
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041F728);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00416F54.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00416FF4.s")

void func_selection_00417240(void) {
    D_selection_00420E18 = *(s16*)((u8*)*(void**)((u8*)D_selection_00421D1C + 8) + 4);
    D_selection_00420E1C = 1;
}

/* Tear down controller-selection state: notify the controller export, clear tracking globals. */
void func_selection_00417268(void) {
    gUvContExports->unk5C(0, D_selection_00420E18);
    D_selection_00421E40 = 0;
    D_selection_00420E1C = 0;
    D_selection_00420E18 = -1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004172B4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004177A8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_004181D0.s")

void func_selection_0041843C(void) {
    D_selection_00420E28 = -1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041844C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00418800.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00418E98.s")

void func_selection_00419020(void) {
    *(s16 *)((u8 *)&D_selection_0041F794 + 0x16) = 1;
    *(s16 *)((u8 *)&D_selection_0041F794 + 0x2A) = 1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00419038.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00419084.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00419194.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00419384.s")

/* Initialize controller context, bind buttons, then read or default an axis value */
void func_selection_00419AA4(void) {
    s32 temp_v0;

    func_selection_00415EF4();
    func_selection_004017E0();
    func_selection_00419084();
    *(s32 *)((u8 *)D_selection_00421D1C + 0x20) = 1;
    if (D_selection_00421BA4 & 1) {
        ((UvContExp_00419AA4 *)gUvContExports)->unk3C((u16)gGameSettings[0].unk6FB8, gGameSettings[0].unk6FBC);
        temp_v0 = ((UvContExp_00419AA4 *)gUvContExports)->unk44(0, gGameSettings[0].unk6FB0, gGameSettings[0].unk6FB4);
        if (temp_v0 >= 0) {
            D_selection_00420E24 = temp_v0;
            D_8002CC8C = temp_v0;
            return;
        }
        D_selection_00420E24 = -1;
        D_8002CC8C = -1;
        return;
    }
    func_selection_00419B9C();
}

void func_selection_00419B78(void) {
    D_selection_00421D28 = 0;
    func_selection_004017E0();
}

void func_selection_00419B9C(void) {
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_0041F5B4);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00419BC8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00419D54.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_00419FB8.s")

/* Unloads language-specific sprite/texture files and frees a sprite slot based on gOptionsLanguage (0=English, 1=French, 2=German). */
void func_selection_0041A0CC(void) {
    switch (gOptionsLanguage) {
    case 0:
        gUvGfxMgrExports->unk74();
        uvUnloadFile(0x55565458, 9);
        uvUnloadFile(0x55565458, 8);
        uvUnloadFile(0x55564254, 0x1A);
        break;
    case 1:
        gUvGfxMgrExports->unk74();
        uvUnloadFile(0x55565458, 0x5A6);
        uvUnloadFile(0x55565458, 8);
        uvUnloadFile(0x55564254, 0x52);
        break;
    case 2:
        gUvGfxMgrExports->unk74();
        uvUnloadFile(0x55565458, 0x5A5);
        uvUnloadFile(0x55565458, 0x5A4);
        uvUnloadFile(0x55564254, 0x51);
        break;
    }
    gUvSprtExports->unk8(D_selection_00420DF8);
    D_selection_00420DF8 = -1;
}

void func_selection_0041A208(void) {
}

void func_selection_0041A210(void) {
    func_selection_00415D9C();
    func_selection_00402E34();
    func_selection_00402E98(&D_selection_00420260);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041A244.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041A5BC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041A7D0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041A960.s")

f32 func_selection_0041ABC8(s32 arg0, s32 arg1, s32 arg2) {
    return (f32) ((arg0 * 0x3C) + arg1) + ((f32) arg2 / 100.0f);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041ABFC.s")

void func_selection_0041B11C(void) {
    gGameSettings->unk174 = 1;
    gGameSettings->pad178[1] = 1;
    func_selection_00402E98(*(s32*)((u8*)D_selection_00421D1C + 4));
}

void func_selection_0041B15C(void) {
    gGameSettings->unk174 = 2;
    gGameSettings->pad178[1] = 1;
    func_selection_00402E98(*(s32*)((u8*)D_selection_00421D1C + 4));
}

/* Inferred struct for items in the InnerObj array (size 0x14) */
/* Inferred struct pointed to by D_selection_00421D1C->unk8 (InnerObj at offset 8 of D1C target) */
/* Inferred struct layout for what D_selection_00421D1C points to (InnerObj* at offset 8) */
/* Inferred struct pointed to by D_selection_00421B90 (cast-only, no extern redecl) */
/* Handles controller confirm input for selection screen, sets game state and selected function pointer */
void func_selection_0041B19C(void) {
    InnerObj_0041B19C *temp_v0;

    D_selection_00420DE8 = 0;
    ((ContExp_0041B19C *)D_selection_00421B90)->unk4(&D_selection_00421CF0, 0, ((D1CTarget_0041B19C *)D_selection_00421D1C)->unk8);
    if (((ContExp_0041B19C *)D_selection_00421B90)->unk8(0, ((D1CTarget_0041B19C *)D_selection_00421D1C)->unk8) == 8) {
        if (*(u8 *)&D_selection_00421CF0 == 0) {
            D_selection_00421CF0 = 1;
            temp_v0 = ((D1CTarget_0041B19C *)D_selection_00421D1C)->unk8;
            D_selection_00421D28 = (s32)D_selection_00420F18[temp_v0->unk8[temp_v0->unk4].unk4];
            D_selection_00420DE8 = 2;
            D_selection_00420DEC = 0;
        }
    } else {
        D_selection_00421CF0 = 0;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041B28C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/selection/func_selection_0041B370.s")

