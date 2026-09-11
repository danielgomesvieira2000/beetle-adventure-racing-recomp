// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

void __entrypoint_func_uvufileld_rom_400000(UvuFileLd_Rom_Exports *exports);
void uvuFileldStub(void);
u8 *uvFileCopy(u8 *arg0);
void uvFreeFilePtr(void *arg0);
void uvSetCopyFileDataFlag(s32 arg0);

extern s32 sCopyFileDataFlag;

void __entrypoint_func_uvufileld_rom_400000(UvuFileLd_Rom_Exports *exports) {
    uvUpdateFileAllocPtr(exports);
    exports->uvFileCopy = uvFileCopy;
    exports->uvuFileldStub = uvuFileldStub;
    exports->uvFreeFilePtr = uvFreeFilePtr;
    exports->uvSetCopyFileDataFlag = uvSetCopyFileDataFlag;
    sCopyFileDataFlag = FALSE;
}

void uvuFileldStub(void) {
}

u8 *uvFileCopy(u8 *data) {
    u8 *ptr;
    s32 fileId;
    s32 fileSize;
    fileId = uvFileReadHeader(data);
    fileSize = uvFileGetSize(fileId);

    if (uvFileGetPadType(fileId) == 'UVRW') { // Unused game format?
        uvFileSearchTag(fileId, (u32 *) &fileSize, (void **) &data, 'COMM', 0);
    }

    if (!sCopyFileDataFlag) {
        ptr = _uvMemAlloc(fileSize, 8);
        _uvMediaCopy(ptr, data, fileSize);
    } else {
        ptr = data;
    }
    uvFileFree(fileId);
    return ptr;
}

void uvFreeFilePtr(void *ptr) {
    _uvMemFree(ptr);
}

void uvSetCopyFileDataFlag(s32 flag) {
    sCopyFileDataFlag = flag;
}
