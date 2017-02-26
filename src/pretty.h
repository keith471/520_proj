#ifndef PRETTY_H
#define PRETTY_H

#include "tree.h"

void prettyPROGRAM(PROGRAM *p, char* filePath);
void prettyPACKAGE(PACKAGE* package);
void prettyTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld);
void prettyVARDECLARATION(VARDECLARATION* vd, int level);
void prettyVARDECLARATIONdistributedbody(VARDECLARATION* vd, int level);
void prettyVARDECLARATIONsingleline(VARDECLARATION* vd);
void prettyID(ID* id);
void prettyTYPEDECLARATION(TYPEDECLARATION* td, int level);
void prettyTYPEDECLARATIONdistributedbody(TYPEDECLARATION* td, int level);
void prettyTYPEDECLARATIONsingle(TYPEDECLARATION* td, int level);
void prettyTYPE(TYPE* type, int level);
void prettyFIELD(FIELD* field, int level);
void prettyFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd);
void prettySTATEMENT(STATEMENT* s, int level, int semicolon, int startAtRwPointer);
void terminateSTATEMENT(int level, int semicolon);
void printTabsPrecedingStatement(int level, int startAtRwPointer);
void prettySWITCHCASE(SWITCHCASE* s, int level);
void prettyBinOp(OperationKind opKind);
void prettyPARAMETER(PARAMETER* param);
void prettyEXPs(EXP* exp);
void prettyEXP(EXP* e);

#endif /* !PRETTY_H */
