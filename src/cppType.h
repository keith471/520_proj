
int nameTableContains(char* name);
char* getStructName();
char* getArrayName();

void initCppTypes();

CPPTYPE* getCppType(CPPTYPE* t);
int equivalent(CPPTYPE* c, CPPTYPE* t);
int equivalentVectors(CPPTYPE* c, CPPTYPE* t);
int equivalentArrays(CPPTYPE* c, CPPTYPE* t);
int equivalentStructs(CPPTYPE* c, CPPTYPE* t);
int equivalentFIELDs(FIELD* currExpected, FIELD* expected, FIELD* currActual, FIELD* actual);

void putCPPTYPE(CPPTYPE* t);

void cppTypePROGRAM(PROGRAM* p);
void cppTypeTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld);
void cppTypeVARDECLARATION(VARDECLARATION* vd);
void cppTypeVARDECLARATIONlist(VARDECLARATION* vd);
void cppTypeTYPEDECLARATION(TYPEDECLARATION* td);
void cppTypeFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd);
void cppTypePARAMETER(PARAMETER* p);
void cppTypePARAMETERlist(PARAMETER* p);
void cppTypeSTATEMENT(STATEMENT* s);
void cppTypeSWITCHCASE(SWITCHCASE* sc);
void cppTypeEXPs(EXP* exps);
void cppTypeEXP(EXP* e);
void cppTypeTYPE(TYPE* t);
void cppTypeSTRUCTTYPE(STRUCTTYPE* t);
void cppTypeFIELD(FIELD* f);
void cppTypeFIELDlist(FIELD* f);
