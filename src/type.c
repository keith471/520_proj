
#include "type.h"
#include "tree.h"
#include "memory.h"

/*
type Person struct {
    x [10]int
}

var keith Person

keith.x[0]

     [
    / \
   .   0
  / \
kei  x


1. keith
2. keith '.'
3. keith '.' x -> primaryExp
4. (primaryExp) '[' exp ']'

typedef struct SYMBOL {
    int lineno;
    char* name;
    SymbolKind kind;
    union {
        struct TYPE* typeS;
        struct TYPE* varS;
        struct FUNCTIONDECLARATION *functionDeclS;
        struct STATEMENT* shortDeclS;
        struct {struct TYPEDECLARATION * typeDecl;
                struct TYPE* type;} typeDeclS;
        struct {struct VARDECLARATION* varDecl;
                struct TYPE* type;} varDeclS;
        struct {struct PARAMETER * param;
                struct TYPE* type;} parameterS;
        struct {struct FIELD* field;
                struct TYPE* type;} fieldS;
    } val;
    // this is a linked list in the SymbolTable hashmap, so we have to have this next field.
    // it doesn't actually have anything to do with the 'current' symbol
    struct SYMBOL *next;
} SYMBOL;


// ...after having given everything a type and added the types to the symbol table...

// where e is an expression of type selectorK
void checkSelectors(EXP* e) {
    SymbolTable* t = getStructScope(e->val.selectorK.receiver, FUCK);
    // seach t for e->val.selectorK.lastSelector
}

SymbolTable* getStructScope(EXP* receivingExp, SymbolTable* t) {
    SymbolTable* structTable;
    SymbolTable* structTable;
    SYMBOL* s;
    TYPE* type;
    switch (receivingExp->kind) {
        case selectorK:
            t = symSELECTOR(e->val.selectorE.receiver->receivingExp);
            break;
        case identifierK:
            // we need to seach the symbol table for the identifier
            s = getSymbol(t, e->val.idE->name, e->lineno);
            // the symbol should have type struct
            switch (s->kind) {
                case typeDeclSym:
                    type = s->val.typeDeclS.type;
                    break;
                case varDeclSym:
                    type = s->val.varDeclS.type;
                    break;
                case shortDeclSym:
                    type = s->val.shortDeclS.type;
                    break;
                case parameterSym:
                    type = s->val.parameterS.type;
                    break;
                default:
                    // error
                    reportStrError("SYMBOL", "%s is not a reference to a struct", e->val.idE->name, e->lineno);
                    return NULL;
            }
            // we have the type of the symbol --> now check if it is a struct
            // type should NOT be NULL since we should have inferred all types by this time
            // the type could be an idK with an underlying type that is eventually a struct
            // make sure to check this!
            if (type->kind == structK) {

            } else if (type->kind == idK) {

            } else {
                // error --> not a struct
            }
            break;
        case indexK:
            // find the id of the array, get its type, and check that the elementType of its type is a struct
            break;
        case argumentsK;
            // function call could return a struct
            // find the function in the symbol table
            // use the reference to the FUNCTIONDECLARATION to get the return type of the function
            // the return type should be a struct
            // if it is a struct, then get access to its symbolTable field and use that
            break;
        default:
            // this is an error
            break;
    }
    return NULL;
}



*/


TYPE *intTYPE, *float64TYPE, *stringTYPE, *runeTYPE, *boolTYPE;

void initTypes() {
    intTYPE = NEW(TYPE);
    intTYPE->kind = intK;
    float64TYPE = NEW(TYPE);
    float64TYPE->kind = float64K;
    stringTYPE = NEW(TYPE);
    stringTYPE->kind = stringK;
    runeTYPE = NEW(TYPE);
    runeTYPE->kind = runeK;
    boolTYPE = NEW(TYPE);
    boolTYPE->kind = boolK;
}

void typePROGRAM(PROGRAM* p) {
    // initialize some base types to use for type comparisons
    initTypes();
    typeTOPLEVELDECLARATION(p->topLevelDeclaration);
}

void typeTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld) {
    if (tld == NULL) return;
    switch(tld->kind) {
        case vDeclK:
            typeVARDECLARATION(tld->val.varDeclTLD);
            break;
        case tDeclK:
            // nothing to do
            //typeTYPEDECLARATION(tld->val.typeDeclTLD);
            break;
        case functionDeclK:
            typeFUNCTIONDECLARATION(tld->val.functionDeclTLD);
            break;
        default:
            break;
    }
    typeTOPLEVELDECLARATION(tld->next);
}

void typeVARDECLARATION(VARDECLARATION* v) {
    if (v == NULL) return;
    typeVARDECLARATIONlist(v);
    typeVARDECLARATION(v->nextDistributed);
}

void typeVARDECLARATIONlist(VARDECLARATION* v) {
    if (v == NULL) return;
    switch(v->kind) {
        case typeOnlyK:
            // already well typed :)
            break;
        case expOnlyK:
            // we need to type check the expression and then use the type of the expression
            // as the type of the variable! We update the type of the expression in the SYMBOL
            // in the symbol table
            typeEXP(v->val.expVD.exp);
            v->val.expVD.symbol->val.varDeclS.type = v->val.expVD.exp->type;
            break;
        case typeAndExpK:
            // this typechecks as long as the expression type checks and is equal to the type of the variable
            // type check the expression
            typeEXP(v->val.typeAndExpVD.exp);
            // check that the type of the expression is equal to the type of the variable
            assertEqualTYPEs(v->val.typeAndExpVD.type, v->val.typeAndExpVD.exp->type);
        default:
            break;
    }
    typeVARDECLARATIONlist(v->next);
}

/*
 * a function declaration type checks if its statements type check
 * TODO additionally, there should be a well-typed return statement on every execution path
 * if the function has a return value --> perhaps add this as a second pass through the statements of the function
 */
void typeFUNCTIONDECLARATION(FUNCTIONDECLARATION* f) {
    typeSTATEMENT(f->statements);
    // checkForReturns(f->statements, f->returnType);
}

void typeSTATEMENT(STATEMENT* s) {
    if (s == NULL) return;
    switch (s->kind) {
        case emptyK:
            // trivially well-typed
            break;
        case expK:
            // type check the expression
            typeEXP(s->val.expS);
            break;
        case incK:
            // type check the expression
            typeEXP(s->val.incS);
            // check that it is an type
            assertNumeric(s->val.incS->type, s->lineno);
            break;
        case decK:
            typeEXP(s->val.decS);
            assertNumeric(s->val.decS->type, s->lineno);
            break;
        case regAssignK:
            // type checks if the left and right exps are both well-typed and have the same type
            typeEXP(s->val.regAssignS.lvalue);
            typeEXP(s->val.regAssignS.exp);
            // assert that the types are equal
            assertEqualTYPEs(s->val.regAssignS.lvalue->type, s->val.regAssignS.exp->type);
            // type check the next assignment
            typeSTATEMENT(s->val.regAssignS.next);
            break;
        case binOpAssignK:
            // type checks if the expressions type check and if the operator accepts two
            // expressions of type(lvalue) and type(exp) and returns a value of type(lvalue)
            typeEXP(s->val.binOpAssignS.lvalue);
            typeEXP(s->val.binOpAssignS.exp);
            assertValidOpUsage(s->val.binOpAssignS.opKind, s->val.binOpAssignS.lvalue->type, s->val.binOpAssignS.exp->type);
            break;
        case shortDeclK:
            // type checks if the exp type checks
            typeEXP(s->val.shortDeclS.exp);
            // if this is a redeclaration, we need to check that the type of the
            // expression is the same as the type of the prevDeclSym
            if (s->val.shortDeclS.isRedecl) {
                // first, we better check that we can assign to this symbol (i.e. it is a variable!)
                assertCanAssign(s->val.shortDeclS.prevDeclSym, s->lineno);
                // then, we assert that the type of the expression is equal to the type of the variable
                assertEqualTYPEs(s->val.shortDeclS.prevDeclSym->type, s->val.shortDeclS.exp->type);
            } else {
                // we need to set the type on the symbol
                s->val.shortDeclS.symbol->val.shortDeclS.type = s->val.shortDeclS.exp->type;
            }
            break;
        case varDeclK:
            // simply type the variable declaration
            typeVARDECLARATION(s->val.varDeclS);
            break;
        case typeDeclK:
            // trivially well-typed
            break;
        case printK:
            // type checks if all the expressions type check AND they all resolve to a base type
            typeEXPs(s->val.printS);
            assertEXPsResolveToBaseType(s->val.printS, s->lineno);
            break;
        case printlnK:
            // type checks if all the expressions type check AND they all resolve to a base type
            typeEXPs(s->val.printlnS);
            assertEXPsResolveToBaseType(s->val.printlnS, s->lineno);
            break;
        case returnK:
            // well-typed if the expression is well-typed
            // the expression should also have the same type as the return of the function but we
            // check this in a second pass
            typeEXP(s->val.returnS);
            break;
        case ifK:
            // type checks if the init statement type checks
            typeSTATEMENT(s->val.ifS.initStatement);
            // and the expression is well-typed and resolves to a bool
            typeEXP(s->val.ifS.condition);
            assertResolvesToBool(s->val.ifS.condition->type, s->lineno);
            // and the statements in the body type check
            typeSTATEMENT(s->val.ifS.body);
            break;
        case ifElseK:
            typeSTATEMENT(s->val.ifElseS.initStatement);
            typeEXP(s->val.ifElseS.condition);
            assertResolvesToBool(s->val.ifElseS.condition->type, s->lineno);
            typeSTATEMENT(s->val.ifElseS.thenPart);
            typeSTATEMENT(s->val.ifElseS.elsePart);
            break;
        case switchK:
            typeSTATEMENTswitch(s);
            break;
        case whileK:
            // type checks if condition type checks and resolves to bool
            typeEXP(s->val.whileS.condition);
            assertResolvesToBool(s->val.whileS.condition->type, s->lineno);
            // and its statements type check
            typeSTATEMENT(s->val.whileS.body);
            break;
        case infiniteLoopK:
            // type checks if its body type checks
            typeSTATEMENT(s->val.infiniteLoopS);
            break;
        case forK:
            typeSTATEMENT(s->val.forS.initStatement);
            typeEXP(s->val.forS.condition);
            assertResolvesToBool(s->val.whileS.condition->type, s->lineno);
            typeSTATEMENT(s->val.forS.postStatement);
            typeSTATEMENT(s->val.forS.body);
            break;
        case breakK:
            // trivially well-typed
            break;
        case continueK:
            // trivially well-typed
            break;
        case blockK:
            // type checks if its statements type check
            typeSTATEMENT(s->val.blockS);
            break;
    }
    typeSTATEMENT(s->next);
}

/*
 * a switch statement with an exp type checks if
 *  the init statement typechecks
 *  the exp typechecks
 *  all the exps for the cases type check to the same type as exp
 *  all the statements in the cases type check
 * a switch statement with no exp type checks if
 *  the init statement type checks
 *  all the exps for the cases type check to bool
 *  all the statements in the cases type check
 */
void typeSTATEMENTswitch(STATEMENT* s) {
    typeSTATEMENT(s->val.switchS.initStatement);
    if (s->val.switchS.condition != NULL) {
        typeEXP(s->val.switchS.condition);
        typeSWITCHCASE(s->val.switchS.cases, s->val.switchS.condition->type);
    } else {
        typeEXP(s->val.switchS.condition);
        typeSWITCHCASE(s->val.switchS.cases, boolTYPE);
    }
}

/*
 * A switch case type checks if
 *  all its expressions have type t
 *  all its statements type check
 */
void typeSWITCHCASE(SWITCHCASE* sc, TYPE* t) {
    if (sc == NULL) return;
    switch(s->kind) {
        case caseK:
            typeEXPs(sc->val.caseC.exps);
            assertCaseEXPsHaveType(sc->val.caseC.exps, t);
            typeSTATEMENT(sc->val.caseC.statements);
            break;
        case defaultK:
            typeSTATEMENT(sc->val.defaultStatementsC);
            break;
    }
    typeSWITCHCASE(sc->next, t);
}

void typeEXPs(EXP* e) {
    if (e == NULL) return;
    typeEXP(e);
    typeEXPs(e->next);
}

void typeEXP(EXP* e) {
    int err;
    CASTCHECKRETURN* ctr;
    switch (e->kind) {
        case identifierK:
            e->type = typeVar(e->val.idE.symbol);
            break;
        case intLiteralK:
            e->type = intTYPE;
            break;
        case floatLiteralK:
            e->type = float64TYPE;
            break;
        case runeLiteralK:
            e->type = runeTYPE;
            break;
        case stringLiteralK:
            e->type = stringTYPE;
            break;
        case rawStringLiteralK:
            e->type = stringTYPE;
            break;
        case plusK:
            typeEXP(e->val.plusE.left);
            typeEXP(e->val.plusE.right);
            e->type = typePlus(e->val.plusE.left->type, e->val.plusE.right->type, e->lineno);
            break;
        case minusK:
            typeEXP(e->val.minusE.left);
            typeEXP(e->val.minusE.right);
            e->type = numericOp(e->val.minusE.left->type, e->val.minusE.right->type, e->lineno);
            break;
        case timesK:
            typeEXP(e->val.timesE.left);
            typeEXP(e->val.timesE.right);
            e->type = numericOp(e->val.timesE.left->type, e->val.timesE.right->type, e->lineno);
            break;
        case divK:
            typeEXP(e->val.divE.left);
            typeEXP(e->val.divE.right);
            e->type = numericOp(e->val.divE.left->type, e->val.divE.right->type, e->lineno);
            break;
        case modK:
            typeEXP(e->val.modE.left);
            typeEXP(e->val.modE.right);
            e->type = intOp(e->val.modE.left->type, e->val.modE.right->type, e->lineno);
            break;
        case bitwiseOrK:
            typeEXP(e->val.bitwiseOrE.left);
            typeEXP(e->val.bitwiseOrE.right);
            e->type = intOp(e->val.bitwiseOrE.left->type, e->val.bitwiseOrE.right->type, e->lineno);
            break;
        case bitwiseAndK:
            typeEXP(e->val.bitwiseAndE.left);
            typeEXP(e->val.bitwiseAndE.right);
            e->type = intOp(e->val.bitwiseAndE.left->type, e->val.bitwiseAndE.right->type, e->lineno);
            break;
        case xorK:
            typeEXP(e->val.xorE.left);
            typeEXP(e->val.xorE.right);
            e->type = intOp(e->val.xorE.left->type, e->val.xorE.right->type, e->lineno);
            break;
        case ltK:
            typeEXP(e->val.ltE.left);
            typeEXP(e->val.ltE.right);
            // we need to check that both types are ordered and equal
            checkOrderedAndEqual(e->val.ltE.left->type, e->val.ltE.right->type, e->lineno);
            e->type = boolTYPE;
            break;
        case gtK:
            typeEXP(e->val.gtE.left);
            typeEXP(e->val.gtE.right);
            checkOrderedAndEqual(e->val.gtE.left->type, e->val.gtE.right->type, e->lineno);
            e->type = boolTYPE;
            break;
        case eqK:
            typeEXP(e->val.eqE.left);
            typeEXP(e->val.eqE.right);
            // all values that we support in golite are comparable, so all we have to do
            // is check that the types are equal
            err = assertEqualTYPEs(e->val.eqE.left->type, e->val.eqE.right->type, lineno);
            if (err) {
                terminateIfErrors();
            }
            e->type = boolTYPE;
            break;
        case neqK:
            typeEXP(e->val.neqE.left);
            typeEXP(e->val.neqE.right);
            err = assertEqualTYPEs(e->val.neqE.left->type, e->val.neqE.right->type, lineno);
            if (err) {
                terminateIfErrors();
            }
            e->type = boolTYPE;
            break;
        case leqK:
            typeEXP(e->val.leqE.left);
            typeEXP(e->val.leqE.right);
            checkOrderedAndEqual(e->val.leqE.left->type, e->val.leqE.right->type, e->lineno);
            e->type = boolTYPE;
            break;
        case geqK:
            typeEXP(e->val.geqE.left);
            typeEXP(e->val.geqE.right);
            checkOrderedAndEqual(e->val.geqE.left->type, e->val.geqE.right->type, e->lineno);
            e->type = boolTYPE;
            break;
        case orK:
            typeEXP(e->val.orE.left);
            typeEXP(e->val.orE.right);
            e->type = boolOp(e->val.orE.left->type, e->val.orE.right->type, e->lineno);
            break;
        case andK:
            typeEXP(e->val.andE.left);
            typeEXP(e->val.andE.right);
            e->type = boolOp(e->val.andE.left->type, e->val.andE.right->type, e->lineno);
            break;
        case leftShiftK:
            typeEXP(e->val.leftShiftE.left);
            typeEXP(e->val.leftShiftE.right);
            e->type = intOp(e->val.leftShiftE.left->type, e->val.leftShiftE.right->type, e->lineno);
            break;
        case rightShiftK:
            typeEXP(e->val.rightShiftE.left);
            typeEXP(e->val.rightShiftE.right);
            e->type = intOp(e->val.rightShiftE.left->type, e->val.rightShiftE.right->type, e->lineno);
            break;
        case bitClearK:
            typeEXP(e->val.bitClearE.left);
            typeEXP(e->val.bitClearE.right);
            e->type = intOp(e->val.bitClearE.left->type, e->val.bitClearE.right->type, e->lineno);
            break;
        case appendK:
            typeEXP(e->val.appendE.slice);
            typeEXP(e->val.appendE.expToAppend);
            // the slice exp needs to have type S which resolves to a slice with elements of type T
            // and the expToAppend needs to have type T
            checkAppendIsValid(e->val.appendE.slice->type, e->val.appendE.expToAppend, e->lineno);
            e->type = e->val.appendE.slice->type;
            break;
        case castK:
            // this is well-typed if
            //  type resolves to int, float, bool, or rune
            //  exp is well-typed and has a type that can be cast to type
            assertCastResolution(e->val.castE->type, e->lineno);
            typeEXP(e->val.castE->exp);
            // basically, we just need to ensure that the type of the expression also resolves to
            // int, float, bool, or rune!
            assertCastResolution(e->val.castE->exp->type, e->lineno);
            e->type = e->val.castE->type;
            break;
        case selectorK:
            //
            //symRECEIVER(e->val.selectorE.receiver, t);
            break;
        case indexK:
            // an index into an array or slice is well-typed if
            // index is well typed and resolves to int
            typeEXP(e->val.indexE.lastIndex);
            // rest is well typed and resolves to a slice or array
            typeEXP(e->val.indexE.rest);
            e->type = getElementType(e->val.indexE.rest->type, e->lineno);
            if (e->type == NULL) {
                terminateIfErrors();
            }
            break;
        case argumentsK:
            // here, we can FINALLY check whether or not this is actually supposed to be a cast
            ctr = checkCast(e);
            switch (ctr->kind) {
                case castKind:
                    prepTypeEXPcast(e, ctr->val.castType);
                    break;
                case functionCallKind:
                    typeEXPfunctioncall(e, ctr->val.functionDecl);
                    break;
            }
            break;
        case uPlusK:
            typeEXP(e->val.uPlusE);
            err = assertNumeric(e->val.uPlusE->type);
            if (err) {
                terminateIfErrors();
            }
            e->type = e->val.uPlusE->type;
            break;
        case uMinusK:
            typeEXP(e->val.uMinusE);
            err = assertNumeric(e->val.uMinusE->type);
            if (err) {
                terminateIfErrors();
            }
            e->type = e->val.uMinusE->type;
            break;
        case uNotK:
            typeEXP(e->val.uNotE);
            err = assertResolvesToBool(e->val.uNotE->type);
            if (err) {
                terminateIfErrors();
            }
            e->type = e->val.uNotE->type;
            break;
        case uXorK:
            typeEXP(e->val.uXorE);
            err = assertResolvesToInt(e->val.uXorE->type);
            if (err) {
                terminateIfErrors();
            }
            e->type = e->val.uXorE->type;
            break;
        default:
            break;
    }
}

/*
 * types the expression as a cast if it indeed is one
 * returns 1 if the expression was a cast and 0 otherwise
 */
CASTCHECKRETURN* checkCast(EXP* e) {
    CASTCHECKRETURN* ctr;
    // e->val.argumentsE.rest should really be an exp of identifierK kind,
    // else we are trying to call something that is not a function
    if (e->val.argumentsE.rest->kind != identifierK) {
        reportError("TYPE", "cannot call a non-function", e->lineno);
        terminateIfErrors();
        return NULL;
    }

    // get the symbol for the identifier
    SYMBOL* s = e->val.argumentsE.rest->val.idE.symbol;

    // check the kind of the symbol to determine if it is a function call or cast
    // in the event it is a cast, get the type of the symbol
    // in the event it is a function call, get the declaration of the function
    int isFunctionCall = 0;
    TYPE* t;
    FUNCTIONDECLARATION* f;
    switch (s->kind) {
        case typeSym:
            // this is actually a cast!
            t = s->val.typeS;
            break;
        case typeDeclSym:
            // this is actually a cast!
            t = s->val.typeDeclS.type;
            break;
        case functionDeclSym:
            // this is indeed a function call
            isFunctionCall = 1;
            f = s->val.functionDeclS;
            break;
        default:
            // neither a cast nor a function call --> error
            reportStrError("TYPE", "%s is not a function or type", s->name, e->lineno);
            terminateIfErrors();
            break;
    }

    ctr = NEW(CASTCHECKRETURN);

    if (isFunctionCall) {
        ctr->kind = functionCallKind;
        ctr->val.functionDecl = f;
    } else {
        ctr->kind = castKind;
        ctr->val.castType = t;
    }

    return ctr;
}

void prepTypeEXPcast(EXP* e, TYPE* t) {
    // firstly, we need to check that there is only one argument to the cast
    int numArgs = countArgs(e->val.argumentsE.args);
    if (numArgs == 0) {
        reportStrError("TYPE", "missing argument to conversion to %s", s->name, e->lineno);
    } else if (numArgs > 1) {
        reportStrError("TYPE", "too many arguments to conversion to %s", s->name, e->lineno);
    }
    terminateIfErrors();

    // the cast is valid
    // repackage e as a cast expression, and then we re-type-check it :)
    e->type = castK;
    CAST* c = makeCAST(t, e->val.argumentsE.args);
    e->val.castE = c;

    // re-type this as a cast :)
    typeEXP(e);
}

/*
 * if this function is called, we've already checked that the function name does indeed
 * identify a function, and we have passed the corresponding function declaration in
 * so, all we need to do is check that the arguments are well typed and have the
 * same types as the parameters the function accepts
 */
void typeEXPfunctioncall(EXP* e, FUNCTIONDECLARATION* fd) {
    // check that the arguments are well-typed
    typeEXPs(e->val.argumentsE.args);
    // check that there are equally many arguments as parameters and that they have the
    // same types
    matchArgsToParams(e->val.argumentsE.args, fd->parameters, fd->parameters, fd->id->name, e->lineno);
    // the expression has the same type as the function's return type
    e->type = fd->returnType;
}

void matchArgsToParams(EXP* args, PARAMETER* currParam, PARAMETER* params, char* name, int lineno) {
    if (args == NULL && params == NULL) return;
    if (args == NULL) {
        reportStrError("TYPE", "too few arguments passed to function %s", name, lineno);
        return;
    }
    if (params == NULL) {
        reportStrError("TYPE", "too many arguments passed to function %s", name, lineno);
        return;
    }
    assertEqualTYPEs(currParam->type, args->type);
    // get the next current param
    if (currParam->nextId == NULL) {
        currParam = params->nextParamSet;
        params = params->nextParamSet;
    } else {
        currParam = currParam->nextId;
    }
    matchArgsToParams(args->next, currParam, params, name, lineno);
}

//==============================================================================
// Helpers
//==============================================================================

int countArgs(EXP* e) {
    if (e == NULL) return 0;
    return 1 + countArgs(e->next);
}

/*
ok so left and right
have to have the same type
and that type has to resolve to something that the operator can accept
*/

TYPE* typePlus(TYPE* left, TYPE* right, int lineno) {
    // assert that the types are the same
    int err1 = assertEqualTYPEs(left, right, lineno);
    // assert that they are both numeric types or strings
    int err2 = assertNumericOrString(left, lineno);
    assertNumericOrString(right, lineno);

    // we don't know what to return if there were errors, so we have to terminate
    if (err1 || err2) {
        terminateIfErrors();
    }
    return left;
}

TYPE* numericOp(TYPE* left, TYPE* right, int lineno) {
    // assert that the types are the same
    int err1 = assertEqualTYPEs(left, right, lineno);
    // assert that they are both numeric types
    int err2 = assertNumeric(left, lineno);
    assertNumeric(right, lineno);

    // we don't know what to return if there were errors, so we have to terminate
    if (err1 || err2) {
        terminateIfErrors();
    }
    return left;
}

TYPE* intOp(TYPE* left, TYPE* right, int lineno) {
    // assert that both types are the same
    int err1 = assertEqualTYPEs(left, right, lineno);
    // assert that both resolve to ints
    int err2 = assertResolvesToInt(left);
    assertResolvesToInt(right);

    // we don't know what to return if there were errors, so we have to terminate
    if (err1 || err2) {
        terminateIfErrors();
    }
    return left;
}

/*
 * returns the type associated with a symbol
 */
TYPE* getSymbolType(SYMBOL *s) {
    switch(s->kind) {
        case typeSym:
            // should never hit this
            break;
        case typeDeclSym:
            // should never hit this
            break;
        case varSym:
            return s->val.varS;
            break;
        case varDeclSym:
            return s->val.varDeclS.type;
            break;
        case shortDeclSym:
            return s->val.shortDeclS.type;
            break;
        case functionDeclSym:
            break;
        case parameterSym:
            return s->val.parameterS.type;
            break;
        case fieldSym:
            return s->val.fieldS.type;
            break;
    }
    // should never reach this
    return NULL;
}

/*
 * ensures that t resolves to a slice or array, and returns the type of
 * the elements in the slice or array if so
 */
TYPE* getElementType(TYPE* t, int lineno) {
    if (t == NULL) {
        reportError("TYPE", "expected slice or array but found void type", lineno);
        return NULL;
    }
    switch (t->kind) {
        case intK:
            reportError("TYPE", "expected slice or array but found int", lineno);
            break;
        case float64K:
            reportError("TYPE", "expected slice or array but found float64", lineno);
            break;
        case runeK:
            reportError("TYPE", "expected slice or array but found rune", lineno);
            break;
        case boolK:
            reportError("TYPE", "expected slice or array but found bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected slice or array but found string", lineno);
            break;
        case idK:
            // check the underlying type!
            return getElementType(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "expected slice or array but found struct", lineno);
            break;
        case sliceK:
            return t->val.sliceT;
            break;
        case arrayK:
            return t->val.arrayT.elementType;
            break;
    }
    return NULL;
}

void checkOrderedAndEqual(TYPE* left, TYPE* right, int lineno) {
    // assert that the types are equal
    int err1 = assertEqualTYPEs(left, right, lineno);
    // assert that both types are ordered
    int err2 = assertOrdered(left, lineno);
    assertOrdered(right, lineno);

    if (err1 || err2) {
        terminateIfErrors();
    }
}

/*
 * checks that:
 *  s resolves to a slice of type []T
 *  t has type T
 * if either of these things is not true, an error is reported and we stop compilation
 */
void checkAppendIsValid(TYPE* s, TYPE* t, int lineno) {
    TYPE* elementType = getSliceElementType(s, lineno);
    if (elementType == NULL) {
        terminateIfErrors();
    }
    int err = assertEqualTYPEs(elementType, t, lineno);
    if (err) {
        terminateIfErrors();
    }
}

TYPE* getSliceElementType(TYPE* t, lineno) {
    if (t == NULL) {
        reportError("TYPE", "expected slice but found void type", lineno);
        return NULL;
    }
    switch(t->kind) {
        case intK:
            reportError("TYPE", "expected slice but found int", lineno);
            break;
        case float64K:
            reportError("TYPE", "expected slice but found float64", lineno);
            break;
        case runeK:
            reportError("TYPE", "expected slice but found rune", lineno);
            break;
        case boolK:
            reportError("TYPE", "expected slice but found bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected slice but found string", lineno);
            break;
        case idK:
            // check the underlying type!
            return getSliceElementType(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "expected slice but found struct", lineno);
            break;
        case sliceK:
            return t->val.sliceT;
            break;
        case arrayK:
            reportError("TYPE", "expected slice but found array", lineno);
            break;
    }
    return NULL;
}

/*
 * reports an error if two types are not equal
 * returns 1 if error and zero otherwise
 */
int assertEqualTYPEs(TYPE *expected, TYPE *actual, int lineno) {

}

/*
 * reports an error if the type cannot be ordered
 * returns 0 if fine and 1 if error
 */
int assertOrdered(TYPE* t, int lineno) {
    if (t == NULL) {
        reportError("TYPE", "expected non-void type but found void type", lineno);
        return 1;
    }
    switch(t->kind) {
        case intK:
            return 0;
            break;
        case float64K:
            return 0;
            break;
        case runeK:
            return 0;
            break;
        case boolK:
            reportError("TYPE", "type bool is not ordered", lineno);
            return 1;
            break;
        case stringK:
            return 0;
            break;
        case idK:
            // check the underlying type!
            return assertOrdered(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "type struct is not ordered", lineno);
            return 1;
            break;
        case sliceK:
            reportError("TYPE", "type slice is not ordered", lineno);
            return 1;
            break;
        case arrayK:
            reportError("TYPE", "type array is not ordered", lineno);
            return 1;
            break;
    }
}

/*
 * reports an error if a type is not numeric (int, float64, rune, or alias to one of the three)
 * returns 1 if error and zero otherwise
 */
int assertNumeric(TYPE* t, int lineno) {
    if (t == NULL) {
        reportError("TYPE", "expected non-void type but found void type", lineno);
        return 1;
    }
    switch(t->kind) {
        case intK:
            return 0;
            break;
        case float64K:
            return 0;
            break;
        case runeK:
            return 0;
            break;
        case boolK:
            reportError("TYPE", "expected numeric type but found bool", lineno);
            return 1;
            break;
        case stringK:
            reportError("TYPE", "expected numeric type but found string", lineno);
            return 1;
            break;
        case idK:
            // check the underlying type!
            return assertNumeric(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "expected numeric type but found struct", lineno);
            return 1;
            break;
        case sliceK:
            reportError("TYPE", "expected numeric type but found slice", lineno);
            return 1;
            break;
        case arrayK:
            reportError("TYPE", "expected numeric type but found array", lineno);
            return 1;
            break;
    }
}

int assertNumericOrString(TYPE* t, int lineno) {
    if (t == NULL) {
        reportError("TYPE", "expected non-void type but found void type", lineno);
        return 1;
    }
    switch(t->kind) {
        case intK:
            return 0;
            break;
        case float64K:
            return 0;
            break;
        case runeK:
            return 0;
            break;
        case boolK:
            reportError("TYPE", "expected numeric or string type but found bool", lineno);
            return 1;
            break;
        case stringK:
            return 0;
            break;
        case idK:
            // check the underlying type!
            return assertNumericOrString(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "expected numeric or string type but found struct", lineno);
            return 1;
            break;
        case sliceK:
            reportError("TYPE", "expected numeric or string type but found slice", lineno);
            return 1;
            break;
        case arrayK:
            reportError("TYPE", "expected numeric or string type but found array", lineno);
            return 1;
            break;
    }

}

/*
 * a redeclaration is valid only if the symbol for the previous declaration has type
 * varDeclSym, varSym, or shortDeclSym
 */
int assertCanAssign(SYMBOL* prevSym, int lineno) {
    if ((prevSym->type != varSym) && (prevSym->type != varDeclSym) && (prevSym->type != shortDeclSym)) {
        reportStrError("TYPE", "cannot assign to %s", prevSym->name, lineno);
    }
}

/*
 * asserts that all exressions of a switch case have type t
 */
void assertCaseEXPsHaveType(EXP* exps, TYPE* t) {
    if (t == NULL) {
        reportError("TYPE", "expected non-void type but found void type", lineno);
        return;
    }
    if (exps == NULL) return;
    assertEqualTYPEs(t, exps->type);
    assertCaseEXPsHaveType(exps->next, t);
}

void assertEXPsResolveToBaseType(EXP* e) {
    if (e == NULL) return;
    assertResolvesToBaseType(e->type);
    assertEXPsResolveToBaseType(e->next);
}

/*
 * asserts that a type resolves to int, float64, rune, bool, or string
 */
void assertResolvesToBaseType(TYPE* t, int lineno) {
    if (t == NULL) {
        reportError("TYPE", "expected base type but found void type", lineno);
        return;
    }
    switch(t->kind) {
        case intK:
            break;
        case float64K:
            break;
        case runeK:
            break;
        case boolK:
            break;
        case stringK:
            break;
        case idK:
            // check the underlying type!
            assertResolvesToBaseType(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "expected base type but found struct", lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected base type but found slice", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected base type but found array", lineno);
            break;
    }
}

void assertResolvesToBool(TYPE* t, int lineno) {
    if (t == NULL) {
        reportError("TYPE", "expected type bool but found void type", lineno);
        return;
    }
    switch(t->kind) {
        case intK:
            reportError("TYPE", "expected type bool but found int", lineno);
            break;
        case float64K:
            reportError("TYPE", "expected type bool but found float64", lineno);
            break;
        case runeK:
            reportError("TYPE", "expected type bool but found rune", lineno);
            break;
        case boolK:
            break;
        case stringK:
            reportError("TYPE", "expected type bool but found string", lineno);
            break;
        case idK:
            // check the underlying type!
            assertResolvesToBool(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "expected type bool but found struct", lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected type bool but found slice", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected type bool but found array", lineno);
            break;
    }
}

/*
 * checks that a type resolves to either int or rune (since rune is just an alias for int!)
 */
int assertResolvesToInt(TYPE* t, int lineno) {
    if (t == NULL) {
        reportError("TYPE", "expected type int but found void type", lineno);
        return 1;
    }
    switch(t->kind) {
        case intK:
            return 0;
            break;
        case float64K:
            reportError("TYPE", "expected type int but found float64", lineno);
            return 1;
            break;
        case runeK:
            return 0;
            break;
        case boolK:
            reportError("TYPE", "expected type int but found bool", lineno);
            return 1;
            break;
        case stringK:
            reportError("TYPE", "expected type int but found string", lineno);
            return 1;
            break;
        case idK:
            // check the underlying type!
            return assertResolvesToInt(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "expected type int but found struct", lineno);
            return 1;
            break;
        case sliceK:
            reportError("TYPE", "expected type int but found slice", lineno);
            return 1;
            break;
        case arrayK:
            reportError("TYPE", "expected type int but found array", lineno);
            return 1;
            break;
    }
}

void assertCastResolution(EXP* e, int lineno) {
    if (t == NULL) {
        reportError("TYPE", "cannot cast to void type", lineno);
        return 1;
    }
    switch(t->kind) {
        case intK:
            break;
        case float64K:
            break;
        case runeK:
            break;
        case boolK:
            break;
        case stringK:
            reportError("TYPE", "cannot cast to string", lineno);
            break;
        case idK:
            // check the underlying type!
            assertCastResolution(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "cannot cast to struct", lineno);
            break;
        case sliceK:
            reportError("TYPE", "cannot cast to slice", lineno);
            break;
        case arrayK:
            reportError("TYPE", "cannot cast to array", lineno);
            break;
    }
}

/*
 * checks that opKind accepts types left and right and returns a value of type left
 */
void assertValidOpUsage(OperationKind opKind, TYPE* left, TYPE* right) {
    switch (opKind) {
        case plusEqOp:
            break;
        case minusEqOp:
            break;
        case timesEqOp:
            break;
        case divEqOp:
            break;
        case modEqOp:
            break;
        case andEqOp:
            break;
        case orEqOp:
            break;
        case xorEqOp:
            break;
        case leftShiftEqOp:
            break;
        case rightShiftEqOp:
            break;
        case bitClearEqOp:
            break;
        default:
            break;
    }
}
