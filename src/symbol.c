
#include "memory.h"
#include "symbol.h"
#include "error.h"

/*
 * Hash an identifier into an index
 */
int Hash(char *str) {
    unsigned int hash = 0;
    while (*str) hash = (hash << 1) + *str++;
    return hash % HashSize;
}

SymbolTable *initSymbolTable() {
    SymbolTable *t;
    int i;
    t = NEW(SymbolTable);
    for (i=0; i < HashSize; i++) t->table[i] = NULL;
    t->next = NULL;
    return t;
}

/*
 * adds an inner scope to the symbol table
 * s is the outer scope
 */
SymbolTable *scopeSymbolTable(SymbolTable *s) {
    SymbolTable *t;
    // create a new inner scope
    t = initSymbolTable();
    // set the inner scope's outer scope to s
    t->next = s;
    // return the inner scope
    return t;
}

SYMBOL *putSymbol(SymbolTable *t, char *name, SymbolKind kind) {
    int i = Hash(name);
    SYMBOL *s;
    for (s = t->table[i]; s; s = s->next) {
        // check if the symbol already exists
        // TODO handle preexisting symbols
        if (strcmp(s->name, name) == 0) return s;
    }
    s = NEW(SYMBOL);
    s->name = name;
    s->kind = kind;
    s->next = t->table[i];
    t->table[i] = s;
    return s;
}

SYMBOL *getSymbol(SymbolTable *t, char *name) {
    int i = Hash(name);
    SYMBOL *s;
    for (s = t->table[i]; s; s = s->next) {
        if (strcmp(s->name,name)==0) return s;
    }
    // if at the outermost scope, we couldn't find the symbol
    // TODO should we return null?
    if (t->next == NULL) return NULL;
    // else, search the next outer scope
    return getSymbol(t->next,name);
}

/*
 * checks to see if a symbol is already defined in the current scope
 */
int alreadyDefined(SymbolTable *t, char *name) {
    int i = Hash(name);
    SYMBOL *s;
    for (s = t->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return 1;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// PROGRAM TRAVERSAL
////////////////////////////////////////////////////////////////////////////////////

/*
 * symbol table format
 * name
 * kind: package, variable, type, parameter, function, field
 * scope: local, global (to distinguish between local and global variables and types)
 * type
 */

void symPROGRAM(PROGRAM *p) {
    symbolTable = initSymbolTable();
    symPACKAGE(p->package, symbolTable);
    symTOPLEVELDECLARATION(p->topLevelDeclaration, symbolTable);
}

void symPACKAGE(PACAKGE* p, SymbolTable* symbolTable) {
    SYMBOL* s;
    // only one package allowed, so there can never be package (re-)naming conflicts
    s = putSymbol(sym, p->name, packageDeclSym);
    s->val.packageDeclS = p;
}

void symTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld, SymbolTable* symbolTable) {
    if (tld == NULL) return;
    switch (tld->kind) {
        case vDeclK:
            symVARDECLARATION(tld->val.varDeclTLD, symbolTable);
            break;
        case tDeclK:
            symTYPEDECLARATION(tld->val.typeDeclTLD, symbolTable);
            break;
        case functionDeclK:
            symFUNCTIONDECLARATION(tld->val.functionDeclTLD, symbolTable);
            break;
        default:
            break;
    }
    symTOPLEVELDECLARATION(tld->next, symbolTable);
}

typedef struct VARDECLARATION {
    int lineno;
    enum { typeOnlyK, expOnlyK, typeAndExpK } kind;
    struct ID* ids; // weed to ensure matching length with exps, if expOnlyK or typeAndExpK
    int isDistributed;  // whether this declaration is part of a distrubuted statement
    int isLocal;    // whether this declaration is local (as opposed to global)
    union {
        struct TYPE* typeVD;
        struct EXP* expVD;
        struct {struct TYPE* type;
                struct EXP* exp;} typeAndExpVD;
    } val;
    struct VARDECLARATION* next;    // for distributed variable declarations; else this is null
} VARDECLARATION;

void symImplementationLOCAL(LOCAL *l, SymbolTable *sym) {
    SYMBOL *s;
    if (l!=NULL) {
        symImplementationLOCAL(l->next,sym);
        symTYPE(l->type,sym);
        if (defSymbol(sym,l->name)) {
            reportStrError("local %s already declared",l->name,l->lineno);
        } else {
            s = putSymbol(sym,l->name,localSym);
            s->val.localS = l;
        }
    }
}

void symVARDECLARATION(VARDECLARATION* vd, SymbolTable* symbolTable) {
    if (vd == NULL) return;
    switch (vd->kind) {
        case typeOnlyK:
            break;
        case expOnlyK:

            break;
        case typeAndExpK:

            break;
        default:
            break;
    }
    symVARDECLARATION(vd->next, symbolTable);
}

void symTYPEDECLARATION(TYPEDECLARATION* td, SymbolTable* symbolTable) {

}

void symFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd, SymbolTable* symbolTable) {

}

typedef struct SYMBOL {
    char *name;
    SymbolKind kind;
    union {
        struct PACAKGE *packageDeclS;
        struct TYPEDECLARATION *typeDeclS;
        struct VARDECLARATION *varDeclS;
        struct FUNCTIONDECLARATION *functionDeclS;
        struct PARAMETER *parameterS;
        struct FIELD *fieldS;
    } val;
    // this is a linked list in the SymbolTable hashmap, so we have to have this next field.
    // it doesn't actually have anything to do with the 'current' symbol
    struct SYMBOL *next;
} SYMBOL;
