#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "tree.h"
#include "memory.h"
#include "error.h"
#include "symbol.h" // only for Hash and notBlank

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
            typeEXP(v->val.expVD.exp, NULL, v);
            if (v->val.expVD.exp->type == NULL) {
                // the expression has no type (it is probably a function call to a void function)
                reportStrError("TYPE", "cannot assign a value with no type to %s", v->id->name, v->lineno);
            }
            if (!v->isBlank) {
                v->val.expVD.symbol->val.varDeclS.type = v->val.expVD.exp->type;
            }
            break;
        case typeAndExpK:
            // this typechecks as long as the expression type checks and is equal to the type of the variable
            // type check the expression
            typeEXP(v->val.typeAndExpVD.exp, NULL, v);
            // check that the type of the expression is equal to the type of the variable
            assertIdenticalTYPEs(v->val.typeAndExpVD.type, v->val.typeAndExpVD.exp->type, v->lineno);
        default:
            break;
    }
    typeVARDECLARATIONlist(v->next);
}

/*
 * a function declaration type checks if its statements type check
 * additionally, there should be a well-typed return statement on every execution path
 * if the function has a return value --> perhaps add this as a second pass through the statements of the function
 */
void typeFUNCTIONDECLARATION(FUNCTIONDECLARATION* f) {
    typeSTATEMENT(f->statements);
    if (f->returnType != NULL) {
        // make sure every execution path has a return statement
        assertReturnOnEveryPath(f->statements, f->returnType, f->id->name, f->lineno);
    }
    // make sure the type of any return statement is identical to the return type of the function
    assertNoInvalidReturns(f->statements, f->returnType, f->id->name);
}

void typeSTATEMENT(STATEMENT* s) {
    if (s == NULL) return;
    TYPE* t;
    switch (s->kind) {
        case emptyK:
            // trivially well-typed
            break;
        case expK:
            // type check the expression
            typeEXP(s->val.expS, s, NULL);
            // check the kind of the expression --> if it is now a cast, then report an error! casts are not
            // allowed as expression statements
            if (s->val.expS->kind == castK) {
                reportError("TYPE", "a cast is not a valid expression statement", s->lineno);
            }
            break;
        case incK:
            // type check the expression
            typeEXP(s->val.incS, s, NULL);
            // check that it is an type
            assertNumeric(s->val.incS->type, s->lineno);
            break;
        case decK:
            typeEXP(s->val.decS, s, NULL);
            assertNumeric(s->val.decS->type, s->lineno);
            break;
        case regAssignK:
            // type checks if the left and right exps are both well-typed and have the same type
            if (!s->val.regAssignS.isBlank) {
                typeEXP(s->val.regAssignS.lvalue, s, NULL);
                typeEXP(s->val.regAssignS.exp, s, NULL);
                // assert that the types are equal
                assertIdenticalTYPEs(s->val.regAssignS.lvalue->type, s->val.regAssignS.exp->type, s->lineno);
            } else {
                // just type the exp and make sure it is not void
                typeEXP(s->val.regAssignS.exp, s, NULL);
                if (s->val.regAssignS.exp->type == NULL) {
                    // the expression has no type (it is probably a function call to a void function)
                    reportError("TYPE", "cannot assign a value with no type to _", s->lineno);
                }
            }
            // type check the next assignment
            typeSTATEMENT(s->val.regAssignS.next);
            break;
        case binOpAssignK:
            // type checks if the expressions type check and if the operator accepts two
            // expressions of type(lvalue) and type(exp) and returns a value of type(lvalue)
            typeEXP(s->val.binOpAssignS.lvalue, s, NULL);
            typeEXP(s->val.binOpAssignS.exp, s, NULL);
            assertValidOpUsage(s->val.binOpAssignS.opKind, s->val.binOpAssignS.lvalue->type, s->val.binOpAssignS.exp->type, s->lineno);
            break;
        case shortDeclK:
            // type checks if the exp type checks
            typeEXP(s->val.shortDeclS.exp, s, NULL);
            if (s->val.shortDeclS.exp->type == NULL) {
                // the expression has no type (it is probably a function call to a function with no return value)
                reportStrError("TYPE", "cannot assign a value with no type to %s", s->val.shortDeclS.id->val.idE.id->name, s->lineno);
            }
            if (!s->val.shortDeclS.isBlank) {
                // if this is a redeclaration, we need to check that the type of the
                // expression is the same as the type of the prevDeclSym
                if (s->val.shortDeclS.isRedecl) {
                    // first, we better check that we can assign to this symbol (i.e. it is a variable!)
                    assertCanAssign(s->val.shortDeclS.prevDeclSym, s->lineno);
                    // then, we assert that the type of the expression is equal to the type of the variable
                    switch (s->val.shortDeclS.prevDeclSym->kind) {
                        case varSym:
                            t = s->val.shortDeclS.prevDeclSym->val.varS;
                            break;
                        case varDeclSym:
                            t = s->val.shortDeclS.prevDeclSym->val.varDeclS.type;
                            break;
                        case shortDeclSym:
                            t = s->val.shortDeclS.prevDeclSym->val.shortDeclS.type;
                            break;
                        default:
                            // will never execute
                            break;
                    }
                    assertIdenticalTYPEs(t, s->val.shortDeclS.exp->type, s->lineno);
                } else {
                    // we need to set the type on the symbol to be the type of the expression
                    s->val.shortDeclS.symbol->val.shortDeclS.type = s->val.shortDeclS.exp->type;
                }
            }
            // type check the next short var decl
            typeSTATEMENT(s->val.shortDeclS.next);
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
            typeEXPs(s->val.printS, s, NULL);
            assertEXPsResolveToBaseType(s->val.printS, s->lineno);
            break;
        case printlnK:
            // type checks if all the expressions type check AND they all resolve to a base type
            typeEXPs(s->val.printlnS, s, NULL);
            assertEXPsResolveToBaseType(s->val.printlnS, s->lineno);
            break;
        case returnK:
            // well-typed if the expression is well-typed
            // the expression should also have the same type as the return of the function but we
            // check this in a second pass
            typeEXP(s->val.returnS, s, NULL);
            break;
        case ifK:
            // type checks if the init statement type checks
            typeSTATEMENT(s->val.ifS.initStatement);
            // and the expression is well-typed and resolves to a bool
            typeEXP(s->val.ifS.condition, s, NULL);
            assertResolvesToBool(s->val.ifS.condition->type, s->lineno);
            // and the statements in the body type check
            typeSTATEMENT(s->val.ifS.body);
            break;
        case ifElseK:
            typeSTATEMENT(s->val.ifElseS.initStatement);
            typeEXP(s->val.ifElseS.condition, s, NULL);
            assertResolvesToBool(s->val.ifElseS.condition->type, s->lineno);
            typeSTATEMENT(s->val.ifElseS.thenPart);
            typeSTATEMENT(s->val.ifElseS.elsePart);
            break;
        case switchK:
            typeSTATEMENTswitch(s);
            break;
        case whileK:
            // type checks if condition type checks and resolves to bool
            typeEXP(s->val.whileS.condition, s, NULL);
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
            typeEXP(s->val.forS.condition, s, NULL);
            if (s->val.forS.condition != NULL) {
                assertResolvesToBool(s->val.forS.condition->type, s->lineno);
            }
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
        typeEXP(s->val.switchS.condition, s, NULL);
        if (s->val.switchS.condition->type == NULL) {
            reportError("TYPE", "cannot switch on a void value", s->lineno);
        }
        typeSWITCHCASE(s, s->val.switchS.cases, s->val.switchS.condition->type);
    } else {
        typeSWITCHCASE(s, s->val.switchS.cases, boolTYPE);
    }
}

/*
 * A switch case type checks if
 *  all its expressions have type t
 *  all its statements type check
 */
void typeSWITCHCASE(STATEMENT* s, SWITCHCASE* sc, TYPE* t) {
    if (sc == NULL) return;
    switch(sc->kind) {
        case caseK:
            typeEXPs(sc->val.caseC.exps, s, NULL);
            assertCaseEXPsHaveType(sc->val.caseC.exps, t);
            typeSTATEMENT(sc->val.caseC.statements);
            break;
        case defaultK:
            typeSTATEMENT(sc->val.defaultStatementsC);
            break;
    }
    typeSWITCHCASE(s, sc->next, t);
}

void typeEXPs(EXP* e, STATEMENT* s, VARDECLARATION* v) {
    if (e == NULL) return;
    typeEXP(e, s, v);
    typeEXPs(e->next, s, v);
}

/*
 * Takes a reference to the statement or variable declaration in which this expression occurs,
 * in case this expression is an index into an array
 */
void typeEXP(EXP* e, STATEMENT* s, VARDECLARATION* v) {
    if (e == NULL) {
        // needed in case of a return statement with no return expression
        return;
    }
    CASTCHECKRETURN* ctr;
    STRUCTTYPE* structType;
    SYMBOL* symbol;
    TYPE* type;
    ARRAYINDEX* arrayIndex;
    switch (e->kind) {
        case identifierK:
            e->type = getSymbolType(e->val.idE.symbol->name, e->val.idE.symbol, e->lineno);
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
            typeEXP(e->val.plusE.left, s, v);
            typeEXP(e->val.plusE.right, s, v);
            e->type = typePlus(e->val.plusE.left->type, e->val.plusE.right->type, e->lineno);
            if (resolvesToString(e->val.plusE.left->type) && resolvesToString(e->val.plusE.right->type)) {
                e->val.plusE.stringAddition = 1;
            }
            break;
        case minusK:
            typeEXP(e->val.minusE.left, s, v);
            typeEXP(e->val.minusE.right, s, v);
            e->type = numericOp(e->val.minusE.left->type, e->val.minusE.right->type, e->lineno);
            break;
        case timesK:
            typeEXP(e->val.timesE.left, s, v);
            typeEXP(e->val.timesE.right, s, v);
            e->type = numericOp(e->val.timesE.left->type, e->val.timesE.right->type, e->lineno);
            break;
        case divK:
            typeEXP(e->val.divE.left, s, v);
            typeEXP(e->val.divE.right, s, v);
            e->type = numericOp(e->val.divE.left->type, e->val.divE.right->type, e->lineno);
            break;
        case modK:
            typeEXP(e->val.modE.left, s, v);
            typeEXP(e->val.modE.right, s, v);
            e->type = intOp(e->val.modE.left->type, e->val.modE.right->type, e->lineno);
            break;
        case bitwiseOrK:
            typeEXP(e->val.bitwiseOrE.left, s, v);
            typeEXP(e->val.bitwiseOrE.right, s, v);
            e->type = intOp(e->val.bitwiseOrE.left->type, e->val.bitwiseOrE.right->type, e->lineno);
            break;
        case bitwiseAndK:
            typeEXP(e->val.bitwiseAndE.left, s, v);
            typeEXP(e->val.bitwiseAndE.right, s, v);
            e->type = intOp(e->val.bitwiseAndE.left->type, e->val.bitwiseAndE.right->type, e->lineno);
            break;
        case xorK:
            typeEXP(e->val.xorE.left, s, v);
            typeEXP(e->val.xorE.right, s, v);
            e->type = intOp(e->val.xorE.left->type, e->val.xorE.right->type, e->lineno);
            break;
        case ltK:
            typeEXP(e->val.ltE.left, s, v);
            typeEXP(e->val.ltE.right, s, v);
            // we need to check that both types are ordered and equal
            checkOrderedAndEqual(e->val.ltE.left->type, e->val.ltE.right->type, e->lineno);
            e->type = boolTYPE;
            // if both strings, then we deal with comparison differently during codegen
            // so we set the following flag
            if (resolvesToString(e->val.ltE.left->type) && resolvesToString(e->val.ltE.right->type)) {
                e->val.ltE.stringCompare = 1;
            }
            break;
        case gtK:
            typeEXP(e->val.gtE.left, s, v);
            typeEXP(e->val.gtE.right, s, v);
            checkOrderedAndEqual(e->val.gtE.left->type, e->val.gtE.right->type, e->lineno);
            e->type = boolTYPE;
            if (resolvesToString(e->val.gtE.left->type) && resolvesToString(e->val.gtE.right->type)) {
                e->val.gtE.stringCompare = 1;
            }
            break;
        case eqK:
            typeEXP(e->val.eqE.left, s, v);
            typeEXP(e->val.eqE.right, s, v);
            // all values that we support in golite are comparable, so all we have to do
            // is check that the types are equal
            assertIdenticalTYPEs(e->val.eqE.left->type, e->val.eqE.right->type, e->lineno);
            e->type = boolTYPE;
            if (resolvesToString(e->val.eqE.left->type) && resolvesToString(e->val.eqE.right->type)) {
                e->val.eqE.stringCompare = 1;
            }
            break;
        case neqK:
            typeEXP(e->val.neqE.left, s, v);
            typeEXP(e->val.neqE.right, s, v);
            assertIdenticalTYPEs(e->val.neqE.left->type, e->val.neqE.right->type, e->lineno);
            e->type = boolTYPE;
            if (resolvesToString(e->val.neqE.left->type) && resolvesToString(e->val.neqE.right->type)) {
                e->val.neqE.stringCompare = 1;
            }
            break;
        case leqK:
            typeEXP(e->val.leqE.left, s, v);
            typeEXP(e->val.leqE.right, s, v);
            checkOrderedAndEqual(e->val.leqE.left->type, e->val.leqE.right->type, e->lineno);
            e->type = boolTYPE;
            if (resolvesToString(e->val.leqE.left->type) && resolvesToString(e->val.leqE.right->type)) {
                e->val.leqE.stringCompare = 1;
            }
            break;
        case geqK:
            typeEXP(e->val.geqE.left, s, v);
            typeEXP(e->val.geqE.right, s, v);
            checkOrderedAndEqual(e->val.geqE.left->type, e->val.geqE.right->type, e->lineno);
            e->type = boolTYPE;
            if (resolvesToString(e->val.geqE.left->type) && resolvesToString(e->val.geqE.right->type)) {
                e->val.geqE.stringCompare = 1;
            }
            break;
        case orK:
            typeEXP(e->val.orE.left, s, v);
            typeEXP(e->val.orE.right, s, v);
            e->type = boolOp(e->val.orE.left->type, e->val.orE.right->type, e->lineno);
            break;
        case andK:
            typeEXP(e->val.andE.left, s, v);
            typeEXP(e->val.andE.right, s, v);
            e->type = boolOp(e->val.andE.left->type, e->val.andE.right->type, e->lineno);
            break;
        case leftShiftK:
            typeEXP(e->val.leftShiftE.left, s, v);
            typeEXP(e->val.leftShiftE.right, s, v);
            e->type = intOp(e->val.leftShiftE.left->type, e->val.leftShiftE.right->type, e->lineno);
            break;
        case rightShiftK:
            typeEXP(e->val.rightShiftE.left, s, v);
            typeEXP(e->val.rightShiftE.right, s, v);
            e->type = intOp(e->val.rightShiftE.left->type, e->val.rightShiftE.right->type, e->lineno);
            break;
        case bitClearK:
            typeEXP(e->val.bitClearE.left, s, v);
            typeEXP(e->val.bitClearE.right, s, v);
            e->type = intOp(e->val.bitClearE.left->type, e->val.bitClearE.right->type, e->lineno);
            break;
        case appendK:
            typeEXP(e->val.appendE.slice, s, v);
            typeEXP(e->val.appendE.expToAppend, s, v);
            // the slice exp needs to have type S which resolves to a slice with elements of type T
            // and the expToAppend needs to have type T
            checkAppendIsValid(e->val.appendE.slice->type, e->val.appendE.expToAppend->type, e->lineno);
            e->type = e->val.appendE.slice->type;
            break;
        case castK:
            // this is well-typed if
            //  type resolves to int, float, bool, or rune
            //  exp is well-typed and has a type that can be cast to type
            assertCastResolution(e->val.castE->type, e->lineno);
            typeEXP(e->val.castE->exp, s, v);
            // basically, we just need to ensure that the type of the expression also resolves to
            // int, float, bool, or rune!
            assertCastResolution(e->val.castE->exp->type, e->lineno);
            e->type = e->val.castE->type;
            break;
        case selectorK:
            // type the receiving expression; it should end up having a type that resolves to structK
            typeRECEIVER(e->val.selectorE.receiver, s, v);
            // assert that the receiver resolves to a stuct literal
            structType = assertResolvesToStruct(e->val.selectorE.receiver->receivingStruct->type, e->lineno);
            // search the symbol table of the structType for the last selector
            symbol = getSymbolInSymbolTable(structType->symbolTable, e->val.selectorE.lastSelector->name, e->lineno);
            // the type of this expression is the type of the symbol
            // this symbol will definitely have kind fieldSym, so we can access its type directly
            e->type = symbol->val.fieldS.type;
            break;
        case indexK:
            // an index into an array or slice is well-typed if
            // index is well typed and resolves to int
            typeEXP(e->val.indexE.lastIndex, s, v);
            assertResolvesToInt(e->val.indexE.lastIndex->type, e->lineno);
            // rest is well typed
            typeEXP(e->val.indexE.rest, s, v);
            // getElementType checks that rest resolves to a slice or array
            // ==> non-intuitive side-effect - would be good to change this
            type = getElementType(e->val.indexE.rest->type, e->lineno);
            switch (type->kind) {
                case arrayK:
                    // indicate that the statement containing this expression involves an array index
                    arrayIndex = NEW(ARRAYINDEX);
                    arrayIndex->maxIndex = type->val.arrayT.size->val.intLiteralE.decValue;
                    arrayIndex->indexExp = e->val.indexE.lastIndex;
                    arrayIndex->next = NULL;
                    if (s != NULL) {
                        s->arrayIndex = appendARRAYINDEX(s->arrayIndex, arrayIndex);
                    } else {
                        v->arrayIndex = appendARRAYINDEX(v->arrayIndex, arrayIndex);
                    }
                    e->type = type->val.arrayT.elementType;
                    break;
                case sliceK:
                    e->type = type->val.sliceT;
                    break;
                default:
                    // will never hit this
                    break;
            }
            break;
        case argumentsK:
            // here, we can FINALLY check whether or not this is actually supposed to be a cast
            ctr = checkCast(e);
            switch (ctr->kind) {
                case castKind:
                    prepTypeEXPcast(e, ctr->val.cast.type, ctr->val.cast.name, s, v);
                    break;
                case functionCallKind:
                    typeEXPfunctioncall(e, ctr->val.functionDecl, s, v);
                    break;
            }
            break;
        case uPlusK:
            typeEXP(e->val.uPlusE, s, v);
            assertNumeric(e->val.uPlusE->type, e->lineno);
            e->type = e->val.uPlusE->type;
            break;
        case uMinusK:
            typeEXP(e->val.uMinusE, s, v);
            assertNumeric(e->val.uMinusE->type, e->lineno);
            e->type = e->val.uMinusE->type;
            break;
        case uNotK:
            typeEXP(e->val.uNotE, s, v);
            assertResolvesToBool(e->val.uNotE->type, e->lineno);
            e->type = e->val.uNotE->type;
            break;
        case uXorK:
            typeEXP(e->val.uXorE, s, v);
            assertResolvesToInt(e->val.uXorE->type, e->lineno);
            e->type = e->val.uXorE->type;
            break;
        default:
            break;
    }
}

ARRAYINDEX* appendARRAYINDEX(ARRAYINDEX* prevs, ARRAYINDEX* curr) {
    ARRAYINDEX* t;
    if (prevs == NULL) return curr;
    t = prevs;
    while (t->next != NULL) t = t->next;
    t->next = curr;
    return prevs;
}

/*
 * A receiver is well-typed if its receiving expression is well-typed
 */
void typeRECEIVER(RECEIVER* r, STATEMENT* s, VARDECLARATION* v) {
    typeEXP(r->receivingStruct, s, v);
}

/*
 * types the expression as a cast if it indeed is one
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
        ctr->val.cast.type = t;
        ctr->val.cast.name = s->name;
    }

    return ctr;
}

void prepTypeEXPcast(EXP* e, TYPE* t, char* name, STATEMENT* s, VARDECLARATION* v) {
    // firstly, we need to check that there is only one argument to the cast
    int numArgs = countArgs(e->val.argumentsE.args);
    if (numArgs == 0) {
        reportStrError("TYPE", "missing argument to conversion to %s", name, e->lineno);
    } else if (numArgs > 1) {
        reportStrError("TYPE", "too many arguments to conversion to %s", name, e->lineno);
    }
    terminateIfErrors();

    // the cast is valid
    // repackage e as a cast expression, and then we re-type-check it :)
    e->kind = castK;
    CAST* c = makeCAST(t, e->val.argumentsE.args);
    e->val.castE = c;

    // re-type this as a cast :)
    typeEXP(e, s, v);
}

/*
 * if this function is called, we've already checked that the function name does indeed
 * identify a function, and we have passed the corresponding function declaration in
 * so, all we need to do is check that the arguments are well typed and have the
 * same types as the parameters the function accepts
 */
void typeEXPfunctioncall(EXP* e, FUNCTIONDECLARATION* fd, STATEMENT* s, VARDECLARATION* v) {
    // check that the arguments are well-typed
    typeEXPs(e->val.argumentsE.args, s, v);
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
    assertIdenticalTYPEs(currParam->type, args->type, args->lineno);
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

// function-specific

int countArgs(EXP* e) {
    if (e == NULL) return 0;
    return 1 + countArgs(e->next);
}

void assertReturnOnEveryPath(STATEMENT* s, TYPE* returnType, char* name, int lineno) {
    if (!hasReturnOnEveryPath(s, returnType, name)) {
        // error
        reportStrError("TYPE", "missing return at end of function: %s", name, lineno);
    }
}

/*
 * returns 1 if a series of statements has a return along every path through the statements
 * and 0 otherwise
 */
int hasReturnOnEveryPath(STATEMENT* s, TYPE* returnType, char* name) {
    // firstly, we sheck the outermost scope of statements for a return statement
    // all execution paths will reach such a return statement if they don't reach one before,
    // so if there is such a return statement we are done
    if (!hasOuterReturn(s, returnType, name)) {
        // we have to check that at least one of the if/else statements and switch
        // statements have returns along every path
        return hasInnerReturn(s, returnType, name);
    }
    return 1;
}

/*
 * check a series of statements for a return statement and ensure that it has the correct type
 * do not search any blocks for return statements TODO <-- maybe we should look in blockS statements?
 * even if we find a return statement, continue the search to look for additional return statements
 * to make sure they have the correct type
 */
int hasOuterReturn(STATEMENT* s, TYPE* returnType, char* name) {
    int outerReturn = 0;
    while (s != NULL) {
        if (s->kind == returnK) {
            // check that the return type is correct
            if (s->val.returnS == NULL) {
                handleVoidReturnError(returnType, name, s->lineno);
            }
            assertIdenticalTYPEs(returnType, s->val.returnS->type, s->lineno);
            outerReturn = 1;
        }
        s = s->next;
    }
    return outerReturn;
}

/*
 * ensures that at least one if/else statement, switch statement, or infinite loop statement
 * within the set of statements has a return along every path,
 * and that this return is of the correct type
 */
int hasInnerReturn(STATEMENT* s, TYPE* returnType, char* name) {
    int innerReturn = 0;
    while (s != NULL) {
        switch (s->kind) {
            case ifElseK:
                // check that both the thenPart and the elsePart have returns on every path
                if (hasReturnOnEveryPath(s->val.ifElseS.thenPart, returnType, name) && hasReturnOnEveryPath(s->val.ifElseS.elsePart, returnType, name)) {
                    innerReturn = 1;
                }
                break;
            case switchK:
                // check that each case has a return and that there is a default case
                if (switchHasReturn(s->val.switchS.cases, returnType, name)) {
                    innerReturn = 1;
                }
                break;
            case forK:
                // if this loop is obviously infinite (no condition), then we can check its statements for returns along every path
                if (s->val.forS.condition == NULL) {
                    if (hasReturnOnEveryPath(s->val.forS.body, returnType, name)) {
                        innerReturn = 1;
                    }
                }
                break;
            case infiniteLoopK:
                // check that the loop body has a return on every path
                if (hasReturnOnEveryPath(s->val.infiniteLoopS, returnType, name)) {
                    innerReturn = 1;
                }
                break;
            case blockK:
                // check that the block has a return on every path
                if (hasReturnOnEveryPath(s->val.blockS, returnType, name)) {
                    innerReturn = 1;
                }
                break;
            default:
                break;
        }
        s = s->next;
    }
    return innerReturn;
}

/*
 * returns 1 if each case in the switch case has a return and there is a default case
 * with a return, and 0 otherwise
 */
int switchHasReturn(SWITCHCASE* sc, TYPE* returnType, char* name) {
    int defaultCovered = 0;
    int casesCovered = 1; // innocent until proven guilty

    while (sc != NULL) {
        switch (sc->kind) {
            case caseK:
                if (casesCovered) {
                    // check another case
                    casesCovered = hasReturnOnEveryPath(sc->val.caseC.statements, returnType, name);
                } else {
                    // regardless, check the next case for improperly typed returns, but casesCovered
                    // cannot be changed from zero
                    hasReturnOnEveryPath(sc->val.caseC.statements, returnType, name);
                }
                break;
            case defaultK:
                defaultCovered = hasReturnOnEveryPath(sc->val.defaultStatementsC, returnType, name);
                break;
        }
        sc = sc->next;
    }
    return defaultCovered && casesCovered;
}

void assertNoInvalidReturns(STATEMENT* s, TYPE* expected, char* name) {
    if (s == NULL) return;
    switch (s->kind) {
        case returnK:
            if (expected == NULL) {
                // expect a void return
                if (s->val.returnS != NULL) {
                    handleNonVoidReturnError(s->val.returnS->type, name, s->lineno);
                }
            } else {
                // expect a non-void return
                if (s->val.returnS == NULL) {
                    handleVoidReturnError(expected, name, s->lineno);
                } else {
                    assertIdenticalTYPEs(expected, s->val.returnS->type, s->lineno);
                }
            }
            break;
        case ifK:
            assertNoInvalidReturns(s->val.ifS.body, expected, name);
            break;
        case ifElseK:
            assertNoInvalidReturns(s->val.ifElseS.thenPart, expected, name);
            assertNoInvalidReturns(s->val.ifElseS.elsePart, expected, name);
            break;
        case switchK:
            assertNoInvalidReturnsSWITCHCASE(s->val.switchS.cases, expected, name);
            break;
        case whileK:
            assertNoInvalidReturns(s->val.whileS.body, expected, name);
            break;
        case infiniteLoopK:
            assertNoInvalidReturns(s->val.infiniteLoopS, expected, name);
            break;
        case forK:
            assertNoInvalidReturns(s->val.forS.body, expected, name);
            break;
        case blockK:
            assertNoInvalidReturns(s->val.blockS, expected, name);
            break;
        default:
            break;
    }
    assertNoInvalidReturns(s->next, expected, name);
}

void handleNonVoidReturnError(TYPE* actual, char* funcName, int lineno) {
    switch (actual->kind) {
        case intK:
            reportStrError("TYPE", "%s has void return type, but found int", funcName, lineno);
            break;
        case float64K:
            reportStrError("TYPE", "%s has void return type, but found float64", funcName, lineno);
            break;
        case runeK:
            reportStrError("TYPE", "%s has void return type, but found rune", funcName, lineno);
            break;
        case boolK:
            reportStrError("TYPE", "%s has void return type, but found bool", funcName, lineno);
            break;
        case stringK:
            reportStrError("TYPE", "%s has void return type, but found string", funcName, lineno);
            break;
        case idK:
            reportDoubleStrError("TYPE", "%s has void return type, but found %s", funcName, actual->val.idT.id->name, lineno);
            break;
        case structK:
            reportStrError("TYPE", "%s has void return type, but found struct", funcName, lineno);
            break;
        case sliceK:
            reportStrError("TYPE", "%s has void return type, but found slice", funcName, lineno);
            break;
        case arrayK:
            reportStrError("TYPE", "%s has void return type, but found array", funcName, lineno);
            break;
    }
}

void handleVoidReturnError(TYPE* expected, char* funcName, int lineno) {
    switch(expected->kind) {
        case idK:
            reportDoubleStrError("TYPE", "%s has %s return type, but found void", funcName, expected->val.idT.id->name, lineno);
            break;
        case structK:
            reportStrError("TYPE", "%s has struct return type, but found void", funcName, lineno);
            break;
        case sliceK:
            reportStrError("TYPE", "%s has slice return type, but found void", funcName, lineno);
            break;
        case arrayK:
            reportStrError("TYPE", "%s has array return type, but found void", funcName, lineno);
            break;
        case intK:
            reportStrError("TYPE", "%s has int return type, but found void", funcName, lineno);
            break;
        case float64K:
            reportStrError("TYPE", "%s has float64 return type, but found void", funcName, lineno);
            break;
        case runeK:
            reportStrError("TYPE", "%s has rune return type, but found void", funcName, lineno);
            break;
        case boolK:
            reportStrError("TYPE", "%s has bool return type, but found void", funcName, lineno);
            break;
        case stringK:
            reportStrError("TYPE", "%s has string return type, but found void", funcName, lineno);
            break;
    }
}

void assertNoInvalidReturnsSWITCHCASE(SWITCHCASE* sc, TYPE* expected, char* name) {
    if (sc == NULL) return;
    switch (sc->kind) {
        case caseK:
            assertNoInvalidReturns(sc->val.caseC.statements, expected, name);
            break;
        case defaultK:
            assertNoInvalidReturns(sc->val.defaultStatementsC, expected, name);
            break;
    }
    assertNoInvalidReturnsSWITCHCASE(sc->next, expected, name);
}

// operation-specific

/*
 * checks that opKind accepts types left and right and returns a value of type left
 */
void assertValidOpUsage(OperationKind opKind, TYPE* left, TYPE* right, int lineno) {
    switch (opKind) {
        case plusEqOp:
            // assert that both left and right are either strings or numeric and that they have the same type
            typePlus(left, right, lineno);
            break;
        case minusEqOp:
            // assert that both left and right are numeric and have the same type
            numericOp(left, right, lineno);
            break;
        case timesEqOp:
            // assert that both left and right are numeric and have the same type
            numericOp(left, right, lineno);
            break;
        case divEqOp:
            // assert that both left and right are numeric and have the same type
            numericOp(left, right, lineno);
            break;
        case modEqOp:
            // assert that both left and right are numeric and have the same type
            numericOp(left, right, lineno);
            break;
        case andEqOp:
            // assert that both left and right are the same type and both resolve to int
            intOp(left, right, lineno);
            break;
        case orEqOp:
            // assert that both left and right are the same type and both resolve to int
            intOp(left, right, lineno);
            break;
        case xorEqOp:
            // assert that both left and right are the same type and both resolve to int
            intOp(left, right, lineno);
            break;
        case leftShiftEqOp:
            // assert that both left and right are the same type and both resolve to int
            intOp(left, right, lineno);
            break;
        case rightShiftEqOp:
            // assert that both left and right are the same type and both resolve to int
            intOp(left, right, lineno);
            break;
        case bitClearEqOp:
            // assert that both left and right are the same type and both resolve to int
            intOp(left, right, lineno);
            break;
    }
}

// numeric-specific

TYPE* typePlus(TYPE* left, TYPE* right, int lineno) {
    // assert that the types are the same
    assertIdenticalTYPEs(left, right, lineno);
    // assert that they are both numeric types or strings
    assertNumericOrString(left, lineno);
    assertNumericOrString(right, lineno);
    return left;
}

/*
 * reports an error if a type is not numeric (int, float64, rune, or alias to one of the three)
 */
void assertNumeric(TYPE* t, int lineno) {
    switch(t->kind) {
        case intK:
            break;
        case float64K:
            break;
        case runeK:
            break;
        case boolK:
            reportError("TYPE", "expected numeric type but found bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected numeric type but found string", lineno);
            break;
        case idK:
            // check the underlying type!
            assertNumeric(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "expected numeric type but found struct", lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected numeric type but found slice", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected numeric type but found array", lineno);
            break;
    }
}

void assertNumericOrString(TYPE* t, int lineno) {
    switch(t->kind) {
        case intK:
            break;
        case float64K:
            break;
        case runeK:
            break;
        case boolK:
            reportError("TYPE", "expected numeric or string type but found bool", lineno);
            break;
        case stringK:
            break;
        case idK:
            // check the underlying type!
            assertNumericOrString(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "expected numeric or string type but found struct", lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected numeric or string type but found slice", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected numeric or string type but found array", lineno);
            break;
    }
}

/*
 * checks that a type resolves to either int or rune (since rune is just an alias for int!)
 */
void assertResolvesToInt(TYPE* t, int lineno) {
    switch(t->kind) {
        case intK:
            break;
        case float64K:
            reportError("TYPE", "expected type int but found float64", lineno);
            break;
        case runeK:
            break;
        case boolK:
            reportError("TYPE", "expected type int but found bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected type int but found string", lineno);
            break;
        case idK:
            // check the underlying type!
            assertResolvesToInt(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "expected type int but found struct", lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected type int but found slice", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected type int but found array", lineno);
            break;
    }
}

void assertActualTypeInt(TYPE* actual, int lineno) {
    switch (actual->kind) {
        case intK:
            break;
        case idK:
            reportStrError("TYPE", "expected type int but found type %s", actual->val.idT.id->name, lineno);
            break;
        case structK:
            reportError("TYPE", "expected type int but found struct", lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected type int but found slice", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected type int but found array", lineno);
            break;
        case float64K:
            reportError("TYPE", "expected type int but found float64", lineno);
            break;
        case runeK:
            reportError("TYPE", "expected type int but found rune", lineno);
            break;
        case boolK:
            reportError("TYPE", "expected type int but found bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected type int but found string", lineno);
            break;
    }
}

void assertActualTypeFloat64(TYPE* actual, int lineno) {
    switch (actual->kind) {
        case intK:
            reportError("TYPE", "expected type float64 but found type int", lineno);
            break;
        case idK:
            reportStrError("TYPE", "expected type float64 but found type %s", actual->val.idT.id->name, lineno);
            break;
        case structK:
            reportError("TYPE", "expected type float64 but found struct type", lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected type float64 but found slice type", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected type float64 but found array type", lineno);
            break;
        case float64K:
            break;
        case runeK:
            reportError("TYPE", "expected type float64 but found type rune", lineno);
            break;
        case boolK:
            reportError("TYPE", "expected type float64 but found type bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected type float64 but found type string", lineno);
            break;
    }
}

/*
 * assert that both left and right expressions of an operation are numeric and have the same type
 */
TYPE* numericOp(TYPE* left, TYPE* right, int lineno) {
    // assert that the types are the same
    assertIdenticalTYPEs(left, right, lineno);
    // assert that they are both numeric types
    assertNumeric(left, lineno);
    assertNumeric(right, lineno);
    return left;
}

TYPE* intOp(TYPE* left, TYPE* right, int lineno) {
    // assert that both types are the same
    assertIdenticalTYPEs(left, right, lineno);
    // assert that both resolve to ints
    assertResolvesToInt(left, lineno);
    assertResolvesToInt(right, lineno);
    return left;
}

// rune-specific

void assertActualTypeRune(TYPE* actual, int lineno) {
    switch (actual->kind) {
        case runeK:
            break;
        case intK:
            reportError("TYPE", "expected type rune but found type int", lineno);
            break;
        case idK:
            reportStrError("TYPE", "expected type float64 but found type %s", actual->val.idT.id->name, lineno);
            break;
        case structK:
            reportError("TYPE", "expected type rune but found struct type", lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected type rune but found slice type", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected type rune but found array type", lineno);
            break;
        case float64K:
            reportError("TYPE", "expected type rune but found type float64", lineno);
            break;
        case boolK:
            reportError("TYPE", "expected type rune but found type bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected type rune but found type string", lineno);
            break;
    }
}

// string-specific

/*
 * returns true if the type resolves to string and false otherwise
 */
int resolvesToString(TYPE* t) {
    switch (t->kind) {
        case idK:
            return resolvesToString(t->val.idT.underlyingType);
            break;
        case stringK:
            return 1;
            break;
        default:
            return 0;
            break;
    }
}

void assertActualTypeString(TYPE* actual, int lineno) {
    switch (actual->kind) {
        case intK:
            reportError("TYPE", "expected type string but found type int", lineno);
            break;
        case idK:
            reportStrError("TYPE", "expected type string but found type %s", actual->val.idT.id->name, lineno);
            break;
        case structK:
            reportError("TYPE", "expected type string but found struct type", lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected type string but found slice type", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected type string but found array type", lineno);
            break;
        case float64K:
            reportError("TYPE", "expected type string but found type float64", lineno);
            break;
        case runeK:
            reportError("TYPE", "expected type string but found type rune", lineno);
            break;
        case boolK:
            reportError("TYPE", "expected type string but found type bool", lineno);
            break;
        case stringK:
            break;
    }
}

// bool-specific

TYPE* boolOp(TYPE* left, TYPE* right, int lineno) {
    // assert that both types are the same
    assertIdenticalTYPEs(left, right, lineno);
    // assert that both resolve to bools
    assertResolvesToBool(left, lineno);
    assertResolvesToBool(right, lineno);
    return left;
}

void assertResolvesToBool(TYPE* t, int lineno) {
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

void assertActualTypeBool(TYPE* actual, int lineno) {
    switch (actual->kind) {
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
            reportStrError("TYPE", "expected type bool but found type %s", actual->val.idT.id->name, lineno);
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

// slice/array specific

/*
 * checks that:
 *  s resolves to a slice of type []T
 *  t has type T
 * if either of these things is not true, an error is reported and we stop compilation
 */
void checkAppendIsValid(TYPE* s, TYPE* t, int lineno) {
    TYPE* elementType = getSliceElementType(s, lineno);
    assertIdenticalTYPEs(elementType, t, lineno);
}

TYPE* getSliceElementType(TYPE* t, int lineno) {
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
 * ensures that t resolves to a slice or array, and returns the array or slice type if so
 */
TYPE* getElementType(TYPE* t, int lineno) {
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
            return t;
            break;
        case arrayK:
            return t;
            break;
    }
    return NULL;
}

void assertActualTypeArray(TYPE* expected, TYPE* actual, int lineno) {
    switch (actual->kind) {
        case intK:
            reportError("TYPE", "expected array but found int", lineno);
            break;
        case float64K:
            reportError("TYPE", "expected array but found float64", lineno);
            break;
        case runeK:
            reportError("TYPE", "expected array but found rune", lineno);
            break;
        case boolK:
            reportError("TYPE", "expected array but found bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected array but found string", lineno);
            break;
        case idK:
            reportStrError("TYPE", "expected array but found type %s", actual->val.idT.id->name, lineno);
            break;
        case structK:
            reportError("TYPE", "expected array but found struct", lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected array but found slice", lineno);
            break;
        case arrayK:
            assertIdenticalArrays(expected->val.arrayT.size, expected->val.arrayT.elementType, actual->val.arrayT.size, actual->val.arrayT.elementType, lineno);
            break;
    }
}

void assertIdenticalArrays(EXP* expectedSize, TYPE* expectedType, EXP* actualSize, TYPE* actualType, int lineno) {
    // compare the sizes
    if (expectedSize->val.intLiteralE.decValue != actualSize->val.intLiteralE.decValue) {
        reportError("TYPE", "non-identical array types", lineno);
        return;
    }

    // compare the types
    assertIdenticalTYPEs(expectedType, actualType, lineno);
}

void assertActualTypeSlice(TYPE* expectedElementType, TYPE* actual, int lineno) {
    switch (actual->kind) {
        case intK:
            reportError("TYPE", "expected type slice but found int", lineno);
            break;
        case float64K:
            reportError("TYPE", "expected type slice but found float64", lineno);
            break;
        case runeK:
            reportError("TYPE", "expected type slice but found rune", lineno);
            break;
        case boolK:
            reportError("TYPE", "expected type slice but found bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected type slice but found string", lineno);
            break;
        case idK:
            reportStrError("TYPE", "expected type slice but found type %s", actual->val.idT.id->name, lineno);
            break;
        case structK:
            reportError("TYPE", "expected type slice but found struct", lineno);
            break;
        case sliceK:
            assertIdenticalTYPEs(expectedElementType, actual->val.sliceT, lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected type slice but found array", lineno);
            break;
    }
}

// struct-specific

/*
 * checks that a type resolves to a struct literal
 */
STRUCTTYPE* assertResolvesToStruct(TYPE* t, int lineno) {
    switch(t->kind) {
        case intK:
            reportError("TYPE", "expected struct type but found int", lineno);
            break;
        case float64K:
            reportError("TYPE", "expected struct type but found float64", lineno);
            break;
        case runeK:
            reportError("TYPE", "expected struct type but found rune", lineno);
            break;
        case boolK:
            reportError("TYPE", "expected struct type but found bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected struct type but found string", lineno);
            break;
        case idK:
            // check the underlying type!
            return assertResolvesToStruct(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            return t->val.structT;
            break;
        case sliceK:
            reportError("TYPE", "expected struct type but found slice", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected struct type but found array", lineno);
            break;
    }
    return NULL;
}

void assertActualTypeStruct(STRUCTTYPE* expected, TYPE* actual, int lineno) {
    switch (actual->kind) {
        case intK:
            reportError("TYPE", "expected struct type but found int", lineno);
            break;
        case float64K:
            reportError("TYPE", "expected struct type but found float64", lineno);
            break;
        case runeK:
            reportError("TYPE", "expected struct type but found rune", lineno);
            break;
        case boolK:
            reportError("TYPE", "expected struct type but found bool", lineno);
            break;
        case stringK:
            reportError("TYPE", "expected struct type but found string", lineno);
            break;
        case idK:
            reportStrError("TYPE", "expected struct type but found type %s", actual->val.idT.id->name, lineno);
            break;
        case structK:
            assertIdenticalStructs(expected, actual->val.structT, lineno);
            break;
        case sliceK:
            reportError("TYPE", "expected struct type but found slice", lineno);
            break;
        case arrayK:
            reportError("TYPE", "expected struct type but found array", lineno);
            break;
    }
}

void assertIdenticalStructs(STRUCTTYPE* expected, STRUCTTYPE* actual, int lineno) {
    assertIdenticalFIELDs(expected->fields, expected->fields, actual->fields, actual->fields, lineno);
}

void assertIdenticalFIELDs(FIELD* currExpected, FIELD* expected, FIELD* currActual, FIELD* actual, int lineno) {
    if (currExpected == NULL && currActual == NULL) return;
    if (currExpected == NULL) {
        reportError("TYPE", "non-identical struct types", lineno);
        return;
    }
    if (currActual == NULL) {
        reportError("TYPE", "non-identical struct types", lineno);
        return;
    }

    // check that the names are the same
    if (strcmp(currExpected->id->name, currActual->id->name) != 0) {
        reportError("TYPE", "non-identical struct types", lineno);
        return;
    }
    // check that the types are identical
    assertIdenticalTYPEs(currExpected->type, currActual->type, lineno);

    // prepare for recursive case
    if (currExpected->nextId == NULL) {
        currExpected = expected->nextFieldSet;
        expected = expected->nextFieldSet;
    } else {
        currExpected = currExpected->nextId;
    }

    if (currActual->nextId == NULL) {
        currActual = actual->nextFieldSet;
        actual = actual->nextFieldSet;
    } else {
        currActual = currActual->nextId;
    }

    // recurse
    assertIdenticalFIELDs(currExpected, expected, currActual, actual, lineno);
}

// comparison/order-specific

void checkOrderedAndEqual(TYPE* left, TYPE* right, int lineno) {
    // assert that the types are equal
    assertIdenticalTYPEs(left, right, lineno);
    // assert that both types are ordered
    assertOrdered(left, lineno);
    assertOrdered(right, lineno);
}

/*
 * reports an error if the type cannot be ordered
 */
void assertOrdered(TYPE* t, int lineno) {
    switch(t->kind) {
        case intK:
            break;
        case float64K:
            break;
        case runeK:
            break;
        case boolK:
            reportError("TYPE", "type bool is not ordered", lineno);
            break;
        case stringK:
            break;
        case idK:
            // check the underlying type!
            assertOrdered(t->val.idT.underlyingType, lineno);
            break;
        case structK:
            reportError("TYPE", "type struct is not ordered", lineno);
            break;
        case sliceK:
            reportError("TYPE", "type slice is not ordered", lineno);
            break;
        case arrayK:
            reportError("TYPE", "type array is not ordered", lineno);
            break;
    }
}

// switch-case-specific

/*
 * asserts that all exressions of a switch case have type t
 */
void assertCaseEXPsHaveType(EXP* exps, TYPE* t) {
    if (exps == NULL) return;
    assertIdenticalTYPEs(t, exps->type, exps->lineno);
    assertCaseEXPsHaveType(exps->next, t);
}

// generic resolution

void assertEXPsResolveToBaseType(EXP* e, int lineno) {
    if (e == NULL) return;
    assertResolvesToBaseType(e->type, lineno);
    assertEXPsResolveToBaseType(e->next, lineno);
}

/*
 * asserts that a type resolves to int, float64, rune, bool, or string
 */
void assertResolvesToBaseType(TYPE* t, int lineno) {
    if (t == NULL) {
        reportError("TYPE", "cannot print a void value", lineno);
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

// cast resolution

void assertCastResolution(TYPE* t, int lineno) {
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

// symbol/symbol table specific

/*
 * Get a symbol in the given symbol table, if it exists
 */
SYMBOL* getSymbolInSymbolTable(SymbolTable* t, char *name, int lineno) {
    int i = Hash(name);
    SYMBOL *s;
    for (s = t->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    if (strcmp("_", name) == 0) {
        reportError("TYPE", "cannot refer to blank field", lineno);
    } else {
        reportStrError("TYPE", "no such field: %s", name, lineno);
    }
    return NULL;
}

/*
 * returns the type associated with a symbol
 */
TYPE* getSymbolType(char* name, SYMBOL *s, int lineno) {
    switch(s->kind) {
        case typeSym:
            reportStrError("TYPE", "%s is not a variable as expected", name, lineno);
            break;
        case typeDeclSym:
            reportStrError("TYPE", "%s is not a variable as expected", name, lineno);
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
            reportStrError("TYPE", "%s is not a variable as expected", name, lineno);
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

// general/misc

/*
 * reports an error if two types are not equal
 */
void assertIdenticalTYPEs(TYPE *expected, TYPE *actual, int lineno) {
    switch (expected->kind) {
        case idK:
            // two type names are identical if they were created in the same type declaration
            // otherwise they are different
            assertActualTypeIdentifier(expected->val.idT.typeDecl, actual, lineno);
            break;
        case structK:
            // two struct types are identical if they have the same sequence of fields
            // (same ordering), and if corresponding fields have the same names and identical types
            // two anonymous fields are considered to have the same type
            assertActualTypeStruct(expected->val.structT, actual, lineno);
            break;
        case sliceK:
            // two slice types are identical if they have identical element types
            assertActualTypeSlice(expected->val.sliceT, actual, lineno);
            break;
        case arrayK:
            // two array types are identical if they have identical element types and the same
            // array length
            assertActualTypeArray(expected, actual, lineno);
            break;
        case intK:
            assertActualTypeInt(actual, lineno);
            break;
        case float64K:
            assertActualTypeFloat64(actual, lineno);
            break;
        case runeK:
            assertActualTypeRune(actual, lineno);
            break;
        case boolK:
            assertActualTypeBool(actual, lineno);
            break;
        case stringK:
            assertActualTypeString(actual, lineno);
            break;
    }
}

void assertActualTypeIdentifier(TYPEDECLARATION* expectedDecl, TYPE* actual, int lineno) {
    switch (actual->kind) {
        case intK:
            reportStrError("TYPE", "expected type %s but found int type", expectedDecl->id->name, lineno);
            break;
        case float64K:
            reportStrError("TYPE", "expected type %s but found float64 type", expectedDecl->id->name, lineno);
            break;
        case runeK:
            reportStrError("TYPE", "expected type %s but found rune type", expectedDecl->id->name, lineno);
            break;
        case boolK:
            reportStrError("TYPE", "expected type %s but found bool type", expectedDecl->id->name, lineno);
            break;
        case stringK:
            reportStrError("TYPE", "expected type %s but found string type", expectedDecl->id->name, lineno);
            break;
        case idK:
            if (expectedDecl->number != actual->val.idT.typeDecl->number) {
                reportDoubleStrError("TYPE", "expected type %s but found type %s", expectedDecl->id->name, actual->val.idT.id->name, lineno);
            }
            break;
        case structK:
            reportStrError("TYPE", "expected type %s but found struct type", expectedDecl->id->name, lineno);
            break;
        case sliceK:
            reportStrError("TYPE", "expected type %s but found slice type", expectedDecl->id->name, lineno);
            break;
        case arrayK:
            reportStrError("TYPE", "expected type %s but found array type", expectedDecl->id->name, lineno);
            break;
    }
}

/*
 * a redeclaration is valid only if the symbol for the previous declaration has type
 * varDeclSym, varSym, or shortDeclSym
 */
void assertCanAssign(SYMBOL* prevSym, int lineno) {
    if ((prevSym->kind != varSym) && (prevSym->kind != varDeclSym) && (prevSym->kind != shortDeclSym)) {
        reportStrError("TYPE", "cannot assign to %s", prevSym->name, lineno);
    }
}
