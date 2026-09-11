// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "stdarg.h"

typedef struct Emitter_s {
    /* 0x00 */ Vec3F unk0;
    /* 0x0C */ f32 unkC;
    /* 0x10 */ f32 unk10;
    /* 0x14 */ f32 unk14;
    /* 0x18 */ f32 unk18;
    /* 0x1C */ f32 unk1C;
    /* 0x20 */ f32 unk20;
    /* 0x24 */ Vec3F unk24;
    /* 0x30 */ f32 unk30;
    /* 0x34 */ s32 unk34;
    /* 0x38 */ u8 unk38;
    /* 0x39 */ char pad39[7]; /* maybe part of unk38[8]? */
    /* 0x40 */ f64 unk40;
    /* 0x48 */ Vec3F unk48;
    /* 0x54 */ f32 unk54;
    /* 0x58 */ s32 unk58;
    /* 0x5C */ f32 unk5C;
    /* 0x60 */ f32 unk60;
    /* 0x64 */ s32 unk64;
    /* 0x68 */ f32 unk68;
    /* 0x6C */ f32 unk6C;
    /* 0x70 */ f32 unk70;
    /* 0x74 */ f32 unk74;
    /* 0x78 */ s32 unk78;
    /* 0x7C */ s32 unk7C;
    /* 0x80 */ u8 unk80;
    /* 0x81 */ u8 unk81;
    /* 0x82 */ s8 unk82;
    /* 0x83 */ u8 unk83;
    /* 0x84 */ s32 unk84; /* inferred */
    u8 unk88;
} Emitter; /* size = 0x88 */

typedef struct EmitterSettings_s {
    /* 0x00 */ u16 unk0; /* inferred */
    /* 0x02 */ u16 unk2; /* inferred */
    /* 0x04 */ u16 unk4; /* inferred */
    /* 0x06 */ char pad6[2];
    /* 0x08 */ u32 unk8;   /* inferred */
    /* 0x0C */ void *unkC; /* inferred */
} EmitterSettings;         /* size = 0x10 */

extern EmitterSettings *D_uvemitter_rom_00402BF4;

void func_uvemitter_rom_00400578(void);
void func_uvemitter_rom_00400630(Emitter *obj);
s32 func_uvemitter_rom_004006FC(void);
void func_uvemitter_rom_0040075C(u8 arg0);
void func_uvemitter_rom_004007B4(u8 arg0, u8 arg1);
u8 func_uvemitter_rom_00400834(u8 arg0);
void func_uvemitter_rom_0040087C(u8 arg0, Vec3F *arg1);
void func_uvemitter_rom_004008CC(u8 arg0, UnkStruct_uvemitter_rom_004008CC *arg1);
void func_uvemitter_rom_00400920(u8 arg0, Vec3F *arg1);
void func_uvemitter_rom_00400970(u8 arg0, Unkstruct_uvemitter_rom_00400970 *arg1);
void func_uvemitter_rom_00400A0C(u8 arg0, Vec3F arg1);
void func_uvemitter_rom_00400A7C(u8 arg0, Vec3F *arg1);
void func_uvemitter_rom_00400ACC(u8 arg0, Vec3F arg1, Vec3F arg4);
void func_uvemitter_rom_00400BE8(u8 arg0, f32 arg1);
f32 func_uvemitter_rom_00400C5C(u8 arg0);
void func_uvemitter_rom_00400CA8(u8 arg0, f32 arg1);
f32 func_uvemitter_rom_00400D20(u8 arg0);
void func_uvemitter_rom_00400D48(u8 arg0, f32 arg1);
f32 func_uvemitter_rom_00400DC4(u8 arg0);
void func_uvemitter_rom_00400DEC(u8 arg0, f32 arg1);
void func_uvemitter_rom_00400E60(u8 arg0, s32 arg1);
void func_uvemitter_rom_00400EBC(u8 objId, ...);
void func_uvemitter_rom_00401010(u8 arg0);
void func_uvemitter_rom_00401070(u8 arg0, s32 arg1);
void func_uvemitter_rom_004010CC(u8 arg0);
void func_uvemitter_rom_0040113C(u8 arg0);
void func_uvemitter_rom_0040120C(u16 arg0);
void func_uvemitter_rom_0040156C(u8 arg0, s16 arg1);
s32 func_uvemitter_rom_00401A34(Emitter *arg0);
void func_uvemitter_rom_00401B0C(u8 arg0);
void func_uvemitter_rom_00401D38(u8 arg0);
void func_uvemitter_rom_00401DCC(void);
void func_uvemitter_rom_00402028(Emitter *arg0);
void func_uvemitter_rom_004021D8(Emitter *arg0);
void func_uvemitter_rom_004022CC(Emitter *arg0);
void func_uvemitter_rom_004023B0(s32 arg0);
f32 func_uvemitter_rom_00402528(Vec3F arg0, Emitter *arg3, Vec3F *arg4);
s32 func_uvemitter_rom_004026EC(u8 arg0);
s32 func_uvemitter_rom_0040272C(u8 arg0);
s32 func_uvemitter_rom_00402754(s32 *arg0, s32 *arg1);
s16 func_uvemitter_rom_004027E4(void);
f32 func_uvemitter_rom_004027F4(Vec3F arg0, Vec3F arg3, Vec3F arg6, f32 arg9);
void func_uvemitter_rom_0040290C(void);
void func_uvemitter_rom_00402990(void);
void func_uvemitter_rom_00402998(u8 *arg0);
void func_uvemitter_rom_004029A4(u8 arg0, u8 arg1);
u8 func_uvemitter_rom_004029C0(u8 arg0);
void func_uvemitter_rom_004029D8(u8 arg0, f32 arg1);

extern u8 *D_uvemitter_rom_00402A78;
extern s32 D_uvemitter_rom_00402A84;
extern ALSndPlayer *D_uvemitter_rom_00402A10;
extern u8 *D_uvemitter_rom_00402A74;
extern ALBankFile *D_uvemitter_rom_00402BB8;
extern s16 *D_uvemitter_rom_00402BC0;
extern void *D_uvemitter_rom_00402BDC;
extern u16 D_uvemitter_rom_00402BF8;
extern u16 D_uvemitter_rom_00402BD2;
extern u16 D_uvemitter_rom_00402BD0;
extern Emitter *D_uvemitter_rom_00402B94;
extern UvMath_Exports *sMathExports;
extern UvFVec_Rom_Exports *sFvecExports;
extern UvFMtx_Rom_Exports *sFMtxExports;
extern UvAudioMgr_Exports *sAudioMgrExports;
extern UvEar_Rom_Exports *sUvEarExports;
extern f32 D_uvemitter_rom_00402A00;
extern s32 D_uvemitter_rom_00402C08[];
extern s32 D_uvemitter_rom_00403004;
extern s32 D_uvemitter_rom_00402B98;
extern s32 D_uvemitter_rom_00402BC4;
extern f32 D_uvemitter_rom_00402BC8;
extern f32 D_uvemitter_rom_00402BCC;
extern f64 D_uvemitter_rom_00402C00;
extern f32 D_uvemitter_rom_00402A04;
extern u8 D_uvemitter_rom_00402A88[];
extern f32 D_uvemitter_rom_00402B88[];
extern u8 *D_uvemitter_rom_00403008;
extern u8 *D_uvemitter_rom_00402A7C;
extern s32 D_uvemitter_rom_00402A80;
extern ALBank *D_uvemitter_rom_00402BB4;
extern ALInstrument *D_uvemitter_rom_00402BBC;
extern ALSndpConfig D_uvemitter_rom_00402BA8;
extern u16 D_uvemitter_rom_00402BD4;
extern u32 D_uvemitter_rom_00402BD8;
extern EmitterSettings *D_uvemitter_rom_00402BF4;

void __entrypoint_func_uvemitter_rom_400000(UvEmitter_Exports *exports) {
    s32 i;

    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvemitter_rom_004006FC = func_uvemitter_rom_004006FC;
    exports->func_uvemitter_rom_0040075C = func_uvemitter_rom_0040075C;
    exports->func_uvemitter_rom_004007B4 = func_uvemitter_rom_004007B4;
    exports->func_uvemitter_rom_00400834 = func_uvemitter_rom_00400834;
    exports->func_uvemitter_rom_0040087C = func_uvemitter_rom_0040087C;
    exports->func_uvemitter_rom_004008CC = func_uvemitter_rom_004008CC;
    exports->func_uvemitter_rom_00400920 = func_uvemitter_rom_00400920;
    exports->func_uvemitter_rom_00400970 = func_uvemitter_rom_00400970;
    exports->func_uvemitter_rom_00400A0C = func_uvemitter_rom_00400A0C;
    exports->func_uvemitter_rom_00400578 = func_uvemitter_rom_00400578;
    exports->func_uvemitter_rom_00400A7C = func_uvemitter_rom_00400A7C;
    exports->func_uvemitter_rom_00400EBC = func_uvemitter_rom_00400EBC;
    exports->func_uvemitter_rom_00400ACC = func_uvemitter_rom_00400ACC;
    exports->func_uvemitter_rom_00402754 = func_uvemitter_rom_00402754;
    exports->func_uvemitter_rom_00401010 = func_uvemitter_rom_00401010;
    exports->func_uvemitter_rom_00400BE8 = func_uvemitter_rom_00400BE8;
    exports->func_uvemitter_rom_004027E4 = func_uvemitter_rom_004027E4;
    exports->func_uvemitter_rom_00401070 = func_uvemitter_rom_00401070;
    exports->func_uvemitter_rom_00400C5C = func_uvemitter_rom_00400C5C;
    exports->func_uvemitter_rom_0040290C = func_uvemitter_rom_0040290C;
    exports->func_uvemitter_rom_004010CC = func_uvemitter_rom_004010CC;
    exports->func_uvemitter_rom_00400CA8 = func_uvemitter_rom_00400CA8;
    exports->func_uvemitter_rom_00402990 = func_uvemitter_rom_00402990;
    exports->func_uvemitter_rom_0040113C = func_uvemitter_rom_0040113C;
    exports->func_uvemitter_rom_00400D20 = func_uvemitter_rom_00400D20;
    exports->func_uvemitter_rom_00402998 = func_uvemitter_rom_00402998;
    exports->func_uvemitter_rom_0040120C = func_uvemitter_rom_0040120C;
    exports->func_uvemitter_rom_00400D48 = func_uvemitter_rom_00400D48;
    exports->func_uvemitter_rom_004029A4 = func_uvemitter_rom_004029A4;
    exports->func_uvemitter_rom_00401DCC = func_uvemitter_rom_00401DCC;
    exports->func_uvemitter_rom_00400DC4 = func_uvemitter_rom_00400DC4;
    exports->func_uvemitter_rom_004029C0 = func_uvemitter_rom_004029C0;
    exports->func_uvemitter_rom_004023B0 = func_uvemitter_rom_004023B0;
    exports->func_uvemitter_rom_00400DEC = func_uvemitter_rom_00400DEC;
    exports->func_uvemitter_rom_004029D8 = func_uvemitter_rom_004029D8;
    exports->func_uvemitter_rom_004026EC = func_uvemitter_rom_004026EC;
    exports->func_uvemitter_rom_00400E60 = func_uvemitter_rom_00400E60;
    exports->func_uvemitter_rom_0040272C = func_uvemitter_rom_0040272C;

    D_uvemitter_rom_00402BF4 = uvGetSystemProp(4);
    if (D_uvemitter_rom_00402BF4 == NULL) {
        D_uvemitter_rom_00402BD0 = 0xA;
        D_uvemitter_rom_00402BD2 = 0x100;
        D_uvemitter_rom_00402BD8 = 0;
        D_uvemitter_rom_00402BDC = NULL;
    } else {
        if (D_uvemitter_rom_00402BF4->unk0 != 0) {
            D_uvemitter_rom_00402BD0 = D_uvemitter_rom_00402BF4->unk0;
        } else {
            D_uvemitter_rom_00402BD0 = 0xA;
        }
        if (D_uvemitter_rom_00402BF4->unk2 != 0) {
            D_uvemitter_rom_00402BD2 = D_uvemitter_rom_00402BF4->unk2;
        } else {
            D_uvemitter_rom_00402BD2 = 0x10;
        }
        if (D_uvemitter_rom_00402BF4->unk4 != 0) {
            D_uvemitter_rom_00402BD4 = D_uvemitter_rom_00402BF4->unk4;
        } else {
            D_uvemitter_rom_00402BD4 = 0x100;
        }
        D_uvemitter_rom_00402BD8 = D_uvemitter_rom_00402BF4->unk8;
        D_uvemitter_rom_00402BDC = D_uvemitter_rom_00402BF4->unkC;
    }
    if (D_uvemitter_rom_00402BD8 == 0) {
        if (D_uvemitter_rom_00402BD8 == 0) {
            D_uvemitter_rom_00402BD8 =
                (D_uvemitter_rom_00402BD4 * 0x1C) + (D_uvemitter_rom_00402BD2 * 0x30) + 0x64;
        }
        D_uvemitter_rom_00402BF8 |= 1;
        D_uvemitter_rom_00402BDC = _uvMemAllocAlign8(D_uvemitter_rom_00402BD8);
    }
    sMathExports = uvLoadModule('MATH');
    sFvecExports = uvLoadModule('FVEC');
    sFMtxExports = uvLoadModule('FMTX');
    sAudioMgrExports = uvLoadModule('AMGR');
    sUvEarExports = uvLoadModule('AEAR');
    D_uvemitter_rom_00402A74 = _uvMemAlloc(D_uvemitter_rom_00402BD2 * 3, 0x10U);
    uvMemSet(D_uvemitter_rom_00402A74, 0U, D_uvemitter_rom_00402BD2 * 3);
    D_uvemitter_rom_00402A7C = &D_uvemitter_rom_00402A74[D_uvemitter_rom_00402BD2];
    D_uvemitter_rom_00402A78 = (D_uvemitter_rom_00402BD2 * 2) + D_uvemitter_rom_00402A74;
    D_uvemitter_rom_00402B94 = _uvMemAlloc(D_uvemitter_rom_00402BD0 * 0x90, 0x10U);
    uvMemSet(D_uvemitter_rom_00402B94, 0U, D_uvemitter_rom_00402BD0 * 0x90);

    for (i = 0; i < D_uvemitter_rom_00402BD0; i++) {
        func_uvemitter_rom_00400630(&D_uvemitter_rom_00402B94[i]);
        D_uvemitter_rom_00402C08[i] = 1;
    }

    for (i = 0; i < D_uvemitter_rom_00402BD2; i++) {
        D_uvemitter_rom_00402A74[i] = 0xFF;
        D_uvemitter_rom_00402A7C[i] = 0xFF;
    }

    D_uvemitter_rom_00402A80 = 0;
    uvMemSet(D_uvemitter_rom_00402BDC, 0U, D_uvemitter_rom_00402BD8);
    alHeapInit((ALHeap *) &D_uvemitter_rom_00402B98, (u8 *) D_uvemitter_rom_00402BDC,
               (s32) D_uvemitter_rom_00402BD8);
    D_uvemitter_rom_00402BA8.maxSounds = (s32) D_uvemitter_rom_00402BD2;
    D_uvemitter_rom_00402BA8.maxEvents = D_uvemitter_rom_00402BD4;
    D_uvemitter_rom_00402BA8.heap = (ALHeap *) &D_uvemitter_rom_00402B98;
    alSndpNew(D_uvemitter_rom_00402A10, &D_uvemitter_rom_00402BA8);
    uvClkReset(0x6A);
    D_uvemitter_rom_00402C00 = uvClkGetSec(0x6A);
}

void func_uvemitter_rom_00400578(void) {
    func_uvemitter_rom_0040290C();
    alSndpDelete(D_uvemitter_rom_00402A10);
    _uvMemFree(D_uvemitter_rom_00402BC0);
    _uvMemFree(D_uvemitter_rom_00402BB8);
    _uvMemFree(D_uvemitter_rom_00402B94);
    _uvMemFree(D_uvemitter_rom_00402A74);
    if (D_uvemitter_rom_00402BF8 & 1) {
        _uvMemFree(D_uvemitter_rom_00402BDC);
    }
    uvUnloadModule('MATH');
    uvUnloadModule('FVEC');
    uvUnloadModule('FMTX');
    uvUnloadModule('AMGR');
    uvUnloadModule('AEAR');
}

void func_uvemitter_rom_00400630(Emitter *obj) {
    obj->unk80 = D_uvemitter_rom_00402BD2 + 1;
    obj->unk78 = 0;
    obj->unk7C = 0x7FFF;
    obj->unk81 = 0x40;
    obj->unk82 = 0;
    obj->unk83 = 0;
    obj->unk88 = 0;
    obj->unk38 = -1;
    obj->unk34 = 0;
    obj->unk64 = 0;
    obj->unk84 = 0;
    obj->unk58 = 0;
    obj->unk74 = 1.0f;
    obj->unk30 = 1.0f;
    obj->unk68 = 1.0f;
    obj->unk54 = 1.0f;
    obj->unk0.x = 0.0f;
    obj->unk0.y = 0.0f;
    obj->unk0.z = 0.0f;
    obj->unkC = 0.0f;
    obj->unk10 = 0.0f;
    obj->unk14 = 0.0f;
    obj->unk18 = 0.0f;
    obj->unk1C = 0.0f;
    obj->unk20 = 0.0f;
    obj->unk24.x = 0.0f;
    obj->unk24.y = 0.0f;
    obj->unk24.z = 0.0f;
    obj->unk48.x = 0.0f;
    obj->unk48.y = 0.0f;
    obj->unk48.z = 0.0f;
    obj->unk6C = 0.0f;
    obj->unk70 = 0.0f;
    obj->unk5C = 0.0f;
    obj->unk60 = 1000.0f;
    obj->unk40 = -1.0;
}

s32 func_uvemitter_rom_004006FC(void) {
    u8 i;

    for (i = 0; i < D_uvemitter_rom_00402BD0; i++) {
        if (D_uvemitter_rom_00402B94[i].unk88 == 0) {
            D_uvemitter_rom_00402B94[i].unk88 = 1;
            return i;
        }
    }
    return 0xFF;
}

void func_uvemitter_rom_0040075C(u8 arg0) {
    Emitter *emitter;

    if (arg0 < D_uvemitter_rom_00402BD0) {
        emitter = &D_uvemitter_rom_00402B94[arg0];
        func_uvemitter_rom_004022CC(emitter);
        func_uvemitter_rom_00400630(emitter);
    }
}

void func_uvemitter_rom_004007B4(u8 arg0, u8 arg1) {
    Emitter *temp_a0;

    if (arg0 < (s32) D_uvemitter_rom_00402BD0) {
        if (arg1 == 0xFF) {
            func_uvemitter_rom_0040075C(arg0);
        }
        temp_a0 = &D_uvemitter_rom_00402B94[arg0];
        func_uvemitter_rom_004022CC(temp_a0);
        temp_a0->unk38 = arg1;
    }
}

u8 func_uvemitter_rom_00400834(u8 arg0) {
    if (arg0 >= (s32) D_uvemitter_rom_00402BD0) {
        return 0xFFU;
    }
    return D_uvemitter_rom_00402B94[arg0].unk38;
}

void func_uvemitter_rom_0040087C(u8 arg0, Vec3F *arg1) {
    if (arg0 < (s32) D_uvemitter_rom_00402BD0) {
        D_uvemitter_rom_00402B94[arg0].unk0 = *arg1;
    }
}

void func_uvemitter_rom_004008CC(u8 arg0, UnkStruct_uvemitter_rom_004008CC *arg1) {
    Vec3F sp1C;

    if (arg0 < (s32) D_uvemitter_rom_00402BD0) {
        sp1C.x = arg1->unk30.x;
        sp1C.y = arg1->unk30.y;
        sp1C.z = arg1->unk30.z;
        func_uvemitter_rom_0040087C((u8) arg0, (Vec3F *) &sp1C);
    }
}

void func_uvemitter_rom_00400920(u8 arg0, Vec3F *arg1) {
    if (arg0 < (s32) D_uvemitter_rom_00402BD0) {
        *arg1 = D_uvemitter_rom_00402B94[arg0].unk0;
    }
}

void func_uvemitter_rom_00400970(u8 arg0, Unkstruct_uvemitter_rom_00400970 *arg1) {
    if (arg0 < D_uvemitter_rom_00402BD0) {
        sFMtxExports->func_00400B68(&arg1->unk0);
        arg1->unkC0 = D_uvemitter_rom_00402B94[arg0].unk0.x;
        arg1->unkD0 = D_uvemitter_rom_00402B94[arg0].unk0.y;
        arg1->unkE0 = D_uvemitter_rom_00402B94[arg0].unk0.z;
    }
}

void func_uvemitter_rom_00400A0C(u8 arg0, Vec3F arg1) {
    if (arg0 < (s32) D_uvemitter_rom_00402BD0) {
        D_uvemitter_rom_00402C08[arg0] = 0;
        D_uvemitter_rom_00402B94[arg0].unk24 = arg1;
    }
}

void func_uvemitter_rom_00400A7C(u8 arg0, Vec3F *arg1) {
    if (arg0 < (s32) D_uvemitter_rom_00402BD0) {
        *arg1 = D_uvemitter_rom_00402B94[arg0].unk24;
    }
}

void func_uvemitter_rom_00400ACC(u8 arg0, Vec3F arg1, Vec3F arg4) {
    f32 diffX;
    f32 diffY;
    f32 diffZ;
    Emitter *obj;
    f32 len;

    obj = &D_uvemitter_rom_00402B94[arg0];
    if (!(arg0 < D_uvemitter_rom_00402BD0)) {
        return;
    }

    sFvecExports->uvVec3FCopy(&obj->unk0, (Vec3F *) &arg1);
    sFvecExports->uvVec3FCopy((Vec3F *) &obj->unkC, (Vec3F *) &arg4);
    diffX = obj->unkC - obj->unk0.x;
    diffY = obj->unk10 - obj->unk0.y;
    diffZ = obj->unk14 - obj->unk0.z;
    len = sMathExports->uvSqrtf(SQ(diffX) + SQ(diffY) + SQ(diffZ));
    if (len != 0.0f) {
        obj->unk18 = diffX / len;
        obj->unk1C = diffY / len;
        obj->unk20 = diffZ / len;
    }
}

void func_uvemitter_rom_00400BE8(u8 arg0, f32 arg1) {
    if (arg1 < 0.0f) {
        arg1 = 0.0f;
    }
    if (arg1 > 1.0f) {
        arg1 = 1.0f;
    }
    if (arg0 < (s32) D_uvemitter_rom_00402BD0) {
        D_uvemitter_rom_00402B94[arg0].unk30 = arg1;
    }
}

f32 func_uvemitter_rom_00400C5C(u8 arg0) {
    if (arg0 >= (s32) D_uvemitter_rom_00402BD0) {
        return 0.0f;
    }
    return D_uvemitter_rom_00402B94[arg0].unk30;
}

void func_uvemitter_rom_00400CA8(u8 arg0, f32 arg1) {
    if (arg1 < -1.0f) {
        arg1 = -1.0f;
    }
    if (arg1 > 1.0f) {
        arg1 = 1.0f;
    }
    if (arg0 < (s32) D_uvemitter_rom_00402BD0) {
        D_uvemitter_rom_00402B94[arg0].unk6C = arg1;
    }
}

f32 func_uvemitter_rom_00400D20(u8 arg0) {
    return D_uvemitter_rom_00402B94[arg0].unk6C;
}

void func_uvemitter_rom_00400D48(u8 arg0, f32 arg1) {
    if (!(arg0 < (s32) D_uvemitter_rom_00402BD0)) {
        return;
    }

    if (arg1 <= 0.0f) {
        arg1 = D_uvemitter_rom_00402A00;
    }
    if (arg1 > 2.0f) {
        arg1 = 2.0f;
    }
    D_uvemitter_rom_00402B94[arg0].unk68 = arg1;
}

f32 func_uvemitter_rom_00400DC4(u8 arg0) {
    return D_uvemitter_rom_00402B94[arg0].unk68;
}

void func_uvemitter_rom_00400DEC(u8 arg0, f32 arg1) {
    if (arg1 < 0.0f) {
        arg1 = 0.0f;
    }
    if (arg1 > 1.0f) {
        arg1 = 1.0f;
    }
    if (arg0 < D_uvemitter_rom_00402BD0) {
        D_uvemitter_rom_00402B94[arg0].unk70 = arg1;
    }
}

void func_uvemitter_rom_00400E60(u8 arg0, s32 arg1) {
    if (arg1 < 0) {
        arg1 = 0;
    }
    if (arg1 >= 0x8000) {
        arg1 = 0x7FFF;
    }
    if (arg0 < (s32) D_uvemitter_rom_00402BD0) {
        D_uvemitter_rom_00402B94[arg0].unk34 = arg1;
    }
}

// uvEmitterProps
void func_uvemitter_rom_00400EBC(u8 objId, ...) {
    f32 temp_fv0;
    va_list args;
    Emitter *obj;
    s32 prop;

    if ((objId >= D_uvemitter_rom_00402BD0) || (objId == 0xFF)) {
        return;
    }

    if (args) {
    }
    obj = &D_uvemitter_rom_00402B94[objId];
    // passing an object that undergoes default argument promotion to 'va_start'
    // has undefined behavior (e.g. u8, u16, f32)
#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvarargs"
#endif
    va_start(args, objId);
#if defined(__clang__)
#pragma GCC diagnostic pop
#endif

    while (TRUE) {
        prop = va_arg(args, s32);
        switch (prop) { /* irregular */
            case 0:
                return;
            case 1:
                temp_fv0 = va_arg(args, f64);
                obj->unk5C = SQ(temp_fv0);
                break;
            case 2:
                temp_fv0 = va_arg(args, f64);
                obj->unk60 = SQ(temp_fv0);
                break;
            case 5:
                obj->unk64 = va_arg(args, s32);
                break;
            default:
                return;
        }
    }
}

void func_uvemitter_rom_00401010(u8 arg0) {
    Emitter *temp_v0;

    if ((D_uvemitter_rom_00403004 == 0) && (arg0 < (s32) D_uvemitter_rom_00402BD0)) {
        if (D_uvemitter_rom_00402B94[arg0].unk38 != 0xFF) {
            D_uvemitter_rom_00402B94[arg0].unk84 = 0x10;
        }
    }
}

void func_uvemitter_rom_00401070(u8 arg0, s32 arg1) {
    if ((D_uvemitter_rom_00403004 == 0) && (arg0 < (s32) D_uvemitter_rom_00402BD0)) {
        D_uvemitter_rom_00402B94[arg0].unk84 = 0x20;
        D_uvemitter_rom_00402B94[arg0].unk58 = arg1;
    }
}

void func_uvemitter_rom_004010CC(u8 arg0) {
    Emitter *temp_a0;

    if (arg0 < (s32) D_uvemitter_rom_00402BD0) {
        temp_a0 = &D_uvemitter_rom_00402B94[arg0];
        func_uvemitter_rom_004022CC(temp_a0);
        temp_a0->unk84 = 0;
        if (temp_a0->unk64 & 0x20) {
            func_uvemitter_rom_00400630(temp_a0);
        }
    }
}

void func_uvemitter_rom_0040113C(u8 arg0) {
    u8 i;
    s32 s2 = arg0;

    for (arg0 = 0, i = 0; i < D_uvemitter_rom_00402BD0; i++) {
        if (D_uvemitter_rom_00402B94[i].unk38 == 0xFF) {
            continue;
        }

        if (s2) {
            D_uvemitter_rom_00402B94[i].unk64 |= 0x20;
            func_uvemitter_rom_004010CC(i);
        }
        arg0++;
    }
}

void func_uvemitter_rom_0040120C(u16 arg0) {
    Emitter *obj;
    f32 temp_fs0;
    s32 i;
    Vec3F sp68;
    Vec3F sp5C;

    sFvecExports->uvVec3FSet(&sp68, 0.0f, 0.0f, 0.0f);
    sFvecExports->uvVec3FSet(&sp5C, 0.0f, 0.0f, 0.0f);
    sAudioMgrExports->func_uvaudiomgr_rom_004011EC(&D_uvemitter_rom_00402BC4);
    sAudioMgrExports->func_uvaudiomgr_rom_004011B4(&D_uvemitter_rom_00402BC8);
    sAudioMgrExports->func_uvaudiomgr_rom_00401208(&D_uvemitter_rom_00402BCC);
    func_8000D7F0(&D_uvemitter_rom_00402B98);
    for (i = 0; i < D_uvemitter_rom_00402BD0; i++) {
        obj = &D_uvemitter_rom_00402B94[i];
        D_uvemitter_rom_00402C00 = uvClkGetSec(0x6A);
        if (D_uvemitter_rom_00402C00 == obj->unk40) {
            continue;
        }

        if ((obj->unk88 == 1)
            && (((obj->unk78 == 1)) || (obj->unk78 == 2) || (obj->unk78 == 8) || ((obj->unk84 == 0x10))
                || (obj->unk84 == 0x20))) {
            if (obj->unk78 == 2) {
                obj->unk83++;
            }
            if ((obj->unk78 == 8) || (obj->unk84 == 0x10)) {
                sFvecExports->uvVec3FCopy(&sp68, &obj->unk0);
                if (D_uvemitter_rom_00402C08[i] != 0) {
                    temp_fs0 = (f32) (D_uvemitter_rom_00402C00 - obj->unk40);
                    sFvecExports->uvVec3FSub(&sp5C, &sp68, &obj->unk48);
                    sFvecExports->uvVec3FScale(&sp5C, &sp5C, 1.0f / temp_fs0);
                } else {
                    sFvecExports->uvVec3FCopy(&sp5C, &obj->unk24);
                }
                sFvecExports->uvVec3FCopy(&obj->unk48, &sp68);
                sFvecExports->uvVec3FCopy(&obj->unk24, &sp5C);
            }
            func_uvemitter_rom_0040156C(i, arg0);
            if ((obj->unk7C == 0) || ((obj->unk78 == 2) && ((s32) obj->unk83 >= 5))) {
                if (obj->unk64 & 0x10) {
                    func_uvemitter_rom_004010CC(i);
                } else {
                    func_uvemitter_rom_004010CC(i);
                    func_uvemitter_rom_00401010(i);
                }
            } else if (func_uvemitter_rom_00401A34(obj) == 0) {
                func_uvemitter_rom_00401B0C(i);
            }
        }
        obj->unk40 = D_uvemitter_rom_00402C00;
    }
}

void func_uvemitter_rom_0040156C(u8 arg0, s16 arg1) {
    s32 var_v1;
    f32 var_fa1;
    f32 var_ft4;
    f32 var_fa0;
    Vec3F sp8C;
    Vec3F sp80;
    Vec3F sp74;
    Vec3F sp68;
    f32 sp64;
    s32 sp48;
    Emitter *obj;
    s32 sp58;
    s32 i;

    sp64 = 1.0f;
    obj = &D_uvemitter_rom_00402B94[arg0];
    sp58 = obj->unk64;
    if (sp58 & 8) {
        if (arg1 == 0) {
            *(s32 *) 0 = 0; // fault
        }

        sp48 = sUvEarExports->func_uvear_rom_0040077C(arg1, &obj->unk0);
        sUvEarExports->func_uvear_rom_00400290(sp48, &sp8C);
        sUvEarExports->func_uvear_rom_00400340(sp48, &sp80);
        sUvEarExports->func_uvear_rom_00400494(sp48, &sp74);
        if (sp58 & 2) {
            var_ft4 = func_uvemitter_rom_00402528(sp8C, obj, &sp68);
        } else if (sp58 & 1) {
            /* Empty */
        } else {
            sp68.x = sp8C.x - obj->unk0.x;
            sp68.y = sp8C.y - obj->unk0.y;
            sp68.z = sp8C.z - obj->unk0.z;
            var_ft4 = SQ(sp68.x) + SQ(sp68.y) + SQ(sp68.z);
            sp64 = obj->unk54;
            if ((sp58 & 0x40) && (var_ft4 > 0.0f)) {
                sp64 *= func_uvemitter_rom_004027F4(sp68, sp74, obj->unk24, 330.0f);
            }
        }
        if (obj->unk60 < var_ft4) {
            obj->unk81 = 0x40;
            obj->unk7C = 0;
            obj->unk74 = obj->unk68;
            return;
        }
        if (var_ft4 <= obj->unk5C) {
            var_fa1 = D_uvemitter_rom_00402B94[arg0].unk30;
        } else {
            var_fa1 = (obj->unk30 * (obj->unk60 - var_ft4)) / (obj->unk60 - obj->unk5C);
        }
        if ((D_uvemitter_rom_00402BC4 != 1) && (var_ft4 != 0.0f)) {
            var_fa0 = sFvecExports->uvVec3FScalarProj((Vec3F *) &sp68, &sp80);
            if (var_ft4 < obj->unk5C) {
                var_fa0 *= (var_ft4 / obj->unk5C);
            }

            var_v1 = (s32) ((1.0f - var_fa0) * 64.0f);
            if (var_v1 == 0x80) {
                var_v1 = 0x7F;
            }
        } else {
            var_v1 = 0x40;
        }
    } else {
        var_fa1 = obj->unk30;
        if (D_uvemitter_rom_00402BC4 != 1) {
            var_v1 = (s32) ((obj->unk6C + 1.0f) * 64.0f);
            if (var_v1 == 0x80) {
                var_v1 = 0x7F;
            }
        } else {
            var_v1 = 0x40;
        }
    }
    if (sp58 & 4) {
        obj->unk82 = (s8) (s32) (D_uvemitter_rom_00402BCC * 127.0f);
    } else {
        obj->unk82 = (s8) (s32) (obj->unk70 * 127.0f);
    }
    obj->unk74 = obj->unk68 * sp64;
    obj->unk81 = (s8) var_v1;

    obj->unk7C = (s32) (D_uvemitter_rom_00402B88[D_uvemitter_rom_00402A88[obj->unk38]] * var_fa1
                        * D_uvemitter_rom_00402BC8 * D_uvemitter_rom_00402A04);

    if (D_uvemitter_rom_00403008 == NULL) {
        return;
    }

    for (i = 0; D_uvemitter_rom_00403008[i] != 0xFF; i++) {
        if (D_uvemitter_rom_00403008[i] == obj->unk38) {
            break;
        }
    }

    if (D_uvemitter_rom_00403008[i] != 0xFF) {
        obj->unk7C = 3;
    }
}

s32 func_uvemitter_rom_00401A34(Emitter *arg0) {
    s32 temp_v0;

    if (arg0->unk78 == 8) {
        alSndpSetSound(D_uvemitter_rom_00402A10, arg0->unk80);
        if (D_uvemitter_rom_00402A10->target == -1) {
            *(s32 *) 0 = 0; // fault
        }
        if (alSndpGetState(D_uvemitter_rom_00402A10) == 0) {
            if (arg0->unk64 & 0x10) {
                arg0->unk80 = D_uvemitter_rom_00402BD2 + 1;
                arg0->unk78 = 0;
                if (arg0->unk64 & 0x20) {
                    func_uvemitter_rom_00400630(arg0);
                }
                return 1;
            }

            arg0->unk80 = D_uvemitter_rom_00402BD2 + 1;
            arg0->unk78 = 0;
            arg0->unk83 = 0;
            arg0->unk84 = 0x10;
            return 0;
        }
    }
    return 0;
}

void func_uvemitter_rom_00401B0C(u8 arg0) {
    Emitter *temp_fp;
    Emitter *temp_v0_2;
    s32 i;
    s32 var_s4;
    s32 var_s7;
    s32 j;
    s32 temp;

    temp_fp = &D_uvemitter_rom_00402B94[arg0];

    if (D_uvemitter_rom_00402BD2 < D_uvemitter_rom_00402A80) {
        *(s32 *) 0 = 0; // fault
    }

    var_s7 = temp_fp->unk7C + temp_fp->unk34;
    if (temp_fp->unk7C == 0) {
        var_s7 = 0;
    }
    i = 0;
    var_s4 = FALSE;
    while ((i < D_uvemitter_rom_00402A80) && !(var_s4)) {
        temp_v0_2 = &D_uvemitter_rom_00402B94[D_uvemitter_rom_00402A74[i]];
        temp = temp_v0_2->unk7C + temp_v0_2->unk34;
        if (temp < var_s7) {
            var_s4 = TRUE;
            if (D_uvemitter_rom_00402A80 == D_uvemitter_rom_00402BD2) {
                func_uvemitter_rom_00401D38(D_uvemitter_rom_00402A74[D_uvemitter_rom_00402A80 - 1]);
                D_uvemitter_rom_00402A80--;
            }

            for (j = D_uvemitter_rom_00402A80; j > i; j--) {
                D_uvemitter_rom_00402A74[j] = D_uvemitter_rom_00402A74[j - 1];
            }

            D_uvemitter_rom_00402A74[i] = arg0;
            D_uvemitter_rom_00402A80++;
            if (temp_fp->unk80 != (D_uvemitter_rom_00402BD2 + 1)) {
                D_uvemitter_rom_00402A7C[temp_fp->unk80] = arg0;
            }
        }
        i++;
    }
    if (var_s4 == 0) {
        if (i == D_uvemitter_rom_00402BD2) {
            func_uvemitter_rom_00401D38(arg0);
            return;
        }
        D_uvemitter_rom_00402A74[i] = arg0;
        D_uvemitter_rom_00402A80++;
        if ((D_uvemitter_rom_00402BD2 + 1) != D_uvemitter_rom_00402B94[arg0].unk80) {
            D_uvemitter_rom_00402A7C[D_uvemitter_rom_00402B94[arg0].unk80] = arg0;
        }
    }
}

void func_uvemitter_rom_00401D38(u8 arg0) {
    Emitter *obj;

    obj = &D_uvemitter_rom_00402B94[arg0];
    if (obj->unk80 != (D_uvemitter_rom_00402BD2 + 1)) {
        D_uvemitter_rom_00402A7C[obj->unk80] = 0xFF;
    }
    if (obj->unk64 & 0x10) {
        func_uvemitter_rom_004010CC(arg0);
        return;
    }
    func_uvemitter_rom_004022CC(obj);
    func_uvemitter_rom_00401010(arg0);
}

void func_uvemitter_rom_00401DCC(void) {
    Emitter *temp_s1;
    u8 temp_a2;
    s32 i;
    s32 j;

    for (i = 0; i < D_uvemitter_rom_00402A80; i++) {
        temp_a2 = D_uvemitter_rom_00402A74[i];
        temp_s1 = &D_uvemitter_rom_00402B94[temp_a2];
        switch (temp_s1->unk78) {
            case 0:
                for (j = 0; D_uvemitter_rom_00402A7C[j] != 0xFF && j < D_uvemitter_rom_00402BD2 - 1;
                     j++) {
                }
                D_uvemitter_rom_00402A7C[j] = temp_a2;
                temp_s1->unk80 = j;
                func_uvemitter_rom_00402028(temp_s1);
                break;
            case 1:
                func_uvemitter_rom_00402028(temp_s1);
                break;
            case 2:
                alSndpSetSound(D_uvemitter_rom_00402A10, D_uvemitter_rom_00402BC0[temp_s1->unk80]);
                if (D_uvemitter_rom_00402A10->target == -1) {
                    *(s32 *) 0 = 0; // fault
                }
                if (alSndpGetState(D_uvemitter_rom_00402A10) == 1) {
                    temp_s1->unk78 = 8;
                    temp_s1->unk83 = 0;
                }
                break;
            case 8:
                func_uvemitter_rom_004021D8(temp_s1);
                break;
        }
    }

    i = 0;
    D_uvemitter_rom_00402A84 = D_uvemitter_rom_00402A80;
    for (; i < D_uvemitter_rom_00402BD2; i++) {
        *(D_uvemitter_rom_00402A78 + i) = D_uvemitter_rom_00402A74[i];
        D_uvemitter_rom_00402A74[i] = 0xFF;
        D_uvemitter_rom_00402A7C[i] = 0xFF;
    }
    D_uvemitter_rom_00402A80 = 0;
}

void func_uvemitter_rom_00402028(Emitter *arg0) {
    ALSound *temp_a1;
    ALInstrument *temp_a0;

    alSndpSetSound(D_uvemitter_rom_00402A10, D_uvemitter_rom_00402BC0[arg0->unk80]);
    if (D_uvemitter_rom_00402A10->target == -1) {
        *(s32 *) 0 = 0;
    }
    if (alSndpGetState(D_uvemitter_rom_00402A10) != 0) {
        arg0->unk78 = 1;
        alSndpStop(D_uvemitter_rom_00402A10);
        return;
    }
    alSndpDeallocate(D_uvemitter_rom_00402A10, D_uvemitter_rom_00402BC0[arg0->unk80]);
    if ((s32) arg0->unk38 >= D_uvemitter_rom_00402BB4->instCount) {
        *(s32 *) 0 = 0;
    }

    temp_a0 = D_uvemitter_rom_00402BB4->instArray[arg0->unk38];
    temp_a1 = temp_a0->soundArray[0];
    if (temp_a1 == NULL) {
        *(s32 *) 0 = 0;
    }
    D_uvemitter_rom_00402BC0[arg0->unk80] = alSndpAllocate(D_uvemitter_rom_00402A10, temp_a1);
    alSndpSetSound(D_uvemitter_rom_00402A10, D_uvemitter_rom_00402BC0[arg0->unk80]);
    if (D_uvemitter_rom_00402A10->target == -1) {
        *(s32 *) 0 = 0;
    }
    if (D_uvemitter_rom_00402BC0[arg0->unk80] >= 0) {
        func_uvemitter_rom_004021D8(arg0);
        if (arg0->unk84 == 0x20) {
            alSndpPlayAt(D_uvemitter_rom_00402A10, arg0->unk58);
        } else {
            alSndpPlay(D_uvemitter_rom_00402A10);
        }
        arg0->unk78 = 2;
        arg0->unk84 = 0;
    }
}

void func_uvemitter_rom_004021D8(Emitter *arg0) {
    if (arg0->unk80 >= D_uvemitter_rom_00402BD2) {
        *(s32 *) 0 = 0;
    }
    if (D_uvemitter_rom_00402BC0[arg0->unk80] < 0) {
        *(s32 *) 0 = 0;
    }
    if (arg0->unk81 >= 0x80) {
        *(s32 *) 0 = 0;
    }
    alSndpSetSound(D_uvemitter_rom_00402A10, D_uvemitter_rom_00402BC0[arg0->unk80]);
    if (D_uvemitter_rom_00402A10->target == -1) {
        *(s32 *) 0 = 0;
    }
    alSndpSetVol(D_uvemitter_rom_00402A10, arg0->unk7C);
    alSndpSetPitch(D_uvemitter_rom_00402A10, arg0->unk74);
    alSndpSetFXMix(D_uvemitter_rom_00402A10, arg0->unk82);
    alSndpSetPan(D_uvemitter_rom_00402A10, arg0->unk81);
}

void func_uvemitter_rom_004022CC(Emitter *arg0) {
    u16 temp_v0;
    u8 temp_v1;

    if (arg0->unk78 != 0) {
        if ((s32) arg0->unk80 < (s32) D_uvemitter_rom_00402BD2) {
            alSndpSetSound(D_uvemitter_rom_00402A10, D_uvemitter_rom_00402BC0[arg0->unk80]);
            if (D_uvemitter_rom_00402A10->target == -1) {
                *(s32 *) 0 = 0;
            }
            if (alSndpGetState(D_uvemitter_rom_00402A10) == 1) {
                alSndpSetVol(D_uvemitter_rom_00402A10, 0);
            }
            alSndpStop(D_uvemitter_rom_00402A10);
        }
        arg0->unk80 = D_uvemitter_rom_00402BD2 + 1;
        arg0->unk78 = 0;
        arg0->unk83 = 0;
    }
}

void func_uvemitter_rom_004023B0(s32 arg0) {
    void *sp5C;
    void *sp58;
    u32 sp54;
    ALSound *temp_s3;
    s32 temp_v0;
    s32 var_s1;
    s32 var_s2;

    temp_v0 = uvFileReadHeader(uvGetFileData('UVSX', arg0));
    uvFileSearchTag(temp_v0, &sp54, &sp5C, '.CTL', 0);
    uvFileSearchTag(temp_v0, NULL, &sp58, '.TBL', 0);
    uvFileFree(temp_v0);
    D_uvemitter_rom_00402BB8 = _uvMemAlloc(sp54 + 0x3E8, 0x10U);
    _uvMediaCopy(D_uvemitter_rom_00402BB8, sp5C, sp54);
    alBnkfNew(D_uvemitter_rom_00402BB8, (u8 *) sp58);
    D_uvemitter_rom_00402BB4 = D_uvemitter_rom_00402BB8->bankArray[0];
    D_uvemitter_rom_00402BBC = D_uvemitter_rom_00402BB4->instArray[0];
    D_uvemitter_rom_00402BC0 = _uvMemAlloc(D_uvemitter_rom_00402A10->maxSounds * 2, 0x10U);
    temp_s3 = D_uvemitter_rom_00402BBC->soundArray[0];
    for (var_s1 = 0; var_s1 < D_uvemitter_rom_00402A10->maxSounds; var_s1++) {
        D_uvemitter_rom_00402BC0[var_s1] = alSndpAllocate(D_uvemitter_rom_00402A10, temp_s3);
    }
}

f32 func_uvemitter_rom_00402528(Vec3F arg0, Emitter *arg3, Vec3F *arg4) {
    f32 temp_fv0_2;
    f32 sp28;
    f32 sp24;
    f32 var_ft4;
    f32 sp20;

    arg4->x = arg3->unk0.x - arg0.x;
    arg4->y = arg3->unk0.y - arg0.y;
    arg4->z = arg3->unk0.z - arg0.z;
    if (sFvecExports->uvVec3FDot(arg4, (Vec3F *) &arg3->unk18) >= 0.0f) {
        var_ft4 = SQ(arg4->z) + (SQ(arg4->x) + SQ(arg4->y));
    } else {
        arg4->x = arg3->unkC - arg0.x;
        arg4->y = arg3->unk10 - arg0.y;
        arg4->z = arg3->unk14 - arg0.z;
        temp_fv0_2 = sFvecExports->uvVec3FDot(arg4, (Vec3F *) &arg3->unk18);
        if (temp_fv0_2 < 0.0f) {
            var_ft4 = SQ(arg4->z) + (SQ(arg4->x) + SQ(arg4->y));
        } else {
            sp20 = (arg3->unkC - (temp_fv0_2 * arg3->unk18));
            sp24 = arg3->unk10 - (temp_fv0_2 * arg3->unk1C);
            sp28 = arg3->unk14 - (temp_fv0_2 * arg3->unk20);
            arg4->x = sp20 - arg0.x;
            arg4->y = sp24 - arg0.y;
            arg4->z = sp28 - arg0.z;
            var_ft4 = SQ(arg4->z) + (SQ(arg4->x) + SQ(arg4->y));
        }
    }
    return var_ft4;
}

s32 func_uvemitter_rom_004026EC(u8 arg0) {
    if (D_uvemitter_rom_00402B94[arg0].unk88 == 0) {
        return 0;
    }
    return 1;
}

s32 func_uvemitter_rom_0040272C(u8 arg0) {
    return D_uvemitter_rom_00402B94[arg0].unk84;
}

s32 func_uvemitter_rom_00402754(s32 *arg0, s32 *arg1) {
    Emitter *temp_v1;
    s32 i;

    for (i = 0; i < D_uvemitter_rom_00402A84; i++) {
        temp_v1 = &D_uvemitter_rom_00402B94[D_uvemitter_rom_00402A78[i]];
        if (arg0 != 0) {
            arg0[i] = (s32) temp_v1->unk38;
        }
        if (arg1 != 0) {
            arg1[i] = temp_v1->unk7C;
        }
    }
    return D_uvemitter_rom_00402A84;
}

s16 func_uvemitter_rom_004027E4(void) {
    return D_uvemitter_rom_00402BB4->instCount;
}

f32 func_uvemitter_rom_004027F4(Vec3F arg0, Vec3F arg3, Vec3F arg6, f32 arg9) {
    Vec3F sp44;
    Vec3F sp38;
    Vec3F sp2C;
    f32 temp_fv0;
    f32 sp24;
    f32 var_fv1;

    sFvecExports->uvVec3FCopy(&sp44, (Vec3F *) &arg0);
    sFvecExports->uvVec3FNormalize(&sp44, &sp44);
    sFvecExports->uvVec3FScale(&sp44, &sp44, arg9);
    sFvecExports->uvVec3FSub(&sp2C, &sp44, (Vec3F *) &arg3);
    sp24 = sFvecExports->uvVec3FLen(&sp2C);
    sFvecExports->uvVec3FSub(&sp38, &sp44, (Vec3F *) &arg6);
    temp_fv0 = sFvecExports->uvVec3FLen(&sp38);
    var_fv1 = (temp_fv0 == 0) ? 1.0f : sp24 / temp_fv0;

    return var_fv1;
}

void func_uvemitter_rom_0040290C(void) {
    s32 i;
    for (i = 0; i < D_uvemitter_rom_00402BD0; i++) {
        func_uvemitter_rom_004022CC(&D_uvemitter_rom_00402B94[i]);
    }
    D_uvemitter_rom_00403004 = 1;
}

void func_uvemitter_rom_00402990(void) {
}

void func_uvemitter_rom_00402998(u8 *arg0) {
    D_uvemitter_rom_00403008 = arg0;
}

void func_uvemitter_rom_004029A4(u8 arg0, u8 arg1) {
    D_uvemitter_rom_00402A88[arg0] = arg1;
}

u8 func_uvemitter_rom_004029C0(u8 arg0) {
    return D_uvemitter_rom_00402A88[arg0];
}

void func_uvemitter_rom_004029D8(u8 arg0, f32 arg1) {
    D_uvemitter_rom_00402B88[arg0] = arg1;
}
