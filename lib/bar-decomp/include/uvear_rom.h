#ifndef UVEAR_ROM_H
#define UVEAR_ROM_H
typedef struct UnkStruct_uvear_rom_00400854_s {
    /* 0x00 */ Vec3F vec;
    /* 0x0C */ Vec3F unkC;  /* inferred */
    /* 0x18 */ f32 unk18;   /* inferred */
    /* 0x1C */ f32 unk1C;   /* inferred */
    /* 0x20 */ f32 unk20;   /* inferred */
    /* 0x24 */ f32 unk24;   /* inferred */
    /* 0x28 */ f32 unk28;   /* inferred */
    /* 0x2C */ f32 unk2C;   /* inferred */
    /* 0x30 */ Vec3F unk30; /* inferred */
    /* 0x3C */ char pad3C[4];
    /* 0x40 */ f64 unk40;      /* inferred */
} UnkStruct_uvear_rom_0040085; /* size = 0x48 */

typedef struct UvEar_Rom_Exports_s {
    /* 0x00 */ void (*func_uvear_rom_0040018C)(void);                               /* inferred */
    /* 0x04 */ void (*func_uvear_rom_00400230)(u16, Vec3F);                         /* inferred */
    /* 0x08 */ void (*func_uvear_rom_00400290)(u16, Vec3F *);                       /* inferred */
    /* 0x0C */ void (*func_uvear_rom_004002E0)(u16, Vec3F);                         /* inferred */
    /* 0x10 */ void (*func_uvear_rom_00400340)(u16, Vec3F *);                       /* inferred */
    /* 0x14 */ void (*func_uvear_rom_00400390)(u16, UnkStruct_uvear_rom_0040085 *); /* inferred */
    /* 0x18 */ void (*func_uvear_rom_00400424)(u16, Vec3F);                         /* inferred */
    /* 0x1C */ void (*func_uvear_rom_00400494)(u16, Vec3F *);                       /* inferred */
    /* 0x20 */ void (*func_uvear_rom_004004E4)(u16);                                /* inferred */
    /* 0x24 */ s32 (*func_uvear_rom_0040077C)(s32, Vec3F *);                        /* inferred */
} UvEar_Rom_Exports;                                                                /* size = 0x28 */
#endif /* UVEAR_ROM_H */
