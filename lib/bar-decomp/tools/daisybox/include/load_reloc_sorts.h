#ifndef LOAD_RELOC_SORTS_H
#define LOAD_RELOC_SORTS_H
#include <stdint.h>
typedef struct RelocOrderInfo_s {
    int32_t address;
    int32_t order;
} RelocOrderInfo;

RelocOrderInfo LoadRelocOrder_Parse(char *str);
void LoadRelocOrder(char* relocSortsFileName);
void LoadRelocOrder_Destroy(void);
#endif /* LOAD_RELOC_SORTS_H */