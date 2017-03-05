
#include <stdio.h>
#include "weed.h"
#include "error.h"

void weedPROGRAM(PROGRAM* p) {
    // no need to weed package declarations
    weedTOPLEVELDECLARATION(p->topLevelDeclaration);
}

void weedTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld) {
    if (tld == NULL) return;
    // no need to weed var or type declarations
    // var declarations are weeded by the parser
    // type declarations require no weeding
    switch (tld->kind) {
        case functionDeclK:
            weedFUNCTIONDECLARATION(tld->val.functionDeclTLD);
            break;
        default:
            break;
    }
    weedTOPLEVELDECLARATION(tld->next);
}

/*
void weedVARDECLARATION(VARDECLARATION* vd) {
    if (vd == NULL) return;
    switch (vd->kind) {
        case expOnlyK:
            checkEqualLength_id_exp(vd->ids, vd->val.expVD, vd->lineno);
            break;
        case typeAndExpK:
            checkEqualLength_id_exp(vd->ids, vd->val.typeAndExpVD.exp, vd->lineno);
            break;
        default:
            break;
    }
    weedVARDECLARATION(vd->next);
}
*/

void weedFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd) {
    // we need to weed function statements, but nothing else
    weedSTATEMENT(fd->statements, 0, 0);
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
        case expK:
            checkFunctionCallOrReceiveOp(s->val.expS, s->lineno);
            break;
        case incK:
            checkLvalue(s->val.incS, s->lineno);
            break;
        case decK:
            checkLvalue(s->val.decS, s->lineno);
            break;
        case regAssignK:
            checkLvalues(s->val.regAssignS.lvalue, s->lineno); // cheat and use linked list pointed to
                                                                // by lvalue
            break;
        case binOpAssignK:
            checkLvalue(s->val.binOpAssignS.lvalue, s->lineno);
            break;
        case shortDeclK:
            checkIDs(s->val.shortDeclS.id, s->lineno);  // cheat
            break;
        case ifK:
            weedSTATEMENT(s->val.ifS.initStatement, inLoop, inSwitchCase);
            weedSTATEMENT(s->val.ifS.body, inLoop, inSwitchCase);
            break;
        case ifElseK:
            weedSTATEMENT(s->val.ifElseS.initStatement, inLoop, inSwitchCase);
            weedSTATEMENT(s->val.ifElseS.thenPart, inLoop, inSwitchCase);
            weedSTATEMENT(s->val.ifElseS.elsePart, inLoop, inSwitchCase);
            break;
        case switchK:
            weedSTATEMENT(s->val.switchS.initStatement, inLoop, inSwitchCase);
            weedSWITCHCASE(s->val.switchS.cases, 0, inLoop, 1);
            break;
        case whileK:
            weedSTATEMENT(s->val.whileS.body, 1, inSwitchCase);
            break;
        case infiniteLoopK:
            weedSTATEMENT(s->val.infiniteLoopS, 1, inSwitchCase);
            break;
        case forK:
            weedSTATEMENT(s->val.forS.initStatement, inLoop, inSwitchCase);
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
            if (!inLoop && !inSwitchCase) {
                reportWeedError("continue is not in a loop", s->lineno);
            }
            break;
        default:
            break;
    }
    // weed the next statement
    weedSTATEMENT(s->next, inLoop, inSwitchCase);
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
