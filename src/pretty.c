/*
 * Reconstructs the original program from the AST
 */

#include <stdio.h>
#include "pretty.h"
#include "outputhelpers.h"

// the number of times to indent before printing
int numIndents = 0;
FILE* emitFILE;

void prettyPROGRAM(PROGRAM *p, char* filePath) {
    // set the global file handle
    emitFILE = fopen(filePath, "w");
    prettyPACKAGE(p->package);
    newLineInFile(emitFILE);
    newLineInFile(emitFILE);
    prettyTOPLEVELDECLARATION(p->topLevelDeclaration);
    fclose(emitFILE);
}

void prettyPACKAGE(PACKAGE* package) {
    fprintf(emitFILE, "package %s;", package->name);
}

void prettyTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld) {
    // if null, stop, else print and then recurse on tld->next
    if (tld == NULL) return;
    switch (ltd->kind) {
        case varDeclK:
            prettyVARDECLARATION(tld->val.varDeclTLD, 0);
            break;
        case typeDeclK:
            prettyVARDECLARATION(tld->val.typeDeclTLD, 0);
            break;
        case functionDeclK:
            prettyFUNCTIONDECLARATION(tld->val.functionDeclTLD);
            break;
    }
    newLineInFile(emitFILE);
    prettyTOPLEVELDECLARATION(tld->next);
}

// can be at any level
void prettyVARDECLARATION(VARDECLARATION* vd, int level) {
    printTabsToFile(level, emitFILE);
    fprintf(emitFILE, "var ");
    if (vd->next) {
        // distributed var decl
        fprintf(emitFILE, "(");
        newLineInFile(emitFILE);
        prettyVARDECLARATIONdistributedbody(vd, level + 1);
        printTabsToFile(level, emitFILE);
        fprintf(");");
    } else {
        prettyVARDECLARATIONsingleline(vd);
    }
    newLineInFile(emitFILE);
}

void prettyVARDECLARATIONdistributedbody(VARDECLARATION* vd, int level) {
    if (vd == NULL) return;
    printTabsToFile(level, emitFILE);
    prettyVARDECLARATIONsingleline(vd);
    newLineInFile(emitFILE);
    // recurse
    prettyVARDECLARATIONdistributedbody(vd->next, level);
}

/*
 * prints a single-line variable declaration
 * does not print tabs before or a new line after
 */
void prettyVARDECLARATIONsingleline(VARDECLARATION* vd) {
    prettyID(vd->ids);
    switch (vd->kind) {
        case typeOnlyK:
            fprintf(emitFILE, " ");
            prettyTYPE(vd->val.typeVD);
            break;
        case expOnlyK:
            fprintf(emitFILE, " = ");
            prettyEXPlist(vd->val.expVD);
            break;
        case typeAndExpK:
            fprintf(emitFILE, " ");
            prettyTYPE(vd->val.typeAndExpVD.type);
            fprintf(emitFILE, " = ");
            prettyEXPlist(vd->val.typeAndExpVD.exp);
            break;
    }
    fprintf(emitFILE, ";");
}

/*
 * prints comma-separated list of ids on the same line, at the current rw-pointer location.
 * Does not print a new line after.
 * Does not print tabs first.
 */
void prettyID(ID* id) {
    if (id == NULL) return;
    if (id->next == NULL) {
        // just print the id
        fprintf(emitFILE, "%s", id->name);
    } else {
        // print the id, a comma, and a space
        fprintf(emitFILE, "%s, ", id->name);
        // recurse
        prettyID(id->next);
    }
}

/*
 * a type declaration
 * type declarations have an identifier and a type
 */
 typedef struct TYPEDECLARATION {
     int lineno;
     struct ID* id;
     int isDistributed; // whether this declaration is part of a distributed statement
     struct TYPE* type;
     struct TYPEDECLARATION* next; // for distributed type declarations; else this is null
 } TYPEDECLARATION;

// can be at any level
void prettyTYPEDECLARATION(TYPEDECLARATION* td, int level) {

}

// can only be at top level
void prettyFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd) {

}

// prints type at the current rw-pointer location.
// does not print a new line
void prettyTYPE(TYPE* t) {

}

// prints exp at the current rw-pointer location.
// does not print a new line
void prettyEXP(EXP* exp) {
    // print the single expression
}

void prettyEXPlist(EXP* exp) {
    if (exp == NULL) return;
    if (exp->next == NULL) {
        // just print the exp
        prettyEXP(exp);
    } else {
        // print the exp, a comma, and a space
        prettyEXP(exp);
        fprintf(emitFILE, ", ");
        // recurse
        prettyEXPlist(exp->next);
    }
}
