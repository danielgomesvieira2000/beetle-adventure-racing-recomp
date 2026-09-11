// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef BAR_UV_FILESYSTEM_H
#define BAR_UV_FILESYSTEM_H

#define FILE_NOT_COMPRESSED 1
#define FILE_COMPRESSED 2

#include <PR/ultratypes.h>

s32 uvFileReadHeader(u8* data);
void uvFileFree(s32 idx);
u8* uvFileGetDataPtr(s32 id);
s32 uvFileGetSize(s32 id);
s32 uvFileGetPadType(s32 id);
u32 uvFileGetEntryTag(s32 id, u32* sizeOut, void** dest);
u32 uvFileSearchTag(s32 id, u32* sizeOut, void** data, s32 tag, s32 ocurrenceIndex);
void uvFileSetPadTagStart(s32 id);
u32 uvFileReadBlock(s32 fileId, u32 *sizeOut, void **data, s32 decodeFlag);
#endif /* BAR_UV_FILESYSTEM_H */
