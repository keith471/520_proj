
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

SYMBOL *putSymbol(char *name, SymbolKind kind, SymbolTable *t) {
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

SYMBOL *getSymbol(char *name, SymbolTable *t) {
    int i = Hash(name);
    SYMBOL *s;
    for (s = t->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    // if at the outermost scope, we couldn't find the symbol
    // TODO should we return null?
    if (t->next == NULL) return NULL;
    // else, search the next outer scope
    return getSymbol(name, t->next);
}

/*
 * checks to see if a symbol is already defined in the current scope
 */
int alreadyDefined(char *name, SymbolTable *t) {
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
    s = putSymbol(p->name, packageDeclSym, symbolTable);
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

/*
 * add a symbol to the symbol table for every unique variable
 */
void symVARDECLARATIONids(VARDECLARATION* vd, ID* ids, int varNum, SymbolTable* symbolTable) {
    if (ids == NULL) return;
    SYMBOL* s;
    if (alreadyDefined(ids->name, symbolTable)) {
        // report error
        reportSymError("variable already exists", ids->name, vd->lineno);
    } else {
        s = putSymbol(ids->name, varDeclSym, symbolTable);
        SINGLEVAR* sv;
        sv = NEW(SINGLEVAR);
        sv->varDecl = vd;
        sv->varNum = varNum;
        s->val.varDeclS = sv;
        ids->symbol = s;
    }
    // recurse
    symVARDECLARATIONids(vd, ids->next, varNum + 1);
}

void symVARDECLARATION(VARDECLARATION* vd, SymbolTable* symbolTable) {
    if (vd == NULL) return;
    symVARDECLARATIONids(vd, vd->ids, 0, symbolTable);
    switch (vd->kind) {
        case typeOnlyK:
            // TODO make sure the type exists?
            break;
        case expOnlyK:
            // sym the expressions
            symEXP(vd->val.expVD, symbolTable);
            break;
        case typeAndExpK:
            // TODO make sure the type exists?
            // sym the expressions
            symEXP(vd->val.typeAndExpVD.exp, symbolTable);
            break;
        default:
            break;
    }
    symVARDECLARATION(vd->next, symbolTable);
}

void symTYPEDECLARATION(TYPEDECLARATION* td, SymbolTable* symbolTable) {
    if (td == NULL) return;
    SYMBOL* s;
    // put a symbol for the id of the type
    // TODO check if already defined
    s = putSymbol(td->id->name, typeDeclSym, symbolTable);
    s->val.typeDeclS = td;
    // TODO check that the type already exists?
    symTYPEDECLARATION(td->next, symbolTable);
}

void symFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd, SymbolTable* symbolTable) {
    // put a symbol for the id of the function
    SYMBOL* s;
    // TODO check if already defined?
    s = putSymbol(fd->id->name, functionDeclSym, symbolTable);
    s->val.functionDeclS = fd;
    // TODO return type?
    // create a new scope
    SymbolTable* newScope = scopeSymbolTable(symbolTable);
    // sym the parameters and statements
    symPARAMETER(fd->parameters, newScope);
    symSTATEMENT(fd->statements, newScope);
}

typedef struct PARAMETER {
    int lineno;
    struct ID* ids;
    struct TYPE *type;
    struct PARAMETER *next;
} PARAMETER;

void symPARAMETERids(PARAMETER* p, ID* ids, int parameterNum, SymbolTable* t) {
    if (ids == NULL) return;
    SYMBOL* s;
    if (alreadyDefined(ids->name, symbolTable)) {
        // report error
        reportSymError("parameter already defined", ids->name, vd->lineno);
    } else {
        s = putSymbol(ids->name, varDeclSym, symbolTable);
        SINGLEVAR* sv;
        sv = NEW(SINGLEVAR);
        sv->varDecl = vd;
        sv->varNum = varNum;
        s->val.varDeclS = sv;
        ids->symbol = s;
    }
    // recurse
    symVARDECLARATIONids(vd, ids->next, varNum + 1);
}

void symPARAMETER(PARAMETER* p, SymbolTable* t) {
    // create a symbol for the id
    SYMBOL* s;
    s = putSymbol()
    symPARAMETER(p->next, t);
}

void symSTATEMENT(STATEMENT* s, SymbolTable* t) {
    // TODO
    symSTATEMENT(s->next, t);
}

void symEXP(EXP* e, SymbolTable* t) {
    if (e == NULL) return;
    SYMBOL* s;
    switch (e->kind) {
        case identifierK:
            s = getSymbol(e->val.idE->name, t);
            e->val.idE->symbol = s;
            break;
        case intLiteralK:
            break;
        case floatLiteralK:
            break;
        case runeLiteralK:
            break;
        case stringLiteralK:
            break;
        case plusK:
            symEXP(e->val.plusE.left, t);
            symEXP(e->val.plusE.right, t);
            break;
        case minusK:
            symEXP(e->val.minusE.left, t);
            symEXP(e->val.minusE.right, t);
            break;
        case timesK:
            symEXP(e->val.timesE.left, t);
            symEXP(e->val.timesE.right, t);
            break;
        case divK:
            symEXP(e->val.divE.left, t);
            symEXP(e->val.divE.right, t);
            break;
        case modK:
            symEXP(e->val.modE.left, t);
            symEXP(e->val.modE.right, t);
            break;
        case bitwiseOrK:
            symEXP(e->val.bitwiseOrE.left, t);
            symEXP(e->val.bitwiseOrE.right, t);
            break;
        case bitwiseAndK:
            symEXP(e->val.bitwiseAndE.left, t);
            symEXP(e->val.bitwiseAndE.right, t);
            break;
        case xorK:
            symEXP(e->val.xorE.left, t);
            symEXP(e->val.xorE.right, t);
            break;
        case ltK:
            symEXP(e->val.ltE.left, t);
            symEXP(e->val.ltE.right, t);
            break;
        case gtK:
            symEXP(e->val.gtE.left, t);
            symEXP(e->val.gtE.right, t);
            break;
        case eqK:
            symEXP(e->val.eqE.left, t);
            symEXP(e->val.eqE.right, t);
            break;
        case neqK:
            symEXP(e->val.neqE.left, t);
            symEXP(e->val.neqE.right, t);
            break;
        case leqK:
            symEXP(e->val.leqE.left, t);
            symEXP(e->val.leqE.right, t);
            break;
        case geqK:
            symEXP(e->val.geqE.left, t);
            symEXP(e->val.geqE.right, t);
            break;
        case orK:
            symEXP(e->val.orE.left, t);
            symEXP(e->val.orE.right, t);
            break;
        case andK:
            symEXP(e->val.andE.left, t);
            symEXP(e->val.andE.right, t);
            break;
        case leftShiftK:
            symEXP(e->val.leftShiftE.left, t);
            symEXP(e->val.leftShiftE.right, t);
            break;
        case rightShiftK:
            symEXP(e->val.rightShiftE.left, t);
            symEXP(e->val.rightShiftE.right, t);
            break;
        case bitClearK:
            symEXP(e->val.bitClearE.left, t);
            symEXP(e->val.bitClearE.right, t);
            break;
        case appendK:
            symEXP(e->val.appendE.slice, t);
            symEXP(e->val.appendE.expToAppend, t);
            break;
        case castK:
            symCAST(e->val.castE, t);
            break;
        case selectorK:
            symEXP(e->val.selectorE.rest, t);
            symID(e->val.selectorE.lastSelector, t);
            break;
        case indexK:
            symEXP(e->val.indexE.rest, t);
            symEXP(e->val.indexE.lastIndex, t);
            break;
        case argumentsK:
            symEXP(e->val.argumentsE.rest, t);
            symEXP(e->val.argumentsE.args, t);
            break;
        case uPlusK:
            symEXP(e->val.uPlusE, t);
            break;
        case uMinusK:
            symEXP(e->val.uMinusE, t);
            break;
        case uNotK:
            symEXP(e->val.uNotE, t);
            break;
        case uXorK:
            symEXP(e->val.uXorE, t);
            break;
        case uReceiveK:
            symEXP(e->val.uReceiveE, t);
            break;
        default:
            break;
    }
    symEXP(e->next, t);
}

void symCAST(CAST* c, SymbolTable* t) {
    // TODO check that type of cast exists?
    symEXP(c->exp, t);
}

void symID(ID* id, SymbolTable* t) {
    SYMBOL* s;
    s = getSymbol(id->name, t);
    id->symbol = s;
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
