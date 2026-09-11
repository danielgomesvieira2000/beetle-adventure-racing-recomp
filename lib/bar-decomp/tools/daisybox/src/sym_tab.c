#include <stdio.h>
#include <stdlib.h>
#include "uthash.h"
#include "cjson/cJSON.h"

typedef struct {
    char name[200];
    uint32_t addr; // vram
    UT_hash_handle hh;
} Symbol;

Symbol *Symtab = NULL;

void Symtab_AddSymbol(const char *name, uint32_t addr) {
    Symbol *s = malloc(sizeof(Symbol));

    strncpy(s->name, name, 190);
    s->addr = addr;

    HASH_ADD_STR(Symtab, name, s);
}

uint32_t Symtab_ResolveSymbol(const char *name) {
    Symbol *s;

    HASH_FIND_STR(Symtab, name, s);

    if (s != NULL) {
        return s->addr;
    }
    return -1;
}

void Symtab_Destroy(void) {
    Symbol *current, *tmp;

    HASH_ITER(hh, Symtab, current, tmp) {
        HASH_DEL(Symtab, current);
        free(current);
    }
}
