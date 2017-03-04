/*
 * This file is for all the AST node definitions
 */

#ifndef TREE_H
#define TREE_H

typedef enum { plusEqOp, minusEqOp, timesEqOp, divEqOp,
               modEqOp, andEqOp, orEqOp, xorEqOp, leftShiftEqOp, rightShiftEqOp,
               bitClearEqOp } OperationKind;

typedef enum { decIL, octIL, hexIL } IntLiteralKind;

typedef enum { packageDeclSym, typeDeclSym, varDeclSym, functionDeclSym, parameterSym, fieldSym } SymbolKind;

/*
 * symbol
 */
/*
typedef struct SYMBOL {
    char *name;
    struct TYPE* type;
    // this is a linked list in the SymbolTable hashmap, so we have to have this next field.
    // it doesn't actually have anything to do with the 'current' symbol
    struct SYMBOL *next;
} SYMBOL;
*/

/*
 * The following have symbols:
 *  package declaration
 *  variable declarations
 *  type declarations
 *  function declarations
 *  function parameters
 *  struct fields
 */
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

/*
 * a program consists of a package declaration and top-level declarations
 */
typedef struct PROGRAM {
    struct PACKAGE* package;
    struct TOPLEVELDECLARATION* topLevelDeclaration;
} PROGRAM;

/*
 * a package just has a name
 */
typedef struct PACKAGE {
    char* name;
} PACKAGE;


typedef struct TOPLEVELDECLARATION {
  int lineno;
  enum { vDeclK, tDeclK, functionDeclK } kind;
  union {
      struct VARDECLARATION* varDeclTLD;
      struct TYPEDECLARATION* typeDeclTLD;
      struct FUNCTIONDECLARATION* functionDeclTLD;
  } val;
  struct TOPLEVELDECLARATION *next;
} TOPLEVELDECLARATION;

/*
 * a variable declaration
 * there are three kinds:
 *  - type specified but no expression
 *  - expression specified but no type
 *  - both type and expression given
 */
/*
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
*/

typedef struct VARDECLARATION {
    int lineno;
    enum { typeOnlyK, expOnlyK, typeAndExpK } kind;
    char* name;
    int isDistributed;  // whether this declaration is part of a distrubuted statement
    int isLocal;    // whether this declaration is local (as opposed to global)
    union {
        struct TYPE* typeVD;
        struct EXP* expVD;
        struct {struct TYPE* type;
                struct EXP* exp;} typeAndExpVD;
    } val;
    struct VARDECLARATION* next; // for when multiple variables are declared in one line
} VARDECLARATION;

/*
 * a type declaration
 * type declarations have an identifier and a type
 */
 typedef struct TYPEDECLARATION {
     int lineno;
     struct ID* id;
     int isDistributed; // whether this declaration is part of a distributed statement
     int isLocal;   // whether this declaration is local (as opposed to global)
     struct TYPE* type;
     struct TYPEDECLARATION* next; // for distributed type declarations; else this is null
 } TYPEDECLARATION;

/*
 * a function declaration
 * a function has parameters, statements, and possibly a return type
 */
typedef struct FUNCTIONDECLARATION {
    int lineno;
    struct ID* id;  // the name of the function
    struct PARAMETER *parameters;
    struct TYPE* returnType;
    struct STATEMENT *statements;
    // TODO do we need to record the signature as a string here?
} FUNCTIONDECLARATION;

/*
 * function parameters
 */
typedef struct PARAMETER {
    int lineno;
    struct ID* ids;
    struct TYPE *type;
    struct PARAMETER *next;
} PARAMETER;

/*
 * an identifier
 */
typedef struct ID {
    char* name;
    struct ID* next;
} ID;

/*
 * a cast
 */
typedef struct CAST {
    int lineno;
    struct TYPE* type;
    struct EXP* exp;
} CAST;

/*
 * type
 */
 typedef struct TYPE {
     int lineno;
     enum { idK, structK, sliceK, arrayK } kind;
     union {
         struct ID* idT;
         struct {struct EXP* size;
                 struct TYPE* elementType;} arrayT;
         struct FIELD* structT;  // the fields in the struct
         struct TYPE* sliceT;    // the type of the elements in the slice
         //struct STRUCTT *structT;
     } val;
 } TYPE;

/*
 * field
 */
typedef struct FIELD {
    int lineno;
    struct ID* ids;
    struct TYPE *type;
    struct FIELD *next;
} FIELD;


/*
 * statement
 */
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
        struct {struct EXP* lvalues;    // weed to ensure that all exps are lvalues
                struct EXP* exps;} regAssignS;
        struct {struct EXP* lvalue; // weed to ensure that exp is an lvalue (and that there is just one of them? should already only be one since matches with primaryExp)
                OperationKind opKind;
                struct EXP* exp;} binOpAssignS;
        struct EXP* incS;   // weed to ensure exp is an lvalue
        struct EXP* decS;   // weed to ensure exp is an lvalue
        struct EXP* printS;     // linked-list of expressions
        struct EXP* printlnS;   // linked-list of expressions
        struct VARDECLARATION* varDeclS;
        struct TYPEDECLARATION* typeDeclS;
        struct {struct EXP* ids;    // needs to be weeded (to ensure only ids). also, both need to be weeded for length
                struct EXP* exps;} shortDeclS;
    } val;
    // points to the next statement at the same level as this one
    // nested statements are pointed to by the appropriate statement structs in val
    struct STATEMENT* next;
} STATEMENT;

/*
 * a case of a switch-case statement
 */
typedef struct SWITCHCASE {
    int lineno;
    enum { caseK, defaultK } kind;
    union {
        struct {struct EXP* exps;
                struct STATEMENT* statements;} caseC;
        struct STATEMENT* defaultStatementsC;
    } val;
    struct SWITCHCASE* next; /* the next case */
} SWITCHCASE;

/*
 * an expression
 */
typedef struct EXP {
    int lineno;
    struct TYPE* type;  // the type that this expression evaluates to
    enum { identifierK, intLiteralK, floatLiteralK, runeLiteralK, stringLiteralK,
           plusK, minusK, timesK, divK, modK, bitwiseOrK, bitwiseAndK, xorK, ltK, gtK,
           eqK, neqK, leqK, geqK, orK, andK, leftShiftK, rightShiftK, bitClearK,
           appendK, castK, selectorK, indexK, argumentsK, uPlusK,
           uMinusK, uNotK, uXorK, uReceiveK} kind;
    union {
        struct ID *idE;  // identifier
        struct {int decValue; // value of integer
                IntLiteralKind kind;} intLiteralE;
        float floatLiteralE;    // value of float
        char runeLiteralE;   // value of a rune
        char *stringLiteralE;   // value of string
        struct CAST* castE;
        struct EXP* uPlusE;
        struct EXP* uMinusE;
        struct EXP* uNotE;
        struct EXP* uXorE;
        struct EXP* uReceiveE;
        struct {struct EXP *left;
                struct EXP *right;} plusE;
        struct {struct EXP *left;
                struct EXP *right;} minusE;
        struct {struct EXP *left;
                struct EXP *right;} timesE;
        struct {struct EXP *left;
                struct EXP *right;} divE;
        struct {struct EXP *left;
                struct EXP *right;} modE;
        struct {struct EXP *left;
                struct EXP *right;} bitwiseOrE;
        struct {struct EXP *left;
                struct EXP *right;} bitwiseAndE;
        struct {struct EXP *left;
                struct EXP *right;} xorE;
        struct {struct EXP *left;
                struct EXP *right;} ltE;
        struct {struct EXP *left;
                struct EXP *right;} gtE;
        struct {struct EXP *left;
                struct EXP *right;} eqE;
        struct {struct EXP *left;
                struct EXP *right;} neqE;
        struct {struct EXP *left;
                struct EXP *right;} leqE;
        struct {struct EXP *left;
                struct EXP *right;} geqE;
        struct {struct EXP *left;
                struct EXP *right;} orE;
        struct {struct EXP *left;
                struct EXP *right;} andE;
        struct {struct EXP *left;
                struct EXP *right;} leftShiftE;
        struct {struct EXP *left;
                struct EXP *right;} rightShiftE;
        struct {struct EXP *left;
                struct EXP *right;} bitClearE;
        struct {struct EXP *slice; /* expression that returns the slice to append to */
                struct EXP *expToAppend; /* element to add */} appendE;
        struct {struct EXP* rest;
                struct ID* lastSelector;} selectorE;
        struct {struct EXP* rest;
                struct EXP* lastIndex;} indexE;
        struct {struct EXP* rest;
                struct EXP* args;} argumentsE; // linked list of expressions (using next pointers)
    } val;
    struct EXP* next; // for expression lists; null otherwise
 } EXP;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION INVOKATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
typedef struct FUNCTIONCALL {
    int lineno;
    struct ID* id;
    struct EXP* arguments;
} FUNCTIONCALL;
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// FUNCTION DEFINITIONS //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PROGRAM* makePROGRAM(PACKAGE* package, TOPLEVELDECLARATION* topLevelDeclaration);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PACKAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PACKAGE* makePACKAGE(ID* id);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TOP-LEVEL DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOPLEVELDECLARATION* appendTOPLEVELDECLARATION(TOPLEVELDECLARATION *prevs, TOPLEVELDECLARATION *curr);
TOPLEVELDECLARATION* makeTOPLEVELDECLARATIONvar(VARDECLARATION* varDecl);
TOPLEVELDECLARATION* makeTOPLEVELDECLARATIONtype(TYPEDECLARATION* typeDecl);
TOPLEVELDECLARATION* makeTOPLEVELDECLARATIONfunction(FUNCTIONDECLARATION* functionDecl);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLE DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VARDECLARATION* makeVARDECLARATIONtypeonly(ID* ids, TYPE* type);
VARDECLARATION* makeVARDECLARATIONexponly(ID* ids, EXP* exps);
VARDECLARATION* makeVARDECLARATIONtypeandexp(ID* ids, TYPE* type, EXP* exps);
VARDECLARATION* appendVARDECLARATION(VARDECLARATION *prevs, VARDECLARATION *curr);
VARDECLARATION* markAsDistributedVarDecl(VARDECLARATION* v);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TYPE DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TYPEDECLARATION* makeTYPEDECLARATION(ID* id, TYPE* type);
TYPEDECLARATION* appendTYPEDECLARATION(TYPEDECLARATION* prevs, TYPEDECLARATION* curr);
TYPEDECLARATION* markAsDistributedTypeDecl(TYPEDECLARATION* t);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FUNCTIONDECLARATION* makeFUNCTIONDECLARATION(ID* id, PARAMETER* parameters, TYPE* returnType, STATEMENT* statements);
PARAMETER* makePARAMETER(ID* ids, TYPE* type);
PARAMETER* appendPARAMETER(PARAMETER* prevs, PARAMETER* curr);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATEMENTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

STATEMENT* appendSTATEMENT(STATEMENT* prevs, STATEMENT* curr);
STATEMENT* makeSTATEMENTempty();
STATEMENT* makeSTATEMENTexp(EXP* exp);
STATEMENT* makeSTATEMENTinc(EXP* lval);
STATEMENT* makeSTATEMENTdec(EXP* lval);
STATEMENT* makeSTATEMENTprintln(EXP* exp);
STATEMENT* makeSTATEMENTprint(EXP* exp);
STATEMENT* makeSTATEMENTvardecl(VARDECLARATION* varDecl);
STATEMENT* makeSTATEMENTtypedecl(TYPEDECLARATION* typeDecl);
STATEMENT* makeSTATEMENTshortdecl(EXP* ids, EXP* exps);
STATEMENT* makeSTATEMENTreturn(EXP* exp);
STATEMENT* makeSTATEMENTbreak();
STATEMENT* makeSTATEMENTcontinue();
STATEMENT* makeSTATEMENTassign(EXP* lvalues, EXP* exps);
STATEMENT* makeSTATEMENTbinopassign(EXP* lvalue, OperationKind opKind, EXP* exp);
STATEMENT* makeSTATEMENTif(STATEMENT* initStatement, EXP* condition, STATEMENT* body);
STATEMENT* makeSTATEMENTifelse(STATEMENT* initStatement, EXP* condition, STATEMENT* thenPart, STATEMENT* elsePart);
STATEMENT* makeSTATEMENTwhile(EXP* condition, STATEMENT* body);
STATEMENT* makeSTATEMENTinfiniteloop(STATEMENT* doThisIndefinitely);
STATEMENT* makeSTATEMENTfor(STATEMENT* initStatement, EXP* condition, STATEMENT* postStatement, STATEMENT* body);
STATEMENT* makeSTATEMENTswitch(STATEMENT* initStatement, EXP* condition, SWITCHCASE* cases);
SWITCHCASE* makeSWITCHCASEcase(EXP* exps, STATEMENT* statements);
SWITCHCASE* makeSWITCHCASEdefault(STATEMENT* statements);
SWITCHCASE* appendSWITCHCASE(SWITCHCASE* prevs, SWITCHCASE* curr);
//FUNCTIONCALL* makeFUNCTIONCALL(ID* id, EXP* arguments);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STRUCTS AND FIELDS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//STRUCTT* makeSTRUCTT(ID* id, FIELD* fields);
FIELD* makeFIELD(ID* ids, TYPE* type);
FIELD* appendFIELD(FIELD* prevs, FIELD* curr);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPRESSIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXP* makeEXPappend(EXP* slice, EXP* expToAppend);
EXP* makeEXPplus(EXP* left, EXP* right);
EXP* makeEXPminus(EXP* left, EXP* right);
EXP* makeEXPtimes(EXP* left, EXP* right);
EXP* makeEXPdiv(EXP* left, EXP* right);
EXP* makeEXPmod(EXP* left, EXP* right);
EXP* makeEXPbitwiseor(EXP* left, EXP* right);
EXP* makeEXPbitwiseand(EXP* left, EXP* right);
EXP* makeEXPxor(EXP* left, EXP* right);
EXP* makeEXPlt(EXP* left, EXP* right);
EXP* makeEXPgt(EXP* left, EXP* right);
EXP* makeEXPeq(EXP* left, EXP* right);
EXP* makeEXPneq(EXP* left, EXP* right);
EXP* makeEXPleq(EXP* left, EXP* right);
EXP* makeEXPgeq(EXP* left, EXP* right);
EXP* makeEXPor(EXP* left, EXP* right);
EXP* makeEXPand(EXP* left, EXP* right);
EXP* makeEXPneq(EXP* left, EXP* right);
EXP* makeEXPleftshift(EXP* left, EXP* right);
EXP* makeEXPrightshift(EXP* left, EXP* right);
EXP* makeEXPbitclear(EXP* left, EXP* right);
EXP* makeEXPuplus(EXP* exp);
EXP* makeEXPuminus(EXP* exp);
EXP* makeEXPunot(EXP* exp);
EXP* makeEXPuxor(EXP* exp);
EXP* makeEXPureceive(EXP* exp);
EXP* appendEXP(EXP* prevs, EXP* curr);
EXP* makeEXPintdecliteral(int decValue);
EXP* makeEXPintoctliteral(int decValue);
EXP* makeEXPinthexliteral(int decValue);
EXP* makeEXPfloatliteral(float floatLiteral);
EXP* makeEXPruneliteral(char runeLiteral);
EXP* makeEXPstringliteral(char* stringLiteral);
EXP* makeEXPid(ID* id);
EXP* makeEXPselector(EXP* rest, ID* lastSelector);
EXP* makeEXPindex(EXP* rest, EXP* lastIndex);
EXP* makeEXParguments(EXP* rest, EXP* args);
CAST* makeCAST(TYPE* type, EXP* exp);
EXP* makeEXPcast(CAST* c);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IDS AND TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ID* makeID(char* name);
ID* appendID(ID *prevs, ID *curr);
TYPE *makeTYPEid(ID* id);
TYPE *makeTYPEarray(EXP* size, TYPE* elementType);
TYPE *makeTYPEstruct(FIELD* fields);
TYPE *makeTYPEslice(TYPE* elementType);

#endif /* !TREE_H */
