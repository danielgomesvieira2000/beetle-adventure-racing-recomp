// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

void __entrypoint_func_uvdsetld_rom_400000(UvDSetLd_Rom_Exports* exports);
void func_uvdsetld_rom_00400050(UnkStruct_uvdsetld_rom_00400050* arg0, void (*routine)(s32, s32));
void func_uvdsetld_rom_00400048(void);
void func_uvdsetld_rom_00400050(UnkStruct_uvdsetld_rom_00400050* arg0, void (*routine)(s32, s32));
void func_uvdsetld_rom_00400050(UnkStruct_uvdsetld_rom_00400050* arg0, void (*routine)(s32, s32));
void func_uvdsetld_rom_0040014C(u8* filePtr, void (*routine)(s32, s32));
u8 *func_uvdsetld_rom_00400204(u8 *arg0);
void func_uvdsetld_rom_00400230(void* arg0);

void __entrypoint_func_uvdsetld_rom_400000(UvDSetLd_Rom_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvdsetld_rom_00400204 = func_uvdsetld_rom_00400204;
    exports->func_uvdsetld_rom_00400048 = func_uvdsetld_rom_00400048;
    exports->func_uvdsetld_rom_00400230 = func_uvdsetld_rom_00400230;
}

void func_uvdsetld_rom_00400048(void) {

}

void func_uvdsetld_rom_00400050(UnkStruct_uvdsetld_rom_00400050* arg0, void (*routine)(s32, s32)) {
    s32 i;
    s32 tag;
    u16 temp_a1;
    u16 size;
    u16* ptr;

    tag = arg0->tagByte4 | (arg0->tagByte1 << 0x18) | (arg0->tagByte2 << 0x10) | (arg0->tagByte3 << 8);
    ptr = (u16*)&arg0->size;

    while (tag != 0) {
        if (tag != 'UVMO') {
            uvLoadModule(tag);
        }

        size = *ptr;
        i = 0;
        ptr++;

        while (i < size) {
            temp_a1 = *ptr;
            ptr++;
            routine(tag, temp_a1);
            i++;
        }

        if (tag != 'UVMO') {
            uvUnloadModule(tag);
        }

        tag = ((UnkStruct_uvdsetld_rom_00400050 *)ptr)->tagByte4 |
            (((UnkStruct_uvdsetld_rom_00400050 *)ptr)->tagByte1 << 0x18) |
            (((UnkStruct_uvdsetld_rom_00400050 *)ptr)->tagByte2 << 0x10) |
            (((UnkStruct_uvdsetld_rom_00400050 *)ptr)->tagByte3 << 0x8);

        ptr += 2;
    }
}

void func_uvdsetld_rom_0040014C(u8* filePtr, void (*routine)(s32, s32)) {
    s32 fileId;
    u32 tag;
    u32 size;
    void* data;
    void* dataPtr;

    fileId = uvFileReadHeader(filePtr);
    tag = uvFileReadBlock(fileId, &size, &data, 1);
    while (tag != 0) {
        switch (tag) {
            case 'COMM':
                dataPtr = data;
                func_uvdsetld_rom_00400050((UnkStruct_uvdsetld_rom_00400050* ) dataPtr, routine);
                _uvMemFree(dataPtr);
                break;
            default:
                break;
        }

        tag = uvFileReadBlock(fileId, &size, &data, 1);
    }
    uvFileFree(fileId);
}

u8 *func_uvdsetld_rom_00400204(u8 *arg0) {
    func_uvdsetld_rom_0040014C(arg0, (void (*)(s32, s32)) uvLoadFile);
    return arg0;
}

void func_uvdsetld_rom_00400230(void* arg0) {
    func_uvdsetld_rom_0040014C(arg0, uvUnloadFile);
}
