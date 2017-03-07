
#include <stdio.h>
#include <string.h>
#include "weed.h"
#include "error.h"

void weedPROGRAM(PROGRAM* p) {
    weedPACKAGE(p->package);
    weedTOPLEVELDECLARATION(p->topLevelDeclaration);
}

void weedPACKAGE(PACKAGE* p) {
    // check that the name of the package is not the blank identifier
    checkForBlankIdentifier_string(p->name, "the package cannot be the blank identifier", p->lineno);
}

void weedTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld) {
    if (tld == NULL) return;
    switch (tld->kind) {
        case vDeclK:
            weedVARDECLARATION(tld->val.varDeclTLD);
            break;
        case tDeclK:
            weedTYPEDECLARATION(tld->val.typeDeclTLD);
            break;
        case functionDeclK:
            weedFUNCTIONDECLARATION(tld->val.functionDeclTLD);
            break;
        default:
            break;
    }
    weedTOPLEVELDECLARATION(tld->next);
}

void weedVARDECLARATION(VARDECLARATION* vd) {
    if (vd == NULL) return;
    if (vd->isEmpty) return;
    switch (vd->kind) {
        case typeOnlyK:
            weedTYPE(vd->val.typeVD);
            break;
        case expOnlyK:
            // make sure that none of the expressions are blank identifiers
            checkForBlankIdentifier_exp(vd->val.expVD); // cheat
            break;
        case typeAndExpK:
            // make sure that none of the expressions are blank identifiers
            weedTYPE(vd->val.typeAndExpVD.type);
            checkForBlankIdentifier_exp(vd->val.typeAndExpVD.exp); // cheat
            break;
        default:
            break;
    }
    weedVARDECLARATION(vd->nextDistributed);
}

void weedTYPEDECLARATION(TYPEDECLARATION* td) {
    if (td == NULL) return;
    // the name of a type declaration can be _, i.e. type _ int is valid
    weedTYPE(td->type);
    weedTYPEDECLARATION(td->nextDistributed);
}

/*
 * make sure that the type is never _
 */
void weedTYPE(TYPE* t) {
    if (t == NULL) return;
    switch (t->kind) {
        case idK:
            checkForBlankIdentifier_string(t->val.idT->name, "type cannot be _", t->lineno);
            break;
        case structK:
            weedFIELD(t->val.structT);
            break;
        case sliceK:
            weedTYPE(t->val.sliceT);
            break;
        case arrayK:
            // make sure that the size does not involve the blank identifier
            checkForBlankIdentifier_exp(t->val.arrayT.size);
            // make sure that the size expression consists only of int literals
            checkArraySize(t->val.arrayT.size);
            weedTYPE(t->val.arrayT.elementType);
            break;
        default:
            break;
    }
}

void weedFIELD(FIELD* f) {
    if (f == NULL) return;
    // the id can be _, but we need to weed the type
    weedTYPE(f->type);
    weedFIELD(f->nextFieldSet);
}

void weedFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd) {
    checkForBlankIdentifier_string(fd->id->name, "function name cannot be _", fd->lineno);
    weedPARAMETER(fd->parameters);
    weedTYPE(fd->returnType);
    weedSTATEMENT(fd->statements, 0, 0);
}

void weedPARAMETER(PARAMETER* p) {
    if (p == NULL) return;
    weedTYPE(p->type);
    weedPARAMETER(p->nextParamSet);
}

void weedSTATEMENT(STATEMENT* s, int inLoop, int inSwitchCase) {
    // we need to weed
    // exp statements
    // assignment statements
    // variable declaration statements
    // inc statements
    // dec statements
    // short var decl statements
    if (s == NULL) return;
    switch (s->kind) {
        case emptyK:
            // nothing to be done
            break;
        case expK:
            // check that the expression is a function call or recieve operation
            checkFunctionCallOrReceiveOp(s->val.expS, s->lineno);
            // check that the expression does not contain a blank identifier
            checkForBlankIdentifier_exp(s->val.expS);
            break;
        case incK:
            checkLvalue(s->val.incS, s->lineno);
            checkForBlankIdentifier_exp(s->val.incS);
            break;
        case decK:
            checkLvalue(s->val.decS, s->lineno);
            checkForBlankIdentifier_exp(s->val.decS);
            break;
        case regAssignK:
            checkLvalues(s->val.regAssignS.lvalue, s->lineno); // cheat and use linked list pointed to
                                                                // by lvalue
            // blank identifier can be on the left of a regular assignment statement, but not the right
            checkForBlankIdentifier_exp(s->val.regAssignS.exp); // cheat
            break;
        case binOpAssignK:
            checkLvalue(s->val.binOpAssignS.lvalue, s->lineno);
            // blank identifier cannot be on either side of the assignment
            checkForBlankIdentifier_exp(s->val.binOpAssignS.lvalue); // cheat
            checkForBlankIdentifier_exp(s->val.binOpAssignS.exp); // cheat
            break;
        case shortDeclK:
            // check that all of the expressions are ids
            checkIDs(s->val.shortDeclS.id, s->lineno);  // cheat
            // check that none of the ids are parenthesized
            checkForParentheses(s->val.shortDeclS.id); // cheat
            // blank identifier cannot be used on right side of assignment
            checkForBlankIdentifier_exp(s->val.shortDeclS.exp); // cheat
            break;
        case varDeclK:
            weedVARDECLARATION(s->val.varDeclS);
            break;
        case typeDeclK:
            weedTYPEDECLARATION(s->val.typeDeclS);
            break;
        case printK:
            // can't print a blank identifier
            checkForBlankIdentifier_exp(s->val.printS);
            break;
        case printlnK:
            // can't print a blank identifier
            checkForBlankIdentifier_exp(s->val.printlnS);
            break;
        case returnK:
            // cannot return a blank identifier
            checkForBlankIdentifier_exp(s->val.returnS);
            break;
        case ifK:
            weedSTATEMENT(s->val.ifS.initStatement, inLoop, inSwitchCase);
            checkForBlankIdentifier_exp(s->val.ifS.condition);
            weedSTATEMENT(s->val.ifS.body, inLoop, inSwitchCase);
            break;
        case ifElseK:
            weedSTATEMENT(s->val.ifElseS.initStatement, inLoop, inSwitchCase);
            checkForBlankIdentifier_exp(s->val.ifElseS.condition);
            weedSTATEMENT(s->val.ifElseS.thenPart, inLoop, inSwitchCase);
            weedSTATEMENT(s->val.ifElseS.elsePart, inLoop, inSwitchCase);
            break;
        case switchK:
            weedSTATEMENT(s->val.switchS.initStatement, inLoop, inSwitchCase);
            checkForBlankIdentifier_exp(s->val.switchS.condition);
            weedSWITCHCASE(s->val.switchS.cases, 0, inLoop, 1);
            break;
        case whileK:
            checkForBlankIdentifier_exp(s->val.whileS.condition);
            weedSTATEMENT(s->val.whileS.body, 1, inSwitchCase);
            break;
        case infiniteLoopK:
            weedSTATEMENT(s->val.infiniteLoopS, 1, inSwitchCase);
            break;
        case forK:
            weedSTATEMENT(s->val.forS.initStatement, inLoop, inSwitchCase);
            checkForBlankIdentifier_exp(s->val.forS.condition);
            weedSTATEMENT(s->val.forS.postStatement, inLoop, inSwitchCase);
            weedSTATEMENT(s->val.forS.body, 1, inSwitchCase);
            break;
        case breakK:
            // only valid if we are in a loop or switch case
            if (!inLoop && !inSwitchCase) {
                reportWeedError("break is not in a loop or switch-case statement", s->lineno);
            }
            break;
        case continueK:
            // only valid if we are in a loop
            if (!inLoop) {
                reportWeedError("continue is not in a loop", s->lineno);
            }
            break;
        case blockK:
            weedSTATEMENT(s->val.blockS, inLoop, inSwitchCase);
            break;
        default:
            break;
    }
    // weed the next statement
    weedSTATEMENT(s->next, inLoop, inSwitchCase);
}

void weedCAST(CAST* c) {
    weedTYPE(c->type);
    checkForBlankIdentifier_exp(c->exp);
}

void weedSWITCHCASE(SWITCHCASE* s, int defaultSeen, int inLoop, int inSwitchCase) {
    if (s == NULL) return;
    switch (s->kind) {
        case caseK:
            weedSTATEMENT(s->val.caseC.statements, inLoop, inSwitchCase);
            break;
        case defaultK:
            if (defaultSeen) {
                // record error (but continue)
                reportWeedError("duplicate default case", s->lineno);
            }
            defaultSeen = 1;
            weedSTATEMENT(s->val.defaultStatementsC, inLoop, inSwitchCase);
            break;
        default:
            break;
    }
    weedSWITCHCASE(s->next, defaultSeen, inLoop, inSwitchCase);
}

/////////////////////////////////////////////////////////////////////////////////
// HELPERS
/////////////////////////////////////////////////////////////////////////////////

/**
 * Checks whether an id list and and expression list have equal length
 */
/*
void checkEqualLength_id_exp(ID* ids, EXP* exps, int lineno) {
    if (ids == NULL && exps == NULL) return;
    if (ids == NULL) {
        // exps is longer than ids
        reportWeedError("more expressions than identifiers", lineno);
        return;
    }
    if (exps == NULL) {
        // ids is longer than exps
        reportWeedError("more identifiers than expressions", lineno);
        return;
    }
    checkEqualLength_id_exp(ids->next, exps->next, lineno);
}
*/

/**
 * Checks whether two expression lists have equal length
 */
/*
void checkEqualLength_exp_exp(EXP* exps1, EXP* exps2, int lineno) {
    if (exps1 == NULL && exps2 == NULL) return;
    if (exps1 == NULL) {
        // exps2 is longer than exps1
        reportWeedError("more expressions on right side than left side", lineno);
        return;
    }
    if (exps2 == NULL) {
        // exps1 is longer than exps2
        reportWeedError("more expressions on left side than right side", lineno);
        return;
    }
    checkEqualLength_exp_exp(exps1->next, exps2->next, lineno);
}
*/

/**
 * Checks that all expressions in an expression list are lvalues
 * an lvalue is:
 * identifier
 * array indexing expression
 * field access
 *
 */
void checkLvalues(EXP* exps, int lineno) {
    if (exps == NULL) return;
    lvalueHelper(exps, lineno);
    checkLvalues(exps->next, lineno);
}

/**
 * Checks that there is only a single expression in the list and that it is an lvalue
 */
void checkLvalue(EXP* exp, int lineno) {
    if (exp->next != NULL) {
        reportWeedError("expected one lvalue but found multiple", lineno);
    }
    lvalueHelper(exp, lineno);
}

/**
 * helper for checking lvalues
 */
void lvalueHelper(EXP* exp, int lineno) {
    switch (exp->kind) {
        case identifierK:
            break;
        case selectorK:
            break;
        case indexK:
            break;
        case intLiteralK:
            reportWeedError("expected lvalue but found int literal", lineno);
            break;
        case floatLiteralK:
            reportWeedError("expected lvalue but found float literal", lineno);
            break;
        case runeLiteralK:
            reportWeedError("expected lvalue but found rune literal", lineno);
            break;
        case stringLiteralK:
            reportWeedError("expected lvalue but found string literal", lineno);
            break;
        case rawStringLiteralK:
            reportWeedError("expected lvalue but found raw string literal", lineno);
            break;
        case plusK:
            reportWeedError("expected lvalue but found addition expression", lineno);
            break;
        case minusK:
            reportWeedError("expected lvalue but found subtraction expression", lineno);
            break;
        case timesK:
            reportWeedError("expected lvalue but found mutiplication expression", lineno);
            break;
        case divK:
            reportWeedError("expected lvalue but found division expression", lineno);
            break;
        case modK:
            reportWeedError("expected lvalue but found mod expression", lineno);
            break;
        case bitwiseOrK:
            reportWeedError("expected lvalue but found bitwise or expression", lineno);
            break;
        case bitwiseAndK:
            reportWeedError("expected lvalue but found bitwise and expression", lineno);
            break;
        case xorK:
            reportWeedError("expected lvalue but found xor expression", lineno);
            break;
        case ltK:
            reportWeedError("expected lvalue but found lt expression", lineno);
            break;
        case gtK:
            reportWeedError("expected lvalue but found gt expression", lineno);
            break;
        case eqK:
            reportWeedError("expected lvalue but found eq expression", lineno);
            break;
        case neqK:
            reportWeedError("expected lvalue but found neq expression", lineno);
            break;
        case leqK:
            reportWeedError("expected lvalue but found leq expression", lineno);
            break;
        case geqK:
            reportWeedError("expected lvalue but found geq expression", lineno);
            break;
        case orK:
            reportWeedError("expected lvalue but found or expression", lineno);
            break;
        case andK:
            reportWeedError("expected lvalue but found and expression", lineno);
            break;
        case leftShiftK:
            reportWeedError("expected lvalue but found left shift expression", lineno);
            break;
        case rightShiftK:
            reportWeedError("expected lvalue but found right shift expression", lineno);
            break;
        case bitClearK:
            reportWeedError("expected lvalue but found bit clear expression", lineno);
            break;
        case appendK:
            reportWeedError("expected lvalue but found append expression", lineno);
            break;
        case castK:
            reportWeedError("expected lvalue but found cast expression", lineno);
            break;
        case argumentsK:
            reportWeedError("expected lvalue but found function call expression", lineno);
            break;
        case uPlusK:
            reportWeedError("expected lvalue but found unary plus expression", lineno);
            break;
        case uMinusK:
            reportWeedError("expected lvalue but found unary minus expression", lineno);
            break;
        case uNotK:
            reportWeedError("expected lvalue but found unary not expression", lineno);
            break;
        case uXorK:
            reportWeedError("expected lvalue but found unary xor expression", lineno);
            break;
        case uReceiveK:
            reportWeedError("expected lvalue but found receive expression", lineno);
            break;
        default:
            break;
    }
}

/**
 * Checks that a list of expressions consists of only identifiers
 */
void checkIDs(EXP* exps, int lineno) {
    if (exps == NULL) return;
    switch (exps->kind) {
        case identifierK:
            break;
        case selectorK:
            reportWeedError("expected identifier but found field access", lineno);
            break;
        case indexK:
            reportWeedError("expected identifier but found array index", lineno);
            break;
        case intLiteralK:
            reportWeedError("expected identifier but found int literal", lineno);
            break;
        case floatLiteralK:
            reportWeedError("expected identifier but found float literal", lineno);
            break;
        case runeLiteralK:
            reportWeedError("expected identifier but found rune literal", lineno);
            break;
        case stringLiteralK:
            reportWeedError("expected identifier but found string literal", lineno);
            break;
        case rawStringLiteralK:
            reportWeedError("expected identifier but found raw string literal", lineno);
            break;
        case plusK:
            reportWeedError("expected identifier but found addition expression", lineno);
            break;
        case minusK:
            reportWeedError("expected identifier but found subtraction expression", lineno);
            break;
        case timesK:
            reportWeedError("expected identifier but found mutiplication expression", lineno);
            break;
        case divK:
            reportWeedError("expected identifier but found division expression", lineno);
            break;
        case modK:
            reportWeedError("expected identifier but found mod expression", lineno);
            break;
        case bitwiseOrK:
            reportWeedError("expected identifier but found bitwise or expression", lineno);
            break;
        case bitwiseAndK:
            reportWeedError("expected identifier but found bitwise and expression", lineno);
            break;
        case xorK:
            reportWeedError("expected identifier but found xor expression", lineno);
            break;
        case ltK:
            reportWeedError("expected identifier but found lt expression", lineno);
            break;
        case gtK:
            reportWeedError("expected identifier but found gt expression", lineno);
            break;
        case eqK:
            reportWeedError("expected identifier but found eq expression", lineno);
            break;
        case neqK:
            reportWeedError("expected identifier but found neq expression", lineno);
            break;
        case leqK:
            reportWeedError("expected identifier but found leq expression", lineno);
            break;
        case geqK:
            reportWeedError("expected identifier but found geq expression", lineno);
            break;
        case orK:
            reportWeedError("expected identifier but found or expression", lineno);
            break;
        case andK:
            reportWeedError("expected identifier but found and expression", lineno);
            break;
        case leftShiftK:
            reportWeedError("expected identifier but found left shift expression", lineno);
            break;
        case rightShiftK:
            reportWeedError("expected identifier but found right shift expression", lineno);
            break;
        case bitClearK:
            reportWeedError("expected identifier but found bit clear expression", lineno);
            break;
        case appendK:
            reportWeedError("expected identifier but found append expression", lineno);
            break;
        case castK:
            reportWeedError("expected identifier but found cast expression", lineno);
            break;
        case argumentsK:
            reportWeedError("expected identifier but found function call expression", lineno);
            break;
        case uPlusK:
            reportWeedError("expected identifier but found unary plus expression", lineno);
            break;
        case uMinusK:
            reportWeedError("expected identifier but found unary minus expression", lineno);
            break;
        case uNotK:
            reportWeedError("expected identifier but found unary not expression", lineno);
            break;
        case uXorK:
            reportWeedError("expected identifier but found unary xor expression", lineno);
            break;
        case uReceiveK:
            reportWeedError("expected identifier but found receive expression", lineno);
            break;
        default:
            break;
    }
    checkIDs(exps->next, lineno);
}

void checkForParentheses(EXP* exps) {
    if (exps == NULL) return;
    if (exps->isParenthesized) reportWeedError("non-name on left side of :=", exps->lineno);
    checkForParentheses(exps->next);
}

/**
 * checks that the expression is a function call or receive op
 */
void checkFunctionCallOrReceiveOp(EXP* exp, int lineno) {
    switch (exp->kind) {
        case argumentsK:
            break;
        case uReceiveK:
            break;
        default:
            reportWeedError("invalid expression statement", lineno);
            break;
    }
}

void checkForBlankIdentifier_exp(EXP* e) {
    if (e == NULL) return;
    switch(e->kind) {
        case identifierK:
            checkForBlankIdentifier_string(e->val.idE->name, "cannot use _ as value", e->lineno);
            break;
        case intLiteralK:
            break;
        case floatLiteralK:
            break;
        case runeLiteralK:
            break;
        case stringLiteralK:
            break;
        case rawStringLiteralK:
            break;
        case plusK:
            checkForBlankIdentifier_exp(e->val.plusE.left);
            checkForBlankIdentifier_exp(e->val.plusE.right);
            break;
        case minusK:
            checkForBlankIdentifier_exp(e->val.minusE.left);
            checkForBlankIdentifier_exp(e->val.minusE.right);
            break;
        case timesK:
            checkForBlankIdentifier_exp(e->val.timesE.left);
            checkForBlankIdentifier_exp(e->val.timesE.right);
            break;
        case divK:
            checkForBlankIdentifier_exp(e->val.divE.left);
            checkForBlankIdentifier_exp(e->val.divE.right);
            break;
        case modK:
            checkForBlankIdentifier_exp(e->val.modE.left);
            checkForBlankIdentifier_exp(e->val.modE.right);
            break;
        case bitwiseOrK:
            checkForBlankIdentifier_exp(e->val.bitwiseOrE.left);
            checkForBlankIdentifier_exp(e->val.bitwiseOrE.right);
            break;
        case bitwiseAndK:
            checkForBlankIdentifier_exp(e->val.bitwiseAndE.left);
            checkForBlankIdentifier_exp(e->val.bitwiseAndE.right);
            break;
        case xorK:
            checkForBlankIdentifier_exp(e->val.xorE.left);
            checkForBlankIdentifier_exp(e->val.xorE.right);
            break;
        case ltK:
            checkForBlankIdentifier_exp(e->val.ltE.left);
            checkForBlankIdentifier_exp(e->val.ltE.right);
            break;
        case gtK:
            checkForBlankIdentifier_exp(e->val.gtE.left);
            checkForBlankIdentifier_exp(e->val.gtE.right);
            break;
        case eqK:
            checkForBlankIdentifier_exp(e->val.eqE.left);
            checkForBlankIdentifier_exp(e->val.eqE.right);
            break;
        case neqK:
            checkForBlankIdentifier_exp(e->val.neqE.left);
            checkForBlankIdentifier_exp(e->val.neqE.right);
            break;
        case leqK:
            checkForBlankIdentifier_exp(e->val.leqE.left);
            checkForBlankIdentifier_exp(e->val.leqE.right);
            break;
        case geqK:
            checkForBlankIdentifier_exp(e->val.geqE.left);
            checkForBlankIdentifier_exp(e->val.geqE.right);
            break;
        case orK:
            checkForBlankIdentifier_exp(e->val.orE.left);
            checkForBlankIdentifier_exp(e->val.orE.right);
            break;
        case andK:
            checkForBlankIdentifier_exp(e->val.andE.left);
            checkForBlankIdentifier_exp(e->val.andE.right);
            break;
        case leftShiftK:
            checkForBlankIdentifier_exp(e->val.leftShiftE.left);
            checkForBlankIdentifier_exp(e->val.leftShiftE.right);
            break;
        case rightShiftK:
            checkForBlankIdentifier_exp(e->val.rightShiftE.left);
            checkForBlankIdentifier_exp(e->val.rightShiftE.right);
            break;
        case bitClearK:
            checkForBlankIdentifier_exp(e->val.bitClearE.left);
            checkForBlankIdentifier_exp(e->val.bitClearE.right);
            break;
        case appendK:
            // the slice to append to cannot be the blank identifier
            checkForBlankIdentifier_exp(e->val.appendE.slice);
            // the expression to append cannot be the blank identifier
            checkForBlankIdentifier_exp(e->val.appendE.expToAppend);
            break;
        case castK:
            weedCAST(e->val.castE);
            break;
        case selectorK:
            // can't refer to a blank field
            checkForBlankIdentifier_exp(e->val.selectorE.rest);
            checkForBlankIdentifier_string(e->val.selectorE.lastSelector->name, "cannot refer to a blank field", e->lineno);
            break;
        case indexK:
            // TODO some more checks of rest? Seems like we better check that the first thing in rest
            // is an identifier
            checkForBlankIdentifier_exp(e->val.indexE.rest);
            checkForBlankIdentifier_exp(e->val.indexE.lastIndex);
            break;
        case argumentsK:
            checkForBlankIdentifier_exp(e->val.argumentsE.rest);
            checkForBlankIdentifier_exp(e->val.argumentsE.args);
            break;
        case uPlusK:
            checkForBlankIdentifier_exp(e->val.uPlusE);
            break;
        case uMinusK:
            checkForBlankIdentifier_exp(e->val.uMinusE);
            break;
        case uNotK:
            checkForBlankIdentifier_exp(e->val.uNotE);
            break;
        case uXorK:
            checkForBlankIdentifier_exp(e->val.uXorE);
            break;
        case uReceiveK:
            checkForBlankIdentifier_exp(e->val.uReceiveE);
            break;
        default:
            break;
    }
    checkForBlankIdentifier_exp(e->next);
}

void checkForBlankIdentifier_string(char* s, char* message, int lineno) {
    if (strcmp("_", s) == 0) {
        reportWeedError(message, lineno);
    }
}

/*
 * check that the expression only involves integer literals (or runes)
 */
void checkArraySize(EXP* e) {
    switch (e->kind) {
        case identifierK:
            reportStrError("WEED", "non-constant array bound %s", e->val.idE->name, e->lineno);
            break;
        case intLiteralK:
            // all good!
            break;
        case floatLiteralK:
            reportFloatError("WEED", "invalid array bound %f", e->val.floatLiteralE, e->lineno);
            break;
        case runeLiteralK:
            reportCharError("WEED", "invalid array bound %c", e->val.runeLiteralE, e->lineno);
            break;
        case stringLiteralK:
            reportStrError("WEED", "invalid array bound %s", e->val.stringLiteralE, e->lineno);
            break;
        case rawStringLiteralK:
            reportStrError("WEED", "invalid array bound %s", e->val.rawStringLiteralE, e->lineno);
            break;
        case plusK:
            checkArraySize(e->val.plusE.left);
            checkArraySize(e->val.plusE.right);
            break;
        case minusK:
            checkArraySize(e->val.minusE.left);
            checkArraySize(e->val.minusE.right);
            break;
        case timesK:
            checkArraySize(e->val.timesE.left);
            checkArraySize(e->val.timesE.right);
            break;
        case divK:
            checkArraySize(e->val.divE.left);
            checkArraySize(e->val.divE.right);
            break;
        case modK:
            checkArraySize(e->val.modE.left);
            checkArraySize(e->val.modE.right);
            break;
        case bitwiseOrK:
            checkArraySize(e->val.bitwiseOrE.left);
            checkArraySize(e->val.bitwiseOrE.right);
            break;
        case bitwiseAndK:
            checkArraySize(e->val.bitwiseAndE.left);
            checkArraySize(e->val.bitwiseAndE.right);
            break;
        case xorK:
            checkArraySize(e->val.xorE.left);
            checkArraySize(e->val.xorE.right);
            break;
        case ltK:
            checkArraySize(e->val.ltE.left);
            checkArraySize(e->val.ltE.right);
            break;
        case gtK:
            checkArraySize(e->val.gtE.left);
            checkArraySize(e->val.gtE.right);
            break;
        case eqK:
            checkArraySize(e->val.eqE.left);
            checkArraySize(e->val.eqE.right);
            break;
        case neqK:
            checkArraySize(e->val.neqE.left);
            checkArraySize(e->val.neqE.right);
            break;
        case leqK:
            checkArraySize(e->val.leqE.left);
            checkArraySize(e->val.leqE.right);
            break;
        case geqK:
            checkArraySize(e->val.geqE.left);
            checkArraySize(e->val.geqE.right);
            break;
        case orK:
            checkArraySize(e->val.orE.left);
            checkArraySize(e->val.orE.right);
            break;
        case andK:
            checkArraySize(e->val.andE.left);
            checkArraySize(e->val.andE.right);
            break;
        case leftShiftK:
            checkArraySize(e->val.leftShiftE.left);
            checkArraySize(e->val.leftShiftE.right);
            break;
        case rightShiftK:
            checkArraySize(e->val.rightShiftE.left);
            checkArraySize(e->val.rightShiftE.right);
            break;
        case bitClearK:
            checkArraySize(e->val.bitClearE.left);
            checkArraySize(e->val.bitClearE.right);
            break;
        case appendK:
            // totally invalid to stick an append as an array size
            reportWeedError("invalid array bound: append", e->lineno);
            break;
        case castK:
            // all we can check for at this point is that the expression being casted should be an int literal
            // in type checking phase, we'll need to check that the type being casted to is an int
            checkArraySize(e->val.castE->exp);
            break;
        case selectorK:
            // totally invalid (non-constant array bound)
            reportWeedError("invalid (non-constant) array bound: field access", e->lineno);
            break;
        case indexK:
            reportWeedError("invalid (non-constant) array bound: array access", e->lineno);
            break;
        case argumentsK:
            reportWeedError("invalid (non-constant) array bound: function call", e->lineno);
            break;
        case uPlusK:
            checkArraySize(e->val.uPlusE);
            break;
        case uMinusK:
            checkArraySize(e->val.uMinusE);
            break;
        case uNotK:
            reportWeedError("invalid array bound: unary not", e->lineno);
            break;
        case uXorK:
            checkArraySize(e->val.uXorE);
            break;
        case uReceiveK:
            reportWeedError("invalid array bound: unary receive", e->lineno);
            break;
        default:
            break;
    }

}
