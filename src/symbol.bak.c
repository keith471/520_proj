
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

// VARIABLE DECLARATIONS

/*
 * add a symbol to the symbol table for every unique variable
 */
void symVARDECLARATION(VARDECLARATION* vd, SymbolTable* symbolTable) {
    if (vd == NULL) return;
    symVARDECLARATIONlist(vd, symbolTable);
    symVARDECLARATION(vd->nextDistributed, symbolTable);
}

void symVARDECLARATIONlist(VARDECLARATION* vd, SymbolTable* symbolTable) {
    if (vd == NULL) return;
    // check that a variable with the same name has not already been defined
    if (alreadyDefined(vd->id->name, symbolTable)) {
        // report error
        reportSymError("invalid redeclaration", vd->id->name, vd->lineno);
    } else {
        // create a symbol for the id
        SYMBOL* s;
        s = putSymbol(vd->id->name, varDeclSym, symbolTable);
        // set the symbol's value to this variable declaration
        s->val.varDeclS = vd;
        // set this variable declaration's id's symbol to the symbol we just made
        vd->id->symbol = s;
    }
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
    symVARDECLARATIONlist(vd->next, symbolTable);
}

// TYPE DECLARATIONS

void symTYPEDECLARATION(TYPEDECLARATION* td, SymbolTable* symbolTable) {
    if (td == NULL) return;
    // put a symbol for the id of the type
    if (alreadyDefined(td->id->name, symbolTable)) {
        // report error
        reportSymError("invalid redeclaration", td->id->name, td->lineno);
    } else {
        SYMBOL* s;
        s = putSymbol(td->id->name, typeDeclSym, symbolTable);
        s->val.typeDeclS = td;
        td->id->symbol = s;
    }
    // TODO check that the type already exists?
    symTYPEDECLARATION(td->next, symbolTable);
}

// FUNCTION DECLARATIONS

void symFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd, SymbolTable* symbolTable) {
    // put a symbol for the id of the function
    if (alreadyDefined(fd->id->name, symbolTable)) {
        // report error
        reportSymError("invalid redeclaration", fd->id->name, fd->lineno);
    } else {
        SYMBOL* s;
        s = putSymbol(fd->id->name, functionDeclSym, symbolTable);
        s->val.functionDeclS = fd;
        fd->id->symbol = s;
    }
    // TODO return type?
    // create a new scope
    SymbolTable* newScope = scopeSymbolTable(symbolTable);
    // sym the parameters and statements within the new scope
    symPARAMETER(fd->parameters, newScope);
    symSTATEMENT(fd->statements, newScope);
}

// PARAMETERS

void symPARAMETER(PARAMETER* p, SymbolTable* symbolTable) {
    if (p == NULL) return;
    symPARAMETERlist(p, symbolTable);
    symPARAMETER(p->nextParamSet, symbolTable);
}

void symPARAMETERlist(PARAMETER* p, SymbolTable* t) {
    if (p == NULL) return;
    // check that a parameter with the same name has not already been defined
    if (alreadyDefined(p->id->name, t)) {
        // report error
        reportSymError("a parameter with this name already exists", p->id->name, p->lineno);
    } else {
        // create a symbol for the id
        SYMBOL* s;
        s = putSymbol(p->id->name, parameterSym, t);
        // set the symbol's value to this parameter
        s->val.parameterS = p;
        // set this parameter's id's symbol to the symbol we just made
        p->id->symbol = s;
    }
    // TODO check type exists?
    symPARAMETERlist(p->nextId, t);
}

// STATEMENTS

typedef struct STATEMENT {
    int lineno;
    enum { emptyK, expK, incK, decK, regAssignK, binOpAssignK, shortDeclK, varDeclK,
           typeDeclK, printK, printlnK, returnK, ifK, ifElseK, switchK, whileK,
           infiniteLoopK, forK, breakK, continueK } kind;
    union{
        // break, continue, and empty statements have no associated val
        struct EXP* expS;
        struct EXP *returnS; // return expression
        struct {struct STATEMENT* initStatement;
                struct EXP *condition;
                struct STATEMENT *body;} ifS;
        struct {struct STATEMENT* initStatement;
                struct EXP *condition;
                struct STATEMENT *thenPart;
                struct STATEMENT *elsePart; /* could be another if --> else if statement! */} ifElseS;
        struct {struct EXP *condition;
                struct STATEMENT *body;} whileS;
        struct STATEMENT* infiniteLoopS; // the body of an infinite loop
        struct {struct STATEMENT* initStatement;
                struct EXP* condition;
                struct STATEMENT* postStatement;
                struct STATEMENT* body;} forS;
        struct {struct STATEMENT* initStatement;
                struct EXP* condition;
                struct SWITCHCASE* cases;} switchS;
        struct {struct EXP* lvalue;    // weed to ensure that all exps are lvalues
                struct EXP* exp;
                struct STATEMENT* next;} regAssignS;
        struct {struct EXP* lvalue; // weed to ensure that exp is an lvalue (and that there is just one of them? should already only be one since matches with primaryExp)
                OperationKind opKind;
                struct EXP* exp;} binOpAssignS;
        struct EXP* incS;   // weed to ensure exp is an lvalue
        struct EXP* decS;   // weed to ensure exp is an lvalue
        struct EXP* printS;     // linked-list of expressions
        struct EXP* printlnS;   // linked-list of expressions
        struct VARDECLARATION* varDeclS;
        struct TYPEDECLARATION* typeDeclS;
        struct {struct EXP* id;    // needs to be weeded (to ensure only ids). also, both need to be weeded for length
                struct EXP* exp;
                struct STATEMENT* next;} shortDeclS;
    } val;
    // points to the next statement at the same level as this one
    // nested statements are pointed to by the appropriate statement structs in val
    struct STATEMENT* next;
} STATEMENT;

void symSTATEMENT(STATEMENT* s, SymbolTable* t) {
    if (s == NULL) return;
    switch (s->kind) {
        case emptyK:
            break;
        case expK:
            symEXP(s->val.expS, t);
            break;
        case incK:
            symEXP(s->val.incS, t);
            break;
        case decK:
            symEXP(s->val.decS, t);
            break;
        case regAssignK:
            // sym lvalue
            symEXP(s->val.regAssignS.lvalue, t);
            // sym exp
            symEXP(s->val.regAssignS.exp, t);
            // recurse
            symSTATEMENT(s->val.regAssignS.next, t);
            break;
        case binOpAssignK:
            // TODO here
            break;
        case shortDeclK:
            break;
        case varDeclK:
            break;
        case typeDeclK:
            break;
        case printK:
            break;
        case printlnK:
            break;
        case returnK:
            break;
        case ifK:
            break;
        case ifElseK:
            break;
        case switchK:
            break;
        case whileK:
            break;
        case infiniteLoopK:
            break;
        case forK:
            break;
        case breakK:
            break;
        case continueK:
            break;
        default:
            break;
    }
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
        case rawStringLiteralK:
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
