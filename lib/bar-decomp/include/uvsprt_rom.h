#ifndef UVSPRT_ROM_H
#define UVSPRT_ROM_H
typedef struct UvSprt_Rom_Exports_s {
    /* 0x00 */ void (*uvModuleCleanup)(void);                      /* inferred */
    /* 0x04 */ s32 (*func_uvsprt_rom_004001EC)(void);                       /* inferred */
    /* 0x08 */ void (*func_uvsprt_rom_0040023C)(s32);                   /* inferred */
    /* 0x0C */ void (*uvSpriteDrawAll)(void);                      /* inferred */
    /* 0x10 */ void (*uvSpriteRender)(s32);                  /* inferred */
    /* 0x14 */ s16 (*uvGetSpriteWidth)(s32);                   /* inferred */
    /* 0x18 */ s16 (*uvGetSpriteHeight)(s32);                   /* inferred */
    /* 0x1C */ void (*uvSprtProps)(s32, ...);             /* inferred */
    /* 0x20 */ void (*uvSpriteDrawInit)(Gfx**);                /* inferred */
    /* 0x24 */ Gfx* (*uvSpriteDraw)(Sprite*);             /* inferred */
    /* 0x28 */ void (*uvSpriteDrawFinish)(Gfx**);                /* inferred */
} UvSprt_Rom_Exports;                               /* size = 0x2C */


// Sprite props
#define SPRT_PROP_SCALE(x, y) 1, (x), (y)
#define SPRT_PROP_POS(x, y) 2, (x), (y)
#define SPRT_PROP_3(x) 3, (x)
#define SPRT_PROP_6(x) 6, (x)
#define SPRT_PROP_COLOR(r, g, b, a) 7, (r), (g), (b), (a)
#define SPRT_PROP_BLIT(x) 9, (x)
#define SPRT_PROP_FAST_COPY(x) 10, (x)
#define SPRT_PROP_TRANSPARENT(x) 11, (x)
#define SPRT_PROP_CUTOUT(x) 12, (x)
#define SPRT_PROP_EXTERN(x) 14, (x)
#define SPRT_PROP_END 0

#endif /* UVSPRT_ROM_H */
