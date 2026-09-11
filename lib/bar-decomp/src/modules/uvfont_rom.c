// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

typedef struct FontMessage_s {
    s32 x;
    s32 y;
    f32 scaleX;
    f32 scaleY;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    s16 str16[50];
    ParsedUVFT *font;
} FontMessage; // size = 0x70


extern ParsedUVFT *sParsedFont;
extern UvString_Exports *sUvStringExports;
extern UvGfxMgr_Exports *sUvGfxMgrExports;
extern UvCback_Exports *sUvCbackExports;
extern UvGfxState_Rom_Exports *sUvGfxStateExports;
extern UvSprt_Rom_Exports *sUvSpriteExports;
extern s32 sFontCurWidth; // sFontCurWidth
extern f32 sFontScaleX;   // sFontScaleX
extern f32 sFontScaleY;   // sFontScaleY
extern u8 sFontColorRed;
extern u8 sFontColorGreen;
extern u8 sFontColorBlue;
extern u8 sFontColorAlpha;
extern Sprite sFontSprite;
extern s32 sFontMesgCount;
extern FontMessage sFontMessages[];
extern s32 D_uvfont_rom_00402558;

void uvModuleCleanup(void);
void uvSetFont(s32 id);
void uvFontScale(f64 arg0, f64 arg1);
void uvFontColor(u8 red, u8 green, u8 blue, u8 alpha);
s32 uvFontStrlLen(s32 arg0);
s32 uvFontStr16WidthFont(s32 arg0);
s32 _uvFontStrWidth(ParsedUVFT *font, const char *str);
s32 uvFontWidth(const char *str);
s32 uvFontHeight(void);
s32 uvFontPrintStr16(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void uvFontPrintStr(s32 x, s32 y, char *str);
s32 _uvFontSpriteWidth(Sprite *sprite, s16 *str16, ParsedUVFT *font);
void uvFontMsgGenDlist(Gfx **gdl, FontMessage *fontMesg);
void func_uvfont_rom_00400840(s32 arg0);
void uvFontGenDList(void);

void __entrypoint_func_uvfont_rom_400000(UvFont_Exports *exports);

void __entrypoint_func_uvfont_rom_400000(UvFont_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->uvModuleCleanup = uvModuleCleanup;
    exports->uvFontGenDList = uvFontGenDList;
    exports->uvSetFont = uvSetFont;
    exports->uvFontScale = uvFontScale;
    exports->uvFontColor = uvFontColor;
    exports->uvFontStrlLen = uvFontStrlLen;
    exports->uvFontStr16WidthFont = uvFontStr16WidthFont;
    exports->uvFontWidth = uvFontWidth;
    exports->uvFontHeight = uvFontHeight;
    exports->uvFontPrintStr16 = uvFontPrintStr16;
    exports->uvFontPrintStr = uvFontPrintStr;
#line 59
    sUvStringExports = uvLoadModule('STRG');
    sUvGfxMgrExports = uvLoadModule('GMGR');
    sUvCbackExports = uvLoadModule('CBCK');
    sUvGfxStateExports = uvLoadModule('STAT');
    sUvSpriteExports = uvLoadModule('SPRT');
    sUvCbackExports->func_uvcback_rom_0040016C(sUvGfxMgrExports->func_uvgfxmgr_rom_00400AB8(1),
                                               (s32) func_uvfont_rom_00400840, 0, 0);
}

void uvModuleCleanup(void) {
    sUvCbackExports->func_uvcback_rom_00400320(sUvGfxMgrExports->func_uvgfxmgr_rom_00400AB8(1),
                                               (s32) func_uvfont_rom_00400840);
    uvUnloadModule('STRG');
    uvUnloadModule('GMGR');
    uvUnloadModule('CBCK');
    uvUnloadModule('STAT');
}

void uvSetFont(s32 id) {
    if (((sParsedFont = uvGetLoadedFile('UVFT', id)) != 0)
        || (((sParsedFont = uvLoadFile('UVFT', id)) != 0))) {
        sFontCurWidth = sParsedFont->bitmap->width;
    }
}

void uvFontScale(f64 x, f64 y) {
    sFontScaleX = x;
    sFontScaleY = y;
}

void uvFontColor(u8 red, u8 green, u8 blue, u8 alpha) {
    sFontColorRed = red;
    sFontColorGreen = green;
    sFontColorBlue = blue;
    sFontColorAlpha = alpha;
}

s32 uvFontStrlLen(s32 arg0) {
    return 0;
}

s32 uvFontStr16WidthFont(s32 arg0) {
    return 0;
}

s32 _uvFontStrWidth(ParsedUVFT *font, const char *str) {
    s32 len;
    char *ch;
    s32 width;
    s32 i;
    s32 j;

    len = sUvStringExports->uvStrlen(str);

    width = 0;
    for (i = 0, j = 0; i < len; i++, j++) {
        if (str[j] == 0) {
            return width;
        }
        if (str[j] == '\n') {
            continue;
        }
        ch = sUvStringExports->uvStrChr(font->str, str[j]);
        if (ch != 0) {
            width += font->bitmap[ch - font->str].width;
        } else {
            width += sFontCurWidth;
        }
    }

    return width * sFontScaleX;
}

s32 uvFontWidth(const char *str) {
    return _uvFontStrWidth(sParsedFont, str);
}

s32 uvFontHeight(void) {
    return (s32) ((f32) sParsedFont->bitmap->actualHeight * sFontScaleY);
}

s32 uvFontPrintStr16(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    return 0;
}

#ifdef NON_MATCHING
void uvFontPrintStr(s32 x, s32 y, char *str) {
    char *chrPos;
    s32 i;
    char *s1;
    s32 len;
    UvString_Exports *t1;

    y += (s32) (sParsedFont->bitmap->actualHeight * sFontScaleY);
    y = sUvGfxMgrExports->uvGetScreenHeight() - y;
    sFontMessages[sFontMesgCount].x = x;
    sFontMessages[sFontMesgCount].y = y;
    sFontMessages[sFontMesgCount].r = sFontColorRed;
    sFontMessages[sFontMesgCount].g = sFontColorGreen;
    sFontMessages[sFontMesgCount].b = sFontColorBlue;
    sFontMessages[sFontMesgCount].a = sFontColorAlpha;
    sFontMessages[sFontMesgCount].scaleX = sFontScaleX;
    sFontMessages[sFontMesgCount].scaleY = sFontScaleY;
    if (1)
        ;
    len = sUvStringExports->uvStrlen(str);
    if (len > 0x32) {
        str[0x31] = 0;
        len = 0x32;
    }

    for (i = 0; i < len; i++) {
        chrPos = sUvStringExports->uvStrChr(sParsedFont->str, str[i]);
        if (chrPos != NULL) {
            sFontMessages[sFontMesgCount].str16[i] = chrPos - sParsedFont->str;
        } else {
            sFontMessages[sFontMesgCount].str16[i] = -2;
        }
    }

    sFontMessages[sFontMesgCount].str16[len] = -1;
    sFontMessages[sFontMesgCount].font = sParsedFont;
    if (sFontMesgCount < 0x31) {
        sFontMesgCount++;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvfont_rom/uvFontPrintStr.s")
#endif

s32 _uvFontSpriteWidth(Sprite *sprite, s16 *str16, ParsedUVFT *font) {
    s32 width;
    s32 i;
    Bitmap *bitmap;

    bitmap = sprite->bitmap;
    for (i = 0, width = 0; str16[i] != -1; i++) {
        if (!(i < 0x32)) {
            break;
        }
        if ((str16[i] == -2) || (str16[i] == -3)) {
            bitmap[i] = font->bitmap[0];
            bitmap[i].buf = NULL;
            width += font->bitmap[0].width;
        } else {
            bitmap[i] = font->bitmap[str16[i]];
            width += font->bitmap[str16[i]].width;
        }
    }
    sprite->nbitmaps = i;
    return width;
}

void uvFontMsgGenDlist(Gfx **gdl, FontMessage *fontMesg) {
    sFontSprite.height = fontMesg->font->bitmap->actualHeight;
    sFontSprite.width = _uvFontSpriteWidth(&sFontSprite, fontMesg->str16, fontMesg->font);
    sFontSprite.bmfmt = fontMesg->font->bmfmt;
    sFontSprite.bmsiz = fontMesg->font->bmsiz;
    sFontSprite.x = fontMesg->x;
    sFontSprite.y = fontMesg->y;
    sFontSprite.red = fontMesg->r;
    sFontSprite.green = fontMesg->g;
    sFontSprite.blue = fontMesg->b;
    sFontSprite.alpha = fontMesg->a;
    sFontSprite.scalex = fontMesg->scaleX;
    sFontSprite.scaley = fontMesg->scaleY;
    sFontSprite.rsp_dl = *gdl;
    sFontSprite.rsp_dl_next = NULL;
    if (sUvSpriteExports->uvSpriteDraw(&sFontSprite) != NULL) {
        *gdl = sFontSprite.rsp_dl_next;
        *gdl = *gdl - 1;
    }
}

#ifdef NEEDS_DATA
extern s32 D_uvfont_rom_004009F0;
extern s32 D_uvfont_rom_00402558;

void func_uvfont_rom_00400840(s32 arg0) {
    static s32 D_uvfont_rom_004009F0;

    D_uvfont_rom_004009F0++;
    if (D_uvfont_rom_004009F0 != 0) {
        D_uvfont_rom_004009F0 = -1;
        D_uvfont_rom_00402558 = 0;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvfont_rom/func_uvfont_rom_00400840.s")
#endif

void uvFontGenDList(void) {
    Gfx **gdl;
    s32 i;

    if (sFontMesgCount == 0) {
        return;
    }

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();
    sUvSpriteExports->uvSpriteDrawInit(gdl);
    for (i = 0; i < sFontMesgCount; i++) {
        uvFontMsgGenDlist(gdl, &sFontMessages[i]);
    }

    sUvSpriteExports->uvSpriteDrawFinish(gdl);
    *gdl -= 1;
    D_uvfont_rom_00402558 += sFontMesgCount;
    sFontMesgCount = 0;
    if (sUvGfxStateExports != NULL) {
        sUvGfxStateExports->func_uvgfxstate_rom_004022B0();
    }
}
