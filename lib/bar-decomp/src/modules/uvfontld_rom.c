// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

extern ParsedUVFT* D_uvfontld_rom_004003DC;

void __entrypoint_func_uvfontld_rom_400000(UvFontLd_Exports* exports);
void func_uvfontld_rom_00400048(void);
ParsedUVFT* func_uvfontld_rom_00400050(u8* data);
void func_uvfontld_rom_00400324(ParsedUVFT* arg0);

void __entrypoint_func_uvfontld_rom_400000(UvFontLd_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvfontld_rom_00400050 = func_uvfontld_rom_00400050;
    exports->func_uvfontld_rom_00400048 = func_uvfontld_rom_00400048;
    exports->func_uvfontld_rom_00400324 = func_uvfontld_rom_00400324;
}

void func_uvfontld_rom_00400048(void) {

}

ParsedUVFT* func_uvfontld_rom_00400050(u8* data) {
    s32 fileId;
    u32 nbytes;
    s32* srcAddr;
    s32 i;
    u32 tag;
    s32 bitmCount;
    s32 bitmapData[2];

    D_uvfontld_rom_004003DC = (ParsedUVFT*)_uvMemAllocAlign8(sizeof(ParsedUVFT));
    D_uvfontld_rom_004003DC->unk4 = 0;
    fileId = uvFileReadHeader(data);
    while ((tag = uvFileReadBlock(fileId, &nbytes, (void**)&srcAddr, 1)) != NULL) {
        switch (tag) {
        case 'STRG':
            D_uvfontld_rom_004003DC->str = _uvMemAllocAlign8(nbytes + 1);
            _uvMediaCopy((void*)D_uvfontld_rom_004003DC->str, srcAddr, nbytes);
            D_uvfontld_rom_004003DC->str[nbytes] = '\0';
            _uvMemFree(srcAddr);
            break;
        case 'FRMT':
            _uvMediaCopy(bitmapData, srcAddr, nbytes);
            D_uvfontld_rom_004003DC->bmfmt = bitmapData[0];
            D_uvfontld_rom_004003DC->bmsiz = bitmapData[1];
            _uvMemFree(srcAddr);
            break;
        case 'BITM':
            bitmCount = nbytes / sizeof(Bitmap);
            D_uvfontld_rom_004003DC->bitmap = srcAddr;
            break;
        case 'IMAG':
            D_uvfontld_rom_004003DC->imag[D_uvfontld_rom_004003DC->unk4] = _uvMemAlloc(nbytes, 8U);
            _uvMediaCopy(D_uvfontld_rom_004003DC->imag[D_uvfontld_rom_004003DC->unk4], srcAddr, nbytes);
            _uvMemFree(srcAddr);
            D_uvfontld_rom_004003DC->unk4++;
            break;
        default:

            break;
        }
    }
    uvFileFree(fileId);
    D_uvfontld_rom_004003DC->str[bitmCount] = '\0';

    // update indexes to pointers allocated above
    for (i = 0; i < bitmCount; i++) {
        D_uvfontld_rom_004003DC->bitmap[i].buf = D_uvfontld_rom_004003DC->imag[(u32)D_uvfontld_rom_004003DC->bitmap[i].buf];
    }
    return D_uvfontld_rom_004003DC;
}

void func_uvfontld_rom_00400324(ParsedUVFT* arg0) {
    ParsedUVFT* s3 = arg0;
    s32 i;

    for (i = 0; i < s3->unk4; i++) {
        _uvMemFree(s3->imag[i]);
    }

    _uvMemFree(s3->str);
    _uvMemFree(s3->bitmap);
    _uvMemFree(s3);
}
