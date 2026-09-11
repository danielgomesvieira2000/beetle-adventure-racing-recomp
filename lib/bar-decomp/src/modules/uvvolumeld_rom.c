// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

void func_uvvolumeld_rom_00400048(void);
void* func_uvvolumeld_rom_00400050(u8* arg0);
void func_uvvolumeld_rom_004001F8(UnkStruct_UvVolumeLd_rom_00400050 *arg0);

void __entrypoint_func_uvvolumeld_rom_400000(UvVolumeLd_Rom_Exports* exports) {
    uvUpdateFileAllocPtr(exports);
    exports->func_uvvolumeld_rom_00400050 = func_uvvolumeld_rom_00400050;
    exports->func_uvvolumeld_rom_00400048 = func_uvvolumeld_rom_00400048;
    exports->func_uvvolumeld_rom_004001F8 = func_uvvolumeld_rom_004001F8;
}

void func_uvvolumeld_rom_00400048(void) {

}

void* func_uvvolumeld_rom_00400050(u8* arg0) {
    s32 fileId;
    u32 sp60;
    void* sp5C;
    UnkStruct_UvVolumeLd_rom_00400050_inner* temp_s0;
    UnkStruct_UvVolumeLd_rom_00400050* temp_v0;
    u32 var_v0;
    void* sp4C;
    void* temp_s6;
    s32 i; // var_s2

    temp_v0 = _uvMemAllocAlign8(8);
    temp_v0->unk0 = 0;
    temp_v0->unk4 = NULL;
    fileId = uvFileReadHeader(arg0);
    while ((var_v0 = uvFileReadBlock(fileId, &sp60, &sp5C, 1)) != 0) {
        switch (var_v0) {
        case 'VOBJ':
            sp4C = temp_s6 = sp5C;
            uvConsumeBytes(&temp_v0->unk0, (u8** ) &sp4C, sizeof(temp_v0->unk0));
            temp_v0->unk4 = _uvMemAllocAlign8(temp_v0->unk0 * sizeof(temp_v0->unk4[0]));

            for (i = 0; i < temp_v0->unk0; i++) {
                temp_s0 = &temp_v0->unk4[i];
                uvConsumeBytes(&temp_s0->unk0, (u8** ) &sp4C, sizeof(temp_s0->unk0));
                temp_s0->unk4 = _uvMemAllocAlign8(temp_s0->unk0 * sizeof(temp_s0->unk4[0]));
                uvConsumeBytes(temp_s0->unk4, (u8** ) &sp4C, temp_s0->unk0 * sizeof(temp_s0->unk4[0]));
                uvConsumeBytes(&temp_s0->unk8, (u8** ) &sp4C, sizeof(temp_s0->unk8));
                uvConsumeBytes(&temp_s0->unk14, (u8** ) &sp4C, sizeof(temp_s0->unk14));
            }

            _uvMemFree(temp_s6);
            break;
        default:
            break;
        }
    }
    uvFileFree(fileId);
    return temp_v0;
}

void func_uvvolumeld_rom_004001F8(UnkStruct_UvVolumeLd_rom_00400050* arg0) {
    s32 var_s0;

    for (var_s0 = 0; var_s0 < arg0->unk0; var_s0++) {
        _uvMemFree(arg0->unk4[var_s0].unk4);
    }
    _uvMemFree(arg0->unk4);
    _uvMemFree(arg0);
}
