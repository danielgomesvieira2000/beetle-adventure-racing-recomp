// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

ParsedUVBT* _uvParseUVBT(u8 *); /* extern */
ParsedUVBT* uvParseBlit(u8 *data);
void uvBlitStub(void);
void uvFreeBlit(ParsedUVBT *uvbt);

void __entrypoint_func_uvblit_rom_400000(UvBlit_Rom_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->uvParseBlit = uvParseBlit;
    exports->uvBlitStub = uvBlitStub;
    exports->uvFreeBlit = uvFreeBlit;
}

void uvBlitStub(void) {
}

ParsedUVBT* uvParseBlit(u8 *ptr) {
    s32 fileId;
    u32 tag;
    u32 size;
    void *data;
    void* dataPtr;
    ParsedUVBT* parsedBlit;

    fileId = uvFileReadHeader(ptr);
    tag = uvFileReadBlock(fileId, &size, &data, 1);
    while (tag != 0) {
        switch (tag) {
            case 'COMM':
                dataPtr = data;
                parsedBlit = _uvParseUVBT(dataPtr);
                _uvMemFree(dataPtr);
                break;
            default:
                break;
        }

        tag = uvFileReadBlock(fileId, &size, &data, 1);
    }
    uvFileFree(fileId);
    return parsedBlit;
}

ParsedUVBT *_uvParseUVBT(u8 *arg0) {
    s32 var_a1; // pointer?
    u16 sp9A;
    u16 sp98;
    u16 sp96;
    u16 sp94;
    u16 sp92;
    u16 sp90;
    u16 sp8E;
    s8 remainder2; // var_s3
    s8 remainder1; // var_s4
    s32 j;
    u64 sp80;
    s32 i;
    ParsedUVBT *ret;
    s32 sp74;
    s32 sp70;
    Bitmap *temp_v0_3;
    s32 temp;

    remainder2 = 0;
    remainder1 = 0;

    _uvMediaCopy(&sp80, arg0, 4);
    sp80 >>= 32;
    sp9A = (sp80 & 0xFFFF0000) >> 16;
    sp98 = sp80;
    arg0 += 4;

    _uvMediaCopy(&sp80, arg0, 4);
    sp80 >>= 32;
    sp96 = (sp80 & 0xFFFF0000) >> 16;
    sp94 = sp80;
    arg0 += 4;

    _uvMediaCopy(&sp80, arg0, 4);
    sp80 >>= 32;
    sp92 = (sp80 & 0xFFFF0000) >> 16;
    sp90 = sp80;
    arg0 += 4;

    _uvMediaCopy(&sp80, arg0, 4);
    sp80 >>= 32;
    temp = ((sp80 & 0xFFFF0000) >> 16) & 0xFFFF;
    sp8E = temp & 0xFFFF;
    arg0 += 2;
    if (sp92 < sp8E) {
        sp8E = sp92;
    }

    ret = _uvMemAllocAlign8(sizeof(ParsedUVBT));
    ret->bmfmt = sp9A;
    ret->bitdepth = sp98;
    ret->width = sp96;
    ret->height = sp92;
    ret->unkA = sp90;
    ret->texelHeight = sp8E;

    temp = (sp94 * sp92 * sp98) / 8;
    ret->buf = _uvMemAlloc(temp, 8);
    uvConsumeBytes((void *) ret->buf, &arg0, temp);

    sp74 = sp96 / sp90;
    sp70 = sp92 / sp8E;
    if (sp96 % sp90) {
        sp74++;
        remainder1 = 1;
    }
    if (sp92 % sp8E) {
        sp70++;
        remainder2 = 1;
    }
    ret->nbitmaps = sp74 * sp70;
    ret->bitmap = _uvMemAllocAlign8(ret->nbitmaps * sizeof(Bitmap));

    var_a1 = ret->buf;
    for (i = 0; i < sp70; i++) {
        for (j = 0; j < sp74; j++) {
            temp_v0_3 = &ret->bitmap[j + i * sp74];
            temp = i + 1;
            if (remainder2 && sp70 == temp) {
                temp_v0_3->actualHeight = sp92 % sp8E;
            } else {
                temp_v0_3->actualHeight = sp8E;
            }
            temp = j + 1;
            if (remainder1 && sp74 == temp) {
                temp_v0_3->width = sp96 - (sp90 * (sp74 - 1));
            } else {
                temp_v0_3->width = sp90;
            }
            temp_v0_3->LUToffset = 0;
            temp_v0_3->t = temp_v0_3->LUToffset;
            temp_v0_3->s = temp_v0_3->LUToffset;
            temp_v0_3->width_img = sp90;
            temp_v0_3->buf = var_a1;
            var_a1 += (temp_v0_3->actualHeight * sp90 * sp98) / 8;
        }
    }
    return ret;
}

void uvFreeBlit(ParsedUVBT *uvbt) {
    _uvMemFree(uvbt->buf);
    _uvMemFree(uvbt->bitmap);
    _uvMemFree(uvbt);
}
