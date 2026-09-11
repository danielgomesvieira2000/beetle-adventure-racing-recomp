// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "global_exports.h"
/*__SEEDEXTERNS__*/
u16 uvGetFileInstanceCount(s32 tag, s32 fileId);
s32 uvGetFilesCount(s32 tag);
void *uvGetLoadedFile(s32 formFileTag, s32 fileId);
void func_gamegui_004023BC();
extern void func_gamegui_00400194(s32);
extern void func_gamegui_00400370(s32);
extern void func_gamegui_004014F8(s32);
extern void func_gamegui_00401E7C(void);
extern s32 D_gamegui_00402450;
extern s32 D_gamegui_00402C74;
typedef struct {
    /* 0x00 */ char pad0[0x8];
    /* 0x08 */ void (*unk8)(void *, s32);
    /* 0x0C */ char padC[0x64];
    /* 0x70 */ s32 (*unk70)(void);
    /* 0x74 */ s32 (*unk74)(s32);
    /* 0x78 */ void (*unk78)(s32, s32 *);
} LocalGuiExp_4000B0;
typedef struct {
    /* 0x00 */ char pad0[0x20];
    /* 0x20 */ void (*unk20)(s32);
} SceneExp_4000B0;
extern SceneExp_4000B0 *gSceneExports;
typedef struct { char pad0[0xC]; void (*unkC)(void *); char pad10[0x18]; void (*unk28)(void *, s32, s32); } GameguiGuiExports;
extern GameguiGuiExports *gUvGuiExports;
typedef struct { char pad[0x34]; void (*unk34)(u16); void (*unk38)(s32); } GameguiSndExports;
extern GameguiSndExports *gSndExports;
typedef struct { char pad[0x2C]; void (*unk2C)(void); } GameguiCmidiExports;
extern GameguiCmidiExports *gUvCmidiExports;
typedef struct { char pad[0x80]; void (*unk80)(void); } GameguiEmitterExports;
extern GameguiEmitterExports *gUvEmitterExports;
typedef struct { char pad[0x40]; void (*unk40)(void); } GameguiAudiomgrExports;
extern GameguiAudiomgrExports *gUvAudiomgrExports;
typedef struct { char pad[0x18]; void (*unk18)(s32); } GameguiDebugExports;
extern GameguiDebugExports *gUvDebugExports;
extern s32 D_debugEnable;
extern u8 D_gamegui_00402C68;
extern u8 D_gamegui_00402C78;
extern s32 D_gamegui_00402D04;
extern u16 D_gamegui_00402D16;
extern s32 D_gamegui_00402D28;
void func_gamegui_00400194();
void func_gamegui_00400370();
void func_gamegui_004014F8();
void func_gamegui_00401748();
void func_gamegui_00401A7C();
void func_gamegui_00401AD8();
void func_gamegui_00401AF8();
void func_gamegui_00401D34();
void func_gamegui_00401D94();
void func_gamegui_00401DC4();
void func_gamegui_00401DF4();
void func_gamegui_00401E24();
void func_gamegui_00401E50();
void func_gamegui_00401E7C();
void func_gamegui_00401FE0();
void func_gamegui_00402030();
void func_gamegui_0040203C();
void func_gamegui_00402074();
void func_gamegui_00402088();
void func_gamegui_0040209C();
void func_gamegui_004020BC();
void func_gamegui_004020DC();
void func_gamegui_00402110();
void func_gamegui_00402130();
void func_gamegui_00402144();
void func_gamegui_00402158();
void func_gamegui_0040216C();
void func_gamegui_00402180();
void func_gamegui_0040219C();
void func_gamegui_004021DC();
void func_gamegui_00402358();
u8 func_gamegui_004023B0();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/gamegui/__entrypoint_func_gamegui_400000.s")

void func_gamegui_004000A8(void) {
}

/* Initializes the game GUI: creates a GUI object, loads a resource, registers it, and sets up sub-systems. */
void func_gamegui_004000B0(void) {
    s32 temp_v0;

    if (D_debugEnable != 0) {
        D_gamegui_00402C74 = 1;
        func_gamegui_00401E7C();
        temp_v0 = ((LocalGuiExp_4000B0 *)gUvGuiExports)->unk74((s16) ((LocalGuiExp_4000B0 *)gUvGuiExports)->unk70());
        ((LocalGuiExp_4000B0 *)gUvGuiExports)->unk78(temp_v0, &D_gamegui_00402450);
        ((LocalGuiExp_4000B0 *)gUvGuiExports)->unk8(&D_gamegui_00402C78, temp_v0);
        func_gamegui_00400194(temp_v0);
        func_gamegui_00400370(temp_v0);
        func_gamegui_004014F8(temp_v0);
        gSceneExports->unk20(temp_v0);
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/gamegui/func_gamegui_00400194.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/gamegui/func_gamegui_00400370.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/gamegui/func_gamegui_004014F8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/gamegui/func_gamegui_00401748.s")

void func_gamegui_00401A7C(void) {
    if (D_gamegui_00402C68 != 0) {
        gUvGuiExports->unkC(&D_gamegui_00402C78);
        return;
    }
    gUvFontExports->uvFontGenDList();
}

void func_gamegui_00401AD8(s32 arg0) {
    if (D_debugEnable != 0) {
        D_gamegui_00402C68 = (u8)arg0;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/gamegui/func_gamegui_00401AF8.s")

typedef struct
{
  char pad0[0x1E];
  s16 unk1E;
} Inner_00401D34;
typedef struct
{
  char pad0[0x28];
  Inner_00401D34 *unk28;
} Outer_00401D34;

void func_gamegui_00401D34(Outer_00401D34 *arg0)
{
  Inner_00401D34 *new_var;
  new_var = arg0->unk28;
  if (new_var->unk1E != 1)
  {
    gSndExports->unk34(D_gamegui_00402D16);
    gSndExports->unk38(0);
  }
}

void func_gamegui_00401D94(s32 arg0) {
    gUvCmidiExports->unk2C();
}

void func_gamegui_00401DC4(s32 arg0) {
    gUvEmitterExports->unk80();
}

void func_gamegui_00401DF4(s32 arg0) {
    gUvAudiomgrExports->unk40();
}

void func_gamegui_00401E24(s32 arg0) {
    if (gGameSettings[0].numPlayers >= 2) {
        gGameSettings[0].numAiCars = 0;
    }
    gGameSettings[0].gameStateFlag = 5;
}

void func_gamegui_00401E50(s32 arg0) {
    if (gGameSettings[0].numPlayers < 2) {
        gGameSettings[0].numPlayers = 2;
    }
    gGameSettings[0].gameStateFlag = 6;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/gamegui/func_gamegui_00401E7C.s")

void func_gamegui_00401FE0(void) {
    gUvGuiExports->unk28(&D_gamegui_00402C78, -1, 3);
    gUvDebugExports->unk18(3);
}

void func_gamegui_00402030(s32 arg0) {
    D_gamegui_00402D04 = arg0;
}

void func_gamegui_0040203C(s32 arg0) {
    if (arg0 == D_gamegui_00402D28) {
        *(s32 *)&gGameSettings[0].padE0[0] = 1;
    } else {
        *(s32 *)&gGameSettings[0].padE0[0] = 0;
    }
    gGameSettings[0].gameStateFlag = 9;
}

void func_gamegui_00402074(s32 arg0) {
    gGameStateFlag = 8;
}

void func_gamegui_00402088(s32 arg0) {
    gGameStateFlag = 0xB;
}

void func_gamegui_0040209C(s32 arg0) {
    gGameSettings[0].gameStateFlag = 3;
    gGameSettings[0].unkC = 2;
}

void func_gamegui_004020BC(s32 arg0) {
    gGameSettings[0].gameStateFlag = 4;
    gGameSettings[0].unkC = 2;
}

void func_gamegui_004020DC(s32 arg0) {
    gGameSettings[0].gameStateFlag = 2;
    gGameSettings[0].currentTrack = 0x1B;
    gGameSettings[0].finishedIntroCount = 0;
    gGameSettings[0].unkC = 1;
    gGameSettings[0].pad178[5] = 1;
}

void func_gamegui_00402110(s32 arg0) {
    gGameSettings[0].gameStateFlag = 7;
    gGameSettings[0].currentTrack = 0x21;
}

void func_gamegui_00402130(s32 arg0) {
    gGameStateFlag = 0xA;
}

void func_gamegui_00402144(s32 arg0) {
    gGameStateFlag = 0xC;
}

void func_gamegui_00402158(s32 arg0) {
    gGameStateFlag = 0xD;
}

void func_gamegui_0040216C(s32 arg0) {
    gGameStateFlag = 0xE;
}

void func_gamegui_00402180(s32 arg0) {
    gGameStateFlag = 0xF;
}

void func_gamegui_00402194(void) {
}

void func_gamegui_0040219C(s32 arg0) {
    if (gGameSettings[0].introReplayState != 0) {
        gGameSettings[0].introReplayState = 0;
        return;
    }
    gGameSettings[0].introReplayState = 1;
    gGameSettings[0].gameStateFlag = gGameSettings[0].currentGameState;
}

void func_gamegui_004021D4(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/gamegui/func_gamegui_004021DC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/gamegui/func_gamegui_00402358.s")

u8 func_gamegui_004023B0(void) {
    return D_gamegui_00402C68;
}

void func_gamegui_004023BC(s32 arg0)
{
  u16 new_var2;
  s32 var_s0;
  s32 temp_s3;
  s32 var_s1;
  void *temp_v0;
  var_s1 = 0;
  temp_s3 = uvGetFilesCount(0x55565458);
  var_s0 = 0;
  if (temp_s3 > 0)
  {
    do
    {
      if (uvGetFileInstanceCount(0x55565458, var_s0) > 0)
      {
        temp_v0 = uvGetLoadedFile(0x55565458, var_s0);
        new_var2 = *((u16 *) (((u8 *) temp_v0) + 0x1A));
        var_s1 += new_var2;
      }
      var_s0 += 1;
    }
    while (var_s0 < temp_s3);
  }
}

