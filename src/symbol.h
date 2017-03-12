
#include "tree.h"

#define HashSize 317

typedef struct SymbolTable {
    SYMBOL *table[HashSize];
    struct SymbolTable *next;  // a pointer to the immediate outer scope of this scope (the current symbol table)
} SymbolTable;

typedef struct PutSymbolWrapper {
    SYMBOL* symbol;
    int isRedecl; // whether this symbol is a redeclaration
    SYMBOL* prevDeclSym; // the symbol corresponding to the previous declaration
} PutSymbolWrapper;

typedef enum { intD, float64D, runeD, boolD, stringD, trueD, falseD } DefaultSymbol;

SymbolTable *symbolTable;

void dumpFrame(SymbolTable* scope, int lineno);
void printSymbolTable(SYMBOL* table[]);
void printSymbol(SYMBOL* s);
char* getTypeAsString(TYPE* t);

int Hash(char *str);
SymbolTable* createUniverseBlock();
void addDefault(DefaultSymbol defSym, SymbolKind kind, SymbolTable* symbolTable);
SymbolTable* initSymbolTable();
SymbolTable* scopeSymbolTable(SymbolTable *s);
void scopeExit(SymbolTable* scope, int lineno);
PutSymbolWrapper* putSymbol(SymbolTable *t, char *name, SymbolKind kind, int lineno, int isShortVarDecl);
SYMBOL *getSymbol(SymbolTable *t, char *name, int lineno);

void symPROGRAM(PROGRAM* p, char* filePath);
void symTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld, SymbolTable* t);
void symVARDECLARATION(VARDECLARATION* v, SymbolTable* t);
void symVARDECLARATIONlist(VARDECLARATION* v, SymbolTable* t, int checkedType);
void symTYPEDECLARATION(TYPEDECLARATION* td, SymbolTable* t);
void symFUNCTIONDECLARATION(FUNCTIONDECLARATION* f, SymbolTable* t);
void symPARAMETER(PARAMETER* p, SymbolTable* t);
void symPARAMETERlist(PARAMETER* p, SymbolTable* t, int checkedType);
void symSTATEMENT(STATEMENT* s, SymbolTable* symbolTable);
void symSWITCHCASE(SWITCHCASE* sc, SymbolTable* switchScope);
void symSTATEMENTshortvardecl(STATEMENT* stmt, SymbolTable* symbolTable);
void symFIELD(FIELD* f, SymbolTable* t);
void symFIELDlist(FIELD* f, SymbolTable* t, int checkedType);
void symEXPs(EXP* e, SymbolTable* t);
void symEXP(EXP* e, SymbolTable* t);
void verifyType(TYPE* type, SymbolTable* t);
int notBlank(char* name);
