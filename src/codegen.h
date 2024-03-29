
#include "tree.h"

ID* reservedNames[HashSize];

typedef struct REPLACEMENTID {
    char* name;
    char* replacementName;
    struct REPLACEMENTID* next;
} REPLACEMENTID;

REPLACEMENTID* replacementNames[HashSize];

void addTypeDefs(CPPTYPE* c);
void addOperators(CPPTYPE* c);
void addHeaderCode();
char* getBoundsCheckVarName();
char* getGotoLabel();
void addToReservedNames(char* name);
void initReservedNames();
int isReserved(char* name);
char* findReplacementName(char* name);
char* generateReplacementName(char* name);
void putReplacementName(char* name, char* replacementName);
char* getReplacementName(char* name);
char* getOutputName(char* name);

void genPROGRAM(PROGRAM* p, char* fname);
void genTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld);
void genVARDECLARATION(VARDECLARATION* vd, int level);
void genVARDECLARATIONlist(VARDECLARATION* vd, int level);
void genArrayChecks(ARRAYINDEX* a, int level);
void genSliceChecks(SLICEINDEX* s, int level);
void genFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd);
void genPARAMETER(PARAMETER* p);
void genPARAMETERlist(PARAMETER* p);
void genSTATEMENT(STATEMENT* s, int level, int semicolon, int startAtRwPointer, int breakToGoto, char* gotoLabel);
void genArrayAssign(CPPTYPE* cppType, char* name, EXP* e, int level);
void genBinOp(STATEMENT* s);
void genPrintEXPs(EXP* exps);
void genPrintlnEXPs(EXP* exps);
void genCustomPrintOutput(EXP* e);
void genSWITCHCASE(SWITCHCASE* sc, EXP* e, int level);
void genCaseEXPs(EXP* exps, EXP* condition);
void genEXP(EXP* e);
void genEXPs(EXP* e);
void genCPPTYPE(CPPTYPE* c);
void genSTRUCTTYPE(STRUCTTYPE* s);
void genFIELD(FIELD* f, int level);
void genFIELDlist(FIELD* f, int level);
void genStructComparator(CPPTYPE* c, int inequality);
void genSTRUCTTYPEcomparison(STRUCTTYPE* s, int inequality);
void genFIELDcomparison(FIELD* f, int inequality);
void genFIELDlistComparison(FIELD* f, int inequality);
void genComparison(CPPTYPE* c, char* name, int inequality);
void genArrayComparator(CPPTYPE* c);
void genArrayElementComparison(CPPTYPE* type);

int equivToMain(FUNCTIONDECLARATION* fd);
void genDefault(CPPTYPE* t, int level);
void genDefaultSTRUCTTYPE(STRUCTTYPE* s, int level);
void genDefaultFIELD(FIELD* f, int level);
void genDefaultFIELDlist(FIELD* f, int level);
char* rawify(char* s);
