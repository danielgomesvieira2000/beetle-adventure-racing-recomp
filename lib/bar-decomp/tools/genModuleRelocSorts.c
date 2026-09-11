/*
 * @file module_tool.c
 * Simple tool to read module files
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <argp.h>
#include <stdarg.h>
#include <linux/swab.h>

#define MIPS_OPCODE(x) ((x) >> 26)
#define MIPS_JUMP_TARGET(insn) (((insn) & 0x003FFFFF) << 1)
#define MODULE_FILES_CODE_BYTES_START 0x50
#define EXTERNAL_FUNCS_SYM_BASE_ADDR 0x80000000
#define MIPS_OPCODE(x) ((x) >> 26)
#define MODULE_FILES_SECTION_FAKE_VADDR 0x400000

typedef enum MipsRelocation_e {
    MIPS_RELOC_HI16 = 0,
    MIPS_RELOC_LO16 = 1,
    MIPS_RELOC_26 = 2,
    MIPS_UNK_RELOC_3 = 3,
    MIPS_UNK_RELOC_4 = 4,
    MIPS_RELOC_32 = 5,
    MIPS_UNK_RELOC_6 = 6,
} MipsRelocation;

typedef enum MipsInstructionSection_e {
    INSTRUCTION_SECTION_TEXT,
    INSTRUCTION_SECTION_DATA,
    INSTRUCTION_SECTION_RODATA,
} MipsInstructionSection;

typedef enum SymbolSection_e {
    SYM_SECTION_TEXT,
    SYM_SECTION_RODATA,
    SYM_SECTION_DATA,
    SYM_SECTION_BSS,
} SymbolSection;

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
} ModuleFileHeader; // size 0x44

typedef struct ModuleFileMdbgInfo_s {
    int32_t mdbgTag;
    int32_t mdbgSize;
    char mdbgInfo[0x20]; // Module debug info
} ModuleFileMdbgInfo;

typedef struct RelaInfo_s {
    int32_t relaTag;
    int32_t relaSize;
} RelaInfo;

static char CurrentTagName[5];
static FILE *TextRelocsSortFile;
static char TextTextRelocsSortFilePath[300];
static char FileName[200];
static FILE* DataRelocsSortFile;
static char DataRelocsSortFilePath[300];
static FILE *SymbolsFile;
static int EntrySymVaddr;

static int TextRelocsOrder = 0;
static int DataRelocsOrder = 0;

void trimPath(const char *path, char *out) {
    const char *name = strrchr(path, '/'); // find last /
    if (name) {
        name++; // skip '/'
    } else {
        name = path; // no directory
    }

    strcpy(out, name);

    char *dot = strrchr(out, '.'); // remove extension
    if (dot) {
        *dot = '\0';
    }
}

size_t getFileSize(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    size_t fSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return fSize;
}

void byteSwapModuleFileHeader(ModuleFileHeader *header) {
    int *ptr = (int *) header;

    for (int32_t i = 0; i < sizeof(ModuleFileHeader); i += sizeof(int32_t), ptr++) {
        *ptr = __swab32(*ptr);
    }
}

static char *tagToString(uint32_t tag) {
    tag = __swab32(tag);
    char *tagPtr = (char *) &tag;
    for (int i = 0; i < 4; i++) {
        CurrentTagName[i] = tagPtr[i];
    }
    CurrentTagName[5] = '\0';
    return CurrentTagName;
}

void createTextRelocsSortsFile(void) {
    snprintf(TextTextRelocsSortFilePath, sizeof(TextTextRelocsSortFilePath),
             "linker_scripts/us/module_files/%s_text_reloc_sorts.txt", FileName);

    TextRelocsSortFile = fopen(TextTextRelocsSortFilePath, "w");

    if (TextRelocsSortFile == NULL) {
        perror("Can't create relocs file!\n");
        exit(EXIT_FAILURE);
    }

    fclose(TextRelocsSortFile);
}

void appendTextRelocSorts(const char *format, ...) {
    va_list arglist;

    TextRelocsSortFile = fopen(TextTextRelocsSortFilePath, "a");
    if (TextRelocsSortFile == NULL) {
        perror("Can't append to relocs file!");
        exit(EXIT_FAILURE);
    }
    va_start(arglist, format);
    vfprintf(TextRelocsSortFile, format, arglist);
    va_end(arglist);
    fclose(TextRelocsSortFile);
    TextRelocsOrder++;
}

void createDataRelocsSortFile(void) {
    snprintf(DataRelocsSortFilePath, sizeof(DataRelocsSortFilePath),
             "linker_scripts/us/module_files/%s_data_reloc_sorts.txt", FileName);

    DataRelocsSortFile = fopen(DataRelocsSortFilePath, "w");

    if (DataRelocsSortFile == NULL) {
        perror("Can't create relocs file!\n");
        exit(EXIT_FAILURE);
    }

    fclose(DataRelocsSortFile);
}

void appendDataRelocSorts(const char *format, ...) {
    va_list arglist;

    DataRelocsSortFile = fopen(DataRelocsSortFilePath, "a");
    if (DataRelocsSortFile == NULL) {
        perror("Can't append to relocs file!");
        exit(EXIT_FAILURE);
    }
    va_start(arglist, format);
    vfprintf(DataRelocsSortFile, format, arglist);
    va_end(arglist);
    fclose(DataRelocsSortFile);
    DataRelocsOrder++;
}

#define CURRENT_MIPS_OP (&instructionBase[addend])

void uvDoModuleRelocs(uint8_t *ovlStartPtr, ModuleCommInfo *info, RelaInfo *relaInfo) {
    int32_t symBase;
    int32_t addend;
    int32_t mipsLo16;
    uint32_t haveHi16;
    uint32_t symbolSection;
    uint8_t *instructionBase;
    int32_t instructionBaseOffset;
    uint8_t *lui;
    int32_t luiRelocOffset;
    union {
        uint32_t lui;
        uint32_t targetInstructionSection;
    } u;
    uint32_t relocType;
    uint32_t pairedHiLoImm;

    uint32_t *relocs = (uint32_t *) (relaInfo + 1);

    haveHi16 = false;


    for (int i = 0; i < info->relocCount; i++) {
        uint32_t entry = __swab32(relocs[i]);

        symbolSection = (uint32_t) entry >> 0x1C;
        u.targetInstructionSection = (uint32_t) (entry & 0x0C000000) >> 0x1A; // 0
        relocType = (uint32_t) (entry & 0x03C00000) >> 0x16;                  // 0
        addend = MIPS_JUMP_TARGET(entry);                                     // 20

        switch (symbolSection) {
            case SYM_SECTION_TEXT:
                symBase = MODULE_FILES_SECTION_FAKE_VADDR;
                break;
            case SYM_SECTION_RODATA:
                symBase = MODULE_FILES_SECTION_FAKE_VADDR + info->textSize;
                break;
            case SYM_SECTION_DATA:
                symBase = MODULE_FILES_SECTION_FAKE_VADDR + info->textSize + info->rodataSize;
                break;
            case SYM_SECTION_BSS:
                symBase = MODULE_FILES_SECTION_FAKE_VADDR + info->textSize + info->rodataSize
                          + info->dataSize;
                break;
        }

        switch (u.targetInstructionSection) {
            case INSTRUCTION_SECTION_TEXT:
                instructionBase = ovlStartPtr;
                instructionBaseOffset = MODULE_FILES_CODE_BYTES_START;
                break;
            case INSTRUCTION_SECTION_DATA:
                instructionBase = &ovlStartPtr[info->textSize + info->rodataSize];
                instructionBaseOffset =
                    MODULE_FILES_CODE_BYTES_START + info->textSize + info->rodataSize;
                break;
            case INSTRUCTION_SECTION_RODATA:
                instructionBase = &ovlStartPtr[info->textSize];
                instructionBaseOffset = MODULE_FILES_CODE_BYTES_START + info->textSize;
                break;
        }

        switch (relocType) {
            case MIPS_RELOC_HI16:
                haveHi16 = true;
                lui = CURRENT_MIPS_OP;
                luiRelocOffset = addend;
                break;

            case MIPS_RELOC_LO16: {
                uint32_t *p_lui = (uint32_t *) lui;
                uint32_t *p_lo = (uint32_t *) CURRENT_MIPS_OP;

                uint32_t lui_native = __builtin_bswap32(*p_lui);
                uint32_t lo_native = __builtin_bswap32(*p_lo);

                int16_t lo_imm = lo_native & 0xFFFF;
                uint32_t full = ((lui_native & 0xFFFF) << 16) + lo_imm + symBase;

                if (lo_imm < 0) {
                    full -= 0x10000;
                }

                appendTextRelocSorts("address: 0x%x order: %d\n", luiRelocOffset, TextRelocsOrder);
                appendTextRelocSorts("address: 0x%x order: %d\n", addend, TextRelocsOrder);

                haveHi16 = false;
                break;
            }
            case MIPS_UNK_RELOC_3:
            case MIPS_UNK_RELOC_4:
                break;
            case MIPS_RELOC_26: {
                uint32_t *p = (uint32_t *) CURRENT_MIPS_OP;
                uint32_t insn = __builtin_bswap32(*p);
                uint32_t imm26 = insn & 0x03FFFFFF;
                uint32_t target = MODULE_FILES_SECTION_FAKE_VADDR | (imm26 << 2);
                appendTextRelocSorts("address: 0x%x order: %d\n", addend, TextRelocsOrder);
                break;
            }
            case MIPS_UNK_RELOC_6:
            case MIPS_RELOC_32: {
                uint32_t *p = (uint32_t *) CURRENT_MIPS_OP;
                uint32_t insn = __builtin_bswap32(*p);
                insn += symBase;

                appendDataRelocSorts("address: 0x%x order: %d\n", addend, DataRelocsOrder);
                break;
            }
            default:
                break;
        }
    }
}

void genRelocSorts(ModuleFileHeader *header, RelaInfo *relaInfo) {
    uint32_t *relocs = (uint32_t *) (relaInfo + 1);
    int count = header->commInfo.relocCount;

    printf("\n-- Relocation Entries --\n");

    for (int i = 0; i < count; i++) {

        uint32_t entry = __swab32(relocs[i]);

        uint32_t cmd = entry >> 28;
        uint32_t section = (entry & 0x0C000000) >> 26;
        uint32_t type = (entry & 0x03C00000) >> 22;
        uint32_t addend = MIPS_JUMP_TARGET(entry);

        //printf("\n[%d]\n", i);
        //printf("  Raw:        %08X\n", entry);
        //printf("  Instruction section:        %s\n", mipInstrSectionToString(section));
        //printf("  Symbol section:    %s\n", symbolSectionToString(cmd));
        //printf("  Type:       %u (%s)\n", type, relocTypeToString(type));
        //printf("  Addend:     0x%X\n", addend);

        // Optional: compute file offset of relocation target
        uint32_t sectionBase = 0;

        switch (section) {
            case 0:
                sectionBase = MODULE_FILES_CODE_BYTES_START;
                break;
            case 1:
                sectionBase = MODULE_FILES_CODE_BYTES_START + header->commInfo.textSize;
                break;
            case 2:
                sectionBase = MODULE_FILES_CODE_BYTES_START + header->commInfo.textSize
                              + header->commInfo.rodataSize;
                break;
        }

        //printf("  File offset: 0x%X\n", addend);
        if (type != MIPS_RELOC_32) {
            appendTextRelocSorts("rom: 0x%x order: %d\n", addend, TextRelocsOrder);
        } else {
            appendDataRelocSorts("rom: 0x%x order: %d\n", addend, DataRelocsOrder);
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Gen module relocs sort {input}\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "rb");

    if (fp == NULL) {
        perror("Can't open input file");
        exit(EXIT_FAILURE);
    }

    size_t fileSize = getFileSize(fp);
    uint8_t *buf = malloc(fileSize);

    if (buf == NULL) {
        perror("Can't allocate memory!\n");
        exit(EXIT_FAILURE);
    }

    fread(buf, fileSize, 1, fp);
    ModuleFileHeader fileHeader = *(ModuleFileHeader *) buf;
    byteSwapModuleFileHeader(&fileHeader);

    // Validate file
    if (fileHeader.formTag != 'FORM' || fileHeader.moduTag != 'MODU' || fileHeader.padTag != 'PAD '
        || fileHeader.commTag != 'COMM') {
        printf("Invalid module file\n");
        free(buf);
        exit(EXIT_FAILURE);
    }

    int mdbgEntry = MODULE_FILES_CODE_BYTES_START + fileHeader.commInfo.textSize
                    + fileHeader.commInfo.dataSize + fileHeader.commInfo.rodataSize;
    ModuleFileMdbgInfo *mdbgInfo = (ModuleFileMdbgInfo *) &buf[mdbgEntry];

    RelaInfo *relaInfo = (RelaInfo *) &buf[mdbgEntry + sizeof(ModuleFileMdbgInfo)];

    trimPath(argv[1], FileName);

    createTextRelocsSortsFile();
    createDataRelocsSortFile();
    EntrySymVaddr = MODULE_FILES_SECTION_FAKE_VADDR + fileHeader.commInfo.entryPointOffset;
    genRelocSorts(&fileHeader, relaInfo);

    fclose(fp);
    free(buf);
    return 0;
}
