#ifndef MODULE_TYPES_H
#define MODULE_TYPES_H

typedef struct ModuleCommInfo_s {
    uint32_t headerSize;
    int32_t entryPointOffset;
    int32_t textSize;
    int32_t rodataSize;
    int32_t dataSize;
    int32_t bssSize;
    int32_t relocCount; // Reloc count?
    int32_t nameTag;
    int32_t relaContents;
} ModuleCommInfo; // size = 0x24

typedef struct ModuleFileHeader_s {
    int32_t formTag;
    int32_t fileSize;
    int32_t moduTag;
    int32_t padTag;
    int32_t padSize;
    int32_t padContents; // Structure padding
    int32_t commTag;
    int32_t commSize;
    ModuleCommInfo commInfo;
} ModuleFileHeader; // size 0x48

typedef struct ModuleFileMdbgInfo_s {
    int32_t mdbgTag;
    int32_t mdbgSize;
    char mdbgInfo[0x20]; // Module debug info
} ModuleFileMdbgInfo;

typedef struct CodeInfo_s {
    int32_t codeTag;
    int32_t codeSize;
} CodeInfo;

typedef struct RelaInfo_s {
    int32_t relaTag;
    int32_t relaSize;
} RelaInfo;

// Encoded reloc types
typedef enum MipsRelocation_e {
    MIPS_RELOC_HI16 = 0,
    MIPS_RELOC_LO16 = 1,
    MIPS_RELOC_26 = 2,
    MIPS_UNK_RELOC_3 = 3,
    MIPS_UNK_RELOC_4 = 4,
    MIPS_RELOC_32 = 5,
    MIPS_UNK_RELOC_6 = 6,
} MipsRelocation;

#endif /* MODULE_TYPES_H */