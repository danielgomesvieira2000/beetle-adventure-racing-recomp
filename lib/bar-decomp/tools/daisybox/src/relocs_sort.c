#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "uthash.h"
#include "cjson/cJSON.h"

typedef struct Reloc_s {
    int32_t address; // key
    int32_t order;
    UT_hash_handle hh;
} Reloc;

Reloc* RelocTab = NULL;

void RelocSort_AddReloc(int32_t address, int order) {
    Reloc *s = malloc(sizeof(Reloc));

    s->address = address;
    s->order = order;
    HASH_ADD_INT(RelocTab, address, s);
}

int32_t RelocSort_GetRelocOrder(int32_t key) {
    Reloc *s;

    HASH_FIND_INT(RelocTab, &key, s);

    if (s != NULL) {
        //printf("Reloc with addr %x has order %d\n", s->address, s->order);
        return s->order;
    }

    return UINT32_MAX;
}

void RelocSort_Destroy(void) {
    Reloc *current;
    Reloc *tmp;

    HASH_ITER(hh, RelocTab, current, tmp) {
        HASH_DEL(RelocTab, current);
        free(current);
    }
}
