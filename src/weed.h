
#include "tree.h"

void weedPROGRAM(PROGRAM* p);
void weedPACKAGE(PACKAGE* p);
void weedTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld);
void weedVARDECLARATION(VARDECLARATION* vd);
void weedTYPEDECLARATION(TYPEDECLARATION* td);
void weedTYPE(TYPE* t);
void weedFIELD(FIELD* f);
void weedPARAMETER(PARAMETER* p);
void weedFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd);
void weedSTATEMENT(STATEMENT* s, int inLoop, int inSwitchCase);
void weedCAST(CAST* c);
void weedSWITCHCASE(SWITCHCASE* s, int defaultSeen, int inLoop, int inSwitchCase);

/////////////////////////////////////////////////////////////////////////////////
// HELPERS
/////////////////////////////////////////////////////////////////////////////////

//void checkEqualLength_id_exp(ID* ids, EXP* exps, int lineno);
//void checkEqualLength_exp_exp(EXP* exps1, EXP* exps2, int lineno);
void checkLvalues(EXP* exps, int lineno);
void checkLvalue(EXP* exp, int lineno);
void lvalueHelper(EXP* exp, int lineno);
void checkIDs(EXP* exps, int lineno);
void checkFunctionCallOrReceiveOp(EXP* exp, int lineno);
void checkForBlankIdentifier_exp(EXP* e);
void checkForBlankIdentifier_string(char* s, char* message, int lineno);
