/* this is where we build up the AST */
#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "tree.h"

extern int yylineno;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAM
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PROGRAM* makePROGRAM(PACKAGE* package, TOPLEVELDECLARATION* topLevelDeclaration) {
    PROGRAM* p;
    p = NEW(PROGRAM);
    p->package = package;
    p->topLevelDeclaration = topLevelDeclaration;
    return p;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PACKAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PACKAGE* makePACKAGE(ID* id) {
    PACKAGE* p;
    p = NEW(PACKAGE);
    p->name = id->name;
    return p;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TOP-LEVEL DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Append a declaration node to the list of declarations
 * prevs is the list of previous declarations
 * curr is the new declaration
 */
TOPLEVELDECLARATION* appendTOPLEVELDECLARATION(TOPLEVELDECLARATION *prevs, TOPLEVELDECLARATION *curr) {
    TOPLEVELDECLARATION *t;
    // prevs will be null for the first declaration that we parse
    if (prevs == NULL) return curr;
    // get a second reference, t, to the head of the list
    t = prevs;
    // move to the end of the linkedlist of declarations given by prevs
    while (t->next != NULL) t = t->next;
    // append curr to the end of the list
    t->next = curr;
    // return the head of the list
    return prevs;
}

/*
 * Make a top-level variable declaration
 */
TOPLEVELDECLARATION* makeTOPLEVELDECLARATIONvar(VARDECLARATION* varDecl) {
    TOPLEVELDECLARATION *d;
    d = NEW(TOPLEVELDECLARATION);
    d->lineno = yylineno;
    d->kind = varDeclK;
    d->val.varDeclTLD = varDecl;
    d->next = NULL;
    return d;
}

/*
 * Make a top-level variable declaration
 */
TOPLEVELDECLARATION* makeTOPLEVELDECLARATIONfunction(FUNCTIONDECLARATION* functionDecl) {
    TOPLEVELDECLARATION *d;
    d = NEW(TOPLEVELDECLARATION);
    d->lineno = yylineno;
    d->kind = functionDeclK;
    d->val.functionDeclTLD = functionDecl;
    d->next = NULL;
    return d;
}

/*
 * Make a top-level type declaration
 */
TOPLEVELDECLARATION* makeTOPLEVELDECLARATIONtype(TYPEDECLARATION* typeDecl) {
    TOPLEVELDECLARATION *d;
    d = NEW(TOPLEVELDECLARATION);
    d->lineno = yylineno;
    d->kind = typeDeclK;
    d->val.typeDeclTLD = typeDecl;
    d->next = NULL;
    return d;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLE DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Make a variable declaration for variables initialized with type only
 */
VARDECLARATION* makeVARDECLARATIONtypeonly(ID* ids, TYPE* type) {
    VARDECLARATION *d;
    d = NEW(VARDECLARATION);
    d->lineno = yylineno;
    d->kind = typeOnlyK;
    d->ids = ids;
    d->isDistributed = 0;
    d->val.typeVD = type;
    d->next = NULL;
    return d;
}

/*
 * Make a variable declaration for variables initialized with expression only
 */
VARDECLARATION* makeVARDECLARATIONexponly(ID* ids, EXP* exps) {
    VARDECLARATION *d;
    d = NEW(VARDECLARATION);
    d->lineno = yylineno;
    d->kind = expOnlyK;
    d->ids = ids;
    d->isDistributed = 0;
    d->val.expVD = exps;
    d->next = NULL;
    return d;
}

/*
 * Make a variable declaration for variables initialized with type and expressions
 */
VARDECLARATION* makeVARDECLARATIONtypeandexp(ID* ids, TYPE* type, EXP* exps) {
    VARDECLARATION *d;
    d = NEW(VARDECLARATION);
    d->lineno = yylineno;
    d->kind = typeAndExpK;
    d->ids = ids;
    d->isDistributed = 0;
    d->val.typeAndExpVD.type = type;
    d->val.typeAndExpVD.exp = exps;
    d->next = NULL;
    return d;
}

/*
 * for appending variable declarations defined in a distributed var statement
 */
VARDECLARATION* appendVARDECLARATION(VARDECLARATION *prevs, VARDECLARATION *curr) {
    // update curr to indicate that this it is part of a distrubuted variable declaration
    curr->isDistributed = 1;
    // prevs will be null for the first declaration that we parse
    if (prevs == NULL) return curr;
    // get a second reference, t, to the head of the list
    VARDECLARATION *t;
    t = prevs;
    // move to the end of the linkedlist of declarations given by prevs
    while (t->next != NULL) t = t->next;
    // append curr to the end of the list
    t->next = curr;
    // return the head of the list
    return prevs;
}

VARDECLARATION* markAsDistributedVarDecl(VARDECLARATION* v) {
    v->isDistributed = 1;
    return v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TYPE DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TYPEDECLARATION* makeTYPEDECLARATION(ID* id, TYPE* type) {
    TYPEDECLARATION* t;
    t = NEW(TYPEDECLARATION);
    t->lineno = yylineno;
    t->id = id;
    t->isDistributed = 0;
    t->type = type;
    t->next = NULL;
    return t;
}

TYPEDECLARATION* appendTYPEDECLARATION(TYPEDECLARATION* prevs, TYPEDECLARATION* curr) {
    // update curr to indicate that this it is part of a distrubuted type declaration
    curr->isDistributed = 1;
    // prevs will be null for the first declaration that we parse
    if (prevs == NULL) return curr;
    // get a second reference, t, to the head of the list
    TYPEDECLARATION *t;
    t = prevs;
    // move to the end of the linked list of declarations given by prevs
    while (t->next != NULL) t = t->next;
    // append curr to the end of the list
    t->next = curr;
    // return the head of the list
    return prevs;
}

TYPEDECLARATION* markAsDistributedTypeDecl(TYPEDECLARATION* t) {
    t->isDistributed = 1;
    return t;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FUNCTIONDECLARATION* makeFUNCTIONDECLARATION(ID* id, PARAMETER* parameters, TYPE* returnType, STATEMENT* statements) {
    FUNCTIONDECLARATION* f;
    f = NEW(FUNCTIONDECLARATION);
    f->lineno = yylineno;
    f->id = id;
    f->parameters = parameters;
    f->returnType = returnType;
    f->statements = statements;
    return f;
}

PARAMETER* makePARAMETER(ID* ids, TYPE* type) {
    PARAMETER* p;
    p = NEW(PARAMETER);
    p->lineno = yylineno;
    p->ids = ids;
    p->type = type;
    p->next = NULL;
    return p;
}

PARAMETER* appendPARAMETER(PARAMETER* prevs, PARAMETER* curr) {
    PARAMETER *t;
    // prevs will be null for the first parameter that we parse
    if (prevs == NULL) return curr;
    // get a second reference, t, to the head of the list
    t = prevs;
    // move to the end of the linked list of parameters given by prevs
    while (t->next != NULL) t = t->next;
    // append curr to the end of the list
    t->next = curr;
    // return the head of the list
    return prevs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATEMENTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

STATEMENT* appendSTATEMENT(STATEMENT* prevs, STATEMENT* curr) {
    STATEMENT *t;
    // prevs will be null for the first statement that we parse for any given block
    if (prevs == NULL) return curr;
    // get a second reference, t, to the head of the list
    t = prevs;
    // move to the end of the linked list of statements given by prevs
    while (t->next != NULL) t = t->next;
    // append curr to the end of the list
    t->next = curr;
    // return the head of the list
    return prevs;
}

STATEMENT* makeSTATEMENTempty() {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = emptyK;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTexp(EXP* exp) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = expK;
    s->val.expS = exp;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTinc(EXP* lval) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = incK;
    s->val.incS = lval;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTdec(EXP* lval) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = decK;
    s->val.decS = lval;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTprintln(EXP* exp) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = printlnK;
    s->val.printlnS = exp;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTprint(EXP* exp) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = printK;
    s->val.printS = exp;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTvardecl(VARDECLARATION* varDecl) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = varDeclK;
    s->val.varDeclS = varDecl;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTtypedecl(TYPEDECLARATION* typeDecl) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = typeDeclK;
    s->val.typeDeclS = typeDecl;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTshortdecl(EXP* ids, EXP* exps) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = shortDeclK;
    s->val.shortDeclS.ids = ids;
    s->val.shortDeclS.exps = exps;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTreturn(EXP* exp) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = returnK;
    s->val.returnS = exp;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTbreak() {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = breakK;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTcontinue() {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = continueK;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTassign(EXP* lvalues, EXP* exps) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = regAssignK;
    s->val.regAssignS.lvalues = lvalues;
    s->val.regAssignS.exps = exps;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTbinopassign(EXP* lvalue, OperationKind opKind, EXP* exp) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = binOpAssignK;
    s->val.binOpAssignS.lvalue = lvalue;
    s->val.binOpAssignS.opKind = opKind;
    s->val.binOpAssignS.exp = exp;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTif(STATEMENT* initStatement, EXP* condition, STATEMENT* body) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = ifK;
    s->val.ifS.initStatement = initStatement;
    s->val.ifS.condition = condition;
    s->val.ifS.body = body;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTifelse(STATEMENT* initStatement, EXP* condition, STATEMENT* thenPart, STATEMENT* elsePart) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = ifElseK;
    s->val.ifElseS.initStatement = initStatement;
    s->val.ifElseS.condition = condition;
    s->val.ifElseS.thenPart = thenPart;
    s->val.ifElseS.elsePart = elsePart;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTwhile(EXP* condition, STATEMENT* body) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = whileK;
    s->val.whileS.condition = condition;
    s->val.whileS.body = body;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTinfiniteloop(STATEMENT* doThisIndefinitely) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = infiniteLoopK;
    s->val.infiniteLoopS = doThisIndefinitely;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTfor(STATEMENT* initStatement, EXP* condition, STATEMENT* postStatement, STATEMENT* body) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = forK;
    s->val.forS.initStatement = initStatement;
    s->val.forS.condition = condition;
    s->val.forS.postStatement = postStatement;
    s->val.forS.body = body;
    s->next = NULL;
    return s;
}

STATEMENT* makeSTATEMENTswitch(STATEMENT* initStatement, EXP* condition, SWITCHCASE* cases) {
    STATEMENT* s;
    s = NEW(STATEMENT);
    s->lineno = yylineno;
    s->kind = switchK;
    s->val.switchS.initStatement = initStatement;
    s->val.switchS.condition = condition;
    s->val.switchS.cases = cases;
    s->next = NULL;
    return s;
}

SWITCHCASE* makeSWITCHCASEcase(EXP* exps, STATEMENT* statements) {
    SWITCHCASE* c;
    c = NEW(SWITCHCASE);
    c->lineno = yylineno;
    c->kind = caseK;
    c->val.caseC.exps = exps;
    c->val.caseC.statements = statements;
    c->next = NULL;
    return c;
}

SWITCHCASE* makeSWITCHCASEdefault(STATEMENT* statements) {
    SWITCHCASE* c;
    c = NEW(SWITCHCASE);
    c->lineno = yylineno;
    c->kind = defaultK;
    c->val.defaultStatementsC = statements;
    c->next = NULL;
    return c;
}

SWITCHCASE* appendSWITCHCASE(SWITCHCASE* prevs, SWITCHCASE* curr) {
    SWITCHCASE *t;
    // prevs will be null for the first case that we parse
    if (prevs == NULL) return curr;
    // get a second reference, t, to the head of the list
    t = prevs;
    // move to the end of the linked list of cases given by prevs
    while (t->next != NULL) t = t->next;
    // append curr to the end of the list
    t->next = curr;
    // return the head of the list
    return prevs;
}

/*
FUNCTIONCALL* makeFUNCTIONCALL(ID* id, EXP* arguments) {
    FUNCTIONCALL* f;
    f = NEW(FUNCTIONCALL);
    f->lineno = yylineno;
    f->id = id;
    f->arguments = arguments;
    return f;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STRUCTS AND FIELDS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
STRUCTT* makeSTRUCTT(ID* id, FIELD* fields) {
    STRUCTT* t;
    t = NEW(STRUCTT);
    t->lineno = yylineno;
    t->id = id;
    t->fields = fields;
    return t;
}
*/

FIELD* makeFIELD(ID* ids, TYPE* type) {
    FIELD* t;
    t = NEW(FIELD);
    t->lineno = yylineno;
    t->ids = ids;
    t->type = type;
    t->next = NULL;
    return t;
}

FIELD* appendFIELD(FIELD* prevs, FIELD* curr) {
    FIELD *t;
    // prevs will be null for the first field that we parse
    if (prevs == NULL) return curr;
    // get a second reference, t, to the head of the list
    t = prevs;
    // move to the end of the linked list of fields given by prevs
    while (t->next != NULL) t = t->next;
    // append curr to the end of the list
    t->next = curr;
    // return the head of the list
    return prevs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPRESSIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXP* makeEXPappend(EXP* slice, EXP* expToAppend) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = appendK;
    e->val.appendE.slice = slice;
    e->val.appendE.expToAppend = expToAppend;
    e->next = NULL;
    return e;
}

EXP* makeEXPplus(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = plusK;
    e->val.plusE.left = left;
    e->val.plusE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPminus(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = minusK;
    e->val.minusE.left = left;
    e->val.minusE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPtimes(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = timesK;
    e->val.timesE.left = left;
    e->val.timesE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPdiv(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = divK;
    e->val.divE.left = left;
    e->val.divE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPmod(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = modK;
    e->val.modE.left = left;
    e->val.modE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPbitwiseor(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = bitwiseOrK;
    e->val.bitwiseOrE.left = left;
    e->val.bitwiseOrE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPbitwiseand(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = bitwiseAndK;
    e->val.bitwiseAndE.left = left;
    e->val.bitwiseAndE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPxor(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = xorK;
    e->val.xorE.left = left;
    e->val.xorE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPlt(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = ltK;
    e->val.ltE.left = left;
    e->val.ltE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPgt(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = gtK;
    e->val.gtE.left = left;
    e->val.gtE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPeq(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = eqK;
    e->val.eqE.left = left;
    e->val.eqE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPneq(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = neqK;
    e->val.neqE.left = left;
    e->val.neqE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPleq(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = leqK;
    e->val.leqE.left = left;
    e->val.leqE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPgeq(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = geqK;
    e->val.geqE.left = left;
    e->val.geqE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPor(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = orK;
    e->val.orE.left = left;
    e->val.orE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPand(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = andK;
    e->val.andE.left = left;
    e->val.andE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPleftshift(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = leftShiftK;
    e->val.leftShiftE.left = left;
    e->val.leftShiftE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPrightshift(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = rightShiftK;
    e->val.rightShiftE.left = left;
    e->val.rightShiftE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPbitclear(EXP* left, EXP* right) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = bitClearK;
    e->val.bitClearE.left = left;
    e->val.bitClearE.right = right;
    e->next = NULL;
    return e;
}

EXP* makeEXPuplus(EXP* exp) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = uPlusK;
    e->val.uPlusE = exp;
    e->next = NULL;
    return e;
}

EXP* makeEXPuminus(EXP* exp) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = uMinusK;
    e->val.uMinusE = exp;
    e->next = NULL;
    return e;
}

EXP* makeEXPunot(EXP* exp) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = uNotK;
    e->val.uNotE = exp;
    e->next = NULL;
    return e;
}

EXP* makeEXPuxor(EXP* exp) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = uXorK;
    e->val.uXorE = exp;
    e->next = NULL;
    return e;
}

EXP* makeEXPureceive(EXP* exp) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = uReceiveK;
    e->val.uReceiveE = exp;
    e->next = NULL;
    return e;
}

EXP* appendEXP(EXP* prevs, EXP* curr) {
    EXP *t;
    // prevs will be null for the first exp that we parse
    if (prevs == NULL) return curr;
    // get a second reference, t, to the head of the list
    t = prevs;
    // move to the end of the linked list of exps given by prevs
    while (t->next != NULL) t = t->next;
    // append curr to the end of the list
    t->next = curr;
    // return the head of the list
    return prevs;
}

EXP* makeEXPintdecliteral(int decValue) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = intLiteralK;
    e->val.intLiteralE.decValue = decValue;
    e->val.intLiteralE.kind = decIL;
    e->next = NULL;
    return e;
}

EXP* makeEXPintoctliteral(int decValue) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = intLiteralK;
    e->val.intLiteralE.decValue = decValue;
    e->val.intLiteralE.kind = octIL;
    e->next = NULL;
    return e;
}

EXP* makeEXPinthexliteral(int decValue) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = intLiteralK;
    e->val.intLiteralE.decValue = decValue;
    e->val.intLiteralE.kind = hexIL;
    e->next = NULL;
    return e;
}

EXP* makeEXPfloatliteral(float floatLiteral) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = floatLiteralK;
    e->val.floatLiteralE = floatLiteral;
    e->next = NULL;
    return e;
}

EXP* makeEXPruneliteral(char runeLiteral) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = runeLiteralK;
    e->val.runeLiteralE = runeLiteral;
    e->next = NULL;
    return e;
}

EXP* makeEXPstringliteral(char* stringLiteral) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = stringLiteralK;
    e->val.stringLiteralE = stringLiteral;
    e->next = NULL;
    return e;
}

EXP* makeEXPid(ID* id) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = idK;
    e->val.idE = id;
    e->next = NULL;
    return e;
}

/*
SLICE* makeSLICEwithstep(EXP* start, EXP* end, EXP* step) {
    SLICE* s;
    s = NEW(SLICE);
    s->lineno = yylineno;
    s->kind = withStepK;
    s->val.withStepS.start = start;
    s->val.withStepS.end = end;
    s->val.withStepS.step = step;
    return s;
}

SLICE* makeSLICEwithoutstep(EXP* start, EXP* end) {
    SLICE* s;
    s = NEW(SLICE);
    s->lineno = yylineno;
    s->kind = withoutStepK;
    s->val.withoutStepS.start = start;
    s->val.withoutStepS.end = end;
    return s;
}

EXP* makeEXPslice(EXP* rest, SLICE* lastSlice) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = sliceK;
    e->val.sliceE.rest = rest;
    e->val.sliceE.lastSlice = lastSlice;
    e->next = NULL;
    return e;
}
*/

EXP* makeEXPselector(EXP* rest, ID* lastSelector) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = selectorK;
    e->val.selectorE.rest = rest;
    e->val.selectorE.lastSelector = lastSelector;
    e->next = NULL;
    return e;
}

EXP* makeEXPindex(EXP* rest, EXP* lastIndex) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = indexK;
    e->val.indexE.rest = rest;
    e->val.indexE.lastIndex = lastIndex;
    e->next = NULL;
    return e;
}

EXP* makeEXParguments(EXP* rest, EXP* args) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = argumentsK;
    e->val.argumentsE.rest = rest;
    e->val.argumentsE.args = args;
    e->next = NULL;
    return e;
}

CAST* makeCAST(TYPE* type, EXP* exp) {
    CAST* c;
    c = NEW(CAST);
    c->lineno = yylineno;
    c->type = type;
    c->exp = exp;
    return c;
}

EXP* makeEXPcast(CAST* c) {
    EXP* e;
    e = NEW(EXP);
    e->lineno = yylineno;
    e->type = NULL; // for now
    e->kind = castK;
    e->val.castE = c;
    e->next = NULL;
    return e;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OTHER
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ID* makeID(char* name) {
    ID* i;
    i = NEW(ID);
    i->name = name;
    i->next = NULL;
    return i;
}

ID* appendID(ID *prevs, ID *curr) {
    ID *t;
    // prevs will be null for the first id that we parse
    if (prevs == NULL) return curr;
    // get a second reference, t, to the head of the list
    t = prevs;
    // move to the end of the linked list of ids given by prevs
    while (t->next != NULL) t = t->next;
    // append curr to the end of the list
    t->next = curr;
    // return the head of the list
    return prevs;
}

TYPE *makeTYPEid(ID* id) {
    TYPE *t;
    t = NEW(TYPE);
    t->lineno = yylineno;
    t->kind = idK;
    t->val.idT = id;
    return t;
}

TYPE *makeTYPEarray(EXP* size, TYPE* elementType) {
    TYPE *t;
    t = NEW(TYPE);
    t->lineno = yylineno;
    t->kind = arrayK;
    t->val.arrayT.size = size;
    t->val.arrayT.elementType = elementType;
    return t;
}

TYPE *makeTYPEstruct(FIELD* fields) {
    TYPE *t;
    t = NEW(TYPE);
    t->lineno = yylineno;
    t->kind = structK;
    t->val.structT = fields;
    return t;
}

TYPE *makeTYPEslice(TYPE* elementType) {
    TYPE *t;
    t = NEW(TYPE);
    t->lineno = yylineno;
    t->kind = sliceK;
    t->val.sliceT = elementType;
    return t;
}
