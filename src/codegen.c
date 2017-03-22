#include <stdio.h>
#include <stdlib.h>
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
    // traverse the AST, writing each line's equivalent C++ code to emitFILE
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

}

void genTYPEDECLARATION(TYPEDECLARATION* td, int level) {

}

void genFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd) {
    /*
     We need to recognize the main function! If there is a function called main that has no arguments
     and either no return type or int return type, then this is main :)
    */
    if (isMain(fd)) {

    }
}

/*
 * returns 1 if the function declaration is for the main function (program entry point)
 * and 0 otherwise
 */
int isMain(FUNCTIONDECLARATION* fd) {

}
