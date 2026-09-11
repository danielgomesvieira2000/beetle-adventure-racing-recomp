#ifndef MANAGER_TYPES_H
#define MANAGER_TYPES_H
#define BAR_MODULE_FILES_COUNT 133

typedef struct ModuleHardcodedInfo_s {
    int32_t headeredSize;
    int32_t relocVal; // Reloc value not overwritten
} ModuleHardcodedInfo;

typedef struct BarModuleFilesInfo_s {
    char *moduleName;
    ModuleHardcodedInfo info;
} BarModuleFilesInfo;


#endif /* MANAGER_TYPES_H */