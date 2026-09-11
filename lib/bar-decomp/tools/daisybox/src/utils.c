#include "global.h"
#include "module_types.h"


/*
* @brief Gets the size of a file.
* @param fp File pointer
* @return Size of the file in bytes.
*/
size_t Utils_GetFileSize(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    size_t fSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return fSize;
}

/*
* @brief Trims the path of a file, removing the directory and extension.
* @param path Path to the file.
* @param out Output buffer for the trimmed path.
*/
void Utils_TrimPath(const char *path, char *out) {
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

int Utils_EncodeInstructionSection(asection *sec) {
    if (strcmp(sec->name, ".text") == 0) {
        return 0;
    } else if (strcmp(sec->name, ".data") == 0) {
        return 1;
    } else if (strcmp(sec->name, ".rodata") == 0) {
        return 2;
    } else if (strcmp(sec->name, ".bss") == 0) {
        return 3;
    }

    return -1;
}

int Utils_EncodeSymbolSection(asection *sec) {
    if (strcmp(sec->name, ".text") == 0) {
        return 0;
    } else if (strcmp(sec->name, ".rodata") == 0) {
        return 1;
    } else if (strcmp(sec->name, ".data") == 0) {
        return 2;
    } else if (strcmp(sec->name, ".bss") == 0) {
        return 3;
    }

    return -1;
}

uint32_t Utils_EncodeReloc(uint32_t symbolSection, uint32_t targetSection, uint32_t relocType,
                           int32_t addend) {
    uint32_t word = 0;

    word |= (symbolSection & 0xF) << 28;
    word |= (targetSection & 0x3) << 26;
    word |= (relocType & 0xF) << 22;
    word |= ((addend >> 1) & 0x3FFFFF);

    return word;
}

int Utils_ConvertRelocType(int relocType) {
    switch (relocType) {
        case R_MIPS_26:
            return MIPS_RELOC_26;
        case R_MIPS_HI16:
            return MIPS_RELOC_HI16;
        case R_MIPS_LO16:
            return MIPS_RELOC_LO16;
        case R_MIPS_32:
            return MIPS_RELOC_32;
        default:
            log_warn("!!! Unknown reloc type: %d", relocType);
            return -1;
    }
}

int Utils_StringToTag(char *tagStr) {
    int ptr;

    if (strlen(tagStr) < 4) {
        printf("Invalid size for tag string!\n");
        return -1;
    }

    memcpy(&ptr, tagStr, 4);
    return ptr;
}