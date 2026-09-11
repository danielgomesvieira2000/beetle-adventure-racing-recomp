#ifndef RELOC_SORTS_H
#define RELOC_SORTS_H
#include <stdint.h>

void RelocSort_AddReloc(int32_t address, int order);
int32_t RelocSort_GetRelocOrder(int32_t address);
void RelocSort_Destroy(void);
#endif /* RELOC_SORTS_H */