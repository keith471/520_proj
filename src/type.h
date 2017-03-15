
#include "tree.h"

/*
 * returned by the function that tests whether a function call is actually a cast
 */
typedef struct CASTCHECKRETURN {
    enum { functionCallKind, castKind } kind;
    union {
        struct {struct TYPE* type;  // the type being casted to
                char* name; /* the name of the type being casted to */} cast;
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
void prepTypeEXPcast(EXP* e, TYPE* t, char* name);
void typeEXPfunctioncall(EXP* e, FUNCTIONDECLARATION* fd);
void matchArgsToParams(EXP* args, PARAMETER* currParam, PARAMETER* params, char* name, int lineno);

int countArgs(EXP* e);
TYPE* typePlus(TYPE* left, TYPE* right, int lineno);
TYPE* numericOp(TYPE* left, TYPE* right, int lineno);
TYPE* boolOp(TYPE* left, TYPE* right, int lineno);
TYPE* intOp(TYPE* left, TYPE* right, int lineno);
TYPE* getSymbolType(SYMBOL *s);
TYPE* getElementType(TYPE* t, int lineno);
void checkOrderedAndEqual(TYPE* left, TYPE* right, int lineno);
void checkAppendIsValid(TYPE* s, TYPE* t, int lineno);
TYPE* getSliceElementType(TYPE* t, int lineno);
void assertEqualTYPEs(TYPE *expected, TYPE *actual, int lineno);
void assertOrdered(TYPE* t, int lineno);
void assertNumeric(TYPE* t, int lineno);
void assertNumericOrString(TYPE* t, int lineno);
void assertCanAssign(SYMBOL* prevSym, int lineno);
void assertCaseEXPsHaveType(EXP* exps, TYPE* t);
void assertEXPsResolveToBaseType(EXP* e, int lineno);
void assertResolvesToBaseType(TYPE* t, int lineno);
void assertResolvesToBool(TYPE* t, int lineno);
void assertResolvesToInt(TYPE* t, int lineno);
void assertCastResolution(TYPE* t, int lineno);
void assertValidOpUsage(OperationKind opKind, TYPE* left, TYPE* right);
