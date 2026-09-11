#ifndef UVEMITTER_ROM_H
#define UVEMITTER_ROM_H

typedef struct UnkStruct_uvemitter_rom_004008CC_s {
    char pad[0x30];
    Vec3F unk30;
} UnkStruct_uvemitter_rom_004008CC;

typedef struct Unkstruct_uvemitter_rom_00400970_s {
    /* 0x00 */ Mtx4F unk0;          /* inferred */
    /* 0x40 */ char pad40[0x80];    /* maybe part of unk0[3]? */
    /* 0xC0 */ f32 unkC0;           /* inferred */
    /* 0xC4 */ char padC4[0xC];     /* maybe part of unkC0[4]? */
    /* 0xD0 */ f32 unkD0;           /* inferred */
    /* 0xD4 */ char padD4[0xC];     /* maybe part of unkD0[4]? */
    /* 0xE0 */ f32 unkE0;           /* inferred */
} Unkstruct_uvemitter_rom_00400970; /* size = 0xE4 */

typedef struct UvEmitter_Exports_s {
    /* 0x00 */ void (*func_uvemitter_rom_00400578)(void);
    /* 0x04 */ s32 (*func_uvemitter_rom_004006FC)(void);
    /* 0x08 */ void (*func_uvemitter_rom_0040075C)(u8);
    /* 0x0C */ void (*func_uvemitter_rom_004007B4)(u8, u8);
    /* 0x10 */ u8 (*func_uvemitter_rom_00400834)(u8);
    /* 0x14 */ void (*func_uvemitter_rom_0040087C)(u8, Vec3F *);
    /* 0x18 */ void (*func_uvemitter_rom_004008CC)(u8, UnkStruct_uvemitter_rom_004008CC *);
    /* 0x1C */ void (*func_uvemitter_rom_00400920)(u8, Vec3F *);
    /* 0x20 */ void (*func_uvemitter_rom_00400970)(u8, Unkstruct_uvemitter_rom_00400970 *);
    /* 0x24 */ void (*func_uvemitter_rom_00400A0C)(u8, Vec3F);
    /* 0x28 */ void (*func_uvemitter_rom_00400A7C)(u8, Vec3F *);
    /* 0x2C */ void (*func_uvemitter_rom_00400ACC)(u8, Vec3F, Vec3F);
    /* 0x30 */ void (*func_uvemitter_rom_00400BE8)(u8, f32);
    /* 0x34 */ f32 (*func_uvemitter_rom_00400C5C)(u8);
    /* 0x38 */ void (*func_uvemitter_rom_00400CA8)(u8, f32);
    /* 0x3C */ f32 (*func_uvemitter_rom_00400D20)(u8);
    /* 0x40 */ void (*func_uvemitter_rom_00400D48)(u8, f32);
    /* 0x44 */ f32 (*func_uvemitter_rom_00400DC4)(u8);
    /* 0x48 */ void (*func_uvemitter_rom_00400DEC)(u8, f32);
    /* 0x4C */ void (*func_uvemitter_rom_00400E60)(u8, s32);
    /* 0x50 */ void (*func_uvemitter_rom_00400EBC)(u8, ...);
    /* 0x54 */ void (*func_uvemitter_rom_00401010)(u8);
    /* 0x58 */ void (*func_uvemitter_rom_00401070)(u8, s32);
    /* 0x5C */ void (*func_uvemitter_rom_004010CC)(u8);
    /* 0x60 */ void (*func_uvemitter_rom_0040113C)(u8);
    /* 0x64 */ void (*func_uvemitter_rom_0040120C)(u16);
    /* 0x68 */ void (*func_uvemitter_rom_00401DCC)(void);
    /* 0x6C */ void (*func_uvemitter_rom_004023B0)(s32);
    /* 0x70 */ s32 (*func_uvemitter_rom_004026EC)(u8);
    /* 0x74 */ s32 (*func_uvemitter_rom_0040272C)(u8);
    /* 0x78 */ s32 (*func_uvemitter_rom_00402754)(s32 *, s32 *);
    /* 0x7C */ s16 (*func_uvemitter_rom_004027E4)(void);
    /* 0x80 */ void (*func_uvemitter_rom_0040290C)(void);
    /* 0x84 */ void (*func_uvemitter_rom_00402990)(void);
    /* 0x88 */ void (*func_uvemitter_rom_00402998)(u8 *);
    /* 0x8C */ void (*func_uvemitter_rom_004029A4)(u8, u8);
    /* 0x90 */ u8 (*func_uvemitter_rom_004029C0)(u8);
    /* 0x94 */ void (*func_uvemitter_rom_004029D8)(u8, f32);
} UvEmitter_Exports;

#endif /* UVEMITTER_ROM_H */
