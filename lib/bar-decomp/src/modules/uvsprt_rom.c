// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "stdarg.h"
#include <PR/sp.h>
#include "uvasset_types.h"

#define MY_K0 (175 & 0x1ff)
#define MY_K1 (-43 & 0x1ff)
#define MY_K2 (-89 & 0x1ff)
#define MY_K3 (222 & 0x1ff)
#define MY_K4 (114 & 0x1ff)
#define MY_K5 (42 & 0x1ff)
#define SPRITE_SURF G_RM_TEX_EDGE
#define SPRITE_SURF2 G_RM_TEX_EDGE2

typedef struct uvSprite_s {
    /* 00 */ u8 unk0; // Flag to draw the sprite
    /* 01 */ u8 unk1;
    /* 02 */ u8 unk2; // Another flag to draw the sprite?
    /* 04 */ s16 height;
    /* 06 */ s16 unk6;
    /* 08 */ u16 textureId;
    /* 0A */ u16 unkA;
    /* 0xC */ Bitmap *bitmap;
    /* 10 */ Sprite sprite;
} uvSprite_t; // size 0x54

typedef struct UnkStruct_uvsprt_rom_004033CC_s {
    char pad[0x58];
    s32 (*unk58)(void);
} UnkStruct_uvsprt_rom_004033CC;

void __entrypoint_func_uvsprt_rom_400298(UvSprt_Rom_Exports* exports);
void uvSpriteFromBitmap(uvSprite_t *sprite, ParsedUVTX *uvtx);
void func_uvsprt_rom_004000A4(uvSprite_t *sprite, ParsedUVTX *uvtx);
s32 func_uvsprt_rom_004001EC(void);
void func_uvsprt_rom_0040023C(s32 arg0);
void uvModuleCleanup(void);
void uvSpriteDisplayList(Gfx **gdlh, uvSprite_t *sp);
void uvSpriteDrawAll(void);
void uvSpriteRender(s32 arg0);
void uvSpriteSetBlit(uvSprite_t *sprite, s32 blitId);
s16 uvGetSpriteWidth(s32 arg0);
s16 uvGetSpriteHeight(s32 arg0);
void uvSprtProps(s32 arg0, ...);
void uvDrawBitmap(Gfx **gdl, Sprite *s, Bitmap *b, s32 x, s32 y, s32 xx, s32 yy, s32 fs,
                              s32 ft, s32 sx, s32 sy);
void uvSpriteDrawInit(Gfx **glistp);
Gfx* uvSpriteDraw(Sprite* sp);
void uvSpriteDrawFinish(Gfx **glistp);

// .data
s32 *sPrevBitmapBuf = NULL;
u16 sSpriteAttribute = 0;

s32 sUnkModuleInfo[] = {0x002C0000, &__entrypoint_func_uvsprt_rom_400298};

// .bss
extern s32 sScissorXmax;
extern s32 sScissorYmax;
extern s32 sScissorXmin;
extern s32 sScissorYmin;
extern UvGfxMgr_Exports *sUvGfxMgrExports;
extern UnkStruct_uvsprt_rom_004033CC *sUvGfxStateExports;
extern uvSprite_t *sSprites;
extern s32 sMaxSprites;

void uvSpriteFromBitmap(uvSprite_t *sprite, ParsedUVTX *uvtx) {
    Bitmap *bitmap;

    if ((u16) sprite->textureId == 0xFFFF) {
        sprite->bitmap = NULL;
    }
    bitmap = sprite->bitmap;
    if (bitmap == NULL) {
        bitmap = _uvMemAllocAlign8(sizeof(Bitmap));
        sprite->bitmap = bitmap;
    }
    bitmap->width = uvtx->width - 1;
    sprite->bitmap->width_img = (s16) uvtx->width;
    sprite->bitmap->s = 0;
    sprite->bitmap->t = 0;
    sprite->bitmap->buf = *(s32*)(uvtx->data); // TODO: What's happening here?
    sprite->bitmap->actualHeight = (s16) uvtx->height;
    sprite->bitmap->LUToffset = 0;
}

void func_uvsprt_rom_004000A4(uvSprite_t *sprite, ParsedUVTX *uvtx) {
    Sprite *sp = &sprite->sprite;

    if (uvtx != NULL) {
        sp->width = uvtx->width - 1;
        sp->height = uvtx->height - 1;
        sp->bmheight = uvtx->height - 1;
        sp->bmHreal = (s16) uvtx->height;
        sp->bmfmt = G_IM_FMT_RGBA;
        sp->bmsiz = G_IM_SIZ_16b;
        sp->attr = 0x200;

        if (uvtx->unk21 != 1) {
            sp->attr |= SP_TRANSPARENT;
        }
        if (uvtx->unk21 == 1) {
            sp->bmfmt = G_IM_FMT_I;
        } else if (uvtx->unk21 == 2) {
            sp->bmfmt = G_IM_FMT_IA;
        } else {
            sp->bmfmt = 0;
        }
        if (uvtx->unk24 == 4) {
            sp->bmsiz = G_IM_SIZ_4b;
        } else if (uvtx->unk24 == 8) {
            sp->bmsiz = G_IM_SIZ_8b;
        } else {
            sp->bmsiz = G_IM_SIZ_16b;
        }
        sp->scalex = 1.0f;
        sp->scaley = 1.0f;
    }

    sp->expx = 0;
    sp->expy = 0;
    sp->zdepth = 0x1234;
    sp->red = 0xFF;
    sp->green = 0xFF;
    sp->blue = 0xFF;
    sp->alpha = 0xFF;
    sp->startTLUT = 0;
    sp->nTLUT = 0;
    sp->LUT = NULL;
    sp->istart = 0;
    sp->istep = 1;
    sp->nbitmaps = 1;
    sp->ndisplist = 0x24;
    sp->scalex = 1.0f;
    sp->scaley = 1.0f;
    sp->bitmap = sprite->bitmap;
    sp->frac_s = 0;
    sp->frac_t = 0;
}

s32 func_uvsprt_rom_004001EC(void) {
    s32 i;

    for (i = 0; i < sMaxSprites; i++) {
        if (sSprites[i].unk1 == 0) {
            sSprites[i].unk1 = 1;
            return i;
        }
    }

    return -1;
}

void func_uvsprt_rom_0040023C(s32 arg0) {
    sSprites[arg0].unk0 = 0;
    sSprites[arg0].unk1 = 0;
}

void uvModuleCleanup(void) {
    uvUnloadModule('GMGR');
}

void __entrypoint_func_uvsprt_rom_400298(UvSprt_Rom_Exports* exports) {
    s32 *spriteCount;
    uvSprite_t *sp;
    s32 i;

    uvUpdateFileAllocPtr(exports);
    exports->uvModuleCleanup = uvModuleCleanup;
    exports->uvSpriteDrawFinish = uvSpriteDrawFinish;
    exports->func_uvsprt_rom_004001EC = func_uvsprt_rom_004001EC;
    exports->func_uvsprt_rom_0040023C = func_uvsprt_rom_0040023C;
    exports->uvSpriteDrawAll = uvSpriteDrawAll;
    exports->uvSpriteRender = uvSpriteRender;
    exports->uvGetSpriteWidth = uvGetSpriteWidth;
    exports->uvGetSpriteHeight = uvGetSpriteHeight;
    exports->uvSprtProps = uvSprtProps;
    exports->uvSpriteDrawInit = uvSpriteDrawInit;
    exports->uvSpriteDraw = uvSpriteDraw;
    spriteCount = uvGetSystemProp(6);
    if (spriteCount == NULL) {
        sMaxSprites = 20;
    } else {
        if (*spriteCount != 0) {
            sMaxSprites = *spriteCount;
        } else {
            sMaxSprites = 20;
        }
    }
    sUvGfxMgrExports = uvLoadModule('GMGR');
    sUvGfxStateExports = uvLoadModule('STAT');
    sSprites = _uvMemAllocAlign8(sMaxSprites * sizeof(uvSprite_t));
    uvMemSet(sSprites, 0, sMaxSprites * sizeof(uvSprite_t));

    for (i = 0; i < sMaxSprites; i++) {
        sp = i + sSprites;
        sp->unk0 = 0;
        sp->unk1 = 0;
        sp->unk2 = 0;
        sp->sprite.red = sp->sprite.green = sp->sprite.blue = sp->sprite.alpha = 255;
        sp->textureId = 0xFFF;
        sp->bitmap = NULL;
    }
}

void uvSpriteDisplayList(Gfx **gdlh, uvSprite_t *sp) {
    Sprite *sprite = &sp->sprite;

    sprite->rsp_dl = *gdlh;
    sprite->attr &= 0xFFDF;
    sprite->rsp_dl_next = NULL;

    if (uvSpriteDraw(sprite) != NULL) {
        *gdlh = sprite->rsp_dl_next;
        *gdlh = *gdlh - 1;
    }
}

void uvSpriteDrawAll(void) {
    Gfx** gdlh;
    s32 i;
    uvSprite_t* temp_a1;

    gdlh = sUvGfxMgrExports->uvGetDisplayListHead();
    uvSpriteDrawInit(gdlh);
    for (i = 0; i < sMaxSprites; i++) {
        temp_a1 = sSprites + i;
        if ((temp_a1->unk0 == 0) || (temp_a1->unk2 == 0)) {
            continue;
        } else if (temp_a1->textureId == 0xFFF) {
            continue;
        }
        uvSpriteDisplayList(gdlh, temp_a1);
    }

    uvSpriteDrawFinish(gdlh);
    *gdlh -= 1;
    if (sUvGfxStateExports != NULL) {
        sUvGfxStateExports->unk58();
    }
}

void uvSpriteRender(s32 arg0) {
    uvSprite_t *uvSprite;
    Gfx **gdlh;

    if (arg0 < sMaxSprites) {
        uvSprite = &sSprites[arg0];
        if ((u16) uvSprite->textureId != 0xFFF) {
            gdlh = sUvGfxMgrExports->uvGetDisplayListHead();
            uvSpriteDrawInit(gdlh);
            uvSpriteDisplayList(gdlh, uvSprite);
            uvSpriteDrawFinish(gdlh);
            *gdlh -= 1;
            sUvGfxStateExports->unk58();
        }
    }
}

void uvSpriteSetBlit(uvSprite_t *sprite, s32 blitId) {
    ParsedUVBT *uvbt;
    Sprite *sp = &sprite->sprite;
    s16 temp_v1;
    s16 temp_v1_2;

    uvbt = uvGetLoadedFile('UVBT', (uvSprite_t *) blitId);
    if ((uvbt != NULL) || (uvbt = uvLoadFile('UVBT', blitId), (uvbt != NULL))) {
        sprite->sprite.bitmap = uvbt->bitmap;
        sprite->textureId = -1;
        sprite->sprite.width = uvbt->width;
        sprite->sprite.height = uvbt->height;
        sprite->sprite.bitmap = uvbt->bitmap;
        sprite->sprite.nbitmaps = uvbt->nbitmaps;
        sprite->sprite.ndisplist = (uvbt->nbitmaps * 0xC) + 0x18;
        sprite->sprite.bmheight = uvbt->texelHeight;
        sprite->sprite.bmHreal = uvbt->texelHeight;
        sprite->sprite.bmfmt = (u8) uvbt->bmfmt;
        switch (uvbt->bitdepth) { /* irregular */
            case 4:
                sprite->sprite.bmsiz = 0;
                break;
            case 8:
                sprite->sprite.bmsiz = 1;
                break;
            case 16:
                sprite->sprite.bmsiz = 2;
                break;
            case 32:
                sprite->sprite.bmsiz = 3;
                break;
        }
        sprite->sprite.istart = 0;
        sprite->sprite.istep = 1;
        switch (uvbt->bmfmt) {
            case G_IM_FMT_RGBA:
                if (uvbt->bitdepth == 16) {
                    sp->attr = SP_TEXSHUF;
                } else {
                    sp->attr = 0; // maybe SP_TEXSHIFT?
                }
                break;
            case G_IM_FMT_IA:
            case G_IM_FMT_I:
                sp->attr = SP_TEXSHUF;
                break;
            default:
                //_uvDebugPrintf("Sprite Format %d is not supported.\n", uvbt->bmfmt);
                break;
        }
        sp->expx = 0;
        sp->expy = 0;
        sp->zdepth = 0x1234;
        sp->red = 0xFF;
        sp->green = 0xFF;
        sp->blue = 0xFF;
        sp->alpha = 0xFF;
        sp->startTLUT = 0;
        sp->nTLUT = 0;
        sp->LUT = NULL;
        sp->frac_s = 0;
        sp->frac_t = 0;
        sp->scalex = 1.0f;
        sp->scaley = 1.0f;
    }
}

s16 uvGetSpriteWidth(s32 spriteId) {
    if (spriteId >= sMaxSprites) {
        return 0;
    }
    return sSprites[spriteId].sprite.width;
}

s16 uvGetSpriteHeight(s32 spriteId) {
    if (spriteId >= sMaxSprites) {
        return 0;
    }
    return sSprites[spriteId].sprite.height;
}

void uvSprtProps(s32 spriteId, ...) {
    ParsedUVTX* uvVtx;
    Sprite* sprite;
    s32 scaleY;
    s32 scaleX;
    s32 prop;
    uvSprite_t* uvSprite;
    va_list args;

    if (spriteId >= sMaxSprites) {
        return;
    }

    va_start(args, spriteId);

    uvSprite = &sSprites[spriteId];
    sprite = &uvSprite->sprite;

    while (TRUE) {
        prop = va_arg(args, s32);
        switch (prop) {
            case 0:
                return;
            case 1:
                scaleX = va_arg(args, s32);
                if (scaleX <= 0) {
                    scaleX = 1;
                }

                scaleY = va_arg(args, s32);
                if (scaleY <= 0) {
                    scaleY = 1;
                }

                if (sprite->width > 0) {
                    sprite->scalex = scaleX / (f32)sprite->width;
                }

                if (sprite->height > 0) {
                    sprite->scaley = scaleY / (f32)sprite->height;
                }
                if ((sprite->scalex == 1.0f) && (sprite->scaley == 1.0f)) {
                    sprite->attr &= ~SP_SCALE;
                } else {
                    sprite->attr |= SP_SCALE;
                }
                break;
            case 13:
                sprite->scalex = va_arg(args, f32);
                sprite->scaley = va_arg(args, f32);
                if ((sprite->scalex == 1.0f) && (sprite->scaley == 1.0f)) {
                    sprite->attr &= ~SP_SCALE;
                } else {
                    sprite->attr |= SP_SCALE;
                }
                break;
            case 2:
                sprite->x = va_arg(args, s32);
                sprite->y = (sUvGfxMgrExports->uvGetScreenHeight() - va_arg(args, s32)) - sprite->height;
                break;
            case 7:
                sprite->red = va_arg(args, s32);
                sprite->green = va_arg(args, s32);
                sprite->blue = va_arg(args, s32);
                sprite->alpha = va_arg(args, s32);
                break;
            case 3:
                uvSprite->unk0 = va_arg(args, s32);
                break;
            case 6:
                uvSprite->unk2 = va_arg(args, s32);
                break;
            case 9:
                uvSpriteSetBlit(uvSprite, va_arg(args, s32));
                break;
            case 10:
                uvSprite->textureId = va_arg(args, s32);
                if (uvSprite->textureId != 0xFFF) {
                    uvVtx = uvGetLoadedFile('UVTX', uvSprite->textureId);
                    if (uvVtx == NULL) {
                        uvSprite->textureId = 0xFFF;
                    } else {
                        uvSpriteFromBitmap(uvSprite, uvVtx);
                        func_uvsprt_rom_004000A4(uvSprite, uvVtx);
                    }
                }
                break;
            case 11:
                if (va_arg(args, s32) == 0) {
                    sprite->attr &= ~SP_TRANSPARENT;
                } else {
                    sprite->attr |= SP_TRANSPARENT;
                }
                break;
            case 12:
                if (va_arg(args, s32) == 0) {
                    sprite->attr &= ~SP_CUTOUT;
                } else {
                    sprite->attr |= SP_CUTOUT;
                }
                break;
            case 14:
                if (va_arg(args, s32) == 0) {
                    sprite->attr &= ~SP_EXTERN;
                } else {
                    sprite->attr |= SP_EXTERN;
                }
                break;
            default:
                break;
        }
    }
}

void uvDrawBitmap(Gfx **gdl, Sprite *s, Bitmap *b, s32 x, s32 y, s32 xx, s32 yy, s32 fs,
                              s32 ft, s32 sx, s32 sy) {
    s32 rs, rt;
    s32 rxh, ryh;
    s32 rxl, ryl;
    Gfx *gdlh;
    s32 tex_width, tex_height;
    Gtexrect gt, *g;
    s32 s_clamp, t_clamp, s_mask, t_mask, s_lod, t_lod;

    g = &gt;

    tex_width = b->width_img;
    tex_height = s->bmHreal;

    gdlh = *gdl;

    /* Scissoring */
    if ((x >= sScissorXmax) || (y >= sScissorYmax)) {
        return;
    }

    if ((xx < sScissorXmin) || (yy < sScissorYmin)) {
        return;
    }

    if (x < sScissorXmin) {
        rxh = sScissorXmin * 4;
        rs = (b->s << 5) + fs + (((sScissorXmin - x) * sx) >> 5);
#ifdef DEBUG_SCISSOR
        emPrintf("Sprite Scissoring: Left Edge (%d) not in range (%d,%d)\n", x, sScissorXmin,
                 sScissorXmax);
#endif
    } else {
        rxh = x * 4;
        rs = (b->s << 5) + fs;
    };

    if (y < sScissorYmin) {
        ryh = sScissorYmin * 4;
        rt = (b->t << 5) + ft + (((sScissorYmin - y) * sy) >> 5);
#ifdef DEBUG_SCISSOR
        emPrintf("Sprite Scissoring: Top Edge (%d) not in range (%d,%d)\n", y, sScissorYmin,
                 sScissorYmax);
#endif
    } else {
        ryh = y * 4;
        rt = (b->t << 5) + ft;
    }

    if (xx >= sScissorXmax) {
        rxl = sScissorXmax * 4;
#ifdef DEBUG_SCISSOR
        emPrintf("Sprite Scissoring: Right Edge (%d) not in range (%d,%d)\n", xx, sScissorXmin,
                 sScissorXmax);
#endif
    } else {
        rxl = xx * 4;
    };

    if (yy >= sScissorYmax) {
        ryl = sScissorYmax * 4;
#ifdef DEBUG_SCISSOR
        emPrintf("Sprite Scissoring: Bottom Edge (%d) not in range (%d,%d)\n", yy, sScissorYmin,
                 sScissorYmax);
#endif
    } else {
        ryl = yy * 4;
    };

    s_clamp = G_TX_CLAMP;
    t_clamp = G_TX_CLAMP;
    s_mask = G_TX_NOMASK;
    t_mask = G_TX_NOMASK;
    s_lod = G_TX_NOLOD;
    t_lod = G_TX_NOLOD;

#define LOAD_TEX

#ifdef LOAD_TEX
    if (b->buf != sPrevBitmapBuf) {
        switch (s->bmsiz) {
            case G_IM_SIZ_4b:
                if (s->attr & SP_TEXSHUF) {
                    gDPLoadTextureBlock_4bS(gdlh++, b->buf, s->bmfmt, tex_width, tex_height, 0, s_clamp,
                                            t_clamp, s_mask, t_mask, s_lod, t_lod);
                } else {
                    gDPLoadTextureBlock_4b(gdlh++, b->buf, s->bmfmt, tex_width, tex_height, 0, s_clamp,
                                           t_clamp, s_mask, t_mask, s_lod, t_lod);
                };
                break;

            case G_IM_SIZ_8b:
                if (s->attr & SP_TEXSHUF) {
                    gDPLoadTextureBlockS(gdlh++, b->buf, s->bmfmt, G_IM_SIZ_8b, tex_width, tex_height, 0,
                                         s_clamp, t_clamp, s_mask, t_mask, s_lod, t_lod);
                } else {
                    gDPLoadTextureBlock(gdlh++, b->buf, s->bmfmt, G_IM_SIZ_8b, tex_width, tex_height, 0,
                                        s_clamp, t_clamp, s_mask, t_mask, s_lod, t_lod);
                };
                break;
            case G_IM_SIZ_16b:
                if (s->bmfmt == G_IM_FMT_YUV) {
                    if (s->attr & SP_TEXSHUF) {
                        gDPLoadTextureBlockYuvS(gdlh++, b->buf, s->bmfmt, G_IM_SIZ_16b, tex_width,
                                                tex_height, 0, s_clamp, t_clamp, s_mask, t_mask, s_lod,
                                                t_lod);
                    } else {
                        if (b->LUToffset != 0) { /* Split Y and UV areas */
                            unsigned char *uv, *addr;
                            int j;
                            s32 tmem, siz;

                            gDPSetTextureImage(gdlh++, G_IM_FMT_I, G_IM_SIZ_8b, 1, b->buf);
                            gDPSetTile(gdlh++, G_IM_FMT_I, G_IM_SIZ_8b, 0, 256, G_TX_LOADTILE, 0, t_clamp,
                                       t_mask, t_lod, s_clamp, s_mask, s_lod);
                            gDPLoadSync(gdlh++);
                            gDPLoadBlock(gdlh++, G_TX_LOADTILE, 0, 0, tex_width * tex_height - 1,
                                         CALC_DXT(tex_width, 1));
                            gDPLoadSync(gdlh++);

                            uv = ((unsigned char *) b->buf)
                                 + ((tex_width * tex_height) / 2) * b->LUToffset;

                            for (j = 0; j <= (tex_height / 2); j++) {
                                addr = uv + ((tex_height / 2) - 2 - j) * tex_width;
                                siz = 3 * tex_width;
                                tmem = (tex_width / 8) * (tex_height - 2 - 2 * j);
                                if (tmem < 0) {
                                    addr += (-tmem) * 8;
                                    siz -= (-tmem) * 8;
                                    tmem = 0;
                                };
                                if (j == 0)
                                    siz = 2 * tex_width;

                                gDPSetTextureImage(gdlh++, G_IM_FMT_I, G_IM_SIZ_8b, 1, addr);
                                gDPSetTile(gdlh++, G_IM_FMT_I, G_IM_SIZ_8b, 0, tmem, G_TX_LOADTILE, 0,
                                           t_clamp, t_mask, t_lod, s_clamp, s_mask, s_lod);
                                gDPLoadSync(gdlh++);
                                gDPLoadBlock(gdlh++, G_TX_LOADTILE, 0, 0, siz - 1,
                                             CALC_DXT(tex_width, 1));
                                gDPLoadSync(gdlh++);
                            };

                            gDPSetTile(gdlh++, s->bmfmt, G_IM_SIZ_16b, (((tex_width) * 1) + 7) >> 3, 0,
                                       G_TX_RENDERTILE, 0, t_clamp, t_mask, t_lod, s_clamp, s_mask,
                                       s_lod);
                            gDPSetTileSize(gdlh++, G_TX_RENDERTILE, 0, 0,
                                           ((tex_width) -1) << G_TEXTURE_IMAGE_FRAC,
                                           ((tex_height) -1) << G_TEXTURE_IMAGE_FRAC);

                        } else {
                            gDPLoadTextureBlockYuv(gdlh++, b->buf, s->bmfmt, G_IM_SIZ_16b, tex_width,
                                                   tex_height, 0, s_clamp, t_clamp, s_mask, t_mask,
                                                   s_lod, t_lod);
                        };
                    };
                } else {
                    if (s->attr & SP_TEXSHUF) {
                        gDPLoadTextureBlockS(gdlh++, b->buf, s->bmfmt, G_IM_SIZ_16b, tex_width,
                                             tex_height, 0, s_clamp, t_clamp, s_mask, t_mask, s_lod,
                                             t_lod);
                    } else {
                        gDPLoadTextureBlock(gdlh++, b->buf, s->bmfmt, G_IM_SIZ_16b, tex_width, tex_height,
                                            0, s_clamp, t_clamp, s_mask, t_mask, s_lod, t_lod);
                    };
                };
                break;
            case G_IM_SIZ_32b:
                if (s->attr & SP_TEXSHUF) {
                    gDPLoadTextureBlockS(gdlh++, b->buf, s->bmfmt, G_IM_SIZ_32b, tex_width, tex_height, 0,
                                         s_clamp, t_clamp, s_mask, t_mask, s_lod, t_lod);
                } else {
                    gDPLoadTextureBlock(gdlh++, b->buf, s->bmfmt, G_IM_SIZ_32b, tex_width, tex_height, 0,
                                        s_clamp, t_clamp, s_mask, t_mask, s_lod, t_lod);
                };
                break;
        }
        sPrevBitmapBuf = b->buf;
    }
#endif /* LOAD_TEX */

    g->cmd = G_TEXRECT;
    g->tile = 0;

    g->xh = rxh;
    g->xl = rxl;
    g->yh = ryh;
    g->yl = ryl;

    g->s = rs;
    g->t = rt;

    if ((s->attr & SP_FASTCOPY) && (s->bmfmt != G_IM_FMT_YUV))
        g->dsdx = sx * 4;
    else
        g->dsdx = sx;

    g->dtdy = sy;

    gSPTextureRectangle(gdlh++, g->xh, g->yh, g->xl, g->yl, g->tile, g->s, g->t, g->dsdx, g->dtdy);

    gDPPipeSync(gdlh++);

    *gdl = gdlh;
}

void uvSpriteDrawInit(Gfx **gdl) {
    Gfx *gdlh;

    gdlh = *gdl;
    sSpriteAttribute = 0;
    sScissorXmin = 0;
    sScissorYmin = 0;
    sScissorXmax = SCREEN_WIDTH;
    sScissorYmax = SCREEN_HEIGHT;
    // Turn on texturing
    gDPPipeSync(gdlh++);
    gDPSetCycleType(gdlh++, G_CYC_1CYCLE);
    gSPTexture(gdlh++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetAlphaCompare(gdlh++, G_AC_NONE);
    gDPSetTexturePersp(gdlh++, G_TP_NONE);
    gDPSetTextureFilter(gdlh++, G_TF_BILERP);
    gDPSetTextureConvert(gdlh++, G_TC_FILT);
    gDPSetTextureDetail(gdlh++, G_TD_CLAMP);
    gDPSetTextureLOD(gdlh++, G_TL_TILE);
    gDPSetTextureLUT(gdlh++, G_TT_NONE);

    gDPSetRenderMode(gdlh++, SPRITE_SURF, SPRITE_SURF2);
    *gdl = gdlh;
}

Gfx* uvSpriteDraw(Sprite* sp) {
    int i;
    s32 x, y;
    float sx, sy;
    Bitmap* bitmap;
    Gfx* gdlh;
    Gfx* dl_start;
    s32 isx, isy;
    s32 tx, ty;
    s32 tx2, ty2;
    s32 x2, y2;
    float ftx, fty;
    s32 fs, ft;
    s32 ex, ey;
    Sprite* s = sp;

    if (s->attr & SP_HIDDEN) {
        return NULL;
    }

    sPrevBitmapBuf = NULL;

    gdlh = s->rsp_dl_next;
    if (gdlh == NULL) {
        gdlh = s->rsp_dl;
    }
    dl_start = gdlh;

    bitmap = s->bitmap;
    ex = 0;
    ey = 0;

    if (sSpriteAttribute & SP_EXTERN) { /* previous attr was extern? */
        sSpriteAttribute = ~s->attr;  /* Assume previous modes are all different from new ones */
    }


    if (s->attr & SP_EXTERN) { /* current attr is extern? */
        sSpriteAttribute = s->attr;   /* Assume all settings should remain the same */
    }

    if (s->attr != sSpriteAttribute) {
        if ((s->attr & SP_TRANSPARENT) && !(sSpriteAttribute & SP_TRANSPARENT)) {
            gDPSetRenderMode(gdlh++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        } else if (!(s->attr & SP_TRANSPARENT) && (sSpriteAttribute & SP_TRANSPARENT)) {
            gDPSetRenderMode(gdlh++, SPRITE_SURF, SPRITE_SURF2);
        }

        if ((s->attr & SP_CUTOUT) && !(sSpriteAttribute & SP_CUTOUT)) {
            gDPSetBlendColor(gdlh++, 0xff, 0xff, 0xff, 0x01);
            gDPSetAlphaCompare(gdlh++, G_AC_THRESHOLD);
        } else if (!(s->attr & SP_CUTOUT) && (sSpriteAttribute & SP_CUTOUT)) {
            gDPSetAlphaCompare(gdlh++, G_AC_NONE);
        }

        if ((s->attr & SP_TEXSHIFT) && !(sSpriteAttribute & SP_TEXSHIFT)) {
            gDPSetTextureFilter(gdlh++, G_TF_AVERAGE);
        } else if (!(s->attr & SP_TEXSHIFT) && (sSpriteAttribute & SP_TEXSHIFT)) {
            gDPSetTextureFilter(gdlh++, G_TF_BILERP);
        }
    }

    sSpriteAttribute = s->attr;

    if (sSpriteAttribute & SP_Z) {
        gDPSetPrimDepth(gdlh++, s->zdepth, 0);
    };

    gDPSetPrimColor(gdlh++, 0, 0, s->red, s->green, s->blue, s->alpha);

    if (s->bmfmt == G_IM_FMT_IA) {
        if (s->alpha == 0xFF) {
            gDPSetCombineMode(gdlh++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        } else {
            gDPSetCombineMode(gdlh++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        }
    } else if (s->bmfmt == G_IM_FMT_I) { // @diff to PW64
        gDPSetCombineMode(gdlh++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
    } else {
        if (s->alpha == 0xFF) {
            gDPSetCombineMode(gdlh++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
        } else {
            gDPSetCombineMode(gdlh++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        }
    }

    if (s->bmfmt == G_IM_FMT_CI) {
        gDPSetTextureLUT(gdlh++, G_TT_RGBA16);
        gDPLoadTLUT(gdlh++, s->nTLUT, 256 + s->startTLUT, s->LUT);
        gDPLoadSync(gdlh++);
    };


    if ((s->bmfmt == G_IM_FMT_YUV) && (s->attr & SP_FASTCOPY)) {
        gDPSetTextureConvert(gdlh++, G_TC_CONV);
        gDPSetTextureFilter(gdlh++, G_TF_POINT);
        gDPSetCombineMode(gdlh++, G_CC_1CYUV2RGB, G_CC_1CYUV2RGB);
        gDPSetConvert(gdlh++, MY_K0, MY_K1, MY_K2, MY_K3, MY_K4, MY_K5);
    } else if (s->bmfmt == G_IM_FMT_YUV) {
        gDPSetCycleType(gdlh++, G_CYC_2CYCLE);
        gDPSetTextureFilter(gdlh++, G_TF_BILERP);
        gDPSetTextureConvert(gdlh++, G_TC_FILTCONV);
        gDPSetConvert(gdlh++, MY_K0, MY_K1, MY_K2, MY_K3, MY_K4, MY_K5);
        gDPSetCombineMode(gdlh++, G_CC_YUV2RGB, G_CC_PASS2);
    }

    if (s->attr & SP_SCALE) {
        sx = s->scalex;
        sy = s->scaley;
    } else {
        sx = 1.0F;
        sy = 1.0F;
    }

    isx = (int)((1 << 10) / sx + 0.5F);
    isy = (int)((1 << 10) / sy + 0.5F);

    if (bitmap) {
        if ((s->attr & SP_FASTCOPY) && (s->bmfmt != G_IM_FMT_YUV))
            gDPSetCycleType(gdlh++, G_CYC_COPY);

        x = 0.0F;
        y = 0.0F;

        fty = s->y + y * sy;
        ty = (int)(fty + 0.99999F);
        ft = (int)(isy * (ty - fty));
        ft = (ft + 16) >> 5;
        if (s->attr & SP_TEXSHIFT)
            ft += 16; /* 1/2 Texel for AntiAliasing */

        if (s->attr & SP_FRACPOS)
            ft += s->frac_t; /* Micro-positioning */

        y2 = y + s->bmheight;
        ty2 = (int)(s->y + y2 * sy + 0.99999F);

        if ((s->attr & SP_FASTCOPY) && (s->bmfmt != G_IM_FMT_YUV))
            ty2--;

        //rmonPrintf("\tiscale=(%d,%d), %d bitmaps, sprite_size=(%d,%d)\n", isx, isy, s->nbitmaps, s->width, s->height);

        for (i = 0; (i < s->nbitmaps) && (bitmap->width > 0); i++, bitmap++) {
            if ((x + bitmap->width) > s->width) { /* Next bitmap hits edge? */
                int bh;

                x = 0;
                fs = 0;
                ex = 0;
                ey += s->expy;
                y += s->bmheight;     /* Wrap to next line */
                y2 = y + s->bmheight; /* Wrap to next line */

                fty = s->y + y * sy;
                ty = (int)(fty + 0.9999F);
                ft = (int)(isy * (ty - fty));
                ft = (ft + 16) >> 5;
                if (s->attr & SP_TEXSHIFT)
                    ft += 16; /* 1/2 Texel for AntiAliasing */

                if (s->attr & SP_FRACPOS)
                    ft += s->frac_t; /* Micro-positioning */

                ty2 = (int)(s->y + y2 * sy + 0.9999F);

                ty += ey;
                ty2 += ey;

                if ((s->attr & SP_FASTCOPY) && (s->bmfmt != G_IM_FMT_YUV))
                    ty2--;

                /* Fill out to rect. bdy? */

                if ((bitmap->actualHeight != 0))
                    bh = bitmap->actualHeight;
                else
                    bh = s->bmheight;

                if ((y + bh) > s->height) /* Can't wrap any more? */
                    break;
            };

            //rmonPrintf("bm# %d, x,y=(%d,%d) width=%d \n", i, x, y, bitmap->width);

            ftx = s->x + x * sx;
            tx = (s32)(ftx + 0.9999F);
            fs = (s32)(isx * (tx - ftx));
            fs = (fs + 16) >> 5;
            if (s->attr & SP_TEXSHIFT)
                fs += 16; /* 1/2 Texel for AntiAliasing */

            if (s->attr & SP_FRACPOS)
                fs += s->frac_s; /* Micro-positioning */

            x2 = x + bitmap->width;
            tx2 = (int)(s->x + x2 * sx + 0.9999F);

            if ((bitmap->actualHeight != 0)) {
                y2 = y + bitmap->actualHeight;
                ty2 = (s32)(s->y + y2 * sy + 0.9999F);
                ty2 += ey;

                if ((s->attr & SP_FASTCOPY) && (s->bmfmt != G_IM_FMT_YUV))
                    ty2--;
            }

            //rmonPrintf("\tfull(%g,%g) = int(%d,%d) + frac(%d,%d)\n", ftx, fty, tx, ty, fs, ft);

            tx += ex;
            tx2 += ex;
            if ((s->attr & SP_FASTCOPY) && (s->bmfmt != G_IM_FMT_YUV))
                tx2--;

            if (bitmap->buf != NULL) { /* Skip over null bitmaps (blanks) */
                uvDrawBitmap(&gdlh, s, bitmap, tx, ty, tx2, ty2, fs, ft, isx, isy);
            }

            x += bitmap->width;
            ex += s->expx;
        }

        if (s->attr & SP_FASTCOPY)
            gDPSetCycleType(gdlh++, G_CYC_1CYCLE);

        if (s->bmfmt == G_IM_FMT_YUV) {
            gDPSetCycleType(gdlh++, G_CYC_1CYCLE);
            gDPSetTextureFilter(gdlh++, G_TF_BILERP);
            gDPSetTextureConvert(gdlh++, G_TC_FILT);
        };

    } else {
        int rgba;

        x = (s32)s->x;
        y = (s32)s->y;
        x2 = s->x + (s->width * sx) - 1;
        y2 = s->y + (s->height * sy) - 1;

        if ((x >= sScissorXmax) || (y >= sScissorYmax)) {
            //emPrintf("Sprite Scissoring: FILL Upper Left corner (%d,%d) beyond range (%d,%d)\n", x, y, scissor_xmax, scissor_ymax);
        } else if ((x2 < sScissorXmin) || (y2 < sScissorYmin)) {
            //emPrintf("Sprite Scissoring: FILL Lower Right corner (%d,%d) below range (%d,%d)\n", x2, y2, scissor_xmin, scissor_ymin);
        } else {
            if (x < sScissorXmin) {
                x = sScissorXmin;
            }

            if (x2 >= sScissorXmax) {
                x2 = sScissorXmax - 1;
            }

            if (y < sScissorYmin) {
                y = sScissorYmin;
            }

            if (y2 >= sScissorYmax) {
                y2 = sScissorYmax - 1;
            }

            rgba = GPACK_RGBA5551((s->red), (s->green), (s->blue), (s->alpha >> 7));

            gSPTexture(gdlh++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
            gDPSetCycleType(gdlh++, G_CYC_FILL);
            gDPSetFillColor(gdlh++, (rgba << 16) | (rgba));

            gDPFillRectangle(gdlh++, x, y, x2, y2);
            gDPSetCycleType(gdlh++, G_CYC_1CYCLE);
            gSPTexture(gdlh++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        }
    }

    if (s->bmfmt == G_IM_FMT_CI) {
        gDPSetTextureLUT(gdlh++, G_TT_NONE);
    };

    gSPEndDisplayList(gdlh++);

    // assert((gdlh - ogl) < s->ndisplist);
    s->rsp_dl_next = gdlh;
    return (dl_start);
}

void uvSpriteDrawFinish(Gfx **glistp) {
    Gfx *gdlh;

    gdlh = *glistp;

    // Turn off texturing
    gSPTexture(gdlh++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF);
    gDPSetCombineMode(gdlh++, G_CC_SHADE, G_CC_SHADE);

    if (sSpriteAttribute & SP_TRANSPARENT) {
        // Turn off transparency
        gDPSetRenderMode(gdlh++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    }

    if (sSpriteAttribute & SP_CUTOUT) {
        // Turn off Alpha write enable
        gDPSetAlphaCompare(gdlh++, G_AC_NONE);
    }

    gSPEndDisplayList(gdlh++);

    *glistp = gdlh;
}
