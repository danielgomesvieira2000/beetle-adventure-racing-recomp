// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "global_exports.h"

extern UnkStruct_weapon_00400184* D_weapon_00400184;
extern s32 D_weapon_00400198;
extern void func_weapon_004000E0(void);

void __entrypoint_func_weapon_400000(Weapon_Exports *exports) {
    s32 i;

    uvUpdateFileAllocPtr(exports);
    exports->func_weapon_004000E0 = func_weapon_004000E0;
    D_weapon_00400184 = _uvMemAllocAlign8(0x1B0U);
    D_weapon_00400198 = 0;

    for (i = 0; i < 12; i++) {
        UnkStruct_weapon_00400184* v0 = &D_weapon_00400184[i];
        v0->unk0 = -1;
        v0->unk4 = 0;
        v0->unk8 = 0.0f;
        v0->unk14 = 0;

    }
    uvLoadFile('UVMD', 4);
}

void func_weapon_004000E0(void) {
    s32 i;

    for (i = 0; i < 12; i++) {
        UnkStruct_weapon_00400184* v0 = &D_weapon_00400184[i];
        if (v0->unk0 >= 0) {
            gUvDobjExports->unk40(v0->unk0);
        }
    }
    _uvMemFree(D_weapon_00400184);
    uvUnloadFile('UVMD', 4);
}
