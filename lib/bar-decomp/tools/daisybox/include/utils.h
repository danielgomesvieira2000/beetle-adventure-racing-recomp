#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#include "bfd.h"
#include "module_types.h"

//void Utils_Fwrite()
size_t Utils_GetFileSize(FILE *fp);
bool Utils_CompareFileHash(FILE *file1, char* hash) ;
void Utils_TrimPath(const char *path, char *out);
int Utils_EncodeInstructionSection(asection *sec);
int Utils_EncodeSymbolSection(asection *sec);
uint32_t Utils_EncodeReloc(uint32_t symbolSection, uint32_t targetSection, uint32_t relocType,
                            int32_t addend);
int Utils_ConvertRelocType(int relocType);
int Utils_StringToTag(char *tagStr);

#endif // UTILS_H
