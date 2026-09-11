// SPDX-License-Identifier: AGPL-3.0-or-later

/**
 * @file file_manager.c
 *
 * Implements helper functions to allocate and free file data from the ROM
 */
#include "common.h"
#include "os.h"
#include "uv_filesystem.h"

#define FILE_NOT_FOUND -1
#define FILE_INVALID -1
#define FILE_VALID 0

typedef struct UnkStruct_8002D9BC_s {
    u16 fileCount;
    u16 *fileIds;
    u16 *uvtxFileIds;
} UnkStruct_8002D9BC;

typedef struct ModuleExports_s {
    void (*cleanupRoutine)(void);
} ModuleExports;

typedef struct ParsedAsset_s {
    void (*assetParseStub)(void);
    void *(*parseRoutine)(s32);
    void *(*freeRoutine)(ModuleExports *);
} ParsedAsset;

// .bss

u16 sFormFilesCount;
FormFileEntry *sCurrentFileEntry;
s32 gFormFilesSize;
s32 gModuleFilesSize;
u8 *gRomEnd;
FormTableEntry *gFormFiles;
s32 sUnusedDebugCond;
UnkStruct_8002D9BC *D_8002D9BC;

s32 uvGetFormFileIdByTag(s32 tag);
s32 uvLoadModuleCode(s32);
void uvUnloadModule(s32);
void *uvGetLoadedFile(s32, s32);
s32 uvCheckValidFileId(s32 tag, s32 fileId);
void uvFreeFile(s32 tag, ModuleExports *fileId);
void uvUnloadFile(s32 tag, s32 fileId);

#define __FORM0_START main_ROM_END

void uvLoadFormFiles(void) {
    s32 fileId;
    FormFileEntry *formTagsPtr;
    FormTableEntry *var_v0;
    s32 temp_v1;
    u32 size;
    void *data;
    u16 formFilesEntryCount;
    s32 j;
    s32 i;
    u16 var_v1;
    s32 *currentFormTableEntry;

    D_8002D9BC = NULL;
    gFormFilesSize = UVTS_25_ROM_END - FORM0_ROM_END;
    gModuleFilesSize = MODULE_FILES_START - __FORM0_START;
    gRomEnd = UVTS_25_ROM_END;
    fileId = uvFileReadHeader(__FORM0_START);
    sFormFilesCount = 0;
    formFilesEntryCount = 0;

    while (uvFileReadBlock(fileId, &size, &data, FILE_COMPRESSED) != 0) {
        sFormFilesCount++;
        formFilesEntryCount += size / sizeof(int);
    }

    gFormFiles = _uvMemAllocAlign8((sFormFilesCount * sizeof(FormTableEntry))
                                   + (formFilesEntryCount * sizeof(FormFileEntry)));
    uvFileSetPadTagStart(fileId);
    formTagsPtr = (FormFileEntry *) &(0, gFormFiles)[sFormFilesCount]; // Fake match
    for (i = 0; i < sFormFilesCount; i++) {
        gFormFiles[i].tag = uvFileReadBlock(fileId, &size, &data, FILE_NOT_COMPRESSED);
        gFormFiles[i].filesCount = size / sizeof(int);
        gFormFiles[i].fileEntry = formTagsPtr;
        currentFormTableEntry = data;
        for (j = 0; j < gFormFiles[i].filesCount; currentFormTableEntry++, j++) {
            if (sUnusedDebugCond) { // Fake match
            }

            // Check for null entries in the form0 table
            if (*currentFormTableEntry == -1) {
                formTagsPtr[j].romPtr = NULL;
            } else {
                formTagsPtr[j].romPtr = FORM0_ROM_END + *currentFormTableEntry;
            }

            formTagsPtr[j].allocPtr = NULL;
            formTagsPtr[j].unused = 0;
            formTagsPtr[j].instanceCount = 0; // Mark the file as unloaded
        }
        formTagsPtr += gFormFiles[i].filesCount; // Advance to the next form file
        _uvMemFree(data);
    }

    uvFileFree(fileId);
    if (sUnusedDebugCond) {
        for (i = 0; i < sFormFilesCount; i++) {
        }
    }
}

u16 uvGetFileInstanceCount(s32 tag, s32 fileId) {
    s32 formFileId;

    formFileId = uvGetFormFileIdByTag(tag);
    if (formFileId == FILE_NOT_FOUND) {
        return 0;
    }
    if (uvCheckValidFileId(formFileId, fileId) == FILE_INVALID) {
        return 0;
    }
    return gFormFiles[formFileId].fileEntry[fileId].instanceCount;
}

s32 uvGetFilesCount(s32 tag) {
    s32 formFileId;

    formFileId = uvGetFormFileIdByTag(tag);
    if (formFileId == FILE_INVALID) {
        return 0U;
    }
    return gFormFiles[formFileId].filesCount;
}

u8 *uvGetFileData(s32 formFileTag, s32 fileId) {
    s32 formFileId;

    formFileId = uvGetFormFileIdByTag(formFileTag);
    if (formFileId == FILE_NOT_FOUND) {
        return NULL;
    }
    if (uvCheckValidFileId(formFileId, fileId) == FILE_INVALID) {
        return NULL;
    }
    return gFormFiles[formFileId].fileEntry[fileId].romPtr;
}

void *uvGetLoadedFile(s32 formFileTag, s32 fileId) {
    s32 formFileId;

    formFileId = uvGetFormFileIdByTag(formFileTag);
    if (formFileId == FILE_NOT_FOUND) {
        return NULL;
    }
    if (uvCheckValidFileId(formFileId, fileId) == FILE_INVALID) {
        return NULL;
    }
    return (void *) gFormFiles[formFileId].fileEntry[fileId].allocPtr;
}

s32 uvGetUnusedFileInfo(s32 tag, s32 fileId) {
    s32 formFileId;

    formFileId = uvGetFormFileIdByTag(tag);
    if (formFileId == FILE_NOT_FOUND) {
        return 0;
    }
    if (uvCheckValidFileId(formFileId, fileId) == FILE_INVALID) {
        return 0;
    }
    return gFormFiles[formFileId].fileEntry[fileId].unused;
}

void *uvAllocFile(s32 formFileTag, s32 fileId) {
    FormFileEntry *fileEntry;
    s32 formFileId;
    s32 i;

    formFileId = uvGetFormFileIdByTag(formFileTag);
    if (formFileId == FILE_NOT_FOUND) {
        return NULL;
    }
    fileEntry = &gFormFiles[formFileId].fileEntry[fileId];

    // If the file is NULL we can't allocate it
    if (fileEntry->romPtr == NULL) {
        fileEntry->allocPtr = NULL;
        return NULL;
    }

    // TODO: Is this true in normal game conditions?
    if ((formFileTag == 'UVTX') && (D_8002D9BC != NULL)) {
        for (i = 0; i < D_8002D9BC->fileCount; i++) {
            if (fileId == D_8002D9BC->fileIds[i]) {
                formFileId = uvLoadFile('UVTX', D_8002D9BC->uvtxFileIds[i]);
                fileEntry->allocPtr = formFileId;
                return formFileId;
            }
        }
    }

    if (formFileTag == 'UVMO') {
        sCurrentFileEntry = fileEntry;
        fileEntry->allocPtr = uvLoadModuleCode(fileEntry->romPtr);
        sCurrentFileEntry = NULL;
    } else if (gFormFiles[formFileId].tag != 0xFFFF) {
        ParsedAsset *parsedAsset;
        parsedAsset = (ParsedAsset *) uvLoadModule(gFormFiles[formFileId].tag);
        fileEntry->allocPtr = parsedAsset->parseRoutine(fileEntry->romPtr);
        uvUnloadModule(gFormFiles[formFileId].tag);
    } else {
        fileEntry->allocPtr = fileEntry->romPtr;
    }
    return fileEntry->allocPtr;
}

void uvUpdateFileAllocPtr(void *allocPtr) {
    sCurrentFileEntry->allocPtr = allocPtr;
}

void *uvLoadFile(s32 formFileTag, s32 fileId) {
    FormFileEntry *fileEntry;
    s32 formFileId;

    formFileId = uvGetFormFileIdByTag(formFileTag);
    if (formFileId == FILE_NOT_FOUND) {
        return NULL;
    }
    if (uvCheckValidFileId(formFileId, fileId) == FILE_INVALID) {
        return NULL;
    }
    fileEntry = &gFormFiles[formFileId].fileEntry[fileId];
    fileEntry->instanceCount++;
    if (fileEntry->instanceCount == 1) {
        fileEntry->allocPtr = uvAllocFile(formFileTag, fileId);
    }
    return fileEntry->allocPtr;
}

void uvUnloadFile(s32 formFileTag, s32 fileId) {
    FormFileEntry *ptr;
    s32 formFileId;

    formFileId = uvGetFormFileIdByTag(formFileTag);
    if (formFileId == FILE_NOT_FOUND) {
        return;
    }

    if (uvCheckValidFileId(formFileId, fileId) == FILE_INVALID) {
        return;
    }

    ptr = &gFormFiles[formFileId].fileEntry[fileId];

    // Check if the file is not loaded
    if (ptr->instanceCount == 0) {
        return;
    }

    // Mark the file as not loaded
    if (--gFormFiles[formFileId].fileEntry[fileId].instanceCount) {
        return;
    }

    if (formFileTag == 'UVTX') {
        // False, in game?
        if (D_8002D9BC != NULL) {
            s32 i;
            for (i = 0; i < D_8002D9BC->fileCount; i++) {
                if (fileId == D_8002D9BC->fileIds[i]) {
                    uvUnloadFile('UVTX', D_8002D9BC->uvtxFileIds[i]);
                    ptr->allocPtr = NULL;
                    ptr->unused = 0;
                    return;
                }
            }
        }
    }

    // TODO: Find a better match
    fileId = ptr->allocPtr; // get the exports
    if ((void *) fileId != NULL) {
        uvFreeFile(formFileTag, (void *) fileId);
    }
    ptr->allocPtr = NULL;
    ptr->unused = 0;
}

STATIC_FUNC void uvFreeFile(s32 formFileTag, ModuleExports *exports) {
    s32 formFileId;
    ParsedAsset *parsedAsset;

    formFileId = uvGetFormFileIdByTag(formFileTag);
    if (formFileId == FILE_NOT_FOUND) {
        return;
    }

    if (formFileTag == 'UVMO') {
        exports->cleanupRoutine();
        uvMemFree(exports);
        return;
    }

    parsedAsset = uvLoadModule(gFormFiles[formFileId].tag);
    parsedAsset->freeRoutine(exports);
    uvUnloadModule(gFormFiles[formFileId].tag);
}

void uvResetAllFilesInstanceCount(void) {
    s32 i;
    s32 j;

    for (i = 0; i < sFormFilesCount; i++) {
        for (j = 0; j < gFormFiles[i].filesCount; j++) {
            gFormFiles[i].fileEntry[j].instanceCount = 0;
        }
    }
}

u32 uvFileReadBlock(s32 fileId, u32 *sizeOut, void **data, s32 decodeFlag) {
    u32 tag;
    u8 *allocPtr;
    u32 size;
    u8 *dst;

    tag = uvFileGetEntryTag(fileId, sizeOut, data);
    if (tag != 0) {
        allocPtr = _uvMemAllocAlign8(*sizeOut);
        _uvMediaCopy(allocPtr, *data, *sizeOut);
        *data = allocPtr;
    }
    if (tag == 'GZIP') {
        tag = (u32) uvMemRead(allocPtr, 4);
        size = (u32) uvMemRead(allocPtr + 4, 4);
        if (!(decodeFlag & 2)) {
            dst = _uvMemAllocAlign8(size);
            mio0Decode(allocPtr + 8, dst);
            _uvMemFree(allocPtr);
            *data = dst;
        }
        *sizeOut = size;
    }
    return tag;
}

void uvConsumeBytes(void *dst, u8 **ptr, u32 size) {
    u64 mem;

    if (size < sizeof(s64) + 1) {
        mem = uvMemRead(*ptr, size);
        *ptr += size;
        switch (size) {
            case sizeof(s8):
                *(s8 *) dst = mem;
                return;
            case sizeof(s16):
                *(s16 *) dst = mem;
                return;
            case sizeof(s32):
                *(s32 *) dst = mem;
                return;
            case sizeof(s64):
                *(s64 *) dst = mem;
                return;
        }
    } else {
        _uvMediaCopy(dst, *ptr, size);
        *ptr += size;
    }
}

s32 uvGetFormFileIdByTag(s32 tag) {
    static s32 i = 0;

    if (tag == gFormFiles[i].tag) {
        return i;
    }

    for (i = 0; i < sFormFilesCount; i++) {
        if (tag == gFormFiles[i].tag) {
            return i;
        }
    }

    return FILE_NOT_FOUND;
}

s32 uvGetFormFileIdByTagDup(s32 tag) {
    static s32 i = 0;

    if (tag == gFormFiles[i].tag) {
        return i;
    }

    for (i = 0; i < sFormFilesCount; i++) {
        if (tag == gFormFiles[i].tag) {
            return i;
        }
    }
    return FILE_NOT_FOUND;
}

s32 uvCheckValidFileId(s32 tag, s32 fileId) {
    if ((fileId < 0) || (fileId >= gFormFiles[tag].filesCount)) {
        return FILE_INVALID;
    }
    return FILE_VALID;
}

u8 *uvGetFileRomPtr(s32 formFileId) {
    s32 i;
    s32 j;

    for (i = formFileId; i < sFormFilesCount; i++) {
        for (j = 0; j < gFormFiles[i].filesCount; j++) {
            u8 *romPtr = gFormFiles[i].fileEntry[j].romPtr;
            if (romPtr != NULL) {
                return romPtr;
            }
        }
    }

    return UVTS_25_ROM_END;
}

void func_80002088(s32 *arg0, s32 *arg1, s32 *arg2, s32 arg3) {
    s32 i;

    for (i = 0; i < sFormFilesCount; i++) {
        if (i >= arg3) {
            *arg2 = arg3;
            return;
        }

        arg0[i] = gFormFiles[i].tag;
        arg1[i] = uvGetFileRomPtr(i + 1) - uvGetFileRomPtr(i);
    }

    *arg2 = i;
}

void func_8000218C(s32 *arg0, s32 *arg1, s32 *arg2, s32 arg3) {
    s32 j;
    s32 i;

    for (i = 0; i < sFormFilesCount; i++) {
        if (i >= arg3) {
            *arg2 = arg3;
            return;
        }

        arg0[i] = gFormFiles[i].tag;
        for (j = 0, arg1[i] = 0; j < gFormFiles[i].filesCount; j++) {
            arg1[i] += gFormFiles[i].fileEntry[j].unused;
        }
    }

    *arg2 = i;
}

void func_8000226C(s32 *tagPtr, s32 *arg1, s32 *arg2, u32 arg3) {
    u32 var_v0;
    s32 j;
    s32 i;
    s32 temp_a3;
    s32 var_t4;
    s32 var_t2;
    s32 var_t3;

    var_v0 = 0x80000000;
    for (i = 0; i < sFormFilesCount; i++) {
        for (j = 0; j < gFormFiles[i].filesCount; j++) {
            temp_a3 = gFormFiles[i].fileEntry[j].allocPtr;
            if ((temp_a3 < arg3) && (var_v0 < temp_a3)) {
                var_t2 = i;
                var_t3 = j;
                var_v0 = temp_a3;
                var_t4 = arg3 - temp_a3;
            }
        }
    }

    if (var_v0 != 0x80000000) {
        *tagPtr = gFormFiles[var_t2].tag;
        *arg1 = var_t3;
        *arg2 = var_t4;
        return;
    }

    *tagPtr = 'CODE';
    *arg1 = 0;
    *arg2 = arg3 - var_v0;
}

void func_80002390(s32 arg0) {
    if (arg0 == 0xFFFF) {
        D_8002D9BC = NULL;
    } else {
        D_8002D9BC = uvGetLoadedFile('UVTP', arg0);
    }
    if (D_8002D9BC)
        ; // FAKE
}
