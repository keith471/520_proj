
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
void typeSWITCHCASE(STATEMENT* s, SWITCHCASE* sc, TYPE* t);
void typeEXPs(EXP* e, STATEMENT* s, VARDECLARATION* v);
void typeEXP(EXP* e, STATEMENT* s, VARDECLARATION* v);
ARRAYINDEX* appendARRAYINDEX(ARRAYINDEX* prevs, ARRAYINDEX* curr);
SLICEINDEX* appendSLICEINDEX(SLICEINDEX* prevs, SLICEINDEX* curr);
void typeRECEIVER(RECEIVER* r, STATEMENT* s, VARDECLARATION* v);
CASTCHECKRETURN* checkCast(EXP* e);
void prepTypeEXPcast(EXP* e, TYPE* t, char* name, STATEMENT* s, VARDECLARATION* v);
void typeEXPfunctioncall(EXP* e, FUNCTIONDECLARATION* fd, STATEMENT* s, VARDECLARATION* v);
void matchArgsToParams(EXP* args, PARAMETER* currParam, PARAMETER* params, char* name, int lineno);

int countArgs(EXP* e);
TYPE* typePlus(TYPE* left, TYPE* right, int lineno);
TYPE* numericOp(TYPE* left, TYPE* right, int lineno);
TYPE* boolOp(TYPE* left, TYPE* right, int lineno);
TYPE* intOp(TYPE* left, TYPE* right, int lineno);
TYPE* getSymbolType(char* name, SYMBOL *s, int lineno);
TYPE* getElementType(TYPE* t, int lineno);
void checkOrderedAndEqual(TYPE* left, TYPE* right, int lineno);
void checkNoUnderlyingSlice(TYPE* t, int lineno);
void checkNoUnderlyingSliceInFields(FIELD* fields, int lineno);
void checkNoUnderlyingSliceInFieldList(FIELD* field, int lineno);
void checkAppendIsValid(TYPE* s, TYPE* t, int lineno);
TYPE* getSliceElementType(TYPE* t, int lineno);
void assertOrdered(TYPE* t, int lineno);
void assertNumeric(TYPE* t, int lineno);
void assertNumericOrString(TYPE* t, int lineno);
void assertCanAssign(SYMBOL* prevSym, int lineno);
void assertCaseEXPsHaveType(EXP* exps, TYPE* t);
void assertEXPsResolveToBaseType(EXP* e, int lineno);
void assertResolvesToBaseType(TYPE* t, int lineno);
void assertResolvesToBool(TYPE* t, int lineno);
void assertResolvesToInt(TYPE* t, int lineno);
STRUCTTYPE* assertResolvesToStruct(TYPE* t, int lineno);
void assertCastResolution(TYPE* t, int lineno);
void assertValidOpUsage(OperationKind opKind, TYPE* left, TYPE* right, int lineno);
void assertIdenticalTYPEs(TYPE *expected, TYPE *actual, int lineno);
void assertActualTypeInt(TYPE* actual, int lineno);
void assertActualTypeFloat64(TYPE* actual, int lineno);
void assertActualTypeRune(TYPE* actual, int lineno);
void assertActualTypeBool(TYPE* actual, int lineno);
void assertActualTypeString(TYPE* actual, int lineno);
int resolvesToString(TYPE* t);
void assertActualTypeStruct(STRUCTTYPE* expected, TYPE* actual, int lineno);
void assertIdenticalStructs(STRUCTTYPE* expected, STRUCTTYPE* actual, int lineno);
void assertIdenticalFIELDs(FIELD* currExpected, FIELD* expected, FIELD* currActual, FIELD* actual, int lineno);
void assertActualTypeArray(TYPE* expected, TYPE* actual, int lineno);
void assertIdenticalArrays(EXP* expectedSize, TYPE* expectedType, EXP* actualSize, TYPE* actualType, int lineno);
void assertActualTypeSlice(TYPE* expectedElementType, TYPE* actual, int lineno);
void assertActualTypeIdentifier(TYPEDECLARATION* expectedDecl, TYPE* actual, int lineno);
SYMBOL* getSymbolInSymbolTable(SymbolTable* t, char *name, int lineno);
void assertReturnOnEveryPath(STATEMENT* s, TYPE* returnType, char* name, int lineno);
int hasReturnOnEveryPath(STATEMENT* s, TYPE* returnType, char* name);
int hasOuterReturn(STATEMENT* s, TYPE* returnType, char* name);
int hasInnerReturn(STATEMENT* s, TYPE* returnType, char* name);
int switchHasReturn(SWITCHCASE* sc, TYPE* returnType, char* name);
void assertNoInvalidReturns(STATEMENT* s, TYPE* expected, char* name);
void handleNonVoidReturnError(TYPE* actual, char* funcName, int lineno);
void handleVoidReturnError(TYPE* expected, char* funcName, int lineno);
void assertNoInvalidReturnsSWITCHCASE(SWITCHCASE* sc, TYPE* expected, char* name);
