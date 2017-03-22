#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "codegen.h"
#include "outputhelpers.h"

FILE* emitFILE;

void addHeaderCode() {
   FILE* headerFILE;
   char c;

   // TODO make this not hard-coded
   headerFILE = fopen("./src/headercode.cpp", "r");

   while ((c = fgetc(headerFILE)) != EOF) {
       fputc(c, emitFILE);
   }

   fclose(headerFILE);
}

void genPROGRAM(PROGRAM* p, char* fname) {
    emitFILE = fopen(fname, "w");
    addHeaderCode();
    newLineInFile(emitFILE);
    // TODO first, we need to make a pass that finds and aggregates structs and arrays
    // and then makes typedefs for them at the top of the file

    // next, traverse the AST, writing each line's equivalent C++ code to emitFILE
    // we completely ignore the package declaration
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
            genTYPEDECLARATION(tld->val.typeDeclTLD, 0);
            break;
        case functionDeclK:
            genFUNCTIONDECLARATION(tld->val.functionDeclTLD);
            break;
    }
    genTOPLEVELDECLARATION(tld->next);
}

void genVARDECLARATION(VARDECLARATION* vd, int level) {
    if (vd == NULL) return;
    genVARDECLARATIONlist(vd, level);
    genVARDECLARATION(vd->nextDistributed, level);
}

/*
 * starting at the current line, moves the rw-pointer level tabs in and prints
 * a variable declaration there. Follows it with a semicolon and a new line
 */
void genVARDECLARATIONlist(VARDECLARATION* vd, int level) {
    if (vd == NULL) return;
    printTabsToFile(emitFILE, level);
    switch (vd->kind) {
        case typeOnlyK:
            // need to have a function that assigns defaults depending on the type!
            genTYPE(vd->val.typeVD);
            fprintf(emitFILE, " %s = ", vd->id->name);
            genDefault(vd->val.typeVD);
            break;
        case expOnlyK:
            // get the type from the expression
            genTYPE(vd->val.expVD.exp->type);
            fprintf(emitFILE, " %s = ", vd->id->name);
            genEXP(vd->val.expVD.exp);
            break;
        case typeAndExpK:
            // easy --> you already have all the information you need
            genTYPE(vd->val.typeAndExpVD.type);
            fprintf(emitFILE, " %s = ", vd->id->name);
            genEXP(vd->val.typeAndExpVD.exp);
            break;
    }
    // finish it up with a semicolon and a new line
    fprintf(emitFILE, ";");
    newLineInFile(emitFILE);
    genVARDECLARATIONlist(vd->next, level);
}

void genTYPEDECLARATION(TYPEDECLARATION* td, int level) {

}

void genFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd) {
    // print the function signature
    if (isMain(fd)) {
        fprintf(emitFILE, "int main() {");
    } else {
        if (fd->returnType == NULL) {
            fprintf(emitFILE, "void ");
        } else {
            genTYPE(fd->returnTYPE);
        }
        fprintf(emitFILE, "%s(", fd->id->name);
        genPARAMETER(fd->parameters);
        fprintf(emitFILE, ") {");
    }
    // print the function body
    newLineInFile(emitFILE);
    genSTATEMENT(fd->statements, 1);
    // close off the function body
    fprintf(emitFILE, "}");
    newLineInFile(emitFILE);
    newLineInFile(emitFILE);
}

void genPARAMETER(PARAMETER* p) {
    if (p == NULL) return;
    genPARAMETERlist(p);
    if (p->nextParamSet != NULL) {
        fprintf(emitFILE, " ,");
        genPARAMETER(p->nextParamSet);
    }
}

void genPARAMETERlist(PARAMETER* p) {
    if (p == NULL) return;
    genTYPE(p->type);
    fprintf(emitFILE, " %s", p->id->name);
    if (p->nextId != NULL) {
        fprintf(emitFILE, ", ");
        genPARAMETERlist(p->nextId);
    }
}

/*
 * outputs the type at the current read/write pointer
 * does not print a new line after
 */
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
            // TODO seems we need to use the underlying type here!!!
            genTYPE(t->val.idT.underlyingType);
            break;
        case structK:
            // TODO seems we need to perform a pass through the entire program first to collect structs
            // so that we can declare them before the functions (determine where exactly to declare them!).
            // Also, if two structs have identical fields, then it seems we just declare the struct once
            // (though honestly we probably don't have to do this - declaring two structs with the same fields
            // but different names is probably fine)
            break;
        case sliceK:
            fprintf(emitFILE, "vector<");
            genTYPE(t->val.sliceT);
            fprintf(emitFILE, ">");
            break;
        case arrayK:
            // TODO we need to traverse the program first and find any arrays and declare a type for them
            break;
    }
}

/*
 * generates the expression in C++ at the current rw-pointer
 * does not print a semicolon or new line after
 */
void genEXP(EXP* e) {

}

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

/*
 * generates the default value a variable is assigned to based on its type
 * prints the value at the current rw-pointer; does not print a new line after
 */
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
            // TODO seems we need to use the underlying type here!!!
            genDefault(t->val.idT.underlyingType);
            break;
        case structK:
            // TODO seems we need to perform a pass through the entire program first to collect structs
            // so that we can declare them before the functions (determine where exactly to declare them!).
            // Also, if two structs have identical fields, then it seems we just declare the struct once
            // (though honestly we probably don't have to do this - declaring two structs with the same fields
            // but different names is probably fine)
            break;
        case sliceK:
            fprintf(emitFILE, "vector<");
            genTYPE(t->val.sliceT);
            fprintf(emitFILE, ">()");
            break;
        case arrayK:
            // TODO we need to traverse the program first and find any arrays and declare a type for them
            break;
    }
}
