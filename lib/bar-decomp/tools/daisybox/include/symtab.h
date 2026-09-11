#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdint.h>

void Symtab_AddSymbol(const char *name, uint32_t addr);
uint32_t Symtab_ResolveSymbol(const char *name);
void Symtab_Destroy(void);
#endif /* SYMTAB_H */
