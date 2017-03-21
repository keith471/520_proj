/*
 * This file is for all the AST node definitions
 */

#ifndef TREE_H
#define TREE_H

#define HashSize 317

typedef enum { plusEqOp, minusEqOp, timesEqOp, divEqOp,
               modEqOp, andEqOp, orEqOp, xorEqOp, leftShiftEqOp, rightShiftEqOp,
               bitClearEqOp } OperationKind;

typedef enum { decIL, octIL, hexIL } IntLiteralKind;

/*
 * typeSym and varSym are for system predefined types and variables, e.g. bool and true
 */
typedef enum { typeSym, typeDeclSym, varSym, varDeclSym, shortDeclSym, functionDeclSym, parameterSym, fieldSym } SymbolKind;

typedef struct SymbolTable {
    int isUniverseBlock; // whether this is the table for the universe block
    int startLineno; // the lineno at which the scope of this symbol table begins
    int endLineno;  // the lineno at which the scope of this symbol table ends
    struct SYMBOL *table[HashSize];
    struct SymbolTable *next;  // a pointer to the immediate outer scope of this scope (the current symbol table)
} SymbolTable;

/*
 * The following have symbols:
 *  variable declarations
 *  type declarations
 *  function declarations
 *  function parameters
 *  struct fields
 */
typedef struct SYMBOL {
    int lineno;
    char* name;
    SymbolKind kind;
    union {
        struct TYPE* typeS;
        struct TYPE* varS;
        struct FUNCTIONDECLARATION *functionDeclS;
        struct {struct STATEMENT* statement;
                struct TYPE* type; /* will be NULL until type checking */} shortDeclS;
        struct {struct TYPEDECLARATION * typeDecl;
                struct TYPE* type;} typeDeclS;
        struct {struct VARDECLARATION* varDecl;
                struct TYPE* type; /* could be NULL */} varDeclS;
        struct {struct PARAMETER * param;
                struct TYPE* type;} parameterS;
        struct {struct FIELD* field;
                struct TYPE* type;} fieldS;
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
    int lineno;
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
typedef struct VARDECLARATION {
    int lineno;
    enum { typeOnlyK, expOnlyK, typeAndExpK } kind;
    struct ID* id;
    int isEmpty; // whether this is an empty variable declaration
    int isDistributed;  // whether this declaration is part of a distrubuted statement
    int isLocal;    // whether this declaration is local (as opposed to global)
    union {
        struct TYPE* typeVD; // cannot be NULL
        struct {struct EXP* exp; /* in later phases, can access the type through exp or the symbol */
                struct SYMBOL* symbol; /* set in symbol phase */} expVD;
        struct {struct TYPE* type; // cannot be NULL
                struct EXP* exp;} typeAndExpVD;
    } val;
    struct VARDECLARATION* next; // for when multiple variables are declared in one line
    struct VARDECLARATION* nextDistributed; // for distributed variable declarations
} VARDECLARATION;

/*
 * a type declaration
 * type declarations have an identifier and a type
 */
typedef struct TYPEDECLARATION {
    int lineno;
    int number; // a number unique to this type declaration
    struct ID* id;
    int isEmpty;   // whether this is an empty type declaration, i.e. type ()
    int isDistributed; // whether this declaration is part of a distributed statement
    int isLocal;   // whether this declaration is local (as opposed to global)
    struct TYPE* type; // cannot be NULL
    struct TYPEDECLARATION* nextDistributed; // for distributed type declarations; else this is null
} TYPEDECLARATION;

/*
 * a function declaration
 * a function has parameters, statements, and possibly a return type
 */
typedef struct FUNCTIONDECLARATION {
    int lineno;
    struct ID* id;  // the name of the function
    struct PARAMETER *parameters;
    struct TYPE* returnType; // can be NULL
    struct STATEMENT *statements;
} FUNCTIONDECLARATION;

/*
 * function parameters
 */
typedef struct PARAMETER {
    int lineno;
    struct ID* id;
    struct TYPE *type; // cannot be NULL
    struct PARAMETER* nextId;   // for the next parameter in the list, e.g. a, b, c int
    struct PARAMETER* nextParamSet; // for the next set of parameters, e.g. a, b, c int, d, e float64
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
    struct TYPE* type; // the type we're casting to
    struct EXP* exp;
} CAST;

/*
 * type
 */
typedef struct TYPE {
    int lineno;
    enum { idK, structK, sliceK, arrayK, intK, float64K, runeK, boolK, stringK } kind;
    union {
        struct {struct ID* id;
                struct TYPEDECLARATION* typeDecl; // a reference to this type's type declaration, set in symbol phase
                struct TYPE* underlyingType; /* set in symbol phase */} idT;
        struct {struct EXP* size; // this will have to be an int literal expression (guaranteed by weeding)
                struct TYPE* elementType;} arrayT;
        //struct FIELD* structT;  // the fields in the struct
        struct STRUCTTYPE* structT;
        struct TYPE* sliceT;    // the type of the elements in the slice
    } val;
} TYPE;
/*
typedef struct TYPE {
    int lineno;
    enum { idK, structK, sliceK, arrayK, intK, float64K, runeK, boolK, stringK } kind;
    union {
        struct ID* idT;
        struct {struct EXP* size;
                struct TYPE* elementType;} arrayT;
        struct FIELD* structT;  // the fields in the struct
        struct TYPE* sliceT;    // the type of the elements in the slice
        //struct STRUCTT *structT;
    } val;
} TYPE;
*/

typedef struct STRUCTTYPE {
    int lineno;
    struct FIELD* fields;
    SymbolTable* symbolTable; // set in the symbol phase
} STRUCTTYPE;

/*
 * field
 */
typedef struct FIELD {
    int lineno;
    struct ID* id;
    struct TYPE *type;
    struct FIELD* nextId;
    struct FIELD* nextFieldSet;
} FIELD;

/*
 * statement
 */
typedef struct STATEMENT {
    int lineno;
    enum { emptyK, expK, incK, decK, regAssignK, binOpAssignK, shortDeclK, varDeclK,
           typeDeclK, printK, printlnK, returnK, ifK, ifElseK, switchK, whileK,
           infiniteLoopK, forK, breakK, continueK, blockK } kind;
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
                struct SYMBOL* symbol; // the symbol for the id (not null if this is not a redecl)
                struct EXP* exp;
                int isRedecl; // whether this variable was redeclared in the short decl statement
                struct SYMBOL* prevDeclSym; // the symbol for the variable's previous declaration
                                        // (NOTE: we don't actually know that the SYMBOL has type varDeclSym!
                                        // this should be the first thing that the type checker checks)
                                        // NOTE 2: if it IS a varDeclSym but the sym has no type (NULL type) since the
                                        // previous var decl was expOnly, then we can let the exp of this short decl
                                        // dicate the type!
                struct STATEMENT* next;} shortDeclS;
        struct STATEMENT* blockS;   // all the statements in the block
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
    enum { identifierK, intLiteralK, floatLiteralK, runeLiteralK, stringLiteralK, rawStringLiteralK,
           plusK, minusK, timesK, divK, modK, bitwiseOrK, bitwiseAndK, xorK, ltK, gtK,
           eqK, neqK, leqK, geqK, orK, andK, leftShiftK, rightShiftK, bitClearK,
           appendK, castK, selectorK, indexK, argumentsK, uPlusK,
           uMinusK, uNotK, uXorK } kind;
    int isParenthesized;    // a flag that we check during weeding of short declaration statements
    union {
        struct {struct ID* id;
                struct SYMBOL* symbol; /* symbol of this identifier, set in the symbol phase */ } idE;  // identifier
        struct {int decValue; // value of integer
                IntLiteralKind kind;} intLiteralE;
        float floatLiteralE;    // value of float
        char runeLiteralE;   // value of a rune
        char *stringLiteralE;   // value of string
        char *rawStringLiteralE;
        struct CAST* castE;
        struct EXP* uPlusE;
        struct EXP* uMinusE;
        struct EXP* uNotE;
        struct EXP* uXorE;
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
        struct {struct RECEIVER* receiver;
                struct ID* lastSelector;} selectorE;
        struct {struct EXP* rest;
                struct EXP* lastIndex;} indexE;
        struct {struct EXP* rest;
                struct EXP* args;} argumentsE; // linked list of expressions (using next pointers)
    } val;
    struct EXP* next; // for expression lists; null otherwise
 } EXP;

 typedef struct RECEIVER {
     int lineno;
     struct EXP* receivingStruct; // this had better evaluate to a struct
 } RECEIVER;

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
VARDECLARATION* makeVARDECLARATIONtypeonlyhelper(ID* id, TYPE* type, VARDECLARATION* next);
VARDECLARATION* makeVARDECLARATIONexponly(ID* ids, EXP* exps);
VARDECLARATION* makeVARDECLARATIONexponlyhelper(ID* id, EXP* exp, VARDECLARATION* next);
VARDECLARATION* makeVARDECLARATIONtypeandexp(ID* ids, TYPE* type, EXP* exps);
VARDECLARATION* makeVARDECLARATIONtypeandexphelper(ID* id, TYPE* type, EXP* exp, VARDECLARATION* next);
VARDECLARATION* appendVARDECLARATION(VARDECLARATION *prevs, VARDECLARATION *curr);
VARDECLARATION* markAsDistributedVarDecl(VARDECLARATION* v);
VARDECLARATION* makeVARDECLARATIONempty();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TYPE DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TYPEDECLARATION* makeTYPEDECLARATION(ID* id, TYPE* type);
TYPEDECLARATION* appendTYPEDECLARATION(TYPEDECLARATION* prevs, TYPEDECLARATION* curr);
TYPEDECLARATION* markAsDistributedTypeDecl(TYPEDECLARATION* t);
TYPEDECLARATION* makeTYPEDECLARATIONempty();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FUNCTIONDECLARATION* makeFUNCTIONDECLARATION(ID* id, PARAMETER* parameters, TYPE* returnType, STATEMENT* statements);
PARAMETER* makePARAMETER(ID* ids, TYPE* type);
PARAMETER* makePARAMETERhelper(ID* id, TYPE* type, PARAMETER* nextId);
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
STATEMENT* makeSTATEMENTshortdeclhelper(EXP* id, EXP* exp, STATEMENT* next);
STATEMENT* makeSTATEMENTreturn(EXP* exp);
STATEMENT* makeSTATEMENTbreak();
STATEMENT* makeSTATEMENTcontinue();
STATEMENT* makeSTATEMENTassign(EXP* lvalues, EXP* exps);
STATEMENT* makeSTATEMENTassignhelper(EXP* lvalue, EXP* exp, STATEMENT* next);
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
STATEMENT* makeSTATEMENTblock(STATEMENT* stmts);
//FUNCTIONCALL* makeFUNCTIONCALL(ID* id, EXP* arguments);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STRUCTS AND FIELDS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//STRUCTT* makeSTRUCTT(ID* id, FIELD* fields);
FIELD* makeFIELD(ID* ids, TYPE* type);
FIELD* makeFIELDhelper(ID* id, TYPE* type, FIELD* nextId);
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
EXP* appendEXP(EXP* prevs, EXP* curr);
EXP* makeEXPintdecliteral(int decValue);
EXP* makeEXPintoctliteral(int decValue);
EXP* makeEXPinthexliteral(int decValue);
EXP* makeEXPfloatliteral(float floatLiteral);
EXP* makeEXPruneliteral(char runeLiteral);
EXP* makeEXPstringliteral(char* stringLiteral);
EXP* makeEXPrawstringliteral(char* rawStringLiteral);
EXP* makeEXPid(ID* id);
EXP* makeEXPselector(RECEIVER* receiver, ID* lastSelector);
RECEIVER* makeRECEIVER(EXP* e);
EXP* makeEXPindex(EXP* rest, EXP* lastIndex);
EXP* makeEXParguments(EXP* rest, EXP* args);
CAST* makeCAST(TYPE* type, EXP* exp);
EXP* makeEXPcast(CAST* c);
EXP* markEXPparenthesized(EXP* e);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IDS AND TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ID* makeID(char* name);
ID* appendID(ID *prevs, ID *curr);
TYPE *makeTYPEid(ID* id);
TYPE *makeTYPEarray(EXP* size, TYPE* elementType);
TYPE *makeTYPEstruct(STRUCTTYPE* structType);
STRUCTTYPE* makeSTRUCTTYPE(FIELD* fields);
TYPE *makeTYPEslice(TYPE* elementType);

#endif /* !TREE_H */
