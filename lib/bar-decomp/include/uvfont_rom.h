#ifndef UVFONT_ROM_H
#define UVFONT_ROM_H

typedef struct ParsedUVFT {
    char *str;
    s32 unk4;
    u8 bmfmt;
    u8 bmsiz;
    Bitmap *bitmap;
    void *imag[0x28];
} ParsedUVFT; // size = 0x80

typedef struct UvFont_Exports_s {
    /* 0x00 */ void (*uvModuleCleanup)(void);
    /* 0x04 */ void (*uvSetFont)(s32);
    /* 0x08 */ void (*uvFontScale)(f64, f64);
    /* 0x0C */ void (*uvFontColor)(u8, u8, u8, u8);
    /* 0x10 */ s32 (*uvFontStrlLen)(s32);
    /* 0x14 */ s32 (*uvFontStr16WidthFont)(s32);
    /* 0x18 */ s32 (*uvFontWidth)(const char *);
    /* 0x1C */ s32 (*uvFontHeight)(void);
    /* 0x20 */ s32 (*uvFontPrintStr16)(s32, s32, s32, s32);
    /* 0x24 */ void (*uvFontPrintStr)(s32, s32, char *);
    /* 0x28 */ void (*uvFontGenDList)(void);
} UvFont_Exports;
#endif /* UVFONT_ROM_H */
