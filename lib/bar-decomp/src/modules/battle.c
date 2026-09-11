/*__SEEDEXTERNS__*/
typedef struct {
    /* 0x00 */ float unk0;
    /* 0x04 */ float unk4;
    /* 0x08 */ int unk8;
} BattleViewEntry_004090F0; /* size = 0xC */

typedef struct {
    /* 0x00 */ char pad[0x3C];
    /* 0x3C */ void (*unk3C)(float, float, int, int);
} UvGfxMgrExp_004090F0;
extern BattleViewEntry_004090F0 D_battle_00409D28[];
extern UvGfxMgrExp_004090F0 *gUvGfxMgrExports;
typedef struct {
    /* 0x00 */ int unk0;
    /* 0x04 */ int unk4;
    /* 0x08 */ char pad8[0xC];
    /* 0x14 */ void *unk14;
    /* 0x18 */ char pad18[0x10];
    /* 0x28 */ int unk28;
} Node_00407F30;
typedef struct { char pad0[0x14]; void (*unk14)(void *, void *); char pad18[0x18]; void (*unk30)(void *, int); } UvDobjExp_00407F30;
typedef struct { char pad0[0xCC]; void (*unkCC)(void *); } SndExp_00407F30;
extern void *D_battle_00409EF8[];
typedef struct { char pad[0xFC]; void (*unkFC)(void *, int, int); } MiscExp_00401A68;
extern MiscExp_00401A68 *gMiscExports;
extern int D_battle_00409AB0;
extern int D_battle_00409AB4;
extern int D_battle_00409AC0;
extern int D_battle_00409AC8;
extern int D_battle_00409AD0;
extern int D_battle_0040A058;
extern int D_battle_0040A05C;
extern int D_battle_0040A060;
extern int D_battle_0040A064;
typedef struct { char pad[0x38]; void (*unk38)(int, int); } UvDobjExp_407E78;
typedef struct { char pad[0x2C]; void (*unk2C)(int); } UvPfxExp_407E78;
typedef struct { char pad[0xCC]; void (*unkCC)(void *); } SndExp_407E78;
extern UvDobjExp_407E78 *gUvDobjExports;
extern UvPfxExp_407E78 *gUvPfxExports;
extern SndExp_407E78 *gSndExports;
typedef struct {
    /* 0x00 */ char pad0[0x4];
    /* 0x04 */ int unk4;
    /* 0x08 */ char pad8[0xC];
    /* 0x14 */ int unk14;
    /* 0x18 */ char pad18[0x10];
    /* 0x28 */ int unk28;
    /* 0x2C */ char unk2C[0x8];
    /* 0x34 */ char unk34[0x8];
} BattleObj_407E78;
typedef struct BattleObj_00405034_s {
    void *unk0;
    char pad4[0x5A0];
    float unk5A4;
    int   unk5A8;
} BattleObj_00405034;
typedef struct BattleSubObj_00405034_s {
    char pad0[0xC];
    void *unkC;
} BattleSubObj_00405034;
typedef struct CamTarget_00405034_s {
    char pad0[0x1C];
    float unk1C;
} CamTarget_00405034;
typedef struct CamExports_00405034_s {
    char pad0[0x20];
    void (*unk20)(void *, int, int, int, double, int);
} CamExports_00405034;
typedef struct {
    char pad[0x20];
    void (*unk20)(void *, int, int, int, double, int);
} CamExp_404F9C;
extern CamExp_404F9C *gCamExports;
typedef struct {
    char pad0[0xC];
    void *unkC;
} SubObj_404F9C;
typedef struct {
    char pad0[0x1C];
    float unk1C;
} CamTarget_404F9C;
typedef struct {
    SubObj_404F9C *unk0;
    char pad4[0x5A8];
    float unk5AC;
    int   unk5B0;
} Obj_404F9C;
extern float D_battle_00409C38;
typedef struct {
    char pad21C[0x21C];
    /* 0x21C */ int unk21C;
    char pad220[0x8];
    /* 0x228 */ float unk228;
    /* 0x22C */ float unk22C;
    /* 0x230 */ float unk230;
    /* 0x234 */ float unk234;
    /* 0x238 */ float unk238;
} Inner_404F18;
typedef struct {
    char pad0[0xC];
    /* 0x00C */ Inner_404F18 *unkC;
} Mid_404F18;
typedef struct {
    /* 0x000 */ Mid_404F18 *unk0;
    char pad4[0x59C];
    /* 0x5A0 */ float unk5A0;
} Outer_404F18;
typedef struct { char pad0[0xC]; void (*unkC)(void); } Exports_00401B34;
extern void func_80000CA4(int);
extern void func_80000D58(int);
extern void func_battle_00401BB8(void);
extern short D_80025D76;
extern Exports_00401B34 *D_battle_0040A044;
extern int D_battle_0040A068;
extern int D_debugEnable;
extern int gDebugFrateDisp;
typedef struct {
    /* 0x000 */ int unk0;
    /* 0x004 */ unsigned char pad4[0x5B8 - 0x4];
} Node_400D38; /* size = 0x5B8 */

extern void func_battle_00405ADC(Node_400D38 *, int);
extern Node_400D38 D_battle_0040A080[];
extern int D_battle_0040C2B4;
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/__entrypoint_func_battle_400000.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_004006A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00400788.s")

/* Iterate D_battle_0040A080 array; call func_battle_00405ADC on each entry whose first field matches arg0. */
void func_battle_00400D38(int arg0) {
    Node_400D38 *var_s0;
    int var_s1;

    var_s1 = 0;
    if (D_battle_0040C2B4 > 0) {
        var_s0 = &D_battle_0040A080[0];
        do {
            if (arg0 == var_s0->unk0) {
                func_battle_00405ADC(var_s0, 0);
            }
            var_s1 += 1;
            var_s0++;
        } while (var_s1 < D_battle_0040C2B4);
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00400DB0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00401318.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_004013D8.s")

/* Register a set of battle HUD elements with the misc exports table. */
void func_battle_00401A68(void) {
    gMiscExports->unkFC(&D_battle_00409AB0, D_battle_0040A058, 0xB4);
    gMiscExports->unkFC(&D_battle_00409AB4, D_battle_0040A05C, 0xAA);
    gMiscExports->unkFC(&D_battle_00409AC0, D_battle_0040A060, 0x96);
    gMiscExports->unkFC(&D_battle_00409AC8, D_battle_0040A064, 0xA0);
    gMiscExports->unkFC(&D_battle_00409AD0, D_battle_0040A068, 0x96);
}

/* battle module update: conditionally call vtable unkC, render frame, display debug frame rate */
void func_battle_00401B34(void) {
    if (D_80025D76 != 0) {
        D_battle_0040A044->unkC();
    }
    func_80000CA4(D_battle_0040A068);
    func_battle_00401BB8();
    func_80000D58(D_battle_0040A068);
    if ((D_debugEnable != 0) && (gDebugFrateDisp == 2)) {
        func_battle_00401A68();
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00401BB8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00401DB0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00402750.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_0040282C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00402CD0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00402F6C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_0040372C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00403A54.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00403D08.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00404074.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_0040425C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00404438.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00404580.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00404694.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_004049C8.s")

/* Sets battle object color/alpha fields to 1.0f and enables it when current value >= 0. */
void func_battle_00404F18(Outer_404F18 *arg0, float arg1) {
    if (arg0->unk5A0 >= 0.0f) {
        arg0->unk5A0 = arg1;
        arg0->unk0->unkC->unk234 = D_battle_00409C38;
        arg0->unk0->unkC->unk238 = 1.0f;
        arg0->unk0->unkC->unk228 = 1.0f;
        arg0->unk0->unkC->unk22C = 1.0f;
        arg0->unk0->unkC->unk230 = 1.0f;
        arg0->unk0->unkC->unk21C = 1;
    }
}

/* Inferred camera export table; only unk20 is used here */
/* Sub-object reached via arg0->unk0; offset 0xC holds a camera-target pointer */
/* Camera target; offset 0x1C is a float used as the pitch/angle input */
/* Primary object passed as arg0 */
/* Arms a one-shot camera call the first time arg0->unk5AC drops below zero */
void func_battle_00404F9C(Obj_404F9C *arg0, float arg1) {
    void *sp34;

    sp34 = arg0->unk0->unkC;
    if (arg0->unk5AC >= 0.0f) {
        arg0->unk5AC = arg1;
        if (arg0->unk5B0 == 0) {
            arg0->unk5B0 = 1;
            gCamExports->unk20(sp34, 0xA, 0, 9, (double)(-(((CamTarget_404F9C *)sp34)->unk1C)), 0);
        }
    }
}

/* notify camera on first non-negative trigger; scale target radius by 2.5 for camera shake distance */
void func_battle_00405034(BattleObj_00405034 *arg0, float arg1) {
    void *sp34;

    sp34 = ((BattleSubObj_00405034 *)arg0->unk0)->unkC;
    if (arg0->unk5A4 >= 0.0f) {
        arg0->unk5A4 = arg1;
        if (arg0->unk5A8 == 0) {
            arg0->unk5A8 = 1;
            ((CamExports_00405034 *)gCamExports)->unk20(sp34, 0xA, 0, 9, (double)(((CamTarget_00405034 *)sp34)->unk1C * 2.5f), 0);
        }
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_004050D4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00405538.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00405764.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00405A1C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00405ADC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00405C3C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00405D34.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00405E68.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00406170.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00406344.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_0040652C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00406784.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00406804.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00406900.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00406954.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00406AD0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00406C84.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00406E98.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00407254.s")

/* Initialize a battle object: clear fields, copy float from module data, increment counter, then attach dobj twice (add + update). */
typedef struct {
    /* 0x0000 */ char pad0[0x1D6C];
    /* 0x1D6C */ int  unk1D6C;
    /* 0x1D70 */ float unk1D70;
} BattleData_40733C;
typedef struct {
    /* 0x00 */ char pad0[0x38];
    /* 0x38 */ void (*unk38)(void *, int, void *);
} UvDobjExp_40733C;
typedef struct {
    /* 0x00 */ char  pad0[0x4];
    /* 0x04 */ int   unk4;
    /* 0x08 */ int   unk8;
    /* 0x0C */ float unkC;
    /* 0x10 */ char  pad10[4];
    /* 0x14 */ void *unk14;
} BattleNode_40733C;

void func_battle_0040733C(BattleNode_40733C *arg0)
{
  arg0->unk8 = 0;
  arg0->unkC = ((BattleData_40733C *) (&D_battle_0040A080))->unk1D70;
  arg0->unk4 = 0;
  ((BattleData_40733C *) (&D_battle_0040A080))->unk1D6C = ((BattleData_40733C *) (&D_battle_0040A080))->unk1D6C + 1;
  ((UvDobjExp_40733C *) gUvDobjExports)->unk38(arg0->unk14, 2, arg0);
  ((UvDobjExp_40733C *) gUvDobjExports)->unk38(arg0->unk14, 1, arg0);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_004073BC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00407740.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_004078EC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00407A68.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00407C58.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00407DC4.s")

/* Teardown helper: hide dobj, kill pfx handle, stop two sound channels on a battle object. */
void func_battle_00407E78(BattleObj_407E78 *arg0) {
    int temp_a0;

    arg0->unk4 = 0;
    ((UvDobjExp_407E78 *)gUvDobjExports)->unk38(arg0->unk14, 2);
    ((UvDobjExp_407E78 *)gUvDobjExports)->unk38(arg0->unk14, 1);
    temp_a0 = arg0->unk28;
    if (temp_a0 >= 0) {
        gUvPfxExports->unk2C(temp_a0);
        arg0->unk28 = -1;
    }
    gSndExports->unkCC(arg0->unk2C);
    gSndExports->unkCC(arg0->unk34);
}

/* Initializes a battle entity: marks active, sets dobj anim, clears timer, starts sound loops */
void func_battle_00407F30(Node_00407F30 *arg0) {
    arg0->unk4 = 1;
    ((UvDobjExp_00407F30 *)gUvDobjExports)->unk14(arg0->unk14, D_battle_00409EF8[arg0->unk0]);
    arg0->unk28 = -1;
    ((UvDobjExp_00407F30 *)gUvDobjExports)->unk30(arg0->unk14, 2);
    ((UvDobjExp_00407F30 *)gUvDobjExports)->unk30(arg0->unk14, 1);
    ((SndExp_00407F30 *)gSndExports)->unkCC((void *)((char *)arg0 + 0x2C));
    ((SndExp_00407F30 *)gSndExports)->unkCC((void *)((char *)arg0 + 0x34));
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00407FF4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_0040807C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_004081D8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00408454.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_004085F8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00408858.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00408B70.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00408DC8.s")

/* Look up a 12-byte record by index (*arg0) from D_battle_00409D28 and call the gfx setter at export offset 0x3C. */
void func_battle_004090F0(int *arg0) {
    BattleViewEntry_004090F0 *temp_v0;

    temp_v0 = &D_battle_00409D28[*arg0];
    gUvGfxMgrExports->unk3C(temp_v0->unk0, temp_v0->unk4, temp_v0->unk8, 0x3F000000);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00409144.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00409314.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00409410.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_0040969C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00409838.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/battle/func_battle_00409910.s")

