// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

#define IMTX_STACK_SIZE 100

// Todo replace these structs with the actual exports of uvcback_rom and uvgfxmgr_rom
typedef struct UnkStruct_00400764_s {
    /* 0x00 */ char pad0[0x10];
    /* 0x10 */ void (*unk10)(s32, void (*)(s32), s32, s32); /* inferred */
    /* 0x14 */ void (*unk14)(s32, s32);
} UnkStruct_00400764;                               /* size = 0x18 */

typedef struct UnkStruct_00400760_s {
    /* 0x00 */ char pad0[8];
    /* 0x08 */ Gfx **(*unk8)(void);                    /* inferred */
    /* 0x0C */ char padC[4];
    /* 0x10 */ s32 (*unk10)(s32);
} UnkStruct_00400760;                               /* size = 0x14 */

extern UnkStruct_00400760 *sUvGfxMgrExports;
extern UnkStruct_00400764 *sUvCbckExports;
extern void *D_uvimtx_rom_00400774;
extern s32 sRspMatricesCount[];
extern s32 sNonLoadedMatricesCount[];
extern s32 sUnusedArray1[];
extern s32 sUnusedArray2[];
extern s16 sIMtxTick;
extern s16 sIMtxStackIdx;
extern s16 sIMtxStackSize;
extern Mtx* sMatrixStack[];

void __entrypoint_func_uvimtx_rom_400000(UvImtx_Rom_Exports *arg0);
void func_uvimtx_rom_0040018C(void);
void uvIMtxStackInit(s32 arg0);
void uvIMtxStackInit(s32 arg0);
void uvIMtxCopy(Mtx *dst, Mtx src);
void uvIMtxSetIdentity(Mtx *m);
void uvImtxStub(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_uvimtx_rom_00400338(uvMtx *dst, uvMtx src);
void func_uvimtx_rom_00400410(Mtx mtx);
void uvGfxMtxViewPop(void);
void func_uvimtx_rom_004004D8(Mtx mtx);
Mtx *uvIMtxPush(Mtx arg0, u16 arg16);
void func_uvimtx_rom_0040062C(Mtx *arg0, u16 arg1);
void func_uvimtx_rom_0040062C(Mtx *arg0, u16 arg1);
s32 func_uvimtx_rom_004006E0(s32 arg0);

void __entrypoint_func_uvimtx_rom_400000(UvImtx_Rom_Exports* arg0) {
    s16* stackSize;

    uvUpdateFileAllocPtr((s32) arg0);
    arg0->func_uvimtx_rom_0040018C = func_uvimtx_rom_0040018C;
    arg0->func_uvimtx_rom_004006E0 = func_uvimtx_rom_004006E0;
    arg0->uvIMtxCopy = uvIMtxCopy;
    arg0->uvIMtxSetIdentity = uvIMtxSetIdentity;
    arg0->uvImtxStub = uvImtxStub;
    arg0->func_uvimtx_rom_00400338 = func_uvimtx_rom_00400338;
    arg0->func_uvimtx_rom_00400410 = func_uvimtx_rom_00400410;
    arg0->uvGfxMtxViewPop = uvGfxMtxViewPop;
    arg0->func_uvimtx_rom_004004D8 = func_uvimtx_rom_004004D8;
    arg0->uvIMtxPush = uvIMtxPush;
    arg0->func_uvimtx_rom_0040062C = func_uvimtx_rom_0040062C;
    stackSize = uvGetSystemProp(8);
    if (stackSize == NULL) {
        sIMtxStackSize = IMTX_STACK_SIZE;
    } else {
        if (*stackSize != 0) {
            sIMtxStackSize = *stackSize;
        } else {
            sIMtxStackSize = IMTX_STACK_SIZE;
        }
    }
    sUvGfxMgrExports = uvLoadModule('GMGR');
    sUvCbckExports = uvLoadModule('CBCK');

    // Create two 4x4 dynamic matrices
    *sMatrixStack = _uvMemAlloc(sIMtxStackSize * sizeof(Mtx), 4 * 4);
    D_uvimtx_rom_00400774 = _uvMemAlloc(sIMtxStackSize * sizeof(Mtx), 4 * 4);
    sUvCbckExports->unk10(sUvGfxMgrExports->unk10(1), uvIMtxStackInit, 0, 0);
}


void func_uvimtx_rom_0040018C(void) {
    _uvMemFree(*sMatrixStack);
    _uvMemFree(D_uvimtx_rom_00400774);
    sUvCbckExports->unk14(sUvGfxMgrExports->unk10(1), &uvIMtxStackInit);
    uvUnloadModule('GMGR');
    uvUnloadModule('CBCK');
}

void uvIMtxStackInit(s32 arg0) {
    sIMtxTick ^= 1;
    sRspMatricesCount[sIMtxTick] = 0;
    sNonLoadedMatricesCount[sIMtxTick] = 0;
    sIMtxStackIdx = 0;
}

void uvIMtxCopy(Mtx* dst, Mtx src) {
    dst->m[0][0] = src.m[0][0];
    dst->m[0][1] = src.m[0][1];
    dst->m[0][2] = src.m[0][2];
    dst->m[0][3] = src.m[0][3];
    dst->m[1][0] = src.m[1][0];
    dst->m[1][1] = src.m[1][1];
    dst->m[1][2] = src.m[1][2];
    dst->m[1][3] = src.m[1][3];
    dst->m[2][0] = src.m[2][0];
    dst->m[2][1] = src.m[2][1];
    dst->m[2][2] = src.m[2][2];
    dst->m[2][3] = src.m[2][3];
    dst->m[3][0] = src.m[3][0];
    dst->m[3][1] = src.m[3][1];
    dst->m[3][2] = src.m[3][2];
    dst->m[3][3] = src.m[3][3];
}

void uvIMtxSetIdentity(Mtx* m) {
    m->m[0][0] = 0x10000;
    m->m[0][1] = 0;
    m->m[0][2] = 1;
    m->m[0][3] = 0;
    m->m[1][1] = 0x10000;
    m->m[1][0] = 0;
    m->m[1][3] = 1;
    m->m[1][2] = 0;
    m->m[2][0] = 0;
    m->m[2][1] = 0;
    m->m[2][2] = 0;
    m->m[2][3] = 0;
    m->m[3][0] = 0;
    m->m[3][1] = 0;
    m->m[3][2] = 0;
    m->m[3][3] = 0;
}

void uvImtxStub(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {

}

void func_uvimtx_rom_00400338(uvMtx* dst, uvMtx src) {
    uvMtx* srcPtr = &src;
    uvMtx* dstPtr = dst;
    Mtx stack;

    if (dst == srcPtr) {
        dstPtr = (uvMtx*)&stack;
    }

    dstPtr->u.i[0][1] = src.u.i[1][0];
    dstPtr->u.i[0][2] = src.u.i[2][0];
    dstPtr->u.i[1][0] = src.u.i[0][1];
    dstPtr->u.i[1][2] = src.u.i[2][1];
    dstPtr->u.i[2][0] = src.u.i[0][2];
    dstPtr->u.i[2][1] = src.u.i[1][2];
    dstPtr->u.f[0][1] = src.u.f[1][0];
    dstPtr->u.f[0][2] = src.u.f[2][0];
    dstPtr->u.f[1][0] = src.u.f[0][1];
    dstPtr->u.f[1][2] = src.u.f[2][1];
    dstPtr->u.f[2][0] = src.u.f[0][2];
    dstPtr->u.f[2][1] = src.u.f[1][2];

    if (srcPtr == dstPtr) {
        uvIMtxCopy((Mtx*)dst, stack);
    }
}

void func_uvimtx_rom_00400410(Mtx mtx) {
    uvIMtxPush(mtx, G_MTX_LOAD);
}

void uvGfxMtxViewPop(void) {
    Gfx** gdlh;

    gdlh = sUvGfxMgrExports->unk8();

    gSPPopMatrix((*gdlh)++, G_MTX_MODELVIEW);
}

void func_uvimtx_rom_004004D8(Mtx mtx) {
    uvIMtxPush(mtx, G_MTX_PROJECTION | G_MTX_LOAD);
}

Mtx* uvIMtxPush(Mtx mtx, u16 params) {
    Mtx* stackMatrix;
    // Stack is full
    if (sIMtxStackIdx >= sIMtxStackSize) {
        return NULL;
    }

    stackMatrix = &sMatrixStack[sIMtxTick][sIMtxStackIdx];
    uvIMtxCopy(stackMatrix, mtx);
    sIMtxStackIdx++;
    func_uvimtx_rom_0040062C(stackMatrix, params);
    return stackMatrix;
}

void func_uvimtx_rom_0040062C(Mtx* mtx, u16 params) {
    Gfx** gdlh;

    gdlh = sUvGfxMgrExports->unk8();
    gSPMatrix(gdlh[0]++, VIRTUAL_TO_PHYSICAL2(mtx), params);
    if (!(params & G_MTX_LOAD)) {
        sNonLoadedMatricesCount[sIMtxTick]++;
    }
    sRspMatricesCount[sIMtxTick]++;
}

s32 func_uvimtx_rom_004006E0(s32 arg0) {
    switch (arg0) {
    case 0:
        return sUnusedArray1[0 - sIMtxTick];
    case 1:
        return sUnusedArray2[0 - sIMtxTick];
    default:
        return 0;
    }
}
