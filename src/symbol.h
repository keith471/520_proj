
#include "tree.h"

typedef enum { intD, float64D, runeD, boolD, stringD, trueD, falseD } DefaultSymbol;

// holds the names of all the identifiers in the program - these will be offlimits to us
// during code generation (i.e. we can't generate any of our own variables with these names)
ID* nameTable[HashSize];

typedef struct PutSymbolWrapper {
    SYMBOL* symbol;
    int isRedecl; // whether this symbol is a redeclaration
    SYMBOL* prevDeclSym; // the symbol corresponding to the previous declaration
} PutSymbolWrapper;

SymbolTable *symbolTable;

void scopeExit(SymbolTable* t);
void dumpFrame(SymbolTable* scope);
void printSymbolTable(SYMBOL* table[]);
void printSymbol(SYMBOL* s);
char* getTypeAsString(TYPE* t);

int Hash(char *str);
SymbolTable* createUniverseBlock();
void addDefault(DefaultSymbol defSym, SymbolKind kind, SymbolTable* symbolTable);
SymbolTable* initSymbolTable(int startLineno);
SymbolTable* scopeSymbolTable(SymbolTable *s, int startLineno);
PutSymbolWrapper* putSymbol(SymbolTable *t, char *name, SymbolKind kind, int lineno, int isShortVarDecl);
SYMBOL *getSymbol(SymbolTable *t, char *name, int lineno);

void initNameTable();
void putName(char* name);

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
void symRECEIVER(RECEIVER* r, SymbolTable* t);
void verifyType(TYPE* type, SymbolTable* t);
void symSTRUCTTYPE(STRUCTTYPE* s, SymbolTable *t);
int notBlank(char* name);
