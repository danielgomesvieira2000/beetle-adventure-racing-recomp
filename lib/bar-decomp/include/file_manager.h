#ifndef BAR_FILE_MANAGER
#define BAR_FILE_MANAGER

/*
 * Starts at 0x80036010
 * Represents a file entry in the file system table
 */
typedef struct FormFileEntry_s {
    u8 *romPtr;        // ROM offset
    u8 *allocPtr;      // Dynamic ptr of each file
    s32 instanceCount; // Times a file is loaded, used for marking the file as loaded or unloaded
    s32 unused;        // Unused field, used for debugging
} FormFileEntry;

/*
 * Starts at 0x80035F38
 * Represents an entry in the file system table
 */
typedef struct FormTableEntry_s {
    s32 tag;
    u16 filesCount;
    FormFileEntry *fileEntry;
} FormTableEntry;


/* Functions */
void func_80002088(s32 *arg0, s32 *arg1, s32 *arg2, s32 arg3);
void func_8000218C(s32 *arg0, s32 *arg1, s32 *arg2, s32 arg3);
void func_8000226C(s32 *tagPtr, s32 *arg1, s32 *arg2, u32 arg3);
void func_80002390(s32 arg0);
void *uvAllocFile(s32 formFileTag, s32 fileId);
s32 uvCheckValidFileId(s32 tag, s32 fileId);
void uvConsumeBytes(void *dst, u8 **ptr, u32 size);
u32 uvFileReadBlock(s32 fileId, u32 *sizeOut, void **data, s32 decodeFlag);
u8 *uvGetFileData(s32 formFileTag, s32 fileId);
u16 uvGetFileInstanceCount(s32 tag, s32 fileId);
u8 *uvGetFileRomPtr(s32 formFileId);
s32 uvGetFilesCount(s32 tag);
s32 uvGetFormFileIdByTag(s32 tag);
s32 uvGetFormFileIdByTagDup(s32 tag);
void *uvGetLoadedFile(s32 formFileTag, s32 fileId);
s32 uvGetUnusedFileInfo(s32 tag, s32 fileId);
void *uvLoadFile(s32 formFileTag, s32 fileId);
void uvLoadFormFiles(void);
void uvResetAllFilesInstanceCount(void);
void uvUnloadFile(s32 formFileTag, s32 fileId);
void uvUpdateFileAllocPtr(void *allocPtr);

extern FormTableEntry* gFormFiles;
#endif /* BAR_FILE_MANAGER */
