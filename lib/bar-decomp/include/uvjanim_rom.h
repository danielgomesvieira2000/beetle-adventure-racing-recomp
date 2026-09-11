#ifndef UVJANIM_ROM_H
#define UVJANIM_ROM_H

typedef struct UnkStruct_UvJanim_rom_00400144_s {
    /* 0x00 */ Quat *unk0;
    /* 0x04 */ Vec3F *unk4;
    /* 0x08 */ Vec3F *unk8;
    /* 0x0C */ u8 *unkC;
    /* 0x10 */ u8 unk10;
    /* 0x11 */ u8 unk11;
    /* 0x12 */ u16 unk12;
} UnkStruct_UvJanim_rom_00400144;

typedef struct UnkStruct_uvjanim_rom_00401704_s {
    s32 unk0;
    char pad[0x8];
    s32 unkC;
    s8 unk10;
} UnkStruct_uvjanim_rom_00401704;

typedef struct UvJanim_Exports_s {
    /* 0x00 */ void (*func_uvjanim_rom_004000F0)(void);
    /* 0x04 */ void (*func_uvjanim_rom_00400144)(UnkStruct_UvJanim_rom_00400144 *,
                                                 UnkStruct_UvJanim_rom_00400144 *);
    /* 0x08 */ void (*func_uvjanim_rom_00400278)(s32, UnkStruct_UvJanim_rom_00400144 *);
    /* 0x0C */ void (*func_uvjanim_rom_00400434)(UnkStruct_UvJanim_rom_00400144 *,
                                                 UnkStruct_UvJanim_rom_00400144 *,
                                                 UnkStruct_UvJanim_rom_00400144 *, f32);
    /* 0x10 */ void (*func_uvjanim_rom_00400C0C)(s32, s32, f32);
    /* 0x14 */ void (*func_uvjanim_rom_00400CFC)(UnkStruct_UvJanim_rom_00400144 *, s32, f32);
    /* 0x18 */ void (*func_uvjanim_rom_00401288)(s32, s32, f32, f32);
    /* 0x1C */ void (*func_uvjanim_rom_00401380)(UnkStruct_UvJanim_rom_00400144 *, s32, f32, f32);
    /* 0x20 */ void (*func_uvjanim_rom_00401704)(s32, UnkStruct_uvjanim_rom_00401704 *); /* inferred */
} UvJanim_Exports; /* size = 0x20 */

#endif /* UVJANIM_ROM_H */

