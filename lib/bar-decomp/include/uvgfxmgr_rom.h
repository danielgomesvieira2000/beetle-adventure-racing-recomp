#ifndef UVGFXMGR_ROM_H
#define UVGFXMGR_ROM_H
typedef struct UvGfxMgr_Exports_s {
    /* 0x00 */ void (*func_uvgfxmgr_rom_00400794)(void);
    /* 0x04 */ void (*func_uvgfxmgr_rom_004007F8)(void);
    /* 0x08 */ Gfx** (*uvGetDisplayListHead)(void);
    /* 0x0C */ void (*uvGfxDisplayList)(Gfx*);
    /* 0x10 */ s32 (*func_uvgfxmgr_rom_00400AB8)(s32);
    /* 0x14 */ void (*(*func_uvgfxmgr_rom_00400AF0)(void (*)(void*, void*)))(void*, void*);
    /* 0x18 */ s32 (*func_uvgfxmgr_rom_00400B04)(s32);
    /* 0x1C */ void (*func_uvgfxmgr_rom_00400B18)(s32);
    /* 0x20 */ void (*func_uvgfxmgr_rom_00400B24)(void);
    /* 0x24 */ void (*func_uvgfxmgr_rom_00400F20)(u16);
    /* 0x28 */ void (*func_uvgfxmgr_rom_00400F58)(s32);
    /* 0x2C */ s32 (*func_uvgfxmgr_rom_00400F64)(void);
    /* 0x30 */ void (*func_uvgfxmgr_rom_00400FF8)(f32);
    /* 0x34 */ f32 (*func_uvgfxmgr_rom_00401004)(void);
    /* 0x38 */ void (*uvGfxEnableGamma)(s32);
    /* 0x3C */ void (*func_uvgfxmgr_rom_0040107C)(f32, f32, f32, f32);
    /* 0x40 */ void (*func_uvgfxmgr_rom_00401370)(s8, s8, s8, s8);
    /* 0x44 */ void (*func_uvgfxmgr_rom_00401430)(f32, f32, f32, f32);
    /* 0x48 */ void (*func_uvgfxmgr_rom_004016AC)(s8, s8, s8, s8);
    /* 0x4C */ void (*func_uvgfxmgr_rom_00401714)(s32, s32, u32, s32, s32, s32);
    /* 0x50 */ void (*func_uvgfxmgr_rom_00401788)(u8, u8, u8);
    /* 0x54 */ void (*func_uvgfxmgr_rom_00401914)(void);
    /* 0x58 */ void (*func_uvgfxmgr_rom_00401BD4)(s32, s32, s32, s32);
    /* 0x5C */ void (*func_uvgfxmgr_rom_00401C5C)(s32);
    /* 0x60 */ void (*func_uvgfxmgr_rom_00401D94)(s16);
    /* 0x64 */ void (*func_uvgfxmgr_rom_00401DC4)(void);
    /* 0x68 */ void* (*func_uvgfxmgr_rom_0040204C)(void);
    /* 0x6C */ s32 (*func_uvgfxmgr_rom_00402058)(void);
    /* 0x70 */ s32 (*func_uvgfxmgr_rom_00402074)(void);
    /* 0x74 */ void (*func_uvgfxmgr_rom_00402090)(void);
    /* 0x78 */ void (*uvCopyFrameBuf)(s32);
    /* 0x7C */ void (*uvSetScreenHeight)(s32);
    /* 0x80 */ void (*uvSetScreenWidth)(s32);
    /* 0x84 */ s32 (*uvGetScreenHeight)(void);
    /* 0x88 */ s32 (*uvGetScreenWidth)(void);
    /* 0x8C */ void (*func_uvgfxmgr_rom_004021C8)(void);
    /* 0x90 */ void (*uvGfxSetDepthSource)(s32);
    /* 0x94 */ void (*uvGfxPrimDepth)(s32);
} UvGfxMgr_Exports;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#endif /* UVGFXMGR_ROM_H */
