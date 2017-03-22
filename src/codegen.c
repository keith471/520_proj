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

}

/*
 We need to recognize the main function! If there is a function called main that has no arguments
 and either no return type or int return type, then this is main :) We should mark this function
 in the type phase

 TODO mark main function!!
*/
