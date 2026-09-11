#ifndef UVTEXTURE_ROM_H
#define UVTEXTURE_ROM_H

typedef struct {
    u8 pad0[0x2];
    s16 bmfmt;
    s16 bitdepth;
    s16 width;
    s16 height;
    s16 unkA;
    s16 texelHeight;
    s16 nbitmaps;
    void *buf;
    Bitmap *bitmap;
} ParsedUVBT;
#endif /* UVTEXTURE_ROM_H */
