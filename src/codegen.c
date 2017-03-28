#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "codegen.h"
#include "outputhelpers.h"
#include "pretty.h" // for printTabsPrecedingStatement and terminateSTATEMENT
#include "cppType.h" // for cppTypeTYPE

FILE* emitFILE;

// the head of the lined list of structs/arrays we discoverred in the C++ typing phase
extern CPPTYPE* head;

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
    addTypeDefs(c->next);
}

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
    // first, add the constant header code
    addHeaderCode();
    newLineInFile(emitFILE);
    // next, add the struct and array typedef declarations
    fprintf(emitFILE, "// typedefs");
    newLineInFile(emitFILE);
    addTypeDefs(head);
    newLineInFile(emitFILE);
    // next, traverse the AST, writing each line's equivalent C++ code to emitFILE
    // we completely ignore the package declaration
    fprintf(emitFILE, "// code");
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
    printTabsToFile(level, emitFILE);
    switch (vd->kind) {
        case typeOnlyK:
            // need to have a function that assigns defaults depending on the type!
            genCPPTYPE(vd->val.typeVD->cppType);
            fprintf(emitFILE, " %s = ", vd->id->name);
            genDefault(vd->val.typeVD->cppType, level);
            break;
        case expOnlyK:
            // get the type from the expression
            genCPPTYPE(vd->val.expVD.exp->type->cppType);
            fprintf(emitFILE, " %s = ", vd->id->name);
            genEXP(vd->val.expVD.exp);
            break;
        case typeAndExpK:
            // easy --> you already have all the information you need
            genCPPTYPE(vd->val.typeAndExpVD.type->cppType);
            fprintf(emitFILE, " %s = ", vd->id->name);
            genEXP(vd->val.typeAndExpVD.exp);
            break;
    }
    // finish it up with a semicolon and a new line
    fprintf(emitFILE, ";");
    newLineInFile(emitFILE);
    genVARDECLARATIONlist(vd->next, level);
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
        fprintf(emitFILE, "%s(", fd->id->name);
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
    fprintf(emitFILE, " %s", p->id->name);
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
            printTabsPrecedingStatement(level, startAtRwPointer);
            genEXP(s->val.expS);
            terminateSTATEMENT(level, semicolon);
            break;
        case incK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            genEXP(s->val.incS);
            fprintf(emitFILE, "++");
            terminateSTATEMENT(level, semicolon);
            break;
        case decK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            genEXP(s->val.decS);
            fprintf(emitFILE, "--");
            terminateSTATEMENT(level, semicolon);
            break;
        case regAssignK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            genEXP(s->val.regAssignS.lvalue);
            fprintf(emitFILE, " = ");
            genEXP(s->val.regAssignS.exp);
            terminateSTATEMENT(level, semicolon);
            genSTATEMENT(s->val.regAssignS.next, level, semicolon, startAtRwPointer);
            break;
        case binOpAssignK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            // print the lvalue
            genEXP(s->val.binOpAssignS.lvalue);
            // print the operator
            //genBinOp(s->val.binOpAssignS.opKind);
            // print the exp
            genEXP(s->val.binOpAssignS.exp);
            terminateSTATEMENT(level, semicolon);
            break;
        case shortDeclK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            if (s->val.shortDeclS.isRedecl) {
                // just print the name and exp
                fprintf(emitFILE, "%s = ", s->val.shortDeclS.id->val.idE.id->name);
                genEXP(s->val.shortDeclS.exp);
            } else {
                // print the C++ type and then the name and exp
                genCPPTYPE(s->val.shortDeclS.exp->type->cppType);
                fprintf(emitFILE, " %s = ", s->val.shortDeclS.id->val.idE.id->name);
                genEXP(s->val.shortDeclS.exp);
            }
            terminateSTATEMENT(level, semicolon);
            genSTATEMENT(s->val.shortDeclS.next, level, semicolon, startAtRwPointer);
            break;
        case varDeclK:
            genVARDECLARATION(s->val.varDeclS, level);
            break;
        case typeDeclK:
            // nothing to do
            break;
        case printK:
            // TODO
            /*
            printTabsPrecedingStatement(level, startAtRwPointer);

            terminateSTATEMENT(level, semicolon);
            */
            break;
        case printlnK:
            // TODO
            /*
            printTabsPrecedingStatement(level, startAtRwPointer);

            terminateSTATEMENT(level, semicolon);
            */
            break;
        case returnK:
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
            genSTATEMENT(s->val.switchS.initStatement, level + 1, 1, 0);
            // print the switch cases within the block
            printTabsToFile(level + 1, emitFILE);
            // TODO this will be weird due to the fact that a case can have multiple expressions
            // we'll want to gen this as an if/else instead (default case is else!)
            /*
            fprintf(emitFILE, "switch (");
            genEXP(s->val.switchS.condition);
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            genSWITCHCASE(s, level + 2);
            printTabsToFile(level + 1, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            */
            // close the block
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            newLineInFile(emitFILE);
            break;
        case whileK:
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
            printTabsPrecedingStatement(level, startAtRwPointer);
            // TODO figure out what is allowed and where init statement(s) should be printed
            // (probably in a new scope outside the for)
            /*
            fprintf(emitFILE, "for (");
            // print the init statement in line
            genSTATEMENT(s->val.forS.initStatement, level + 1, 1, 0);
            */
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

void genSWITCHCASE(SWITCHCASE* sc, int level) {
    if (sc == NULL) return;
    printTabsToFile(level, emitFILE);
    switch (sc->kind) {
        case caseK:

            break;
        case defaultK:
            break;
    }
    genSWITCHCASE(sc->next, level);
}


/*
 * generates the expression in C++ at the current rw-pointer
 * does not print a semicolon or new line after
 */
void genEXP(EXP* e) {
    switch (e->kind) {
        case identifierK:
            fprintf(emitFILE, "%s", e->val.idE.id->name);
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
            // TODO
            /*
            fprintf(emitFILE, "(");
            fprintf(emitFILE, "^");
            genEXP(e->val.uXorE);
            fprintf(emitFILE, ")");
            */
            break;
        case plusK:
            fprintf(emitFILE, "(");
            genEXP(e->val.plusE.left);
            fprintf(emitFILE, "+");
            genEXP(e->val.plusE.right);
            fprintf(emitFILE, ")");
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
            // TODO modify to support more types
            fprintf(emitFILE, "(");
            genEXP(e->val.ltE.left);
            fprintf(emitFILE, "<");
            genEXP(e->val.ltE.right);
            fprintf(emitFILE, ")");
            break;
        case gtK:
            // TODO modify to support more types
            fprintf(emitFILE, "(");
            genEXP(e->val.gtE.left);
            fprintf(emitFILE, ">");
            genEXP(e->val.gtE.right);
            fprintf(emitFILE, ")");
            break;
        case eqK:
            // TODO modify to support more types
            fprintf(emitFILE, "(");
            genEXP(e->val.eqE.left);
            fprintf(emitFILE, "==");
            genEXP(e->val.eqE.right);
            fprintf(emitFILE, ")");
            break;
        case neqK:
            // TODO modify to support more types
            fprintf(emitFILE, "(");
            genEXP(e->val.neqE.left);
            fprintf(emitFILE, "!=");
            genEXP(e->val.neqE.right);
            fprintf(emitFILE, ")");
            break;
        case leqK:
            // TODO modify to support more types
            fprintf(emitFILE, "(");
            genEXP(e->val.leqE.left);
            fprintf(emitFILE, "<=");
            genEXP(e->val.leqE.right);
            fprintf(emitFILE, ")");
            break;
        case geqK:
            // TODO modify to support more types
            fprintf(emitFILE, "(");
            genEXP(e->val.geqE.left);
            fprintf(emitFILE, ">=");
            genEXP(e->val.geqE.right);
            fprintf(emitFILE, ")");
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
            // TODO
            /*
            fprintf(emitFILE, "(");
            genEXP(e->val.bitClearE.left);
            fprintf(emitFILE, "&^");
            genEXP(e->val.bitClearE.right);
            fprintf(emitFILE, ")");
            */
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
    fprintf(emitFILE, " %s;", f->id->name);
    newLineInFile(emitFILE);
    genFIELDlist(f->nextId, level);
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
    fprintf(emitFILE, ".%s = ", f->id->name);
    genDefault(f->type->cppType, level);
    fprintf(emitFILE, ";");
    newLineInFile(emitFILE);
    genFIELDlist(f->nextId, level);
}

char* rawify(char* s) {
    // TODO
    return "";
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
