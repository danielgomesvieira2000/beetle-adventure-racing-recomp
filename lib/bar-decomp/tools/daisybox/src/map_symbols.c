#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils.h"
#include "stb_log.h"
#include "cjson/cJSON.h"
#include "symtab.h"
#include "utils.h"

static cJSON* Root;

void MapSymbols_Init(char* mapFileName) {
    char* mapFileData;
    size_t mapFileSize;
    FILE* mapFile;

    mapFile = fopen(mapFileName, "rb");

    if (mapFile == NULL) {
        perror("Can't open map file!\n");
        exit(EXIT_FAILURE);
    }

    mapFileSize = Utils_GetFileSize(mapFile);
    mapFileData = malloc(mapFileSize);
    if (mapFileData == NULL) {
        perror("Can't alloc memory for map file data!");
        fclose(mapFile);
        exit(EXIT_FAILURE);
    }

    fread(mapFileData, mapFileSize, 1, mapFile);

    Root = cJSON_Parse(mapFileData);
    if (Root == NULL) {
        fclose(mapFile);
        free(mapFileData);
        printf("JSON parse failed!\n");
        exit(EXIT_FAILURE);
    }

    fclose(mapFile);
    free(mapFileData);
}

void MapSymbols_LoadLinkerFileSymbols(char* filePath) {
    char* linkSymsPath = filePath;
    FILE* undefinedSymsFile = fopen(linkSymsPath, "r");
    if (undefinedSymsFile == NULL) {
        log_fatal("Can't open undefined syms file!\n");
        perror(linkSymsPath);
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), undefinedSymsFile) != NULL) {
        char symName[256];
        uint32_t symAddr;
        if (sscanf(line, "%s = 0x%X;", symName, &symAddr) == 2) {
            log_info("Loading hardcoded symbol: %s = 0x%X\n", symName, symAddr);
            Symtab_AddSymbol(symName, symAddr);
        }
    }

    fclose(undefinedSymsFile);
}

void MapSymbols_Load(void) {
    cJSON *segments = cJSON_GetObjectItem(Root, "segments");
    cJSON *seg;
    cJSON_ArrayForEach(seg, segments) {
        cJSON *files = cJSON_GetObjectItem(seg, "files");
        cJSON *file;
        cJSON_ArrayForEach(file, files) {
            cJSON *symbols = cJSON_GetObjectItem(file, "symbols");

            cJSON *sym;
            cJSON_ArrayForEach(sym, symbols) {
                const char *name = cJSON_GetObjectItem(sym, "name")->valuestring;

                const char *vram = cJSON_GetObjectItem(sym, "vram")->valuestring;

                uint32_t addr = strtoul(vram, NULL, 16);
                //printf("Loading symbol: %s\n", name);
                Symtab_AddSymbol(name, addr);
            }
        }
    }

    MapSymbols_LoadLinkerFileSymbols("linker_scripts/us/kernel_link_scripts_syms.txt");
    MapSymbols_LoadLinkerFileSymbols("linker_scripts/us/hw_syms.txt");
}

void MapSymbols_Destroy(void) {
    free(Root);
    Symtab_Destroy();
}
