#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "codegen.h"
#include "outputhelpers.h"
#include "pretty.h" // for printTabsPrecedingStatement and terminateSTATEMENT
#include "helpers.h" // for concat
#include "cppType.h" // for cppTypeTYPE, and nameTableContains
#include "symbol.h" // for Hash, nameTable, and putName
#include "memory.h"

FILE* emitFILE;

// the head of the linked list of structs/arrays we discoverred in the C++ typing phase
extern CPPTYPE* head;

int boundsCheckVarNo = 1;

void addTypeDefs(CPPTYPE* c) {
    if (c == NULL) return;
    switch (c->kind) {
        case cppArrayK:
            fprintf(emitFILE, "typedef ");
            genCPPTYPE(c->val.arrayT.elementType);
            fprintf(emitFILE, " %s[", c->val.arrayT.name);
            fprintf(emitFILE, "%d];", c->val.arrayT.size);
            newLineInFile(emitFILE);
            break;
        case cppStructK:
            fprintf(emitFILE, "typedef struct ");
            genSTRUCTTYPE(c->val.structT.structType);
            fprintf(emitFILE, " %s;", c->val.structT.name);
            newLineInFile(emitFILE);
            break;
        default:
            // will never hit this
            break;
    }
    newLineInFile(emitFILE);
    addTypeDefs(c->next);
}

void addOperators(CPPTYPE* c) {
    if (c == NULL) return;
    switch (c->kind) {
        case cppArrayK:
            genArrayComparator(c);
            break;
        case cppStructK:
            genStructComparator(c, 0);
            newLineInFile(emitFILE);
            genStructComparator(c, 1);
            break;
        default:
            break;
    }
    newLineInFile(emitFILE);
    addOperators(c->next);
}

void addHeaderCode() {
   FILE* headerFILE;
   char c;

   headerFILE = fopen("./src/headercode.cpp", "r");

   while ((c = fgetc(headerFILE)) != EOF) {
       fputc(c, emitFILE);
   }

   fclose(headerFILE);
}

char* getBoundsCheckVarName() {
    char number[100]; // more than we need
    char* boundsCheckVarName;
    while (1) {
        sprintf(number, "%d", boundsCheckVarNo);
        boundsCheckVarNo++;
        boundsCheckVarName = concat("boundsVar_", number);
        if (!nameTableContains(boundsCheckVarName)) {
            break;
        }
    }
    return boundsCheckVarName;
}

void addToReservedNames(char* name) {
    int i = Hash(name);
    ID* id = NEW(ID);
    id->name = name;
    id->next = reservedNames[i];
    reservedNames[i] = id;
}

void initReservedNames() {
    int i;
    for (i = 0; i < HashSize; i++) reservedNames[i] = NULL;
    addToReservedNames("alignas");
    addToReservedNames("alignof");
    addToReservedNames("and");
    addToReservedNames("and_eq");
    addToReservedNames("asm");
    addToReservedNames("atomic_cancel");
    addToReservedNames("atomic_commit");
    addToReservedNames("atomic_noexcept");
    addToReservedNames("auto");
    addToReservedNames("bitand");
    addToReservedNames("bitor");
    addToReservedNames("bool");
    addToReservedNames("break");
    addToReservedNames("case");
    addToReservedNames("catch");
    addToReservedNames("char");
    addToReservedNames("char16_t");
    addToReservedNames("char32_t");
    addToReservedNames("class");
    addToReservedNames("compl");
    addToReservedNames("concept");
    addToReservedNames("const");
    addToReservedNames("constexpr");
    addToReservedNames("const_cast");
    addToReservedNames("continue");
    addToReservedNames("decltype");
    addToReservedNames("default");
    addToReservedNames("default");
    addToReservedNames("delete");
    addToReservedNames("do");
    addToReservedNames("double");
    addToReservedNames("dynamic_cast");
    addToReservedNames("else");
    addToReservedNames("enum");
    addToReservedNames("explicit");
    addToReservedNames("export");
    addToReservedNames("extern");
    addToReservedNames("false");
    addToReservedNames("float");
    addToReservedNames("for");
    addToReservedNames("friend");
    addToReservedNames("goto");
    addToReservedNames("if");
    addToReservedNames("import");
    addToReservedNames("inline");
    addToReservedNames("int");
    addToReservedNames("long");
    addToReservedNames("module");
    addToReservedNames("mutable");
    addToReservedNames("namespace");
    addToReservedNames("new");
    addToReservedNames("noexcept");
    addToReservedNames("not");
    addToReservedNames("not_eq");
    addToReservedNames("nullptr");
    addToReservedNames("operator");
    addToReservedNames("or");
    addToReservedNames("or_eq");
    addToReservedNames("private");
    addToReservedNames("protected");
    addToReservedNames("public");
    addToReservedNames("register");
    addToReservedNames("reinterpret_cast");
    addToReservedNames("requires");
    addToReservedNames("return");
    addToReservedNames("short");
    addToReservedNames("signed");
    addToReservedNames("sizeof");
    addToReservedNames("static");
    addToReservedNames("static_assert");
    addToReservedNames("static_cast");
    addToReservedNames("struct");
    addToReservedNames("switch");
    addToReservedNames("synchronized");
    addToReservedNames("template");
    addToReservedNames("this");
    addToReservedNames("thread_local");
    addToReservedNames("throw");
    addToReservedNames("true");
    addToReservedNames("try");
    addToReservedNames("typedef");
    addToReservedNames("typeid");
    addToReservedNames("typename");
    addToReservedNames("union");
    addToReservedNames("unsigned");
    addToReservedNames("using");
    addToReservedNames("virtual");
    addToReservedNames("void");
    addToReservedNames("volatile");
    addToReservedNames("wchar_t");
    addToReservedNames("while");
    addToReservedNames("xor");
    addToReservedNames("xor_eq");
}

int isReserved(char* name) {
    int i = Hash(name);
    ID* id;
    for (id = reservedNames[i]; id; id = id->next) {
        if (strcmp(name, id->name) == 0) return 1;
    }
    return 0;
}

char* findReplacementName(char* name) {
    int i = Hash(name);
    REPLACEMENTID* id = replacementNames[i];
    for (id = replacementNames[i]; id; id = id->next) {
        if (strcmp(name, id->name) == 0) {
            return id->replacementName;
        }
    }
    return NULL;
}

char* generateReplacementName(char* name) {
    char number[100]; // more than we need
    char* replacementName;
    int i = 1;
    while (1) {
        sprintf(number, "%d", i);
        i++;
        replacementName = concat(name, number);
        if (!nameTableContains(replacementName)) {
            break;
        }
    }
    return replacementName;
}

void putReplacementName(char* name, char* replacementName) {
    int i = Hash(name);
    REPLACEMENTID* id = NEW(REPLACEMENTID);
    id->name = name;
    id->replacementName = replacementName;
    id->next = replacementNames[i];
    replacementNames[i] = id;
}

char* getReplacementName(char* name) {
    char* replacementName = findReplacementName(name);
    if (replacementName != NULL) return replacementName;
    // generate a replacement name, add it to replacementNames, and return it
    replacementName = generateReplacementName(name);
    // add the replacement name to the name table
    putName(replacementName);
    // add the replacement name to replacementNames
    putReplacementName(name, replacementName);
    return replacementName;
}

char* getOutputName(char* name) {
    // first, check that the name is not a keyword
    if (isReserved(name)) {
        return getReplacementName(name);
    }
    return name;
}

void genPROGRAM(PROGRAM* p, char* fname) {
    emitFILE = fopen(fname, "w");
    // first, add the constant header code
    addHeaderCode();
    newLineInFile(emitFILE);
    // next, add the struct and array typedef declarations
    fprintf(emitFILE, "// typedefs");
    newLineInFile(emitFILE);
    newLineInFile(emitFILE);
    addTypeDefs(head);
    newLineInFile(emitFILE);
    // then, add the comparison operators for structs
    fprintf(emitFILE, "// operators");
    newLineInFile(emitFILE);
    newLineInFile(emitFILE);
    addOperators(head);
    newLineInFile(emitFILE);
    // next, create the reservedNames hashmap
    initReservedNames();
    // next, traverse the AST, writing each line's equivalent C++ code to emitFILE
    // we completely ignore the package declaration
    fprintf(emitFILE, "// code");
    newLineInFile(emitFILE);
    newLineInFile(emitFILE);
    genTOPLEVELDECLARATION(p->topLevelDeclaration);
    // close emitFILE
    fclose(emitFILE);
}

void genTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld) {
    if (tld == NULL) return;
    switch (tld->kind) {
        case vDeclK:
            genVARDECLARATION(tld->val.varDeclTLD, 0);
            break;
        case tDeclK:
            // we don't do anything here! the cppType phase took care of this
            break;
        case functionDeclK:
            genFUNCTIONDECLARATION(tld->val.functionDeclTLD);
            break;
    }
    newLineInFile(emitFILE);
    genTOPLEVELDECLARATION(tld->next);
}

void genVARDECLARATION(VARDECLARATION* vd, int level) {
    if (vd == NULL) return;
    if (vd->isEmpty) return;
    genVARDECLARATIONlist(vd, level);
    genVARDECLARATION(vd->nextDistributed, level);
}

/*
 * starting at the current line, moves the rw-pointer level tabs in and prints
 * a variable declaration there. Follows it with a semicolon and a new line
 */
void genVARDECLARATIONlist(VARDECLARATION* vd, int level) {
    if (vd == NULL) return;
    if (!vd->isBlank) {
        // print any array index checks if need be!
        genArrayChecks(vd->arrayIndex, level);
        printTabsToFile(level, emitFILE);
        switch (vd->kind) {
            case typeOnlyK:
                // need to have a function that assigns defaults depending on the type!
                genCPPTYPE(vd->val.typeVD->cppType);
                fprintf(emitFILE, " %s = ", getOutputName(vd->id->name));
                genDefault(vd->val.typeVD->cppType, level);
                break;
            case expOnlyK:
                // get the type from the expression
                genCPPTYPE(vd->val.expVD.exp->type->cppType);
                fprintf(emitFILE, " %s = ", getOutputName(vd->id->name));
                genEXP(vd->val.expVD.exp);
                break;
            case typeAndExpK:
                // easy --> you already have all the information you need
                genCPPTYPE(vd->val.typeAndExpVD.type->cppType);
                fprintf(emitFILE, " %s = ", getOutputName(vd->id->name));
                genEXP(vd->val.typeAndExpVD.exp);
                break;
        }
        // finish it up with a semicolon and a new line
        fprintf(emitFILE, ";");
        newLineInFile(emitFILE);
    }
    genVARDECLARATIONlist(vd->next, level);
}

/*
 * prints tabs and then series of array checks, ending in a new line
 */
void genArrayChecks(ARRAYINDEX* a, int level) {
    if (a == NULL) return;
    printTabsToFile(level, emitFILE);
    fprintf(emitFILE, "int %s = GOLITE_CHECK_BOUNDS(%d, ", getBoundsCheckVarName(), a->maxIndex - 1);
    genEXP(a->indexExp);
    fprintf(emitFILE, ");");
    newLineInFile(emitFILE);
    genArrayChecks(a->next, level);
}

void genFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd) {
    // print the function signature
    if (isMain(fd)) {
        fprintf(emitFILE, "int main() {");
    } else {
        if (fd->returnType == NULL) {
            fprintf(emitFILE, "void ");
        } else {
            genCPPTYPE(fd->returnType->cppType);
            fprintf(emitFILE, " ");
        }
        fprintf(emitFILE, "%s(", getOutputName(fd->id->name));
        genPARAMETER(fd->parameters);
        fprintf(emitFILE, ") {");
    }
    // print the function body
    newLineInFile(emitFILE);
    genSTATEMENT(fd->statements, 1, 1, 0);
    // close off the function body
    fprintf(emitFILE, "}");
    newLineInFile(emitFILE);
    newLineInFile(emitFILE);
}

void genPARAMETER(PARAMETER* p) {
    if (p == NULL) return;
    genPARAMETERlist(p);
    if (p->nextParamSet != NULL) {
        fprintf(emitFILE, ", ");
        genPARAMETER(p->nextParamSet);
    }
}

void genPARAMETERlist(PARAMETER* p) {
    if (p == NULL) return;
    genCPPTYPE(p->type->cppType);
    fprintf(emitFILE, " %s", getOutputName(p->id->name));
    if (p->nextId != NULL) {
        fprintf(emitFILE, ", ");
        genPARAMETERlist(p->nextId);
    }
}

/*
 * if semicolon is 1, we print a semicolon after the statement
 * if startAtRwPointer is 1, we do not print tabs prior to the statement
 * if level == -1, then we print the statement in line
 */
void genSTATEMENT(STATEMENT* s, int level, int semicolon, int startAtRwPointer) {
    if (s == NULL) return;
    switch (s->kind) {
        case emptyK:
            // nothing to do
            break;
        case expK:
            genArrayChecks(s->arrayIndex, level);
            printTabsPrecedingStatement(level, startAtRwPointer);
            genEXP(s->val.expS);
            terminateSTATEMENT(level, semicolon);
            break;
        case incK:
            genArrayChecks(s->arrayIndex, level);
            printTabsPrecedingStatement(level, startAtRwPointer);
            genEXP(s->val.incS);
            fprintf(emitFILE, "++");
            terminateSTATEMENT(level, semicolon);
            break;
        case decK:
            genArrayChecks(s->arrayIndex, level);
            printTabsPrecedingStatement(level, startAtRwPointer);
            genEXP(s->val.decS);
            fprintf(emitFILE, "--");
            terminateSTATEMENT(level, semicolon);
            break;
        case regAssignK:
            genArrayChecks(s->arrayIndex, level);
            if (s->val.regAssignS.isBlank) {
                // if the exp is a function call, then we should print it
                // otherwise, it is useless
                if (s->val.regAssignS.exp->kind == argumentsK) {
                    printTabsPrecedingStatement(level, startAtRwPointer);
                    genEXP(s->val.regAssignS.exp);
                    terminateSTATEMENT(level, semicolon);
                }
            } else {
                printTabsPrecedingStatement(level, startAtRwPointer);
                genEXP(s->val.regAssignS.lvalue);
                fprintf(emitFILE, " = ");
                genEXP(s->val.regAssignS.exp);
                terminateSTATEMENT(level, semicolon);
            }
            genSTATEMENT(s->val.regAssignS.next, level, semicolon, startAtRwPointer);
            break;
        case binOpAssignK:
            genArrayChecks(s->arrayIndex, level);
            printTabsPrecedingStatement(level, startAtRwPointer);
            genBinOp(s);
            terminateSTATEMENT(level, semicolon);
            break;
        case shortDeclK:
            genArrayChecks(s->arrayIndex, level);
            if (!s->val.shortDeclS.isBlank) {
                printTabsPrecedingStatement(level, startAtRwPointer);
                if (s->val.shortDeclS.isRedecl) {
                    // just print the name and exp
                    fprintf(emitFILE, "%s = ", getOutputName(s->val.shortDeclS.id->val.idE.id->name));
                    genEXP(s->val.shortDeclS.exp);
                } else {
                    // print the C++ type and then the name and exp
                    genCPPTYPE(s->val.shortDeclS.exp->type->cppType);
                    fprintf(emitFILE, " %s = ", getOutputName(s->val.shortDeclS.id->val.idE.id->name));
                    genEXP(s->val.shortDeclS.exp);
                }
                terminateSTATEMENT(level, semicolon);
            }
            genSTATEMENT(s->val.shortDeclS.next, level, semicolon, startAtRwPointer);
            break;
        case varDeclK:
            genVARDECLARATION(s->val.varDeclS, level);
            break;
        case typeDeclK:
            // nothing to do
            break;
        case printK:
            if (s->val.printS != NULL) {
                genArrayChecks(s->arrayIndex, level);
                printTabsPrecedingStatement(level, startAtRwPointer);
                fprintf(emitFILE, "cout");
                genPrintEXPs(s->val.printS);
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case printlnK:
            genArrayChecks(s->arrayIndex, level);
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "cout");
            genPrintlnEXPs(s->val.printlnS);
            fprintf(emitFILE, " << endl;");
            newLineInFile(emitFILE);
            break;
        case returnK:
            genArrayChecks(s->arrayIndex, level);
            printTabsPrecedingStatement(level, startAtRwPointer);
            if (s->val.returnS != NULL) {
                fprintf(emitFILE, "return ");
                genEXP(s->val.returnS);
            } else {
                fprintf(emitFILE, "return");
            }
            terminateSTATEMENT(level, semicolon);
            break;
        case ifK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            // we need to put a scope around this if statement
            fprintf(emitFILE, "{");
            newLineInFile(emitFILE);
            genArrayChecks(s->arrayIndex, level + 1);
            genSTATEMENT(s->val.ifS.initStatement, level + 1, 1, 0);
            printTabsToFile(level + 1, emitFILE);
            fprintf(emitFILE, "if (");
            genEXP(s->val.ifS.condition);
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            genSTATEMENT(s->val.ifS.body, level + 2, 1, 0);
            printTabsToFile(level + 1, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            break;
        case ifElseK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            // we'll need to scope this
            fprintf(emitFILE, "{");
            newLineInFile(emitFILE);
            genArrayChecks(s->arrayIndex, level + 1);
            genSTATEMENT(s->val.ifElseS.initStatement, level + 1, 1, 0);
            printTabsToFile(level + 1, emitFILE);
            fprintf(emitFILE, "if (");
            genEXP(s->val.ifElseS.condition);
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            genSTATEMENT(s->val.ifElseS.thenPart, level + 2, 1, 0);
            printTabsToFile(level + 1, emitFILE);
            fprintf(emitFILE, "} else ");
            if (s->val.ifElseS.elsePart) {
                if (s->val.ifElseS.elsePart->kind == ifK || s->val.ifElseS.elsePart->kind == ifElseK) {
                    // if/else
                    // print at the same level, with a trailing semicolon
                    // and starting at rw-pointer
                    genSTATEMENT(s->val.ifElseS.elsePart, level + 1, 1, 1);
                } else {
                    // else block
                    fprintf(emitFILE, "{");
                    newLineInFile(emitFILE);
                    genSTATEMENT(s->val.ifElseS.elsePart, level + 2, 1, 0);
                    printTabsToFile(level + 1, emitFILE);
                    fprintf(emitFILE, "}");
                    newLineInFile(emitFILE);
                }
            } else {
                // empty else
                fprintf(emitFILE, "{");
                newLineInFile(emitFILE);
                printTabsToFile(level + 1, emitFILE);
                fprintf(emitFILE, "}");
                newLineInFile(emitFILE);
            }
            // close the outer scope
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            break;
        case switchK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            // put this in a block
            fprintf(emitFILE, "{");
            newLineInFile(emitFILE);
            genArrayChecks(s->arrayIndex, level + 1);
            genSTATEMENT(s->val.switchS.initStatement, level + 1, 1, 0);
            genSWITCHCASE(s->val.switchS.cases, s->val.switchS.condition, level + 1);
            // close the block
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            break;
        case whileK:
            genArrayChecks(s->arrayIndex, level + 1);
            printTabsPrecedingStatement(level, startAtRwPointer);
            // no surrounding block needed here
            fprintf(emitFILE, "while (");
            genEXP(s->val.whileS.condition);
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            // gen the body
            genSTATEMENT(s->val.whileS.body, level + 1, 1, 0);
            // close off the loop
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            break;
        case infiniteLoopK:
            genArrayChecks(s->arrayIndex, level + 1);
            printTabsPrecedingStatement(level, startAtRwPointer);
            // gen this as a while(true)
            fprintf(emitFILE, "while (true) {");
            newLineInFile(emitFILE);
            genSTATEMENT(s->val.infiniteLoopS, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            break;
        case forK:
            /*
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "for (");
            // print the init statement inline, but do not terminate with a semicolon
            // in case the init statement is null
            genSTATEMENT(s->val.forS.initStatement, -1, 0, 1);
            // semicolon
            fprintf(emitFILE, "; ");
            // print the condition inline
            genEXP(s->val.forS.condition);
            // semicolon
            fprintf(emitFILE, "; ");
            // print the post statement inline, and without terminating with a semicolon
            genSTATEMENT(s->val.forS.postStatement, -1, 0, 1);
            // close of the header of the for loop
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            // statements
            genSTATEMENT(s->val.forS.body, level + 1, 1, 0);
            // close off the for loop
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            */

            printTabsPrecedingStatement(level, startAtRwPointer);
            // we'll need to scope this
            fprintf(emitFILE, "{");
            newLineInFile(emitFILE);
            genArrayChecks(s->arrayIndex, level + 1);
            // print the init statement
            genSTATEMENT(s->val.forS.initStatement, level + 1, 1, 0);
            printTabsToFile(level + 1, emitFILE);
            fprintf(emitFILE, "while (");
            // print the condition inline
            if (s->val.forS.condition == NULL) {
                fprintf(emitFILE, "true");
            } else {
                genEXP(s->val.forS.condition);
            }
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            // statements
            genSTATEMENT(s->val.forS.body, level + 2, 1, 0);
            // print the post statement
            genSTATEMENT(s->val.forS.postStatement, level + 2, 1, 0);
            // close off the for loop
            printTabsToFile(level + 1, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);

            // close off the outer scope
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            break;
        case breakK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "break");
            terminateSTATEMENT(level, semicolon);
            break;
        case continueK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "continue");
            terminateSTATEMENT(level, semicolon);
            break;
        case blockK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "{");
            newLineInFile(emitFILE);
            genSTATEMENT(s->val.blockS, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            break;
    }
    genSTATEMENT(s->next, level, 1, 0);
}

void genBinOp(STATEMENT* s) {

    // first, print the lvalue
    genEXP(s->val.binOpAssignS.lvalue);
    // then print the operator and expression
    switch (s->val.binOpAssignS.opKind) {
        case plusEqOp:
            fprintf(emitFILE, " += ");
            break;
        case minusEqOp:
            fprintf(emitFILE, " -= ");
            break;
        case timesEqOp:
            fprintf(emitFILE, " *= ");
            break;
        case divEqOp:
            fprintf(emitFILE, " /= ");
            break;
        case modEqOp:
            fprintf(emitFILE, " %%= ");
            break;
        case andEqOp:
            fprintf(emitFILE, " &= ");
            break;
        case orEqOp:
            fprintf(emitFILE, " |= ");
            break;
        case xorEqOp:
            fprintf(emitFILE, " ^= ");
            break;
        case leftShiftEqOp:
            fprintf(emitFILE, " <<= ");
            break;
        case rightShiftEqOp:
            fprintf(emitFILE, " >>= ");
            break;
        case bitClearEqOp:
            // this is the only one that is different in C++
            fprintf(emitFILE, " &= ~");
            break;
    }
    // finally, print the expression
    genEXP(s->val.binOpAssignS.exp);
}

void genPrintEXPs(EXP* exps) {
    if (exps == NULL) return;
    fprintf(emitFILE, " << ");
    genEXP(exps);
    genPrintEXPs(exps->next);
}

void genPrintlnEXPs(EXP* exps) {
    if (exps == NULL) return;
    fprintf(emitFILE, " << ");
    genEXP(exps);
    if (exps->next != NULL) {
        fprintf(emitFILE, " << \" \"");
        genPrintlnEXPs(exps->next);
    }
}

/*
 * prints switch cases as if/else statements
 * prints tabs before printing statements, and prints a new line after
 */
void genSWITCHCASE(SWITCHCASE* sc, EXP* e, int level) {
    SWITCHCASE* defaultSc = NULL;
    int firstCase = 1;
    while (sc != NULL) {
        switch (sc->kind) {
            case caseK:
                if (firstCase) {
                    printTabsToFile(level, emitFILE);
                    fprintf(emitFILE, "if (");
                    firstCase = 0;
                } else {
                    fprintf(emitFILE, " else if (");
                }
                genCaseEXPs(sc->val.caseC.exps, e);
                fprintf(emitFILE, ") {");
                newLineInFile(emitFILE);
                genSTATEMENT(sc->val.caseC.statements, level + 1, 1, 0);
                printTabsToFile(level, emitFILE);
                fprintf(emitFILE, "}");
                break;
            case defaultK:
                defaultSc = sc;
                break;
        }
        sc = sc->next;
    }

    if (defaultSc != NULL) {
        fprintf(emitFILE, " else {");
        newLineInFile(emitFILE);
        genSTATEMENT(defaultSc->val.defaultStatementsC, level + 1, 1, 0);
        printTabsToFile(level, emitFILE);
        fprintf(emitFILE, "}");
    }

    newLineInFile(emitFILE);
}

/*
 * prints || separated list of exps being equal to condition
 * prints at the current rw-pointer and does not print anything after
 */
void genCaseEXPs(EXP* exps, EXP* condition) {
    if (exps == NULL) return;
    if (condition != NULL) {
        genEXP(condition);
        fprintf(emitFILE, " == ");
    }
    genEXP(exps);
    if (exps->next != NULL) {
        fprintf(emitFILE, " || ");
        genCaseEXPs(exps->next, condition);
    }
}


/*
 * generates the expression in C++ at the current rw-pointer
 * does not print a semicolon or new line after
 */
void genEXP(EXP* e) {
    if (e == NULL) return; // need this because for and switch expressions could be null
    switch (e->kind) {
        case identifierK:
            if (e->val.idE.leaveNameAsIs) {
                fprintf(emitFILE, "%s", e->val.idE.id->name);
            } else {
                fprintf(emitFILE, "%s", getOutputName(e->val.idE.id->name));
            }
            break;
        case intLiteralK:
            fprintf(emitFILE, "%d", e->val.intLiteralE.decValue);
            break;
        case floatLiteralK:
            fprintf(emitFILE, "%f", e->val.floatLiteralE);
            break;
        case runeLiteralK:
            fprintf(emitFILE, "%s", e->val.runeLiteralE);
            break;
        case stringLiteralK:
            fprintf(emitFILE, "%s", e->val.stringLiteralE);
            break;
        case rawStringLiteralK:
            fprintf(emitFILE, "%s", rawify(e->val.rawStringLiteralE));
            break;
        case uPlusK:
            fprintf(emitFILE, "(");
            fprintf(emitFILE, "+");
            genEXP(e->val.uPlusE);
            fprintf(emitFILE, ")");
            break;
        case uMinusK:
            fprintf(emitFILE, "(");
            fprintf(emitFILE, "-");
            genEXP(e->val.uMinusE);
            fprintf(emitFILE, ")");
            break;
        case uNotK:
            fprintf(emitFILE, "(");
            fprintf(emitFILE, "!");
            genEXP(e->val.uNotE);
            fprintf(emitFILE, ")");
            break;
        case uXorK:
            fprintf(emitFILE, "(");
            fprintf(emitFILE, "~");
            genEXP(e->val.uXorE);
            fprintf(emitFILE, ")");
            break;
        case plusK:
            if (e->val.plusE.stringAddition) {
                fprintf(emitFILE, "concat(");
                genEXP(e->val.plusE.left);
                fprintf(emitFILE, ", ");
                genEXP(e->val.plusE.right);
                fprintf(emitFILE, ")");
            } else {
                fprintf(emitFILE, "(");
                genEXP(e->val.plusE.left);
                fprintf(emitFILE, "+");
                genEXP(e->val.plusE.right);
                fprintf(emitFILE, ")");
            }
            break;
        case minusK:
            fprintf(emitFILE, "(");
            genEXP(e->val.minusE.left);
            fprintf(emitFILE, "-");
            genEXP(e->val.minusE.right);
            fprintf(emitFILE, ")");
            break;
        case timesK:
            fprintf(emitFILE, "(");
            genEXP(e->val.timesE.left);
            fprintf(emitFILE, "*");
            genEXP(e->val.timesE.right);
            fprintf(emitFILE, ")");
            break;
        case divK:
            fprintf(emitFILE, "(");
            genEXP(e->val.divE.left);
            fprintf(emitFILE, "/");
            genEXP(e->val.divE.right);
            fprintf(emitFILE, ")");
            break;
        case modK:
            fprintf(emitFILE, "(");
            genEXP(e->val.modE.left);
            fprintf(emitFILE, "%%");
            genEXP(e->val.modE.right);
            fprintf(emitFILE, ")");
            break;
        case bitwiseOrK:
            fprintf(emitFILE, "(");
            genEXP(e->val.bitwiseOrE.left);
            fprintf(emitFILE, "|");
            genEXP(e->val.bitwiseOrE.right);
            fprintf(emitFILE, ")");
            break;
        case bitwiseAndK:
            fprintf(emitFILE, "(");
            genEXP(e->val.bitwiseAndE.left);
            fprintf(emitFILE, "&");
            genEXP(e->val.bitwiseAndE.right);
            fprintf(emitFILE, ")");
            break;
        case xorK:
            fprintf(emitFILE, "(");
            genEXP(e->val.xorE.left);
            fprintf(emitFILE, "^");
            genEXP(e->val.xorE.right);
            fprintf(emitFILE, ")");
            break;
        case ltK:
            if (e->val.ltE.stringCompare) {
                fprintf(emitFILE, "(strcmp(");
                genEXP(e->val.ltE.left);
                fprintf(emitFILE, ", ");
                genEXP(e->val.ltE.right);
                fprintf(emitFILE, ")");
                fprintf(emitFILE, " < 0)");
            } else {
                fprintf(emitFILE, "(");
                genEXP(e->val.ltE.left);
                fprintf(emitFILE, "<");
                genEXP(e->val.ltE.right);
                fprintf(emitFILE, ")");
            }
            break;
        case gtK:
            if (e->val.gtE.stringCompare) {
                fprintf(emitFILE, "(strcmp(");
                genEXP(e->val.gtE.left);
                fprintf(emitFILE, ", ");
                genEXP(e->val.gtE.right);
                fprintf(emitFILE, ")");
                fprintf(emitFILE, " > 0)");
            } else {
                fprintf(emitFILE, "(");
                genEXP(e->val.gtE.left);
                fprintf(emitFILE, ">");
                genEXP(e->val.gtE.right);
                fprintf(emitFILE, ")");
            }
            break;
        case eqK:
            if (e->val.eqE.stringCompare) {
                fprintf(emitFILE, "(strcmp(");
                genEXP(e->val.eqE.left);
                fprintf(emitFILE, ", ");
                genEXP(e->val.eqE.right);
                fprintf(emitFILE, ")");
                fprintf(emitFILE, " == 0)");
            } else if (e->val.eqE.left->type->cppType->kind == cppArrayK) {
                fprintf(emitFILE, "cmp_%s(", e->val.eqE.left->type->cppType->val.arrayT.name);
                genEXP(e->val.eqE.left);
                fprintf(emitFILE, ", ");
                genEXP(e->val.eqE.right);
                fprintf(emitFILE, ")");
            } else {
                fprintf(emitFILE, "(");
                genEXP(e->val.eqE.left);
                fprintf(emitFILE, "==");
                genEXP(e->val.eqE.right);
                fprintf(emitFILE, ")");
            }
            break;
        case neqK:
            if (e->val.neqE.stringCompare) {
                fprintf(emitFILE, "(strcmp(");
                genEXP(e->val.neqE.left);
                fprintf(emitFILE, ", ");
                genEXP(e->val.neqE.right);
                fprintf(emitFILE, ")");
                fprintf(emitFILE, " != 0)");
            } else if (e->val.neqE.left->type->cppType->kind == cppArrayK) {
                fprintf(emitFILE, "!cmp_%s(", e->val.neqE.left->type->cppType->val.arrayT.name);
                genEXP(e->val.neqE.left);
                fprintf(emitFILE, ", ");
                genEXP(e->val.neqE.right);
                fprintf(emitFILE, ")");
            } else {
                fprintf(emitFILE, "(");
                genEXP(e->val.neqE.left);
                fprintf(emitFILE, "!=");
                genEXP(e->val.neqE.right);
                fprintf(emitFILE, ")");
            }
            break;
        case leqK:
            if (e->val.leqE.stringCompare) {
                fprintf(emitFILE, "(strcmp(");
                genEXP(e->val.leqE.left);
                fprintf(emitFILE, ", ");
                genEXP(e->val.leqE.right);
                fprintf(emitFILE, ")");
                fprintf(emitFILE, " <= 0)");
            } else {
                fprintf(emitFILE, "(");
                genEXP(e->val.leqE.left);
                fprintf(emitFILE, "<=");
                genEXP(e->val.leqE.right);
                fprintf(emitFILE, ")");
            }
            break;
        case geqK:
            if (e->val.geqE.stringCompare) {
                fprintf(emitFILE, "(strcmp(");
                genEXP(e->val.geqE.left);
                fprintf(emitFILE, ", ");
                genEXP(e->val.geqE.right);
                fprintf(emitFILE, ")");
                fprintf(emitFILE, " >= 0)");
            } else {
                fprintf(emitFILE, "(");
                genEXP(e->val.geqE.left);
                fprintf(emitFILE, ">=");
                genEXP(e->val.geqE.right);
                fprintf(emitFILE, ")");
            }
            break;
        case orK:
            fprintf(emitFILE, "(");
            genEXP(e->val.orE.left);
            fprintf(emitFILE, "||");
            genEXP(e->val.orE.right);
            fprintf(emitFILE, ")");
            break;
        case andK:
            fprintf(emitFILE, "(");
            genEXP(e->val.andE.left);
            fprintf(emitFILE, "&&");
            genEXP(e->val.andE.right);
            fprintf(emitFILE, ")");
            break;
        case leftShiftK:
            fprintf(emitFILE, "(");
            genEXP(e->val.leftShiftE.left);
            fprintf(emitFILE, "<<");
            genEXP(e->val.leftShiftE.right);
            fprintf(emitFILE, ")");
            break;
        case rightShiftK:
            fprintf(emitFILE, "(");
            genEXP(e->val.rightShiftE.left);
            fprintf(emitFILE, ">>");
            genEXP(e->val.rightShiftE.right);
            fprintf(emitFILE, ")");
            break;
        case bitClearK:
            fprintf(emitFILE, "(");
            genEXP(e->val.bitClearE.left);
            fprintf(emitFILE, "& ~");
            genEXP(e->val.bitClearE.right);
            fprintf(emitFILE, ")");
            break;
        case appendK:
            fprintf(emitFILE, "golite_slice_append(");
            genEXP(e->val.appendE.slice);
            fprintf(emitFILE, ", ");
            genEXP(e->val.appendE.expToAppend);
            fprintf(emitFILE, ")");
            break;
        case selectorK:
            genEXP(e->val.selectorE.receiver->receivingStruct);
            fprintf(emitFILE, ".");
            prettyID(e->val.selectorE.lastSelector);
            break;
        case indexK:
            /*
            if (e->val.indexE.arrayType != NULL) {
                // add an array bound check to head.cpp
                fprintf(headFILE, "GOLITE_CHECK_BOUNDS(%d, ")
            }
            */
            genEXP(e->val.indexE.rest);
            fprintf(emitFILE, "[");
            genEXP(e->val.indexE.lastIndex);
            fprintf(emitFILE, "]");
            break;
        case argumentsK:
            genEXP(e->val.argumentsE.rest);
            fprintf(emitFILE, "(");
            genEXPs(e->val.argumentsE.args);
            fprintf(emitFILE, ")");
            break;
        case castK:
            fprintf(emitFILE, "(");
            // first, we have to C++ type the type of the cast
            cppTypeTYPE(e->val.castE->type);
            // now we can print the C++ type
            genCPPTYPE(e->val.castE->type->cppType);
            fprintf(emitFILE, ")");
            genEXP(e->val.castE->exp);
            break;
    }
}

void genEXPs(EXP* e) {
    if (e == NULL) return;
    if (e->next == NULL) {
        // just print the exp
        genEXP(e);
    } else {
        // print the exp, a comma, and a space
        genEXP(e);
        fprintf(emitFILE, ", ");
        // recurse
        genEXPs(e->next);
    }
}

void genCPPTYPE(CPPTYPE* c) {
    switch (c->kind) {
        case cppIntK:
            fprintf(emitFILE, "int");
            break;
        case cppDoubleK:
            fprintf(emitFILE, "double");
            break;
        case cppBoolK:
            fprintf(emitFILE, "bool");
            break;
        case cppCharK:
            fprintf(emitFILE, "char");
            break;
        case cppStringK:
            fprintf(emitFILE, "char*");
            break;
        case cppArrayK:
            fprintf(emitFILE, "%s", c->val.arrayT.name);
            break;
        case cppVectorK:
            fprintf(emitFILE, "vector<");
            genCPPTYPE(c->val.vectorT);
            fprintf(emitFILE, ">");
            break;
        case cppStructK:
            fprintf(emitFILE, "%s", c->val.structT.name);
            break;
    }
}

void genSTRUCTTYPE(STRUCTTYPE* s) {
    fprintf(emitFILE, "{");
    newLineInFile(emitFILE);
    // print the fields a level in
    genFIELD(s->fields, 1);
    fprintf(emitFILE, "}");
}

void genFIELD(FIELD* f, int level) {
    if (f == NULL) return;
    genFIELDlist(f, level);
    genFIELD(f->nextFieldSet, level);
}

void genFIELDlist(FIELD* f, int level) {
    if (f == NULL) return;
    printTabsToFile(level, emitFILE);
    genCPPTYPE(f->type->cppType);
    fprintf(emitFILE, " %s;", getOutputName(f->id->name));
    newLineInFile(emitFILE);
    genFIELDlist(f->nextId, level);
}

void genStructComparator(CPPTYPE* c, int inequality) {
    if (inequality) {
        fprintf(emitFILE, "bool operator!=(const %s& lhs, const %s& rhs) {", c->val.structT.name, c->val.structT.name);
    } else {
        fprintf(emitFILE, "bool operator==(const %s& lhs, const %s& rhs) {", c->val.structT.name, c->val.structT.name);
    }
    newLineInFile(emitFILE);
    printTabsToFile(1, emitFILE);
    fprintf(emitFILE, "return ");
    if (c->val.structT.structType->fields == NULL) {
        if (inequality) {
            fprintf(emitFILE, "false");
        } else {
            fprintf(emitFILE, "true");
        }
    } else {
        if (inequality) {
            genSTRUCTTYPEcomparison(c->val.structT.structType, 1);
        } else {
            genSTRUCTTYPEcomparison(c->val.structT.structType, 0);
        }
    }
    fprintf(emitFILE, ";");
    newLineInFile(emitFILE);
    fprintf(emitFILE, "}");
    newLineInFile(emitFILE);
}

void genSTRUCTTYPEcomparison(STRUCTTYPE* s, int inequality) {
    genFIELDcomparison(s->fields, inequality);
}

void genFIELDcomparison(FIELD* f, int inequality) {
    if (f == NULL) return;
    genFIELDlistComparison(f, inequality);
    genFIELDcomparison(f->nextFieldSet, inequality);
}

void genFIELDlistComparison(FIELD* f, int inequality) {
    if (f == NULL) return;
    genComparison(f->type->cppType, getOutputName(f->id->name), inequality);
    if (f->nextId != NULL || f->nextFieldSet != NULL) {
        if (inequality) {
            fprintf(emitFILE, " || ");
        } else {
            fprintf(emitFILE, " && ");
        }
    }
    genFIELDlistComparison(f->nextId, inequality);
}

/*
 * generate comparison code depending on the C++ type
 */
void genComparison(CPPTYPE* c, char* name, int inequality) {
    switch (c->kind) {
        case cppStringK:
            if (inequality) {
                fprintf(emitFILE, "strcmp(lhs.%s, rhs.%s) != 0", name, name);
            } else {
                fprintf(emitFILE, "strcmp(lhs.%s, rhs.%s) == 0", name, name);
            }
            break;
        default:
            if (inequality) {
                fprintf(emitFILE, "lhs.%s != rhs.%s", name, name);
            } else {
                fprintf(emitFILE, "lhs.%s == rhs.%s", name, name);
            }
            break;
    }
}

void genArrayComparator(CPPTYPE* c) {
    fprintf(emitFILE, "bool cmp_%s(%s lhs, %s rhs) {", c->val.arrayT.name, c->val.arrayT.name, c->val.arrayT.name);
    newLineInFile(emitFILE);
    printTabsToFile(1, emitFILE);
    fprintf(emitFILE, "int i;");
    newLineInFile(emitFILE);
    printTabsToFile(1, emitFILE);
    fprintf(emitFILE, "for (i = 0; i < %d; i++) {", c->val.arrayT.size);
    newLineInFile(emitFILE);
    printTabsToFile(2, emitFILE);
    fprintf(emitFILE, "if (");
    genArrayElementComparison(c->val.arrayT.elementType);
    fprintf(emitFILE, ") {");
    newLineInFile(emitFILE);
    printTabsToFile(3, emitFILE);
    fprintf(emitFILE, "return false;");
    newLineInFile(emitFILE);
    printTabsToFile(2, emitFILE);
    fprintf(emitFILE, "}");
    newLineInFile(emitFILE);
    printTabsToFile(1, emitFILE);
    fprintf(emitFILE, "}");
    newLineInFile(emitFILE);
    printTabsToFile(1, emitFILE);
    fprintf(emitFILE, "return true;");
    newLineInFile(emitFILE);
    fprintf(emitFILE, "}");
    newLineInFile(emitFILE);
}

void genArrayElementComparison(CPPTYPE* type) {
    switch (type->kind) {
        case cppStringK:
            fprintf(emitFILE, "strcmp(lhs[i], rhs[i]) != 0");
            break;
        default:
            fprintf(emitFILE, "lhs[i] != rhs[i]");
            break;
    }
}

// HELPERS

/*
 * returns 1 if the function declaration is for the main function (program entry point)
 * and 0 otherwise
 */
int isMain(FUNCTIONDECLARATION* fd) {
    // the name must be main
    if (strcmp(fd->id->name, "main") != 0) {
        return 0;
    }
    // there can be no parameters
    if (fd->parameters != NULL) {
        return 0;
    }
    // the return can only be void or int
    if (fd->returnType != NULL) {
        if (fd->returnType->kind != intK) {
            return 0;
        }
    }
    return 1;
}

void genDefault(CPPTYPE* c, int level) {
    switch (c->kind) {
        case cppIntK:
            fprintf(emitFILE, "0");
            break;
        case cppDoubleK:
            fprintf(emitFILE, "0.0");
            break;
        case cppBoolK:
            fprintf(emitFILE, "0");
            break;
        case cppCharK:
            fprintf(emitFILE, "0");
            break;
        case cppStringK:
            fprintf(emitFILE, "\"\"");
            break;
        case cppArrayK:
            if (c->val.arrayT.size == 0) {
                fprintf(emitFILE, "{}");
                return;
            }
            fprintf(emitFILE, "{");
            newLineInFile(emitFILE);
            printTabsToFile(level + 1, emitFILE);
            genDefault(c->val.arrayT.elementType, level + 1);
            newLineInFile(emitFILE);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            break;
        case cppVectorK:
            fprintf(emitFILE, "vector<");
            genCPPTYPE(c->val.vectorT);
            fprintf(emitFILE, ">()");
            break;
        case cppStructK:
            genDefaultSTRUCTTYPE(c->val.structT.structType, level);
            break;
    }
}

void genDefaultSTRUCTTYPE(STRUCTTYPE* s, int level) {
    fprintf(emitFILE, "{");
    newLineInFile(emitFILE);
    // print the fields a level in
    genDefaultFIELD(s->fields, level + 1);
    printTabsToFile(level, emitFILE);
    fprintf(emitFILE, "}");
}

void genDefaultFIELD(FIELD* f, int level) {
    if (f == NULL) return;
    genDefaultFIELDlist(f, level);
    genDefaultFIELD(f->nextFieldSet, level);
}

void genDefaultFIELDlist(FIELD* f, int level) {
    if (f == NULL) return;
    printTabsToFile(level, emitFILE);
    fprintf(emitFILE, ".%s = ", getOutputName(f->id->name));
    genDefault(f->type->cppType, level);
    if (f->nextId != NULL || f->nextFieldSet != NULL) {
        fprintf(emitFILE, ",");
    }
    newLineInFile(emitFILE);
    genFIELDlist(f->nextId, level);
}

/*
 * adds a backslash before every backslash in s and returns the resulting string
 */
char* rawify(char* s) {
    int count = 0;
    int i;
    for (i = 0; i < strlen(s); i++) {
        if (s[i] == 'a') {
            count++;
        }
    }
    char* newStr = malloc(strlen(s) + count + 1); // +1 for null terminator

    int j;
    for (i = 0, j = 0; i < strlen(s); i++, j++) {
        if (s[i] == '`') {
            newStr[j] = '"';
        } else {
            newStr[j] = s[i];
            if (s[i] == '\\') {
                newStr[j+1] = '\\';
                j++;
            }
        }
    }
    newStr[j] = '\0';

    return newStr;
}

/*
 * generates the default value a variable is assigned to based on its type
 * prints the value at the current rw-pointer; does not print a new line after
 */
/*
void genTYPE(TYPE* t) {
    switch (t->kind) {
        case intK:
            fprintf(emitFILE, "int");
            break;
        case float64K:
            fprintf(emitFILE, "double");
            break;
        case runeK:
            fprintf(emitFILE, "char");
            break;
        case boolK:
            fprintf(emitFILE, "bool");
            break;
        case stringK:
            fprintf(emitFILE, "char*");
            break;
        case idK:
            genTYPE(t->val.idT.underlyingType);
            break;
        case structK:
            break;
        case sliceK:
            fprintf(emitFILE, "vector<");
            genTYPE(t->val.sliceT);
            fprintf(emitFILE, ">");
            break;
        case arrayK:
            break;
    }
}

void genDefault(TYPE* t) {
    switch (t->kind) {
        case intK:
            fprintf(emitFILE, "0");
            break;
        case float64K:
            fprintf(emitFILE, "0.0");
            break;
        case runeK:
            fprintf(emitFILE, "0");
            break;
        case boolK:
            fprintf(emitFILE, "0");
            break;
        case stringK:
            fprintf(emitFILE, "\"\"");
            break;
        case idK:
            genDefault(t->val.idT.underlyingType);
            break;
        case structK:
            break;
        case sliceK:
            fprintf(emitFILE, "vector<");
            genTYPE(t->val.sliceT);
            fprintf(emitFILE, ">()");
            break;
        case arrayK:
            break;
    }
}
*/
