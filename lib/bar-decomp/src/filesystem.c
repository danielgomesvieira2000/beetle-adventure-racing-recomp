// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "os.h"

#define PAD_TAG_START 0xC
#define MAX_FILES 10

static FormFileInfo sCurrentFiles[MAX_FILES];

void uvFileSetPadTagStart(s32);                               /* extern */

s32 uvFileReadHeader(u8* data) {
    FormFileInfo* form;
    s32 i;

    for (form = sCurrentFiles, i = 0; i < MAX_FILES; i++, form++) {
        if (form->fileData == NULL) {
            break;
        }
    }

    if (i == MAX_FILES) {
        *(volatile s32*)0 = 0;
        return -1;
    }

    form->fileData = data;
    form->fileSize = uvMemRead(&data[4], 4) + 8; // Read the file size from the header and add the first eight bytes.
    form->padType = uvMemRead(&data[8], 4);
    uvFileSetPadTagStart(i);
    return i;
}

void uvFileFree(s32 id) {
    if ((id >= 0) && (id < 10)) {
        sCurrentFiles[id].fileData = NULL;
    }
}

u8* uvFileGetDataPtr(s32 id) {
    return sCurrentFiles[id].fileData;
}

s32 uvFileGetSize(s32 id) {
    return sCurrentFiles[id].fileSize;
}

s32 uvFileGetPadType(s32 id) {
    return sCurrentFiles[id].padType;
}

u32 uvFileGetEntryTag(s32 id, u32* sizeOut, void** dest) {
    u32 currentTag;
    s32 pad;
    u32 localStack;


    if (sizeOut == NULL) {
        sizeOut = &localStack;
    }
    if ((u32) sCurrentFiles[id].padTagStart >= (u32) sCurrentFiles[id].fileSize) {
        return 0U;
    }
    do {
        currentTag = uvMemRead(sCurrentFiles[id].padTagStart + sCurrentFiles[id].fileData, 4);
        *sizeOut = uvMemRead(sCurrentFiles[id].padTagStart + sCurrentFiles[id].fileData + 4, 4);
        *dest = sCurrentFiles[id].padTagStart + sCurrentFiles[id].fileData  + 8;

        sCurrentFiles[id].padTagStart += *sizeOut + 8;
    } while (currentTag == 'PAD ');
    return currentTag;
}

u32 uvFileSearchTag(s32 id, u32* sizeOut, void** data, s32 tag, s32 ocurrenceIndex) {
    s32 ocurrenceCount;
    u32 currentTag;
    s32 temp;
    u32 padTagStart;

    ocurrenceCount = 0;
    padTagStart = sCurrentFiles[id].padTagStart;
    uvFileSetPadTagStart(id);

    while ((currentTag = uvFileGetEntryTag(id, sizeOut, data)) != 0) {
        temp = currentTag;
        if ((temp == tag) && (ocurrenceIndex <= ocurrenceCount++)) {
            break;
        }
    }
    sCurrentFiles[id].padTagStart = padTagStart;
    return currentTag;
}

void uvFileSetPadTagStart(s32 id) {
    sCurrentFiles[id].padTagStart = PAD_TAG_START;
}
