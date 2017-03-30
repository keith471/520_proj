
void addTypeDefs(CPPTYPE* c);
void addHeaderCode();
void genPROGRAM(PROGRAM* p, char* fname);
void genTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld);
void genVARDECLARATION(VARDECLARATION* vd, int level);
void genVARDECLARATIONlist(VARDECLARATION* vd, int level);
void genFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd);
void genPARAMETER(PARAMETER* p);
void genPARAMETERlist(PARAMETER* p);
void genSTATEMENT(STATEMENT* s, int level, int semicolon, int startAtRwPointer);
void genBinOp(STATEMENT* s);
void genPrintEXPs(EXP* exps);
void genPrintlnEXPs(EXP* exps);
void genSWITCHCASE(SWITCHCASE* sc, EXP* e, int level);
void genCaseEXPs(EXP* exps, EXP* condition);
void genEXP(EXP* e);
void genEXPs(EXP* e);
void genCPPTYPE(CPPTYPE* c);
void genSTRUCTTYPE(STRUCTTYPE* s);
void genFIELD(FIELD* f, int level);
void genFIELDlist(FIELD* f, int level);

int isMain(FUNCTIONDECLARATION* fd);
void genDefault(CPPTYPE* t, int level);
void genDefaultSTRUCTTYPE(STRUCTTYPE* s, int level);
void genDefaultFIELD(FIELD* f, int level);
void genDefaultFIELDlist(FIELD* f, int level);
char* rawify(char* s);
