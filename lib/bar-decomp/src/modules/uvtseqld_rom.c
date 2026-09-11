// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "os.h"

void func_uvtseqld_rom_00400054(void);
void* func_uvtseqld_rom_0040005C(u8* arg0);
void* func_uvtseqld_rom_00400118(u8* arg0);
void func_uvtseqld_rom_0040027C(void *arg0);
s32 func_uvtseqld_rom_0040029C(s32 arg0);



typedef struct UnkStruct_UvtSeqLd_Rom_00400118_s {
    u16 unk0;
    u8 unk2;
    s32 unk4;
} UnkStruct_UvtSeqLd_Rom_00400118;

typedef struct UnkStruct_UvtSeqLd_Rom_00400118_1_s {
    s8 unk0;
    UnkStruct_UvtSeqLd_Rom_00400118* unk4;
    u8 unk8;
    u8 unk9;
    s32 unkC;
} UnkStruct_UvtSeqLd_Rom_00400118_1;

extern s32 D_uvtseqld_rom_00400320;

void __entrypoint_func_uvtseqld_rom_400000(UvtSeqLd_Rom_Exports* exports);

void __entrypoint_func_uvtseqld_rom_400000(UvtSeqLd_Rom_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvtseqld_rom_0040005C = func_uvtseqld_rom_0040005C;
    exports->func_uvtseqld_rom_00400054 = func_uvtseqld_rom_00400054;
    exports->func_uvtseqld_rom_0040027C = func_uvtseqld_rom_0040027C;
    exports->func_uvtseqld_rom_0040029C = func_uvtseqld_rom_0040029C;
}

void func_uvtseqld_rom_00400054(void) {

}

void* func_uvtseqld_rom_0040005C(u8* arg0) {
    s32 temp_v0;
    void* var_s2;
    u32 sp3C;
    void* sp38;
    u32 var_v0;
    void* var_s0;

    var_s2 = NULL;
    temp_v0 = uvFileReadHeader(arg0);

    while ((var_v0 = uvFileReadBlock(temp_v0, &sp3C, &sp38, 1)) != 0) {
        switch (var_v0) {
            case 'COMM':
                var_s0 = sp38;
                var_s2 = func_uvtseqld_rom_00400118(var_s0);
                _uvMemFree(var_s0);
                break;
        }
    }

    uvFileFree(temp_v0);
    return var_s2;
}

void* func_uvtseqld_rom_00400118(u8* arg0) {
    u16 i;
    u8 sp45;
    u8 sp44;
    UnkStruct_UvtSeqLd_Rom_00400118* temp_s3;
    UnkStruct_UvtSeqLd_Rom_00400118_1* temp_v0;

    uvConsumeBytes(&sp44, &arg0, 1U);
    uvConsumeBytes(&sp45, &arg0, 1U);
    temp_s3 = _uvMemAllocAlign8(sp45 * 8);
    for (i = 0; i < sp45; i++) {
        uvConsumeBytes(&temp_s3[i].unk0, &arg0, sizeof(u16));
        uvConsumeBytes(&temp_s3[i].unk4, &arg0, sizeof(s32));
        temp_s3[i].unk2 = 0xFF;
        if ((sp44 != 0) && (D_uvtseqld_rom_00400320 == 0)) {
            uvLoadFile('UVTX', temp_s3[i].unk0);
        }
    }
    temp_v0 = _uvMemAllocAlign8(sizeof(UnkStruct_UvtSeqLd_Rom_00400118_1));
    uvConsumeBytes(&temp_v0->unk8, &arg0, sizeof(u8));
    uvConsumeBytes(&temp_v0->unk9, &arg0, sizeof(u8));
    uvConsumeBytes(&temp_v0->unkC, &arg0, sizeof(s32));
    temp_v0->unk4 = temp_s3;
    temp_v0->unk0 = sp45;
    return temp_v0;
}


void func_uvtseqld_rom_0040027C(void *arg0) {
    _uvMemFree(arg0);
}

s32 func_uvtseqld_rom_0040029C(s32 arg0) {
    s32 temp_v0;
    u16** temp_v0_2;
    s32 sp1C;

    temp_v0 = uvGetFileData('UVTS', arg0);
    if (temp_v0 == 0) {
        return 0xFFF;
    }
    D_uvtseqld_rom_00400320 = 1;
    temp_v0_2 = func_uvtseqld_rom_0040005C(temp_v0);
    D_uvtseqld_rom_00400320 = 0;
    sp1C = (s32) *temp_v0_2[1];
    func_uvtseqld_rom_0040027C(temp_v0_2);
    return sp1C;
}
