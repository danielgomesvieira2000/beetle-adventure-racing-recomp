// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern void *gCamExports;
extern s32 D_med_00406C00;
extern s32 D_med_00407F28;
extern void *D_med_004080A0;
extern s32 D_med_00407F64;
extern s32 D_med_00407FB0;
extern s32 D_med_00407F68;
extern s32 D_med_00407FC0;
extern s32 D_med_00407FDC;
extern void *gUvTexAnimExports;
void func_med_00400160();
void func_med_004007B4();
void func_med_004008C0();
void func_med_004008F0();
void func_med_00400D18();
void func_med_00401174();
void func_med_004011B4();
void func_med_0040144C(s32 arg0);
void func_med_004014C4();
void func_med_00401C10();
void func_med_00401EF8();
void func_med_004021A0();
void func_med_004023E0();
void func_med_00402920();
void func_med_00402C7C();
void func_med_00404FB8();
void func_med_00404FE0();
void func_med_0040505C();
void func_med_00405128();
void func_med_00405168();
void func_med_004051A8();
void func_med_004052C8();
void func_med_00405338();
void func_med_004053B4();
void func_med_00405488();
void func_med_004057A8();
void func_med_004059E8();
void func_med_00405DC4();
void func_med_00405FEC();
void func_med_00406204();
void func_med_004062BC();
void func_med_004063D4();
void func_med_00406420();
#include "ultra64.h"
/*__SEEDEXTERNS__*/
extern UnkDobjExports* gUvDobjExports;
extern s32 D_med_00406BD8[];
extern s32 D_med_00406BF8[];
extern s32 D_med_00406C08;
void func_med_004008C0(s32);
void func_med_004000DC(void);
typedef struct MedObj_s {
    char pad0[0x248];
    s32 unk248;
} MedObj;
typedef struct D_med_00408028_s {
    f32 unk0;
    f32 unk4;
    f32 unk8;
} D_med_00408028_t;
typedef struct LightExports_s {
    char pad0[0x8];
    void (*unk8)(void *, s32, s32, f64, f64, f64, s32);
} LightExports;
typedef struct SceneExports2_s {
    char pad0[0x2C];
    void *unk2C;
} SceneExports2;
extern MedObj *D_med_00407F08;
extern D_med_00408028_t D_med_00408028;
extern LightExports *gLightExports;
extern SceneExports2 *gSceneExports;
extern f32 D_med_00407FE4;
extern f32 D_med_00407FE8;
extern f32 D_med_00407FEC;
extern f32 D_med_00407FF0;

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/__entrypoint_func_med_400000.s")

void func_med_004000DC(void)
{
  s32 *var_s0;
  s32 temp_a0;
  s32 var_s1;
  var_s1 = 0xFFFF;
 do { var_s0 = D_med_00406BD8; do { temp_a0 = var_s0[2]; if (var_s1 != temp_a0) { gUvDobjExports->unk40(temp_a0); } var_s0++; } while (var_s0 != D_med_00406BF8); } while (0);
  func_med_004008C0((s32) (&D_med_00406C08));
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00400160.s")

void func_med_0040078C(void) {
    func_med_004021A0();
}

void func_med_004007AC(void) {
}

/* Initialize a camera entity node: create cam entity, set up camera params, identity matrix, and initial pose. */
typedef struct {
    /* 0x00 */ char pad0[0x4];
    /* 0x04 */ s32 (*unk4)(void);
    /* 0x08 */ char pad8[0x18];
    /* 0x20 */ void (*unk20)(s32, s32, f64, s32, f64, s32, f64, s32);
} CamExp_4007B4;
typedef struct {
    /* 0x00 */ char pad0[0x1C];
    /* 0x1C */ void (*unk1C)(Mtx4F*);
} UvFMtxExp_4007B4;
extern void *gUvFmtxExports;
extern f32 D_med_00406A94;
extern f32 D_med_00406A98;
extern f32 D_med_00406A9C;
typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ char pad4[0x4];
    /* 0x08 */ Mtx4F unk8;
    /* 0x48 */ f32 unk48;
    /* 0x4C */ f32 unk4C;
    /* 0x50 */ f32 unk50;
    /* 0x54 */ f32 unk54;
    /* 0x58 */ f32 unk58;
    /* 0x5C */ f32 unk5C;
    /* 0x60 */ f32 unk60;
    /* 0x64 */ f32 unk64;
    /* 0x68 */ f32 unk68;
} Node_4007B4;

void func_med_004007B4(Node_4007B4 *arg0)
{
  float new_var;
  s32 temp_v0;
  temp_v0 = ((CamExp_4007B4 *) gCamExports)->unk4();
  new_var = 1.0f;
  arg0->unk0 = temp_v0;
  ((CamExp_4007B4 *) gCamExports)->unk20(temp_v0, 0x9, (f64) D_med_00406A94, 0x6, (f64) new_var, 0x7, (f64) D_med_00406A98, 0);
  arg0->unk48 = 0.0f;
  arg0->unk4C = D_med_00406A9C;
  arg0->unk50 = 10.0f;
  arg0->unk54 = 0.0f;
  arg0->unk58 = 0.0f;
  arg0->unk5C = 0.0f;
  arg0->unk60 = 0.0f;
  arg0->unk64 = 0.0f;
  arg0->unk68 = 0.0f;
  ((UvFMtxExp_4007B4 *) gUvFmtxExports)->unk1C(&arg0->unk8);
  arg0->unk8.m[3][1] = -10.0f;
  arg0->unk8.m[3][2] = 5.0f;
  func_med_00401174(arg0);
}

void func_med_004008C0(s32 arg0) {
    (*(void (**)(s32))((u8*)gCamExports + 0x8))(*(s32*)arg0);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004008F0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00400D18.s")

void func_med_00401174(void) {
    if (D_med_00406C00 == 0) {
        func_med_00400D18();
        return;
    }
    func_med_004008F0();
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004011B4.s")

void func_med_0040144C(s32 arg0) {
    D_med_00407FE4 = (f32) ((arg0 >> 0x18) & 0xFF) / 255.0f;
    D_med_00407FE8 = (f32) ((arg0 >> 0x10) & 0xFF) / 255.0f;
    D_med_00407FEC = (f32) ((arg0 >> 8) & 0xFF) / 255.0f;
    D_med_00407FF0 = (f32) (arg0 & 0xFF) / 255.0f;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004014C4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00401C10.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00401EF8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004021A0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004023E0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00402920.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00402C7C.s")

void func_med_00404FB8(s32 arg0) {
    s32 temp;
    temp = *(s32*)((u8*)&D_med_00407F28 + 0x0);
    if (temp != 0) {
        *(u8*)(temp + 0x23) = *(s32*)((u8*)&D_med_00407F28 + 0x2C);
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00404FE0.s")

typedef struct {
    /* 0x00 */ char pad0[0x28];
    /* 0x28 */ void *unk28;
} MedNode_0040505C;
typedef struct {
    /* 0x00 */ char pad0[0x24];
    /* 0x24 */ f32 unk24;
} MedObj_0040505C;
typedef struct {
    /* 0x00 */ char pad0[0xC];
    /* 0x0C */ s32 (*unkC)(s32, s32, s32, s32);
    /* 0x10 */ char pad10[0x4];
    /* 0x14 */ void (*unk14)(void *, s32);
} MedExp_0040505C;
typedef struct {
    /* 0x00 */ char pad0[0xC4];
    /* 0xC4 */ void (*unkC4)(void *, s32);
} UvGuiExp_0040505C;
extern MedNode_0040505C *D_med_00408048[];
extern void *D_med_00407F70;
extern s32 D_med_00407F60;
extern UvGuiExp_0040505C *gUvGuiExports;

void func_med_0040505C(void *arg0)
{
  void **var_v0;
  s32 var_v1;
  int new_var;
 var_v0 = (void **) D_med_00408048; var_v1 = 0; loop_1: if (arg0 != ((MedNode_0040505C *) (*var_v0))->unk28) {
    var_v1 += 1;
    var_v0 += 1;
    if (var_v1 != 0x10)
    {
      goto loop_1;
    }
  }

  new_var = (var_v1 >> 2) & 1;
 if (1) { }
  if (var_v1 != 0x10)
  {
    gUvGuiExports->unkC4(arg0, ((MedExp_0040505C *) D_med_004080A0)->unkC((var_v1 >> 3) & 1, new_var, var_v1 & 3, (s32) ((MedObj_0040505C *) arg0)->unk24));
    ((MedExp_0040505C *) D_med_004080A0)->unk14(&D_med_00407F70, D_med_00407F60);
  }
}

void func_med_00405128(s32 arg0) {
    (*(void (**)(s32, u8*, s32))((u8*)D_med_004080A0 + 0x10))(D_med_00407F64, &D_med_00407FB0, 0xFA);
}

void func_med_00405168(s32 arg0) {
    (*(void (**)(s32, u8*, s32))((u8*)D_med_004080A0 + 0x10))(D_med_00407F68, &D_med_00407FC0, 0xFB);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004051A8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004052C8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00405338.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004053B4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00405488.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004057A8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004059E8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00405DC4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00405FEC.s")

void func_med_00406204(s32 a0) {
    if (D_med_00407FDC != -1) {
        func_med_004014C4();
    }
}

/* Set a light via gLightExports->unk8, using a scene-object pointer and three f32 RGB/dir values from a global table. */
void func_med_00406238(s32 arg0) {
    if (D_med_00407F08 != NULL) {
        gLightExports->unk8((char *)gSceneExports->unk2C + 0x28, D_med_00407F08->unk248, 1,
                            (f64) D_med_00408028.unk0, (f64) D_med_00408028.unk4,
                            (f64) D_med_00408028.unk8, 0);
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_004062BC.s")

void func_med_004063CC(s16 a0) {
}

void func_med_004063D4(s32 arg0) {
    (*(void (**)(void))((u8*)gUvTexAnimExports + 0x8))();
    (*(void (**)(void))((u8*)gUvTexAnimExports + 0x4))();
}

void func_med_00406418(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/med/func_med_00406420.s")

