// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

#define VERTEX_COUNT_DEFAULT 100

// uvcback_rom exports
typedef struct UnkStruct_uvdgeom_rom_004007AC_s {
    char pad[0x10];
    void (*unk10)(s32, void *, s32, s32);
} UnkStruct_uvdgeom_rom_004007AC;

// uvgeom_rom exports
typedef struct UnkStruct_uvdgeom_rom_004007B0_s {
    /* 0x00 */ s32 pad0;
    /* 0x04 */ void (*unk4)(s32, void *, void *, Gfx **);
    /* 0x08 */ void (*unk8)(s32, s32, s32, s32, s32, Vtx *, s32 *, Gfx **);
    /* 0x0C */ char padC[8];                             /* maybe part of unk8[3]? */
    /* 0x14 */ void (*unk14)(s32, Vtx *, s32 *, Gfx **); /* inferred */
} UnkStruct_uvdgeom_rom_004007B0;                        /* size = 0x18 */

extern void *D_uvdgeom_rom_00400784;
extern UvGfxMgr_Exports *sGfxMgrExports;
extern Vtx *sVertexArray; // Dynamic array of vertices
extern s16 sVertexCount;
extern s16 D_uvdgeom_rom_00400796;
extern s16 sVertexToggle;
extern s32 sMaxVertexCount;
extern s16 D_uvdgeom_rom_00400798;
extern s16 sFirstPoly;
extern s16 sFirstGrid;
extern s16 sFirstTmesh;
extern s16 sFirstTmesh;
extern UnkStruct_uvdgeom_rom_004007B0 *D_uvdgeom_rom_004007B0;

void func_uvdgeom_rom_0040023C(void);
s16 uvVtx(s32 x, s32 y, s32 z, s32 s, s32 t, s32 r, s32 g, s32 b, s32 a);
void uvVtxFlip(u8 flip);
void uvFlipVtx(s32 arg0);
void func_uvdgeom_rom_00400424(s32 arg0, s32 arg1);
void uvVtxBeginPoly(void);
void uvBeginGrid(void);
void uvBeginTmesh(void);
void func_uvdgeom_rom_00400478(void);
void func_uvdgeom_rom_00400500(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void uvVtxEndPoly(void);
s16 uvVtxDup(s32 x, s32 y, s32 z, s32 s, s32 t, s32 r, s32 g, s32 b, s32 a);
void __entrypoint_func_uvdgeom_rom_400000(UvDGeom_Rom_Exports *exports);

extern f32 D_uvdgeom_rom_0040079C;
extern f32 D_uvdgeom_rom_004007A0;
extern s8 D_uvdgeom_rom_004007A4;
extern UnkStruct_uvdgeom_rom_004007AC *D_uvdgeom_rom_004007AC;
extern void *sVertexDataPtrs[2];

void __entrypoint_func_uvdgeom_rom_400000(UvDGeom_Rom_Exports *exports) {
    UvGfxMgr_Exports *temp_v0_2;
    s32 vertexCount;
    s32 *vertexCountPtr;

    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvdgeom_rom_0040023C = func_uvdgeom_rom_0040023C;
    exports->uvVtx = uvVtx;
    exports->func_uvdgeom_rom_00400424 = func_uvdgeom_rom_00400424;
    exports->uvVtxBeginPoly = uvVtxBeginPoly;
    exports->uvBeginGrid = uvBeginGrid;
    exports->uvBeginTmesh = uvBeginTmesh;
    exports->func_uvdgeom_rom_00400478 = func_uvdgeom_rom_00400478;
    exports->func_uvdgeom_rom_00400500 = func_uvdgeom_rom_00400500;
    exports->uvVtxEndPoly = uvVtxEndPoly;
    exports->uvVtxDup = uvVtxDup;
    vertexCountPtr = uvGetSystemProp(SYSTEM_PROPID_VERTEX_COUNT);
    if (vertexCountPtr == NULL) {
        sMaxVertexCount = VERTEX_COUNT_DEFAULT;
    } else {
        vertexCount = *vertexCountPtr;
        if (vertexCount != 0) {
            sMaxVertexCount = vertexCount;
        } else {
            sMaxVertexCount = VERTEX_COUNT_DEFAULT;
        }
    }
    sVertexDataPtrs[0] = _uvMemAlloc(sMaxVertexCount * sizeof(Vtx), 0x10);
    sVertexDataPtrs[1] = _uvMemAlloc(sMaxVertexCount * sizeof(Vtx), 0x10);

    uvMemSet(sVertexDataPtrs[0], 0U, sMaxVertexCount * sizeof(Vtx));
    uvMemSet(sVertexDataPtrs[1], 0U, sMaxVertexCount * sizeof(Vtx));
    sVertexToggle = 0;
    sVertexArray = sVertexDataPtrs[sVertexToggle];
    sVertexCount = 0;
    D_uvdgeom_rom_00400798 = -1;
    D_uvdgeom_rom_00400796 = D_uvdgeom_rom_00400798;
    D_uvdgeom_rom_0040079C = 1.0f;
    D_uvdgeom_rom_004007A0 = 1.0f;
    D_uvdgeom_rom_004007A4 = 0;
    D_uvdgeom_rom_004007B0 = uvLoadModule('GEOM');
    D_uvdgeom_rom_004007AC = uvLoadModule('CBCK');
    sGfxMgrExports = uvLoadModule('GMGR');
    D_uvdgeom_rom_004007AC->unk10(sGfxMgrExports->func_uvgfxmgr_rom_00400AB8(1), uvFlipVtx, 0, 0);
}

void func_uvdgeom_rom_0040023C(void) {
    _uvMemFree(sVertexDataPtrs[0]);
    _uvMemFree(sVertexDataPtrs[1]);
    uvUnloadModule('GEOM');
    uvUnloadModule('CBCK');
    uvUnloadModule('GMGR');
}

s16 uvVtx(s32 x, s32 y, s32 z, s32 s, s32 t, s32 r, s32 g, s32 b, s32 a) {
    if (sMaxVertexCount < sVertexCount) {
        return sVertexCount;
    }
    sVertexArray[sVertexCount].v.ob[0] = x;
    sVertexArray[sVertexCount].v.ob[1] = y;
    sVertexArray[sVertexCount].v.ob[2] = z;
    sVertexArray[sVertexCount].v.tc[0] = s;
    sVertexArray[sVertexCount].v.tc[1] = t;
    sVertexArray[sVertexCount].v.cn[0] = r;
    sVertexArray[sVertexCount].v.cn[1] = g;
    sVertexArray[sVertexCount].v.cn[2] = b;
    sVertexArray[sVertexCount].v.cn[3] = a;
    sVertexCount++;
    D_uvdgeom_rom_00400796 = -1;
    return sVertexCount - 1;
}

void uvVtxFlip(u8 flip) {
    sVertexCount = 0;
    if (flip) {
        sVertexToggle ^= 1;
        sVertexArray = (&sVertexDataPtrs[0])[sVertexToggle];
    }
}

void uvFlipVtx(s32 arg0) {
    uvVtxFlip(TRUE);
}

void func_uvdgeom_rom_00400424(s32 arg0, s32 arg1) {
    D_uvdgeom_rom_00400796 = arg0;
    D_uvdgeom_rom_00400798 = arg0 + arg1;
}

void uvVtxBeginPoly(void) {
    sFirstPoly = sVertexCount;
}

void uvBeginGrid(void) {
    sFirstGrid = sVertexCount;
}

void uvBeginTmesh(void) {
    sFirstTmesh = sVertexCount;
}

void func_uvdgeom_rom_00400478(void) {
    s32 sp28[6];
    Gfx **gdl;
    s16 vtxCount;

    gdl = sGfxMgrExports->uvGetDisplayListHead();
    vtxCount = sVertexCount;
    if (D_uvdgeom_rom_00400796 != -1) {
        sFirstTmesh = D_uvdgeom_rom_00400796;
        vtxCount = D_uvdgeom_rom_00400798;
    }
    D_uvdgeom_rom_004007B0->unk4(vtxCount - sFirstTmesh,
                                 &sVertexArray[sFirstTmesh], sp28, gdl);
}

void func_uvdgeom_rom_00400500(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 sp38[7];
    Gfx **gdl;
    s16 vtxCount;

    gdl = sGfxMgrExports->uvGetDisplayListHead();
    vtxCount = sVertexCount;
    if (D_uvdgeom_rom_00400796 != -1) {
        sFirstGrid = D_uvdgeom_rom_00400796;
        vtxCount = D_uvdgeom_rom_00400798;
    }
    D_uvdgeom_rom_004007B0->unk8(arg0, arg1, arg2, arg3, vtxCount - sFirstGrid,
                                 &sVertexArray[sFirstGrid], sp38 + 1, gdl);
}

void uvVtxEndPoly(void) {
    s32 sp28[7];
    Gfx **gdl;
    s16 vtxCount;

    gdl = sGfxMgrExports->uvGetDisplayListHead();
    vtxCount = sVertexCount;
    if (D_uvdgeom_rom_00400796 != -1) {
        sFirstPoly = D_uvdgeom_rom_00400796;
        vtxCount = D_uvdgeom_rom_00400798;
    }
    D_uvdgeom_rom_004007B0->unk14(vtxCount - sFirstPoly,
                                  &sVertexArray[sFirstPoly], sp28 + 1, gdl);
}

// uvVtx duplicate?
s16 uvVtxDup(s32 x, s32 y, s32 z, s32 s, s32 t, s32 r, s32 g, s32 b, s32 a) {
    if (sMaxVertexCount < sVertexCount) {
        return sVertexCount;
    }
    sVertexArray[sVertexCount].v.ob[0] = x;
    sVertexArray[sVertexCount].v.ob[1] = y;
    sVertexArray[sVertexCount].v.ob[2] = z;
    sVertexArray[sVertexCount].v.tc[0] = s;
    sVertexArray[sVertexCount].v.tc[1] = t;
    sVertexArray[sVertexCount].v.cn[0] = r;
    sVertexArray[sVertexCount].v.cn[1] = g;
    sVertexArray[sVertexCount].v.cn[2] = b;
    sVertexArray[sVertexCount].v.cn[3] = a;
    sVertexCount++;
    D_uvdgeom_rom_00400796 = -1;
    return sVertexCount - 1;
}
