// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "macros.h"
#include "module.h"
#include "global_exports.h"

typedef struct Ripple_s {
    Mtx4F mtx;
    f32 curSize;
    f32 fade;
    f32 initSize;
    s32 active;
} Ripple; // size = 0x50

// uvgfxstate_rom exports
typedef struct UnkStruct_80025C44_s {
    char pad[0xC];
    void (*unkC)(s32);
    s32 pad10;
    void (*unk14)(s32);
    char pad18[0x38];
    void (*unk50)(void);
    void (*unk54)(void);
} UnkStruct_80025C44;

// uvcback_rom exports
typedef struct UnkCbckExports_s {
    char pad[0x10];
    void (*unk10)(s32, void *, s32, s32);
} UnkCbckExports;

// uvchan_rom exports
typedef struct UnkChanExports_s {
    char pad[0x4];
    void (*unk4)(s32, s32, void *, s32);
} UnkChanExports;

extern UnkCbckExports *gUvCbckExports;
extern UnkChanExports *gUvChanExports;
extern UnkStruct_80025C44 *gUvGfxStateExports;

void __entrypoint_func_ripple_400604(Ripple_Exports *exports);
void drawRipple(Ripple *ripple);
void drawRipples(s32 arg0);
void func_ripple_004002D8(void);
void addRipple(Mtx4F *mtx, f32 initSize);
void func_ripple_004005A0(s32 arg0);
s32 getActiveRippleCount(void);
void func_ripple_004006B8(void);
void resetRipple(void);

// .bss
extern Ripple sRipples[48];
extern s32 sActiveRippleCount;

void drawRipple(Ripple *ripple) {
    s32 nSize;
    s32 pSize;
    Mtx4F mtx;
    s32 alpha;

    alpha = (s32) (ripple->fade * 255.0f);
    pSize = (s32) (ripple->initSize * ripple->curSize * 100.0f);
    gUvFmtxExports->func_00400370(&mtx, &ripple->mtx);
    gUvFmtxExports->func_004027CC(&mtx, 1U);
    gUvDGeomExports->uvVtxBeginPoly();
    nSize = -pSize;
    gUvDGeomExports->uvVtx(nSize, nSize, 0, -1024, -1024, 255, 255, 255, alpha);
    gUvDGeomExports->uvVtx(pSize, nSize, 0, 1024, -1024, 255, 255, 255, alpha);
    gUvDGeomExports->uvVtx(pSize, pSize, 0, 1024, 1024, 255, 255, 255, alpha);
    gUvDGeomExports->uvVtx(nSize, pSize, 0, -1024, 1024, 255, 255, 255, alpha);
    gUvDGeomExports->uvVtxEndPoly();
    gUvFmtxExports->uvGfxMtxViewPop();
}

void drawRipples(s32 arg0) {
    s32 i;
    s32 j;
    for (i = 0; i < ARRAY_COUNT(sRipples); i++) {
        if (sRipples[i].active) {
            break;
        }
    }

    if (i == ARRAY_COUNT(sRipples)) {
        return;
    }

    gUvGfxStateExports->unk50();          // uvGfxStatePush
    gUvGfxStateExports->unk14(0x7D);      // uvGfxStateBindTexture
    gUvGfxStateExports->unkC(0x04E00000); // uvGfxStateSetFlags
    for (; i < ARRAY_COUNT(sRipples); i++) {
        if (sRipples[i].active) {
            drawRipple(&sRipples[i]);
        }
    }
    gUvGfxStateExports->unk54(); // uvGfxStatePop
}

void func_ripple_004002D8(void) {
    s32 i;
    Ripple *ripple;

    for (i = 0; i < ARRAY_COUNT(sRipples); i++) {
        ripple = &sRipples[i];
        if (ripple->active) {
            ripple->fade -= 0.02f;
            ripple->curSize += 0.01f;
            if ((ripple->fade < 0.0) || (ripple->curSize > 1.0)) {
                ripple->active = FALSE;
                sActiveRippleCount -= 1;
            }
        }
    }
}

void addRipple(Mtx4F *mtx, f32 initSize) {
    s32 i;
    Ripple *ripple;

    for (i = 0; i < ARRAY_COUNT(sRipples); i++) {
        if (!sRipples[i].active) {
            break;
        }
    }

    if (i == ARRAY_COUNT(sRipples)) {
        return;
    }

    ripple = &sRipples[i];
    gUvFmtxExports->func_00400504(&ripple->mtx, mtx);
    gUvFmtxExports->func_004014D0(&ripple->mtx, 0.01f, 0.01f, 0.01f);
    ripple->initSize = initSize;
    ripple->curSize = 0;
    ripple->fade = 1;
    ripple->active = TRUE;
    sActiveRippleCount++;
}

void func_ripple_004005A0(s32 arg0) {
    s32 sp1C;

    gUvChanExports->unk4(arg0, 6, &sp1C, 0);
    if (sp1C != 0) {
        gUvCbckExports->unk10(sp1C, drawRipples, 0, 0xD2);
    }
}

void __entrypoint_func_ripple_400604(Ripple_Exports *exports) {
    int i;
    uvUpdateFileAllocPtr((s32) exports);
    exports->resetRipple = resetRipple;
    exports->func_ripple_004006B8 = func_ripple_004006B8;
    exports->func_ripple_004002D8 = func_ripple_004002D8;
    exports->addRipple = addRipple;
    exports->func_ripple_004005A0 = func_ripple_004005A0;
    exports->getActiveRippleCount = getActiveRippleCount;
#line 155
    uvLoadFile('UVTX', 125);
    // clang-format off
    for (i = 0; i < ARRAY_COUNT(sRipples); i++) { sRipples[i].active = FALSE; }
    // clang-format on
    sActiveRippleCount = 0;
}

s32 getActiveRippleCount(void) {
    return sActiveRippleCount;
}

void func_ripple_004006B8(void) {
    uvUnloadFile('UVTX', 125);
}

void resetRipple(void) {
    int i;
    // clang-format off
    for (i = 0; i < ARRAY_COUNT(sRipples); i++) { sRipples[i].active = FALSE; }
    // clang-format on
    sActiveRippleCount = 0;
}
