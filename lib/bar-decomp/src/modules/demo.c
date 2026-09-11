// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
typedef struct {
    /* 0x00 */ char pad[0xC];
    /* 0x0C */ void (*unkC)(f32, f32, f32, f32);
} UvGeomExp_00400910;
extern UvGeomExp_00400910 *gUvGeomExports;
typedef struct {
    /* 0x00 */ char pad00[0xC];
    /* 0x0C */ void (*unkC)(s32);
    /* 0x10 */ void (*unk10)(s32);
    /* 0x14 */ void (*unk14)(s32);
    /* 0x18 */ char pad18[0x38];
    /* 0x50 */ void (*unk50)(void);
    /* 0x54 */ void (*unk54)(void);
    /* 0x58 */ char pad58[0x4];
    /* 0x5C */ void (*unk5C)(s32, s32, s32, s32, f32, f32, f32, f32);
} UvGfxStateExp_00400910;
extern UvGfxStateExp_00400910 *gUvGfxStateExports;
extern s32 D_demo_00402270;
extern s32 gNumAiCars;
extern void *gSceneExports;
extern void *D_demo_00402274;
void func_demo_00400244();
void func_demo_00400290();
void func_demo_0040041C();
void func_demo_00400538();
void func_demo_004009F8();
void func_demo_00400BC4();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/demo/__entrypoint_func_demo_400000.s")

void func_demo_00400244(void) {
    gNumAiCars = D_demo_00402270;
    gGameSettings->currentTrack = ((s32 (*)(s32))((s32 *)gSceneExports)[1])(0);
    gGameSettings->unk180 = 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/demo/func_demo_00400290.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/demo/func_demo_0040041C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/demo/func_demo_00400538.s")

/* Set up the graphics state and draw a full-screen demo overlay rectangle. */
void func_demo_00400910(f32 arg0) {
    gUvGfxStateExports->unk50();
    gUvGfxStateExports->unkC(0x04820FFF);
    gUvGfxStateExports->unk10(0x9B7C0000);
    gUvGeomExports->unkC(0.0f, 0.0f, 0.0f, arg0);
    gUvGfxStateExports->unk14(0xFFF);
    gUvGfxStateExports->unk5C(0x16, 0x129, 0x10, 0xDF, 0.0f, 2.0f, 0.0f, 2.0f);
    gUvGfxStateExports->unk54();
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/demo/func_demo_004009F8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/demo/func_demo_00400BC4.s")

