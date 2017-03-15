
#include "tree.h"

/*
 * returned by the function that tests whether a function call is actually a cast
 */
typedef struct CASTCHECKRETURN {
    enum { functionCallKind, castKind } kind;
    union {
        struct TYPE* castType;  // the type being casted to
        struct FUNCTIONDECLARATION* functionDecl; // the declaration of the function being called
    } val;
} CASTCHECKRETURN;

void initTypes();
void typePROGRAM(PROGRAM* p);
void typeTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld);
void typeVARDECLARATION(VARDECLARATION* v);
void typeVARDECLARATIONlist(VARDECLARATION* v);
void typeFUNCTIONDECLARATION(FUNCTIONDECLARATION* f);
void typeSTATEMENT(STATEMENT* s);
void typeSTATEMENTswitch(STATEMENT* s);
void typeSWITCHCASE(SWITCHCASE* sc, TYPE* t);
void typeEXPs(EXP* e);
void typeEXP(EXP* e);
CASTCHECKRETURN* checkCast(EXP* e);
void prepTypeEXPcast(EXP* e, TYPE* t);
void typeEXPfunctioncall(EXP* e, FUNCTIONDECLARATION* fd);
void matchArgsToParams(EXP* args, PARAMETER* currParam, PARAMETER* params, char* name, int lineno);

int countArgs(EXP* e);
TYPE* typePlus(TYPE* left, TYPE* right, int lineno);
TYPE* numericOp(TYPE* left, TYPE* right, int lineno);
TYPE* intOp(TYPE* left, TYPE* right, int lineno);
TYPE* getSymbolType(SYMBOL *s);
TYPE* getElementType(TYPE* t, int lineno);
void checkOrderedAndEqual(TYPE* left, TYPE* right, int lineno);
void checkAppendIsValid(TYPE* s, TYPE* t, int lineno);
TYPE* getSliceElementType(TYPE* t, lineno);
int assertEqualTYPEs(TYPE *expected, TYPE *actual, int lineno);
int assertOrdered(TYPE* t, int lineno);
int assertNumeric(TYPE* t, int lineno);
int assertNumericOrString(TYPE* t, int lineno);
int assertCanAssign(SYMBOL* prevSym, int lineno);
void assertCaseEXPsHaveType(EXP* exps, TYPE* t);
void assertResolvesToBaseType(TYPE* t, int lineno);
void assertResolvesToBool(TYPE* t, int lineno);
int assertResolvesToInt(TYPE* t, int lineno);
void assertCastResolution(EXP* e, int lineno);
void assertValidOpUsage(OperationKind opKind, TYPE* left, TYPE* right);
