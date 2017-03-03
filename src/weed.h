
#include "tree.h"

void weedPROGRAM(PROGRAM* p);

// HELPERS

/**
 * Checks whether an id list and and expression list have equal length
 */
void checkEqualLength_id_exp(ID* ids, EXP* exps);

/**
 * Checks whether two expression lists have equal length
 */
void checkEqualLength_exp_exp(EXP* exps1, EXP* exps2);

/**
 * Checks that all expressions in an expression list are lvalues
 */
void checkLvalues(EXP* exps);

/**
 * Checks that there is only a single expression in the list and that it is an lvalue
 */
void checkLvalue(EXP* exp);

/**
 * Checks that a list of expressions consists of only identifiers
 */
void checkIDs(EXP* exps);

/**
 * checks that the expression is a function call or receive op
 */
void checkFunctionCallOrReceiveOp(EXP* exp);
