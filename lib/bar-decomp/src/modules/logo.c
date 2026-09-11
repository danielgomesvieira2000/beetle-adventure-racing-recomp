// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

typedef struct UnkStruct_80025C88_s {
    s32 pad0;
    void (*unk4)(s32, s32);
} UnkStruct_80025C88;

typedef struct UnkStruct_80025C2C_s {
    /* 0x0 */ char pad0[8];
    /* 0x8 */ f32 (*unk8)(f32);                     /* inferred */
} UnkStruct_80025C2C;

typedef struct UnkStruct_80025C44_s {
    /* 0x00 */ char pad0[0xC];
    /* 0x0C */ void (*unkC)(s32);                        /* inferred */
    /* 0x10 */ void (*unk10)(s32);                       /* inferred */
    /* 0x14 */ char pad14[0x3C];                    /* maybe part of unk10[0x10]? */
    /* 0x50 */ void (*unk50)(void);                        /* inferred */
    /* 0x50 */ void (*unk54)(void);                        /* inferred */
} UnkStruct_80025C44;                               /* size = 0x54 */

typedef struct LogoModule_Exports_s {
    void* func_logo_00400174;
    void* func_logo_0040017C;
    void* func_logo_00400184;
} LogoModule_Exports;

extern UnkStruct_80025C2C* gUvMathExports;
extern UnkStruct_80025C44* gUvGfxStateExports;

extern UnkStruct_80025C08* gUvGfxMgrExports;
extern UnkStruct_80025C14* gUvSprtExports;
extern UnkStruct_80025C74* gGameGuiExports;
extern UnkStruct_80025C88* gScrnExports;

void __entrypoint_func_logo_400000(LogoModule_Exports* arg0) ;
//void uvUpdateFileAllocPtr(void*);
extern void func_logo_00400174(void);
extern void func_logo_0040017C(void);
extern void func_logo_00400184(void);

extern s32 D_logo_004002E0[2]; //padding?
extern s32 D_logo_004002E8[2];
extern s32 D_logo_004002D0[];

void __entrypoint_func_logo_400000(LogoModule_Exports* exports) {
    uvUpdateFileAllocPtr(exports);

    exports->func_logo_00400174 = func_logo_00400174;
    exports->func_logo_0040017C = func_logo_0040017C;
    exports->func_logo_00400184 = func_logo_00400184;

    D_logo_004002E8[0] = gUvSprtExports->unk4();
    D_logo_004002E8[1] = 0;

    uvLoadFile('UVBT', 0x28);

    gUvSprtExports->unk1C(
        D_logo_004002E8[0], 9, 0x28, 2,
        0, 0, 3, 1,
        7, 0, 0, 0,
        0xFF, 0
    );

    gUvGfxMgrExports->unk58(
        0,
        gUvGfxMgrExports->unk88(),
        0,
        gUvGfxMgrExports->unk84()
    );

    gGameGuiExports->unk10(0);
    gGameGuiExports->unk1C(gGameGuiExports->unk4);

    gScrnExports->unk4(0, 0);
}

void func_logo_00400174(void) {

}

void func_logo_0040017C(void) {

}

void func_logo_00400184(void) {
    s32 sp28;
    s32 var_a2;

    if (D_logo_004002E8[1] < 0x5A) {
        var_a2 = (s32) ( gUvMathExports->unk8((++D_logo_004002E8[1] * 1.5707963f) / 90.0f) * (0,255.0f));
    } else {
        var_a2 = 0xFF;
    }
    gUvGfxStateExports->unk50();
    gUvGfxStateExports->unkC(0x04800000);
    gUvGfxStateExports->unk10(0x600000);
    gUvSprtExports->unk1C(D_logo_004002E8[0], 7, var_a2, var_a2, var_a2, 0xFF, 0);
    gUvSprtExports->unk10(D_logo_004002E8[0]);
    gUvGfxStateExports->unk54();
}
