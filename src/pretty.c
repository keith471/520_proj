/*
 * Reconstructs the original program from the AST
 */

#include <stdio.h>
#include "pretty.h"
#include "outputhelpers.h"

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
    switch (tld->kind) {
        case vDeclK:
            prettyVARDECLARATION(tld->val.varDeclTLD, 0);
            break;
        case tDeclK:
            prettyTYPEDECLARATION(tld->val.typeDeclTLD, 0);
            break;
        case functionDeclK:
            prettyFUNCTIONDECLARATION(tld->val.functionDeclTLD);
            break;
    }
    // print an extra new line for white space separation
    newLineInFile(emitFILE);
    prettyTOPLEVELDECLARATION(tld->next);
}

/*
 * can be at any level
 * prints a new line and semicolon after
 */
void prettyVARDECLARATION(VARDECLARATION* vd, int level) {
    if (vd == NULL) return;
    printTabsToFile(level, emitFILE);
    fprintf(emitFILE, "var ");
    if (vd->isEmpty) {
        fprintf(emitFILE, "();");
    } else if (vd->isDistributed) {
        // distributed var decl
        fprintf(emitFILE, "(");
        newLineInFile(emitFILE);
        prettyVARDECLARATIONdistributedbody(vd, level + 1);
        printTabsToFile(level, emitFILE);
        fprintf(emitFILE, ");");
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
    prettyVARDECLARATIONdistributedbody(vd->nextDistributed, level);
}

/*
 * prints a single-line variable declaration
 * does not print tabs before or a new line after, but does print a semicolon
 */
void prettyVARDECLARATIONsingleline(VARDECLARATION* vd) {
    prettyID(vd->id); // we could go to vd->next to get the next id in the variable declaration,
                        // or we could just cheat and use the linked-list of ids pointed to by vd->id!
                        // we cheat to keep things simple.
    switch (vd->kind) {
        case typeOnlyK:
            fprintf(emitFILE, " ");
            prettyTYPE(vd->val.typeVD, -1);
            break;
        case expOnlyK:
            fprintf(emitFILE, " = ");
            prettyEXPs(vd->val.expVD);
            break;
        case typeAndExpK:
            fprintf(emitFILE, " ");
            prettyTYPE(vd->val.typeAndExpVD.type, -1);
            fprintf(emitFILE, " = ");
            prettyEXPs(vd->val.typeAndExpVD.exp);
            break;
        default:
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
 * a type declaration can be at any level
 * this prints the type declaration, followed by a new line
 */
void prettyTYPEDECLARATION(TYPEDECLARATION* td, int level) {
    if (td == NULL) return;
    printTabsToFile(level, emitFILE);
    fprintf(emitFILE, "type ");
    if (td->isEmpty) {
        fprintf(emitFILE, "();");
    } else if (td->isDistributed) {
        // distributed type decl
        fprintf(emitFILE, "(");
        newLineInFile(emitFILE);
        prettyTYPEDECLARATIONdistributedbody(td, level + 1);
        printTabsToFile(level, emitFILE);
        fprintf(emitFILE, ");");
    } else {
        prettyTYPEDECLARATIONsingle(td, level);
    }
    newLineInFile(emitFILE);
}

/*
 * prints the body of a distributed type declaration, followed by a new line
 */
void prettyTYPEDECLARATIONdistributedbody(TYPEDECLARATION* td, int level) {
    if (td == NULL) return;
    printTabsToFile(level, emitFILE);
    prettyTYPEDECLARATIONsingle(td, level);
    newLineInFile(emitFILE);
    // recurse
    prettyTYPEDECLARATIONdistributedbody(td->nextDistributed, level);
}

/*
 * prints the body of a single type declaration, starting at the current rw-pointer location
 * does NOT print a new line after, but does print a semicolon
 */
void prettyTYPEDECLARATIONsingle(TYPEDECLARATION* td, int level) {
    prettyID(td->id);
    fprintf(emitFILE, " ");
    prettyTYPE(td->type, level);
    fprintf(emitFILE, ";");
}

/*
 * pretty prints the type starting at the current rw-pointer location
 * does not print a new line after
 * does not print a semicolon after
 * if level == -1, prints the type inline
 */
void prettyTYPE(TYPE* type, int level) {
    if (type == NULL) return;
    switch (type->kind) {
        case idK:
            prettyID(type->val.idT.id);
            break;
        case structK:
            if (level == -1) {
                // print inline
                fprintf(emitFILE, "struct { ");
                prettyFIELD(type->val.structT->fields, level);
                fprintf(emitFILE, " }");
            } else {
                // print multiline
                fprintf(emitFILE, "struct {");
                newLineInFile(emitFILE);
                prettyFIELD(type->val.structT->fields, level + 1);
                printTabsToFile(level, emitFILE);
                fprintf(emitFILE, "}");
            }
            break;
        case sliceK:
            fprintf(emitFILE, "[]");
            // recurse to print the type of the elements in the slice
            prettyTYPE(type->val.sliceT, level);
            break;
        case arrayK:
            fprintf(emitFILE, "[");
            prettyEXPs(type->val.arrayT.size);
            fprintf(emitFILE, "]");
            // recurse to print the type of the elements in the array
            prettyTYPE(type->val.arrayT.elementType, level);
            break;
        case intK:
            break;
        case float64K:
            break;
        case runeK:
            break;
        case boolK:
            break;
        case stringK:
            break;
        default:
            break;
    }
}

/*
 * pretty print the fields of a struct
 * must print tabs first
 * prints new line after
 * if level == -1, prints inline rather than a new line per field
 */
void prettyFIELD(FIELD* field, int level) {
    if (field == NULL) return;
    if (level == -1) {
        // print inline
        prettyID(field->id); // again, we cheat
        fprintf(emitFILE, " ");
        prettyTYPE(field->type, level);
        fprintf(emitFILE, ";");
    } else {
        // print multiline
        printTabsToFile(level, emitFILE);
        prettyID(field->id); // again, we cheat
        fprintf(emitFILE, " ");
        prettyTYPE(field->type, level);
        fprintf(emitFILE, ";");
        newLineInFile(emitFILE);
    }
    // recurse
    prettyFIELD(field->nextFieldSet, level);
}

/*
 * function declarations can only be at top level, so we don't need a level parameter
 * prints the function declaration, followed by a semicolon and new line
 */
void prettyFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd) {
    fprintf(emitFILE, "func ");
    prettyID(fd->id);
    fprintf(emitFILE, "(");
    prettyPARAMETER(fd->parameters);
    fprintf(emitFILE, ") ");
    // print the return type inline
    if (fd->returnType) {
        prettyTYPE(fd->returnType, -1);
        fprintf(emitFILE, " {");
    } else {
        fprintf(emitFILE, "{");
    }
    newLineInFile(emitFILE);
    // begin printing the statements at level 1 and with a trailing semicolon
    prettySTATEMENT(fd->statements, 1, 1, 0);
    fprintf(emitFILE, "};");
    newLineInFile(emitFILE);
}

/*
 * prints the statement at the given level
 * optionally follows it with a semicolon and new line
 * if level == -1, then prints it inline (no tabs and no new line)
 * else, prints the tabs and terminates the line with a semicolon and new line
 * if level == -1 and semicolon == 0, then prints inline with no trailing semicolon
 * it is also possible that we are not printing inline but we want to print from
 * the current rw-pointer rather than first printing tabs. Thus, we also have the startAtRwPointer flag
 */
void prettySTATEMENT(STATEMENT* s, int level, int semicolon, int startAtRwPointer) {
    if (s == NULL) return;
    switch (s->kind) {
        case emptyK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            terminateSTATEMENT(level, semicolon);
            break;
        case expK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            prettyEXPs(s->val.expS);
            terminateSTATEMENT(level, semicolon);
            break;
        case incK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            prettyEXPs(s->val.incS);
            fprintf(emitFILE, "++");
            terminateSTATEMENT(level, semicolon);
            break;
        case decK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            prettyEXPs(s->val.decS);
            fprintf(emitFILE, "--");
            terminateSTATEMENT(level, semicolon);
            break;
        case regAssignK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            // print the lvalues
            prettyEXPs(s->val.regAssignS.lvalue); // we cheat: instead of iterating
                                                    // to s->val.regAssignS.next, we just
                                                    // rely on the fact that we know
                                                    // s->val.regAssignS.lvalue gives
                                                    // us a list of all the lvalues
            // print the equals
            fprintf(emitFILE, " = ");
            // print the exps
            prettyEXPs(s->val.regAssignS.exp); // cheat
            terminateSTATEMENT(level, semicolon);
            break;
        case binOpAssignK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            // print the lvalue
            prettyEXPs(s->val.binOpAssignS.lvalue);
            // print the equals
            prettyBinOp(s->val.binOpAssignS.opKind);
            // print the exp
            prettyEXPs(s->val.binOpAssignS.exp);
            terminateSTATEMENT(level, semicolon);
            break;
        case shortDeclK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            prettyEXPs(s->val.shortDeclS.id); // cheat
            fprintf(emitFILE, " := ");
            prettyEXPs(s->val.shortDeclS.exp); // cheat
            terminateSTATEMENT(level, semicolon);
            break;
        case varDeclK:
            // this will print a semicolon and new line regardless, but that's fine
            // because we will never have a var declaration that will look unnatural
            // if on a new line, and all var declarations must end with a semicolon.
            // also, prettyVARDECLARATION already prints tabs
            prettyVARDECLARATION(s->val.varDeclS, level);
            break;
        case typeDeclK:
            // this will print a semicolon and new line regardless, but that's fine
            // for similar reasons to the above
            // also, prettyTYPEDECLARATION already prints tabs
            prettyTYPEDECLARATION(s->val.typeDeclS, level);
            break;
        case printK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "print(");
            prettyEXPs(s->val.printS);
            fprintf(emitFILE, ")");
            terminateSTATEMENT(level, semicolon);
            break;
        case printlnK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "println(");
            prettyEXPs(s->val.printlnS);
            fprintf(emitFILE, ")");
            terminateSTATEMENT(level, semicolon);
            break;
        case returnK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "return ");
            prettyEXPs(s->val.returnS);
            terminateSTATEMENT(level, semicolon);
            break;
        case ifK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "if ");
            if (s->val.ifS.initStatement) {
                // print the init statement inline and terminate it with a semicolon
                prettySTATEMENT(s->val.ifS.initStatement, -1, 1, 0);
                fprintf(emitFILE, " ");
            }
            prettyEXPs(s->val.ifS.condition);
            fprintf(emitFILE, " {");
            newLineInFile(emitFILE);
            // print the body at a level deeper and terminating with a semicolon
            prettySTATEMENT(s->val.ifS.body, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            terminateSTATEMENT(level, semicolon);
            break;
        case ifElseK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "if ");
            if (s->val.ifElseS.initStatement) {
                // print the init statement inline and terminate it with a semicolon
                prettySTATEMENT(s->val.ifElseS.initStatement, -1, 1, 0);
                fprintf(emitFILE, " ");
            }
            prettyEXPs(s->val.ifElseS.condition);
            fprintf(emitFILE, " {");
            newLineInFile(emitFILE);
            // print the body at a level deeper and terminating with a semicolon
            prettySTATEMENT(s->val.ifElseS.thenPart, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "} else ");
            if (s->val.ifElseS.elsePart) {
                if (s->val.ifElseS.elsePart->kind == ifK || s->val.ifElseS.elsePart->kind == ifElseK) {
                    // if/else
                    // print at the same level, with a trailing semicolon
                    // and starting at rw-pointer
                    prettySTATEMENT(s->val.ifElseS.elsePart, level, 1, 1);
                } else {
                    // else block
                    fprintf(emitFILE, "{");
                    newLineInFile(emitFILE);
                    prettySTATEMENT(s->val.ifElseS.elsePart, level + 1, 1, 0);
                    printTabsToFile(level, emitFILE);
                    fprintf(emitFILE, "}");
                    terminateSTATEMENT(level, semicolon);
                }
            } else {
                // empty else
                fprintf(emitFILE, "{");
                newLineInFile(emitFILE);
                printTabsToFile(level, emitFILE);
                fprintf(emitFILE, "}");
                terminateSTATEMENT(level, semicolon);
            }
            break;
        case switchK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "switch ");
            if (s->val.switchS.initStatement) {
                // print the init statement inline and terminate it with a semicolon
                prettySTATEMENT(s->val.switchS.initStatement, -1, 1, 0);
                fprintf(emitFILE, " ");
            }
            prettyEXPs(s->val.switchS.condition);
            fprintf(emitFILE, " {");
            newLineInFile(emitFILE);
            prettySWITCHCASE(s->val.switchS.cases, level + 1);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            terminateSTATEMENT(level, semicolon);
            break;
        case whileK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "for ");
            prettyEXPs(s->val.whileS.condition);
            fprintf(emitFILE, " {");
            newLineInFile(emitFILE);
            prettySTATEMENT(s->val.whileS.body, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            terminateSTATEMENT(level, semicolon);
            break;
        case infiniteLoopK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "for {");
            newLineInFile(emitFILE);
            prettySTATEMENT(s->val.infiniteLoopS, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            terminateSTATEMENT(level, semicolon);
            break;
        case forK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "for ");
            prettySTATEMENT(s->val.forS.initStatement, -1, 0, 0);
            fprintf(emitFILE, "; ");
            prettyEXPs(s->val.forS.condition);
            fprintf(emitFILE, "; ");
            prettySTATEMENT(s->val.forS.postStatement, -1, 0, 0);
            fprintf(emitFILE, " {");
            newLineInFile(emitFILE);
            prettySTATEMENT(s->val.forS.body, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            terminateSTATEMENT(level, semicolon);
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
        case blockK:
            printTabsPrecedingStatement(level, startAtRwPointer);
            fprintf(emitFILE, "{");
            newLineInFile(emitFILE);
            prettySTATEMENT(s->val.blockS, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            terminateSTATEMENT(level, semicolon);
        default:
            break;
    }
    // print the next statement
    prettySTATEMENT(s->next, level, 1, 0);
}

void terminateSTATEMENT(int level, int semicolon) {
    if (level == -1) {
        if (semicolon) {
            fprintf(emitFILE, ";");
        }
    } else {
        fprintf(emitFILE, ";");
        newLineInFile(emitFILE);
    }
}

void printTabsPrecedingStatement(int level, int startAtRwPointer) {
    if (level != -1 && !startAtRwPointer) {
        printTabsToFile(level, emitFILE);
    }
}

/*
 * print switch cases at given level and print a new line after (prettySTATEMENT prints a new line after)
 */
void prettySWITCHCASE(SWITCHCASE* s, int level) {
    if (s == NULL) return;
    printTabsToFile(level, emitFILE);
    switch (s->kind) {
        case caseK:
            fprintf(emitFILE, "case ");
            prettyEXPs(s->val.caseC.exps);
            fprintf(emitFILE, ":");
            newLineInFile(emitFILE);
            prettySTATEMENT(s->val.caseC.statements, level + 1, 1, 0);
            break;
        case defaultK:
            fprintf(emitFILE, "default:");
            newLineInFile(emitFILE);
            prettySTATEMENT(s->val.defaultStatementsC, level + 1, 1, 0);
            break;
    }
    // recurse
    prettySWITCHCASE(s->next, level);
}

/*
 * prints the bin op with a space on either side
 */
void prettyBinOp(OperationKind opKind) {
    switch (opKind) {
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
            fprintf(emitFILE, " &^= ");
            break;
    }
}

/*
 * print out function parameters at the current rw-pointer
 * do not print anything after
 */
void prettyPARAMETER(PARAMETER* param) {
    if (param == NULL) return;
    if (param->nextParamSet == NULL) {
        prettyID(param->id); // again, we "cheat"
        fprintf(emitFILE, " ");
        prettyTYPE(param->type, -1);
    } else {
        prettyID(param->id);
        fprintf(emitFILE, " ");
        prettyTYPE(param->type, -1);
        fprintf(emitFILE, ", ");
        // recurse
        prettyPARAMETER(param->nextParamSet);
    }
}

/*
 * prints the expression list at the current rw-pointer location
 * does not print a new line
 */
void prettyEXPs(EXP* exp) {
    if (exp == NULL) return;
    if (exp->next == NULL) {
        // just print the exp
        prettyEXP(exp);
    } else {
        // print the exp, a comma, and a space
        prettyEXP(exp);
        fprintf(emitFILE, ", ");
        // recurse
        prettyEXPs(exp->next);
    }
}

/*
 * pretty prints a single expression starting at the current rw-pointer location
 * does not print anything after
 */
void prettyEXP(EXP* e) {
    switch (e->kind) {
        case identifierK:
            prettyID(e->val.idE);
            break;
        case intLiteralK:
            fprintf(emitFILE, "%d", e->val.intLiteralE.decValue);
            break;
        case floatLiteralK:
            fprintf(emitFILE, "%f", e->val.floatLiteralE);
            break;
        case runeLiteralK:
            fprintf(emitFILE, "%c", e->val.runeLiteralE);
            break;
        case stringLiteralK:
            fprintf(emitFILE, "%s", e->val.stringLiteralE);
            break;
        case rawStringLiteralK:
            fprintf(emitFILE, "%s", e->val.rawStringLiteralE);
            break;
        case uPlusK:
            fprintf(emitFILE, "(");
            fprintf(emitFILE, "+");
            prettyEXP(e->val.uPlusE);
            fprintf(emitFILE, ")");
            break;
        case uMinusK:
            fprintf(emitFILE, "(");
            fprintf(emitFILE, "-");
            prettyEXP(e->val.uMinusE);
            fprintf(emitFILE, ")");
            break;
        case uNotK:
            fprintf(emitFILE, "(");
            fprintf(emitFILE, "!");
            prettyEXP(e->val.uNotE);
            fprintf(emitFILE, ")");
            break;
        case uXorK:
            fprintf(emitFILE, "(");
            fprintf(emitFILE, "^");
            prettyEXP(e->val.uXorE);
            fprintf(emitFILE, ")");
            break;
        case plusK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.plusE.left);
            fprintf(emitFILE, "+");
            prettyEXP(e->val.plusE.right);
            fprintf(emitFILE, ")");
            break;
        case minusK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.minusE.left);
            fprintf(emitFILE, "-");
            prettyEXP(e->val.minusE.right);
            fprintf(emitFILE, ")");
            break;
        case timesK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.timesE.left);
            fprintf(emitFILE, "*");
            prettyEXP(e->val.timesE.right);
            fprintf(emitFILE, ")");
            break;
        case divK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.divE.left);
            fprintf(emitFILE, "/");
            prettyEXP(e->val.divE.right);
            fprintf(emitFILE, ")");
            break;
        case modK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.modE.left);
            fprintf(emitFILE, "%%");
            prettyEXP(e->val.modE.right);
            fprintf(emitFILE, ")");
            break;
        case bitwiseOrK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.bitwiseOrE.left);
            fprintf(emitFILE, "|");
            prettyEXP(e->val.bitwiseOrE.right);
            fprintf(emitFILE, ")");
            break;
        case bitwiseAndK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.bitwiseAndE.left);
            fprintf(emitFILE, "&");
            prettyEXP(e->val.bitwiseAndE.right);
            fprintf(emitFILE, ")");
            break;
        case xorK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.xorE.left);
            fprintf(emitFILE, "^");
            prettyEXP(e->val.xorE.right);
            fprintf(emitFILE, ")");
            break;
        case ltK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.ltE.left);
            fprintf(emitFILE, "<");
            prettyEXP(e->val.ltE.right);
            fprintf(emitFILE, ")");
            break;
        case gtK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.gtE.left);
            fprintf(emitFILE, ">");
            prettyEXP(e->val.gtE.right);
            fprintf(emitFILE, ")");
            break;
        case eqK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.eqE.left);
            fprintf(emitFILE, "==");
            prettyEXP(e->val.eqE.right);
            fprintf(emitFILE, ")");
            break;
        case neqK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.neqE.left);
            fprintf(emitFILE, "!=");
            prettyEXP(e->val.neqE.right);
            fprintf(emitFILE, ")");
            break;
        case leqK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.leqE.left);
            fprintf(emitFILE, "<=");
            prettyEXP(e->val.leqE.right);
            fprintf(emitFILE, ")");
            break;
        case geqK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.geqE.left);
            fprintf(emitFILE, ">=");
            prettyEXP(e->val.geqE.right);
            fprintf(emitFILE, ")");
            break;
        case orK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.orE.left);
            fprintf(emitFILE, "||");
            prettyEXP(e->val.orE.right);
            fprintf(emitFILE, ")");
            break;
        case andK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.andE.left);
            fprintf(emitFILE, "&&");
            prettyEXP(e->val.andE.right);
            fprintf(emitFILE, ")");
            break;
        case leftShiftK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.leftShiftE.left);
            fprintf(emitFILE, "<<");
            prettyEXP(e->val.leftShiftE.right);
            fprintf(emitFILE, ")");
            break;
        case rightShiftK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.rightShiftE.left);
            fprintf(emitFILE, ">>");
            prettyEXP(e->val.rightShiftE.right);
            fprintf(emitFILE, ")");
            break;
        case bitClearK:
            fprintf(emitFILE, "(");
            prettyEXP(e->val.bitClearE.left);
            fprintf(emitFILE, "&^");
            prettyEXP(e->val.bitClearE.right);
            fprintf(emitFILE, ")");
            break;
        case appendK:
            fprintf(emitFILE, "append(");
            prettyEXP(e->val.appendE.slice);
            fprintf(emitFILE, ", ");
            prettyEXP(e->val.appendE.expToAppend);
            fprintf(emitFILE, ")");
            break;
        case selectorK:
            prettyEXP(e->val.selectorE.receiver->receivingStruct);
            /*
            switch (e->val.selectorE.rest->kind) {
                case selectorK:
                    printf("rest is a selector\n");
                    printf("%s\n", e->val.selectorE.rest->val.selectorE.lastSelector->name);
                    break;
                case indexK:
                    printf("rest is an index\n");
                    //printf("%s\n", e->val.selectorE.rest->val.indexE.lastSelector->name);
                    break;
                case idK:
                    printf("rest is an id: %s\n", e->val.selectorE.rest->val.idE->name);
                    break;
                default:
                    printf("rest is something else\n");
                    break;
            }
            */
            fprintf(emitFILE, ".");
            prettyID(e->val.selectorE.lastSelector);
            break;
        case indexK:
            prettyEXP(e->val.indexE.rest);
            fprintf(emitFILE, "[");
            prettyEXP(e->val.indexE.lastIndex);
            fprintf(emitFILE, "]");
            break;
        case argumentsK:
            prettyEXP(e->val.argumentsE.rest);
            fprintf(emitFILE, "(");
            prettyEXPs(e->val.argumentsE.args);
            fprintf(emitFILE, ")");
            break;
        case castK:
            prettyTYPE(e->val.castE->type, -1);
            fprintf(emitFILE, "(");
            prettyEXP(e->val.castE->exp);
            fprintf(emitFILE, ")");
            break;
    }
}
