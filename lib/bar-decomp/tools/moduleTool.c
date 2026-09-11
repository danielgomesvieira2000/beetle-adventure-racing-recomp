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

typedef struct ModuleToolArguments_s {
    char *input;
    char *output;
    bool printRelocs;
    bool applyRelocs;
    bool writeRelocsFile;
    bool writeSymsFile;
} ModuleToolArguments;

/* Program documentation */
static char ModuleToolDoc[] = "Utility tool to read and manipulate Module files";

/* Positional arguments documentation */
static char ArgsDoc[] = "INPUT";

/* Command line options */
static struct argp_option ModuleToolOptions[] = {
    { "print-relocs", 'p', 0, 0, "Print relocation entries from RELA" },
    { "apply-relocs", 'a', 0, 0,
      "Apply relocations from RELA into the output file and create relocs_addrs.txt and "
      "symbol_addrs.txt" },
    { "write-relocs", 'w', 0, 0, "Write relocs_addrs file" },
    { "symbol-addrs", 's', 0, 0, "Write symbol_addrs file" },
    { "output", 'o', "FILE", 0, "Output file" },
    { 0 }
};

static error_t parseOptions(int key, char *arg, struct argp_state *state);

/* Argp configuration */
static struct argp Argp = { ModuleToolOptions, parseOptions, ArgsDoc, ModuleToolDoc };

static int LocalJalsOffsetPtrCount;
static int *LocalJalsOffsetPtr;
static char CurrentTagName[5];
static FILE *RelocsFile;
static char RelocsFilePath[300];
static char FileName[200];
static char SymbolAddrsFilePath[300];
static FILE *SymbolsFile;
static int EntrySymVaddr;

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

void createRelocsFile(void) {
    snprintf(RelocsFilePath, sizeof(RelocsFilePath),
             "linker_scripts/us/module_files/%s_reloc_addrs.txt", FileName);

    RelocsFile = fopen(RelocsFilePath, "w");

    if (RelocsFile == NULL) {
        perror("Can't create relocs file!\n");
        exit(EXIT_FAILURE);
    }

    fclose(RelocsFile);
}

void appendReloc(const char *format, ...) {
    va_list arglist;

    RelocsFile = fopen(RelocsFilePath, "a");
    if (RelocsFile == NULL) {
        perror("Can't append to relocs file!");
        exit(EXIT_FAILURE);
    }
    va_start(arglist, format);
    vfprintf(RelocsFile, format, arglist);
    va_end(arglist);
    fclose(RelocsFile);
}

void createSymbolAddrs(void) {
    snprintf(SymbolAddrsFilePath, sizeof(SymbolAddrsFilePath),
             "linker_scripts/us/module_files/%s_symbol_addrs.txt", FileName);

    SymbolsFile = fopen(SymbolAddrsFilePath, "w");

    if (SymbolsFile == NULL) {
        perror("Can't create relocs file!\n");
        exit(EXIT_FAILURE);
    }

    fclose(SymbolsFile);
}

void appendSymbol(const char *format, ...) {
    va_list arglist;

    SymbolsFile = fopen(SymbolAddrsFilePath, "a");
    if (SymbolsFile == NULL) {
        perror("Can't append to relocs file!");
        exit(EXIT_FAILURE);
    }
    va_start(arglist, format);
    vfprintf(SymbolsFile, format, arglist);
    va_end(arglist);
    fclose(SymbolsFile);
}

void printModuleHeader(ModuleFileHeader *header) {
    printf("-- Module file header -- \n");
    printf("   Form tag: %s\n", tagToString(header->formTag));
    printf("   File size: %d\n", header->fileSize + 8);
    printf("   Module tag: %s\n", tagToString(header->moduTag));
    printf("   Pad tag: %s\n", tagToString(header->padTag));
    printf("   Pad size: %d\n", header->padSize);
    printf("   Comm tag: %s\n", tagToString(header->commTag));
    printf("   Comm size: %d\n", header->commSize);
}

void printCommInfo(ModuleCommInfo *info) {
    printf("\n-- Module Comm info -- \n");
    printf("   Header size: %x\n", info->headerSize);
    printf("   Module entry point offset: %x\n", info->entryPointOffset);
    printf("   Module text size: %x\n", info->textSize);
    printf("   Module rodata size %x\n", info->rodataSize);
    printf("   Module data size: %x\n", info->dataSize);
    printf("   Module bss size: %x\n", info->bssSize);
    printf("   Module Relocation count: %x\n", info->relocCount);
    printf("   nameTag: %s\n", tagToString(info->nameTag));
    printf("   relaContents not overwritten: %x\n", info->relaContents);
}

void printMdbg(ModuleFileMdbgInfo *info) {
    printf("\n-- Module Mdbg info -- \n");
    printf("     MDBG tag: %s\n", tagToString(__swab32(info->mdbgTag)));
    printf("     MDBG size: %x\n", __swab32(info->mdbgSize));
    printf("     MDBG contents: %s\n", info->mdbgInfo);
}

void printRela(RelaInfo *info) {
    printf("\n-- Rela info -- \n");
    printf("     Rela tag: %s\n", tagToString(__swab32(info->relaTag)));
    printf("     Rela size: %x\n", __swab32(info->relaSize));
}

void printExtraInfo(ModuleCommInfo *info) {
    int textRomStart = MODULE_FILES_CODE_BYTES_START;
    int rodataRomStart = info->textSize + MODULE_FILES_CODE_BYTES_START;
    int dataRomStart = info->textSize + MODULE_FILES_CODE_BYTES_START + info->rodataSize;
    int codeRomEnd = info->textSize + MODULE_FILES_CODE_BYTES_START + info->rodataSize + info->dataSize;
    printf("\n-- Extra info -- \n");
    printf("     Text starts at: %x\n", MODULE_FILES_CODE_BYTES_START);
    printf("     Rodata starts at: %x\n", info->textSize + MODULE_FILES_CODE_BYTES_START);
    printf("     Data starts at: %x\n",
           info->textSize + MODULE_FILES_CODE_BYTES_START + info->rodataSize);
    printf("     Code ends at: %x\n",
           info->textSize + MODULE_FILES_CODE_BYTES_START + info->rodataSize + info->dataSize);

    printf("     Text Vram start: %X\n", MODULE_FILES_SECTION_FAKE_VADDR);
    printf("     Rodata Vram start: %X\n", MODULE_FILES_SECTION_FAKE_VADDR + info->textSize);
    printf("     Data Vram start: %X\n", MODULE_FILES_SECTION_FAKE_VADDR + info->textSize + info->rodataSize);
    printf("     Bss Vram start: %X\n", MODULE_FILES_SECTION_FAKE_VADDR + info->textSize + info->rodataSize + info->dataSize);
}

static const char *relocTypeToString(uint32_t type) {
    switch (type) {
        case MIPS_RELOC_HI16:
            return "R_MIPS_HI16";
        case MIPS_RELOC_LO16:
            return "R_MIPS_LO16";
        case MIPS_RELOC_32:
            return "R_MIPS_32";
        case MIPS_UNK_RELOC_3:
            return "MIPS_UNK_RELOC_3?";
        case MIPS_UNK_RELOC_4:
            return "MIPS_UNK_RELOC_4";
        case MIPS_RELOC_26:
            return "R_MIPS_26";
        case MIPS_UNK_RELOC_6:
            return "MIPS_UNK_RELOC_6";
        default:
            return "UNKNOWN";
    }
}

static const char *mipInstrSectionToString(uint32_t mipsInstrSection) {
    switch (mipsInstrSection) {
        case INSTRUCTION_SECTION_TEXT:
            return ".text";
        case INSTRUCTION_SECTION_DATA:
            return ".data";
        case INSTRUCTION_SECTION_RODATA:
            return ".rodata";
        default:
            return "UNKNOWN";
    }
}

static const char *symbolSectionToString(uint32_t symSection) {
    switch (symSection) {
        case SYM_SECTION_TEXT:
            return ".text";
        case SYM_SECTION_DATA:
            return ".data";
        case SYM_SECTION_RODATA:
            return ".rodata";
        case SYM_SECTION_BSS:
            return ".bss";
        default:
            return "UNKNOWN";
    }
}

void printRelocEntries(ModuleFileHeader *header, RelaInfo *relaInfo) {
    uint32_t *relocs = (uint32_t *) (relaInfo + 1);
    int count = header->commInfo.relocCount;

    printf("\n-- Relocation Entries --\n");

    for (int i = 0; i < count; i++) {

        uint32_t entry = __swab32(relocs[i]);

        uint32_t cmd = entry >> 28;
        uint32_t section = (entry & 0x0C000000) >> 26;
        uint32_t type = (entry & 0x03C00000) >> 22;
        uint32_t addend = MIPS_JUMP_TARGET(entry);

        printf("\n[%d]\n", i);
        printf("  Raw:        %08X\n", entry);
        printf("  Instruction section:        %s\n", mipInstrSectionToString(section));
        printf("  Symbol section:    %s\n", symbolSectionToString(cmd));
        printf("  Type:       %u (%s)\n", type, relocTypeToString(type));
        printf("  Addend:     0x%X\n", addend);

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

        printf("  File offset: 0x%X\n", sectionBase + addend);
    }
}

#define CURRENT_MIPS_OP (&instructionBase[addend])

uint32_t reconstructAddr(uint16_t hi, uint16_t lo) {
    return (hi << 16) + (int16_t)lo;
}

void uvDoModuleRelocs(uint8_t *ovlStartPtr, ModuleCommInfo *info, RelaInfo *relaInfo, bool writeSyms,
                      bool writeRelocs) {
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

    int offset = MODULE_FILES_CODE_BYTES_START;
    for (int i = 0; i < info->relocCount; i++, offset += 4) {
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

                printf("offset: %x, full: %x\n", instructionBaseOffset  + addend, full);

                if (symbolSection != SYM_SECTION_TEXT) {
                    if (writeRelocs) {
                        if (haveHi16) {
                        appendReloc("rom:0x%X reloc:MIPS_HI16 symbol:D_%008X\n",
                                    instructionBaseOffset + luiRelocOffset, full);
                        }
                        appendReloc("rom:0x%X reloc:MIPS_LO16 symbol:D_%008X\n",
                                    instructionBaseOffset + addend, full);
                    }
                    if (writeSyms) {
                        appendSymbol("D_00%X = 0x%X;\n", full, full);
                    }
                } else {
                    if (writeRelocs) {
                        if (haveHi16) {
                        appendReloc("rom:0x%X reloc:MIPS_HI16 symbol:func_%s_%08X\n",
                                    instructionBaseOffset + luiRelocOffset, FileName, full);
                        }
                        appendReloc("rom:0x%X reloc:MIPS_LO16 symbol:func_%s_%08X\n",
                                    instructionBaseOffset + addend, FileName, full);
                    }
                }

                haveHi16 = false;
                break;
            }
            case MIPS_UNK_RELOC_3:
            case MIPS_UNK_RELOC_4:
                break;
            case MIPS_RELOC_26: {
                uint32_t *p = (uint32_t *) CURRENT_MIPS_OP;
                uint32_t insn = __builtin_bswap32(*p);
                insn += ((symBase & 0x0FFFFFFF) >> 2);
                *p = __builtin_bswap32(insn);
                int newCount = LocalJalsOffsetPtrCount + 1;

                int *tmp = realloc(LocalJalsOffsetPtr, newCount * sizeof(*LocalJalsOffsetPtr));
                if (tmp == NULL) {
                    perror("realloc failed");
                    abort();
                }
                LocalJalsOffsetPtr = tmp;
                LocalJalsOffsetPtr[LocalJalsOffsetPtrCount] = addend;
                LocalJalsOffsetPtrCount = newCount;
                break;
            }
            case MIPS_UNK_RELOC_6:
            case MIPS_RELOC_32: {
                uint32_t *p = (uint32_t *) CURRENT_MIPS_OP;
                uint32_t insn = __builtin_bswap32(*p);
                insn += symBase;
                *p = __builtin_bswap32(insn);

                if (symbolSection != SYM_SECTION_TEXT) {
                    if (writeRelocs) {
                        appendReloc("rom:0x%X reloc:MIPS_32 symbol:D_%008X\n",
                                    instructionBaseOffset + addend, insn);
                    }
                    if (writeSyms) {
                        appendSymbol("D_00%X = 0x%X;\n", insn, insn);
                    }

                } else {
                    // Some module files like to reference the entry function in data.
                    if (EntrySymVaddr == insn) {
                        printf("Emitting reloc for entry sym\n");
                        if (writeRelocs) {
                            appendReloc("rom:0x%X reloc:MIPS_32 symbol:__entrypoint_func_%s_%x\n",
                                        instructionBaseOffset + addend, FileName, insn);
                        }
                    } else {
                        // Jumptables are only in rodata 
                        if (u.targetInstructionSection == INSTRUCTION_SECTION_RODATA) {
                            if (writeRelocs) {
                                appendReloc("rom:0x%X reloc:MIPS_32 symbol:.L%008X\n",
                                            instructionBaseOffset + addend, insn);
                            }
                        }

                        if (u.targetInstructionSection == INSTRUCTION_SECTION_DATA) {
                            if (symbolSection == SYM_SECTION_TEXT) {
                                if (writeRelocs) {
                                    appendReloc("rom:0x%X reloc:MIPS_32 symbol:func_%s_%08X\n",
                                                instructionBaseOffset + addend, FileName, insn);
                                }
                            } else {
                                 if (writeRelocs) {
                                    appendReloc("rom:0x%X reloc:MIPS_32 symbol:D_%008X\n",
                                                instructionBaseOffset + addend, insn);
                                }
                            }
                        }
                    }
                }

                break;
            }
            default:
                break;
        }
    }
}

void uvDoExternalRelocs(uint8_t *data, size_t size, bool writeRelocs) {
    uint32_t *instrPtr = (uint32_t *) data;

    for (int addend = 0x50; addend < size + MODULE_FILES_CODE_BYTES_START; addend += 4, instrPtr++) {
        uint32_t ins = __swab32(*instrPtr);

        // JAL = 3
        if (MIPS_OPCODE(ins) == 3) {
            bool isLocal = false;

            for (int i = 0; i < LocalJalsOffsetPtrCount; i++) {
                int offset = LocalJalsOffsetPtr[i] + MODULE_FILES_CODE_BYTES_START;
                if (addend == offset) {
                    isLocal = true;
                    break;
                }
            }
            // printf("Jal at offset %x\n", addend);

            if (!isLocal) {
                uint32_t imm26 = ins & 0x03FFFFFF;
                uint32_t target = 0x80000000 | (imm26 << 2);

                if (writeRelocs) {
                    appendReloc("rom:0x%X reloc:MIPS_26 symbol:func_%08X\n", addend, target);
                }
            }
        }
    }
}

static error_t parseOptions(int key, char *arg, struct argp_state *state) {
    ModuleToolArguments *arguments = state->input;

    switch (key) {
        case 'p':
            arguments->printRelocs = true;
            break;
        case 'a':
            arguments->applyRelocs = true;
            break;
        case 'o':
            arguments->output = arg;
            break;
        case 'w':
            arguments->writeRelocsFile = true;
            break;
        case 's':
            arguments->writeSymsFile = true;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num == 0) {
                arguments->input = arg;
            } else {
                argp_usage(state);
            }
            break;

        case ARGP_KEY_END:
            /* Require INPUT */
            if (state->arg_num < 1) {
                argp_error(state, "Missing INPUT file");
            }

            /* If -a is used, -o must be provided */
            if (arguments->applyRelocs && arguments->output == NULL) {
                argp_error(state, "--apply-relocs requires --output");
            }

            if ((arguments->writeSymsFile || arguments->writeRelocsFile) && !arguments->applyRelocs){
                argp_error(state, "--write-relocs or --symbol-addrs requires --apply-relocs");
            }

            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    ModuleToolArguments arguments = { 0 };

    argp_parse(&Argp, argc, argv, 0, 0, &arguments);

    char *inputFilename = arguments.input;
    char *outputFileName = NULL;

    if (arguments.output) {
        outputFileName = arguments.output;
    }

    FILE *fp = fopen(arguments.input, "rb");

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

    printModuleHeader(&fileHeader);
    printCommInfo(&fileHeader.commInfo);

    int mdbgEntry = MODULE_FILES_CODE_BYTES_START + fileHeader.commInfo.textSize
                    + fileHeader.commInfo.dataSize + fileHeader.commInfo.rodataSize;
    ModuleFileMdbgInfo *mdbgInfo = (ModuleFileMdbgInfo *) &buf[mdbgEntry];
    printMdbg(mdbgInfo);

    RelaInfo *relaInfo = (RelaInfo *) &buf[mdbgEntry + sizeof(ModuleFileMdbgInfo)];
    printRela(relaInfo);
    printExtraInfo(&fileHeader.commInfo);

    if (arguments.printRelocs) {
        printRelocEntries(&fileHeader, relaInfo);
    }

    if (arguments.applyRelocs) {
        LocalJalsOffsetPtr = NULL;
        LocalJalsOffsetPtrCount = 0;

        trimPath(arguments.input, FileName);
        if (arguments.writeSymsFile) {
            createSymbolAddrs();
        }

        if (arguments.writeRelocsFile) {
            createRelocsFile();
        }
        EntrySymVaddr = MODULE_FILES_SECTION_FAKE_VADDR + fileHeader.commInfo.entryPointOffset;
        if (arguments.writeSymsFile) {
            appendSymbol("__entrypoint_func_%s_%x = 0x%x;\n", FileName, EntrySymVaddr, EntrySymVaddr);
        }
        uvDoModuleRelocs(&buf[MODULE_FILES_CODE_BYTES_START], &fileHeader.commInfo, relaInfo,
                         arguments.writeSymsFile, arguments.writeRelocsFile);
        uvDoExternalRelocs(&buf[MODULE_FILES_CODE_BYTES_START], fileHeader.commInfo.textSize,
                           arguments.writeRelocsFile);

        FILE *outFile = fopen(outputFileName, "w");

        if (outFile == NULL) {
            perror("Can't write output file!");
            fclose(fp);
            free(buf);
            free(LocalJalsOffsetPtr);
            exit(EXIT_FAILURE);
        }

        fwrite(buf, fileSize, 1, outFile);
        fclose(outFile);
    }

    fclose(fp);
    free(LocalJalsOffsetPtr);
    free(buf);
    return 0;
}
