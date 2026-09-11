#ifndef UVMTX_ROM_H
#define UVMTX_ROM_H

typedef struct UvFMtx_Rom_Exports_s {
    /* 0x000 */ void (*func_00400310)();
    /* 0x004 */ void (*func_0040034C)(f32, f32, f32);
    /* 0x008 */ void (*func_00400370)(Mtx4F*, Mtx4F*);
    /* 0x00C */ void (*func_00400504)(Mtx4F*, Mtx4F*);
    /* 0x010 */ void (*func_00400588)(Mtx4F*, Mtx4F*);
    /* 0x014 */ void (*func_004005D4)(Mtx4F*, Mtx);
    /* 0x018 */ void (*func_00400894)(Mtx*, Mtx4F*);
    /* 0x01C */ void (*func_00400B68)(Mtx4F*);
    /* 0x020 */ void (*func_00400BB8)(Mtx4F*, Mtx4F*, Mtx4F*);
    /* 0x024 */ void (*func_00400FF8)(s32);
    /* 0x028 */ void (*func_00401000)(Mtx4F*, f32, s8);
    /* 0x02C */ void (*func_00401408)(Mtx4F*, f32, f32, f32);
    /* 0x030 */ void (*func_004014D0)(Mtx4F*, f32, f32, f32);
    /* 0x034 */ void (*func_00401580)(Mtx4F*, f32, f32, f32, f32, f32, f32, u8, u8, u8, f32, f32, f32);
    /* 0x038 */ void (*func_00401604)(Mtx4F*, Mtx4F*);
    /* 0x03C */ s32 (*func_00401790)(Mtx4F*, Mtx4F*);
    /* 0x040 */ void (*func_00401B88)(Mtx4F*, Mtx4F*);
    /* 0x044 */ void (*func_00401C3C)(Mtx4F*, Mtx4F*, f32, f32);
    /* 0x048 */ void (*func_00401D0C)(Mtx4F*, Vec3F*, Vec3F*);
    /* 0x04C */ void (*func_00401DA8)(Mtx4F*, Mtx4F*, Mtx4F*);
    /* 0x050 */ void (*func_00401F74)(Mtx4F*, f32, f32, f32, f32, f32, f32);
    /* 0x054 */ void (*func_00402038)(Mtx4F*, f32, f32, f32, f32, f32, f32);
    /* 0x058 */ void (*func_00402110)(Mtx4F*, f32, f32, f32, f32);
    /* 0x05C */ void (*func_004021E0)(Mtx4F*, f32, f32, f32, f32, f32, f32, f32, f32, f32);
    /* 0x060 */ void (*func_0040246C)(Mtx4F*, f32, f32, f32, f32);
    /* 0x064 */ void (*func_00402628)(Mtx4F*, Mtx4F*, f32, f32);
    /* 0x068 */ void (*func_004027CC)(Mtx4F*, u16);
    /* 0x06C */ void (*func_00402858)(Mtx4F*);
    /* 0x070 */ void (*uvGfxMtxViewPop)();
    /* 0x074 */ void (*func_00402908)(Mtx4F*);
    /* 0x078 */ void (*func_004029B8)(Mtx4F*);
    /* 0x07C */ void (*func_004029DC)(Mtx4F*);
    /* 0x080 */ void (*func_00402B30)(Mtx4F*, f32, f32, f32);
    /* 0x084 */ void (*func_00402D40)(Mtx4F*, f32, f32, f32);
    /* 0x088 */ void (*func_00402D84)(Vec3F*, Vec3F*);
    /* 0x08C */ void (*func_00402DB0)(Vec3F*, Vec3F*);
    /* 0x090 */ void (*func_00402DFC)(Vec3F*, Mtx4F*);
    /* 0x094 */ void (*func_00402E2C)(Vec3F*, Mtx4F*);
    /* 0x098 */ void (*func_00402E7C)(Vec3F*, Mtx4F*);
    /* 0x09C */ void (*func_00402EAC)(Vec3F*, Mtx4F*);
    /* 0x0A0 */ void (*func_00402EFC)(Vec3F*, Mtx4F*);
    /* 0x0A4 */ void (*func_00402F2C)(Vec3F*, Mtx4F*);
    /* 0x0A8 */ void (*func_00402F5C)(Mtx4F*, f32*, f32*, f32*);
    /* 0x0AC */ void (*func_00403020)(Mtx4F*, Vec3F*, Vec3F*);
    /* 0x0B0 */ void (*func_004030FC)(Mtx4F*, Vec3F*, Vec3F*);
    /* 0x0B4 */ f32 (*func_00403180)(Mtx4F*, Vec3F*, Vec3F*);
    /* 0x0B8 */ void (*func_00403248)(Mtx4F*, Vec3F*, Vec3F*);
    /* 0x0BC */ void (*func_00403290)(Mtx4F*, Mtx4F*, Mtx4F*);
    /* 0x0C0 */ void (*func_00403480)(Mtx4F*, Vec3F*, f32, f32, f32, f32);
    /* 0x0C4 */ void (*func_00403568)(Mtx4F*, Vec3F*, Mtx4F*, f32, f32);
    /* 0x0C8 */ void (*func_00403648)(Mtx4F*);
} UvFMtx_Rom_Exports;
#endif /* UVMTX_ROM_H */
