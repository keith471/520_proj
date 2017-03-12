
#include "tree.h"

#define HashSize 317

typedef struct SymbolTable {
    SYMBOL *table[HashSize];
    struct SymbolTable *next;  // a pointer to the immediate outer scope of this scope
} SymbolTable;

SymbolTable *symbolTable;
