
#import "weed.h"
#import "error.h"


void weedPROGRAM(PROGRAM* p) {
    // no need to weed package declarations
    weedTOPLEVELDECLARATION(p->topLevelDeclaration);
}

void weedTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld) {
    if (tld == NULL) return;
    // no need to weed type declarations
    switch (tld->kind) {
        case vDeclK:
            weedVARDECLARATION(tld->val.varDeclTLD);
            break;
        case functionDeclK:
            weedFUNCTIONDECLARATION(tld->val.functionDeclTLD);
            break;
    }
    weedTOPLEVELDECLARATION(tld->next);
}

void weedVARDECLARATION(VARDECLARATION* vd) {
    if (vd == NULL) return;
    switch (vd->kind) {
        case expOnlyK:
            checkEqualLength_id_exp(vd->ids, vd->val.expVD);
            break;
        case typeAndExpK:
            checkEqualLength_id_exp(vd->ids, vd->val.typeAndExpVD.exp);
            break;
        default:
            break;
    }
    weedVARDECLARATION(vd->next);
}

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
            checkFunctionCallOrReceiveOp(s->val.expS);
            break;
        case incK:
            checkLvalue(s->val.incS);
            break;
        case decK:
            checkLvalue(s->val.decS);
            break;
        case regAssignK:
            checkEqualLength_exp_exp(s->val.regAssignS.lvalues, s->val.regAssignS.exps);
            checkLvalues(s->val.regAssignS.lvalues);
            break;
        case binOpAssignK:
            checkLvalue(s->val.binOpAssignS.lvalue);
            break;
        case shortDeclK:
            checkEqualLength_exp_exp(s->val.shortDeclS.ids, s->val.shortDeclS.exps);
            checkIDs(s->val.shortDeclS.ids);
            break;
        case varDeclK:
            weedVARDECLARATION(s->val.varDeclS);
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
    weedSWITCHCASE(s->next, defaultSeen);
}

/////////////////////////////////////////////////////////////////////////////////
// HELPERS
/////////////////////////////////////////////////////////////////////////////////

/**
 * Checks whether an id list and and expression list have equal length
 */
void checkEqualLength_id_exp(ID* ids, EXP* exps) {

}

/**
 * Checks whether two expression lists have equal length
 */
void checkEqualLength_exp_exp(EXP* exps1, EXP* exps2) {

}

/**
 * Checks that all expressions in an expression list are lvalues
 */
void checkLvalues(EXP* exps) {

}

/**
 * Checks that there is only a single expression in the list and that it is an lvalue
 */
void checkLvalue(EXP* exp) {

}

/**
 * Checks that a list of expressions consists of only identifiers
 */
void checkIDs(EXP* exps) {

}

/**
 * checks that the expression is a function call or receive op
 */
void checkFunctionCallOrReceiveOp(EXP* exp) {

}
