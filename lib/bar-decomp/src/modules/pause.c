// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s16 D_pause_00405DF4;
/*__SEEDEXTERNS__*/
typedef struct { char pad[0xDC]; void (*unkDC)(void); void (*unkE0)(s32); } SndExp_00403E6C;
typedef struct { char pad[0x2C]; s16 (*unk2C)(s32); } ContExp_00403E6C;
typedef struct { char pad[0x20]; s32 unk20; } PauseObj_00403E6C;
extern void *gUvContExports;
extern s16 D_8002CC8A;
extern void func_pause_004006E4(void);
typedef struct UvFont_Exports_004003B8_s {
    /* 0x00 */ void (*uvModuleCleanup)(void);
    /* 0x04 */ void (*uvSetFont)(s32);
    /* 0x08 */ void (*uvFontScale)(f64, f64);
    /* 0x0C */ void (*uvFontColor)(u8, u8, u8, u8);
    /* 0x10 */ s32 (*uvFontStrlLen)(s32);
    /* 0x14 */ s32 (*uvFontStr16WidthFont)(s32);
    /* 0x18 */ s32 (*uvFontWidth)(const char *);
    /* 0x1C */ s32 (*uvFontHeight)(void);
    /* 0x20 */ s32 (*uvFontPrintStr16)(s32, s32, s32, s32);
    /* 0x24 */ void (*uvFontPrintStr)(s32, s32, char *);
    /* 0x28 */ void (*uvFontGenDList)(void);
} UvFont_Exports_004003B8;
extern UvFont_Exports_004003B8 *gUvFontExports;
extern u8 D_pause_00405D48;
extern s32 D_pause_00405D44;
extern u8 gOptionsMap;
extern u8 D_pause_004054B4;
extern s32 D_pause_00405D3C;
extern s32 D_pause_00405D40;
extern s32 D_pause_00405D34;
extern f32 D_pause_00405D38;
extern u8 D_pause_00405BEC;
extern void *D_pause_00405E4C;
extern u8 D_pause_00405E58;
extern u8 D_pause_00405E60;
extern u8 D_pause_00405E68;
extern u8 D_pause_00405E70;
extern s32 gGameStateFlag;
typedef struct MenuNode_s { s32 unk0; void *unk4; void *unk8; } MenuNode;
extern MenuNode *D_pause_00405E50;
extern MenuNode D_pause_00405618;
extern MenuNode D_pause_00405AA0;
extern MenuNode D_pause_004055BC;
extern MenuNode D_pause_00405950;
typedef struct { char pad0[0x34]; void (*unk34)(void *, s32, s32, s32, void *); } UnkStruct_pause_E44;
extern UnkStruct_pause_E44 *D_pause_00405E44;
typedef struct { s32 unk0; s32 unk4; void (*unk8)(s32); } UnkStruct_pause_E48;
extern UnkStruct_pause_E48 *D_pause_00405E48;
typedef struct { char pad0[0xF4]; void (*unkF4)(s32); } UnkStruct_pause_SndExports;
extern UnkStruct_pause_SndExports *gSndExports;
typedef struct { char pad0[0xC]; void (*unkC)(s32); } UnkStruct_pause_ReplayExports;
extern UnkStruct_pause_ReplayExports *gReplayExports;
extern s32 gOptionsSpeechVol;
extern s32 gOptionsSfxVol;
extern s32 gOptionsMusicVol;
extern u8 gOptionsStereoMono;
extern u8 gOptionsSpeed;
extern u8 gOptionsDisplay;
extern s16 D_8002CC88;
extern s32 D_8002CD98;
extern s16 D_80025D76;
extern u8 D_80025E6A;
void func_pause_00400164();
void func_pause_004001F0();
void func_pause_00400258();
void func_pause_00400428(void *arg0);
void func_pause_0040048C();
void func_pause_00400544();
void func_pause_004006E4();
void func_pause_00400750();
void func_pause_0040092C();
void func_pause_0040098C();
void func_pause_00400C74();
void func_pause_00400CB0();
void func_pause_00400CE0();
void func_pause_00400D10();
void func_pause_00400D40();
void func_pause_00400DA8(void);
void func_pause_00400DB8();
void func_pause_00400E48();
void func_pause_00400EEC();
void func_pause_0040197C();
void func_pause_004020F0();
void func_pause_004021BC();
void func_pause_00402268();
void func_pause_004022B0();
void func_pause_004022F8();
void func_pause_00402340();
void func_pause_00402380();
void func_pause_004023A4();
void func_pause_004023C8();
void func_pause_0040244C();
void func_pause_00402470();
void func_pause_00402550();
void func_pause_004025B4();
void func_pause_00402600();
void func_pause_00402630();
void func_pause_0040264C();
void func_pause_004026A8();
void func_pause_004026C4();
void func_pause_00402854();
void func_pause_00402904();
void func_pause_00402914();
void func_pause_00402D78();
void func_pause_00403F60();
void func_pause_00403FD0();
void func_pause_00404150();
void func_pause_00404160();
void func_pause_004041A0();
void func_pause_0040423C();
void func_pause_00404574();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/__entrypoint_func_pause_400000.s")

void func_pause_00400164(void) {
    if (D_pause_00405E44 != 0) {
        uvUnloadModule(0x6D656E75);
        D_pause_00405E44 = 0;
    }
    (*(void (**)(void *))((u8 *)gSndExports + 0x70))(&D_pause_00405E60);
    (*(void (**)(void *))((u8 *)gSndExports + 0x70))(&D_pause_00405E68);
    (*(void (**)(void *))((u8 *)gSndExports + 0x70))(&D_pause_00405E70);
}

void func_pause_004001F0(s32 arg0) {
    void *temp_v0;

    temp_v0 = *(void **)((u8 *)&D_pause_00405BEC + arg0 * 4);
    D_pause_00405E4C = temp_v0;
    D_pause_00405E50 = *(s32 *)((u8 *)temp_v0 + 0x10);
    func_pause_00400E48();
    func_pause_0040092C();
    *(u8 *)((u8 *)&D_pause_00405E58 + 0) = 1;
    *(u8 *)((u8 *)&D_pause_00405E58 + 1) = 1;
    *(u8 *)((u8 *)&D_pause_00405E58 + 2) = 1;
    *(u8 *)((u8 *)&D_pause_00405E58 + 3) = 1;
    *(u8 *)((u8 *)&D_pause_00405E58 + 4) = 1;
    *(u8 *)((u8 *)&D_pause_00405E58 + 5) = 1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00400258.s")

// Dispatch the current pause-state's update function, then call the owner's unk14 and flush the font display list.
void func_pause_004003B8(void) {
    (*(void (*)(void))(*(s32 *)((u8 *)&D_pause_00405D48 + (*(s16 *)((u8 *)D_pause_00405E50 + 0x12) << 2))))();
    (*((void (**)(MenuNode *, s32))((u8 *)D_pause_00405E44 + 0x14)))(D_pause_00405E50, 1);
    gUvFontExports->uvFontGenDList();
}

void func_pause_00400428(void *arg0)
{
  int temp_v0;
  unsigned int new_var2;
  u8 *new_var;
  new_var = (u8 *) (((u8 *) arg0) + 0x0);
  temp_v0 = (*((s32 (**)(s32, s32)) (((u8 *) D_pause_00405E44) + 0xC)))(0, D_pause_00405E50);
  if (D_pause_00405DF4 == 0)
  {
    if (temp_v0 != 0)
    {
      new_var2 = *new_var;
      *new_var = new_var2 < 1;
    }
  }
  D_pause_00405DF4 = temp_v0;
}

/* Advance the pause-state linked list: dispatch exit for current node, swap to next, dispatch enter, then update unk8->unk4. */
typedef struct PauseExtNode_0040048C_s {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ struct PauseExtNode_0040048C_s *unk4;
    /* 0x08 */ void *unk8;
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s16 unk10;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
} PauseExtNode_0040048C;
typedef struct PauseOwnerExt_0040048C_s {
    /* 0x00 */ char pad0[0x18];
    /* 0x18 */ s16 (*unk18)(void *);
} PauseOwnerExt_0040048C;
typedef struct PauseOwnerTarget_0040048C_s {
    /* 0x00 */ char pad0[0x4];
    /* 0x04 */ s16 unk4;
} PauseOwnerTarget_0040048C;
typedef struct UvGfxMgrExp_0040048C_s {
    /* 0x00 */ char pad0[0x74];
    /* 0x74 */ void (*unk74)(void);
} UvGfxMgrExp_0040048C;
extern UvGfxMgrExp_0040048C *gUvGfxMgrExports;

void func_pause_0040048C(void)
{
  PauseExtNode_0040048C *temp_t2;
  if (((PauseExtNode_0040048C *) D_pause_00405E50)->unk4 != 0)
  {
    (*((void (**)(void)) (((u8 *) (&D_pause_00405D48)) + (((PauseExtNode_0040048C *) D_pause_00405E50)->unk16 << 2))))();
    gUvGfxMgrExports->unk74();
    temp_t2 = (D_pause_00405E50 = ((PauseExtNode_0040048C *) D_pause_00405E50)->unk4);
    (*((void (**)(void)) (((u8 *) (&D_pause_00405D48)) + ((temp_t2->unk14 << 1) << 1))))();
    ((PauseOwnerTarget_0040048C *) ((PauseExtNode_0040048C *) D_pause_00405E50)->unk8)->unk4 = ((PauseOwnerExt_0040048C *) D_pause_00405E44)->unk18(((PauseExtNode_0040048C *) D_pause_00405E50)->unk8);
  }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00400544.s")

void func_pause_004006E4(void) {
    (*(void (**)(s32))((u8 *)gSndExports + 0x44))(gOptionsSpeechVol);
    (*(void (**)(s32))((u8 *)gSndExports + 0x40))(gOptionsSfxVol);
    (*(void (**)(s32))((u8 *)gSndExports + 0x3C))(gOptionsMusicVol);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00400750.s")

void func_pause_0040092C(void) {
    func_pause_004006E4();
    (*(void (**)(void))((u8 *)gSndExports + 0xDC))();
    (*(void (**)(s32))((u8 *)gSndExports + 0xE0))(0xDB);
    D_pause_00405D34 = 0;
    D_pause_00405D38 = 0.5f;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_0040098C.s")

void func_pause_00400C6C(void) {
}

void func_pause_00400C74(void) {
    gReplayExports->unkC(D_80025D76 == 0);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00400CB0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00400CE0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00400D10.s")

void func_pause_00400D40(void) {
    if (uvGetLoadedModule(0x72616365) != 0) {
        gGameStateFlag = 5;
        D_pause_00405D3C = 1;
        D_pause_00405D40 = 1;
    } else if (uvGetLoadedModule(0x6261746C) != 0) {
        gGameStateFlag = 6;
    }
}

void func_pause_00400DA8(void) {
    gGameStateFlag = 0xE;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00400DB8.s")

void func_pause_00400E48(void) {
    if (*(s32 *)((u8 *)gGameSettings + 0x6F74) == 2) {
        *(s16 *)((u8 *)*(s32 *)((u8 *)*(s32 *)((u8 *)&D_pause_004054B4 + 0x8) + 0x8) + 0x2A) = 2;
        *(s16 *)((u8 *)*(s32 *)((u8 *)*(s32 *)((u8 *)&D_pause_004054B4 + 0x8) + 0x8) + 0x18) = 7;
        if (*(s32 *)((u8 *)gGameSettings + 0x6F64) <= 0) {
            *(s16 *)((u8 *)*(s32 *)((u8 *)*(s32 *)((u8 *)&D_pause_004054B4 + 0x8) + 0x8) + 0x16) = 0;
            return;
        }
        *(s16 *)((u8 *)*(s32 *)((u8 *)*(s32 *)((u8 *)&D_pause_004054B4 + 0x8) + 0x8) + 0x16) = 1;
        return;
    }
    *(s16 *)((u8 *)*(s32 *)((u8 *)*(s32 *)((u8 *)&D_pause_004054B4 + 0x8) + 0x8) + 0x2A) = 1;
    *(s16 *)((u8 *)*(s32 *)((u8 *)*(s32 *)((u8 *)&D_pause_004054B4 + 0x8) + 0x8) + 0x16) = 1;
    *(s16 *)((u8 *)*(s32 *)((u8 *)*(s32 *)((u8 *)&D_pause_004054B4 + 0x8) + 0x8) + 0x18) = 6;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00400EEC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_0040197C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_004020F0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_004021BC.s")

void func_pause_00402268(void) {
    D_pause_00405E44->unk34(&gOptionsSpeechVol, 8, 1, 0, D_pause_00405E50);
}

void func_pause_004022B0(void) {
    D_pause_00405E44->unk34(&gOptionsSfxVol, 8, 1, 1, D_pause_00405E50);
}

void func_pause_004022F8(void) {
    D_pause_00405E44->unk34(&gOptionsMusicVol, 8, 1, 2, D_pause_00405E50);
}

void func_pause_00402340(void) {
    func_pause_00400428(&gOptionsStereoMono);
    gSndExports->unkF4(gOptionsStereoMono);
}

void func_pause_00402380(void) {
    func_pause_00400428(&gOptionsSpeed);
}

void func_pause_004023A4(void) {
    func_pause_00400428(&D_80025E6A);
}

void func_pause_004023C8(void) {
    s32 sp1C = *(u8 *)((u8 *)gGameSettings + 0x17B);
    func_pause_00402854(&gOptionsMap, 0, 2);
    if (*(s32 *)((u8 *)gGameSettings + 0x24) == 2 &&
        *(u8 *)((u8 *)gGameSettings + 0x17B) == 1) {
        if (sp1C == 0) {
            *(u8 *)((u8 *)gGameSettings + 0x17B) = 2;
        } else if (sp1C == 2) {
            *(u8 *)((u8 *)gGameSettings + 0x17B) = 0;
        }
    }
}

void func_pause_0040244C(void) {
    func_pause_00400428(&gOptionsDisplay);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00402470.s")

void func_pause_00402548(void) {
}

void func_pause_00402550(void) {
    func_pause_004006E4();
    (*(void (**)(void))((u8 *)gSndExports + 0xDC))();
    if (*(s32 *)((u8 *)gGameSettings + 0x24) == 2) {
        if (*(u8 *)((u8 *)gGameSettings + 0x17B) == 1) {
            *(u8 *)((u8 *)gGameSettings + 0x17B) = 2;
        }
    }
}

void func_pause_004025B4(void) {
    *(s32 *)((u8 *)&D_8002CD98 + 0x2D0) = gGameSettings->optionsMusicVol;
    *(s32 *)((u8 *)&D_8002CD98 + 0x2D4) = gGameSettings->optionsSfxVol;
    *(s32 *)((u8 *)&D_8002CD98 + 0x2D8) = gGameSettings->optionsSpeechVol;
    *(u8 *)((u8 *)&D_8002CD98 + 0x2DC) = gGameSettings->pad178[0];
    *(u8 *)((u8 *)&D_8002CD98 + 0x2DD) = gGameSettings->pad178[1];
    *(u8 *)((u8 *)&D_8002CD98 + 0x2DE) = gGameSettings->pad178[3];
    *(u8 *)((u8 *)&D_8002CD98 + 0x2DF) = gGameSettings->pad178[4];
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00402600.s")

void func_pause_00402630(void) {
    *(u8 *)((u8 *)gGameSettings + 0x17F) = *(s32 *)((u8 *)gGameSettings + 0x44);
    *(s32 *)((u8 *)gGameSettings + 0x8) = 0xE;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_0040264C.s")

void func_pause_004026A8(void) {
    *(u8 *)((u8 *)gGameSettings + 0x17E) = *(s32 *)((u8 *)gGameSettings + 0x44);
    *(s32 *)((u8 *)gGameSettings + 0x8) = 0xE;
}

/* Draw two textured rectangles for the pause screen HUD bar. */
typedef struct {
    /* 0x00 */ char pad0[0x1C];
    /* 0x1C */ void (*unk1C)(s32, s32, s32 *, s32 *, s32);
} UvTextureExp_004026C4;
extern UvTextureExp_004026C4 *gUvTextureExports;
typedef struct {
    /* 0x00 */ char pad0[0x14];
    /* 0x14 */ void (*unk14)(s32);
    /* 0x18 */ char pad18[0x44];
    /* 0x5C */ void (*unk5C)(s32, s32, s32, s32, f32, f32, f32, f32);
} UvGfxStateExp_004026C4;
extern UvGfxStateExp_004026C4 *gUvGfxStateExports;

void func_pause_004026C4(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
  s32 sp34;
  s32 sp30;
  f32 temp_fv0;
  gUvGfxStateExports->unk14(0x11);
  gUvTextureExports->unk1C(0x11, 1, &sp34, &sp30, 0);
  gUvGfxStateExports->unk5C(arg2, (sp34 + arg2) - 1, arg3, (sp30 + arg3) - 1, 0.0f, 1.0f, 0.0f, 1.0f);
  gUvGfxStateExports->unk14(0x12);
  gUvTextureExports->unk1C(0x12, 1, &sp34, &sp30, 0);
  temp_fv0 = ((f32) arg0) / ((f32) arg1);
  gUvGfxStateExports->unk5C(arg2, (s32) (((f32) arg2) + (((f32) ((s32) (((f32) sp34) * temp_fv0))) - 1.0f)), arg3 + 2, (arg3 + sp30) + 1, 0.0f, temp_fv0, 0.0f, 1.f);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00402854.s")

void func_pause_00402904(void) {
    D_8002CC88 = 1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00402914.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00402D78.s")

/* Tear-down: reset pause module, stop BGM, play SFX, re-enable controller, mark active. */
void func_pause_00403E6C(void) {
    func_pause_004006E4();
    ((SndExp_00403E6C *)gSndExports)->unkDC();
    ((SndExp_00403E6C *)gSndExports)->unkE0(0xDB);
    D_8002CC88 = 0;
    D_8002CC8A = ((ContExp_00403E6C *)gUvContExports)->unk2C(0);
    ((PauseObj_00403E6C *)D_pause_00405E50)->unk20 = 1;
}

void func_pause_00403EF0(void) {
    if (D_pause_00405D3C != 0) {
        (*(void (**)(void *, s32, s32, f32, f32))((u8 *)gSndExports + 0x8))(&D_pause_00405E60, 0xD7, 0x7FFF, (*(f32 (**)(void))((u8 *)gSndExports + 0x4C))(), 0.5f);
        D_pause_00405D3C = 0;
    }
}

void func_pause_00403F60(void) {
    if (D_pause_00405D44 != 0) {
        (*(void (**)(void *, s32, s32, f32, f32))((u8 *)gSndExports + 0x8))(&D_pause_00405E70, 0xF6, 0x7FFF, (*(f32 (**)(void))((u8 *)gSndExports + 0x4C))(), 0.5f);
        D_pause_00405D44 = 0;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_00403FD0.s")

void func_pause_00404150(void) {
    D_8002CC88 = 8;
}

void func_pause_00404160(void) {
    *(s16 *)((u8 *)gGameSettings + 0x6F98) = 0;
    *(s16 *)((u8 *)gGameSettings + 0x6F96) = 0;
    D_pause_00405E48->unk8(8);
}

void func_pause_004041A0(void) {
    if ((*(s32 (*)(s32, s32))(*(s32 *)((u8 *)D_pause_00405E44 + 0x8)))(
            0, *(s32 *)((u8 *)D_pause_00405E50 + 0x8)) == 8) {
        if (D_pause_00405E58 == 0) {
            D_pause_00405E58 = 1;
            (*(void (*)(void))(*(s32 *)((u8 *)&D_pause_00405D48
                + (*(s16 *)((u8 *)*(s32 *)((u8 *)*(s32 *)((u8 *)D_pause_00405E50 + 0x8) + 0x8) + 0x4) << 2))))();
        }
    } else {
        D_pause_00405E58 = 0;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/pause/func_pause_0040423C.s")

void func_pause_00404574(void) {
    func_pause_004006E4();
    (*(void (**)(void))((u8 *)gSndExports + 0xDC))();
    (*(void (**)(s32))((u8 *)gSndExports + 0xE0))(0xDB);
    (*(void (**)(s32))((u8 *)gSndExports + 0xE0))(0xE);
}

