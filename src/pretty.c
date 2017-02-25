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
    printTabsToFile(level, emitFILE);
    fprintf(emitFILE, "var ");
    if (vd->isDistributed) {
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
 * does not print tabs before or a new line after, but does print a semicolon
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
            prettyEXP(vd->val.expVD);
            break;
        case typeAndExpK:
            fprintf(emitFILE, " ");
            prettyTYPE(vd->val.typeAndExpVD.type);
            fprintf(emitFILE, " = ");
            prettyEXP(vd->val.typeAndExpVD.exp);
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
    printTabsToFile(level, emitFILE);
    fprintf(emitFILE, "type ");
    if (td->isDistributed) {
        // distributed type decl
        fprintf(emitFILE, "(");
        newLineInFile(emitFILE);
        prettyTYPEDECLARATIONdistributedbody(td, level + 1);
        printTabsToFile(level, emitFILE);
        fprintf(");");
    } else {
        prettyTYPEDECLARATIONsingle(td);
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
    prettyTYPEDECLARATIONdistributedbody(td->next, level);
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
    switch (type->kind) {
        case idK:
            prettyID(type->val.idT);
            break;
        case structK:
            if (level == -1) {
                // print inline
                fprintf(emitFILE, "struct { ");
                prettyFIELD(type->val.structT, level);
                fprintf(emitFILE, " }");
            } else {
                // print multiline
                fprintf(emitFILE, "struct {");
                newLineInFile(emitFILE);
                prettyFIELD(type->val.structT, level + 1);
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
            prettyEXP(type->val.arrayT.size);
            fprintf(emitFILE, "]");
            // recurse to print the type of the elements in the array
            prettyTYPE(type->val.arrayT.elementType, level);
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
        prettyID(field->ids);
        fprintf(emitFILE, " ");
        prettyTYPE(field->type, level);
        fprintf(emifFILE, ";");
    } else {
        // print multiline
        printTabsToFile(level, emitFILE);
        prettyID(field->ids);
        fprintf(emitFILE, " ");
        prettyTYPE(field->type, level);
        fprintf(emifFILE, ";");
        newLineInFile(emitFILE);
    }
    // recurse
    prettyFIELD(field->next, level);
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
    prettyTYPE(fd->returnType, -1);
    fprintf(emitFILE, " {");
    newLineInFile(emitFILE);
    // begin printing the statements at level 1 and with a trailing semicolon
    prettySTATEMENT(fd->statements, 1, 1, 0);
    fprintf(emitFILE, "};")
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
    if (level != -1 && !startAtRwPointer) {
        printTabsToFile(level, emitFILE);
    }
    switch (s->kind) {
        case emptyK:
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case expK:
            prettyEXP(s->val.expS);
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case incK:
            prettyEXP(s->val.incS);
            fprintf(emitFILE, "++");
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case decK:
            prettyEXP(s->val.decS);
            fprintf(emitFILE, "--");
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case regAssignK:
            // print the svalues
            prettyEXP(s->val.regAssignS.lvalues);
            // print the equals
            fprintf(emitFILE, " = ");
            // print the exps
            prettyEXP(s->val.regAssignS.exps);
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case binOpAssignK:
            // print the lvalue
            prettyEXP(s->val.binOpAssignS.lvalue);
            // print the equals
            prettyBinOp(s->val.binOpAssignS.opKind);
            // print the exp
            prettyEXP(s->val.binOpAssignS.exp);
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case shortDeclK:
            prettyEXP(s->val.shortDeclS.ids);
            fprintf(emitFILE, " := ");
            prettyEXP(s->val.shortDeclS.exps);
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case varDeclK:
            // this will print a semicolon and new line regardless, but that's fine
            // because we will never have a var declaration that will look unnatural
            // if on a new line, and all var declarations must end with a semicolon
            prettyVARDECLARATION(s->val.varDeclS, level);
            break;
        case typeDeclK:
            // this will print a semicolon and new line regardless, but that's fine
            // for similar reasons to the above
            prettyTYPEDECLARATION(s->val.typeDeclS, level);
            break;
        case printK:
            fprintf(emitFILE, "print(");
            prettyEXP(s->val.printS);
            fprintf(emitFILE, ")");
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case printlnK:
            fprintf(emitFILE, "println(");
            prettyEXP(s->val.printlnS);
            fprintf(emitFILE, ")");
            newLineInFile(emitFILE);
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case returnK:
            fprintf(emitFILE, "return ");
            prettyEXP(s->val.returnS);
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case ifK:
            fprintf(emitFILE, "if (");
            if (s->val.ifS.initStatement) {
                // print the init statement inline and terminate it with a semicolon
                prettySTATEMENT(s->val.ifS.initStatement, -1, 1, 0);
                fprintf(emitFILE, " ");
            }
            prettyEXP(s->val.ifS.condition);
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            // print the body at a level deeper and terminating with a semicolon
            prettySTATEMENT(s->val.ifS.body, level + 1, 1);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case ifElseK:
            fprintf(emitFILE, "if (");
            if (s->val.ifElseS.initStatement) {
                // print the init statement inline and terminate it with a semicolon
                prettySTATEMENT(s->val.ifElseS.initStatement, -1, 1, 0);
                fprintf(emitFILE, " ");
            }
            prettyEXP(s->val.ifElseS.condition);
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            // print the body at a level deeper and terminating with a semicolon
            prettySTATEMENT(s->val.ifElseS.thenPart, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "} else ");
            if (s->val.ifElseS.elsePart->kind == ifK || s->val.ifElseS.elsePart->kind == ifElseK) {
                // if/else
                // print at the same level, no trailing semicolon (to avoid duplicates),
                // and starting at rw-pointer
                prettySTATEMENT(s->val.ifElseS.elsePart, level, 0, 1);
            } else {
                // else block
                fprintf(emitFILE, "{");
                newLineInFile(emitFILE);
                prettySTATEMENT(s->val.ifElseS.elsePart, level + 1, 1, 0);
                printTabsToFile(level, emitFILE);
                fprintf(emitFILE, "}");
            }
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case switchK:
            fprintf(emitFILE, "switch(");
            if (s->val.switchS.initStatement) {
                // print the init statement inline and terminate it with a semicolon
                prettySTATEMENT(s->val.ifS.initStatement, -1, 1, 0);
                fprintf(emitFILE, " ");
            }
            prettyEXP(s->val.switchS.condition);
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            prettySWITCHCASE(s->val.switchS.cases, level + 1);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case whileK:
            fprintf(emitFILE, "for (");
            prettyEXP(s->val.whileS.condition);
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            prettySTATEMENT(s->val.whileS.body, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case infiniteLoopK:
            fprintf(emitFILE, "for {");
            newLineInFile(emitFILE);
            prettySTATEMENT(s->val.infiniteLoopS, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case forK:
            fprintf(emitFILE, "for (");
            // TODO here
            // tFOR simpleStatement ';' exp ';' postStatement block
            fprintf(emitFILE, ") {");
            newLineInFile(emitFILE);
            prettySTATEMENT(s->val.infiniteLoopS, level + 1, 1, 0);
            printTabsToFile(level, emitFILE);
            fprintf(emitFILE, "}");
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case breakK:
            fprintf(emitFILE, "break");
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
        case continueK:
            fprintf(emitFILE, "continue");
            if (level == -1) {
                if (semicolon) {
                    fprintf(emitFILE, ";");
                }
            } else {
                fprintf(emitFILE, ";");
                newLineInFile(emitFILE);
            }
            break;
    }
    // print the next statement
    prettySTATEMENT(s->next, level, 1, 0);
}

typedef struct STATEMENT {
    int lineno;
    enum { emptyK, expK, incK, decK, regAssignK, binOpAssignK, shortDeclK, varDeclK,
           typeDeclK, printK, printlnK, returnK, ifK, ifElseK, switchK, whileK,
           infiniteLoopK, forK, breakK, continueK } kind;
    union{
        // break, continue, and empty statements have no associated val
        struct EXP* expS;
        struct EXP *returnS; // return expression
        struct {struct STATEMENT* initStatement;
                struct EXP *condition;
                struct STATEMENT *body;} ifS;
        struct {struct STATEMENT* initStatement;
                struct EXP *condition;
                struct STATEMENT *thenPart;
                struct STATEMENT *elsePart; /* could be another if --> else if statement! */} ifElseS;
        struct {struct EXP *condition;
                struct STATEMENT *body;} whileS;
        struct STATEMENT* infiniteLoopS; // the body of an infinite loop
        struct {struct STATEMENT* initStatement;
                struct EXP* condition;
                struct STATEMENT* postStatement;
                struct STATEMENT* body;} forS;
        struct {struct STATEMENT* initStatement;
                struct EXP* condition;
                struct SWITCHCASE* cases;} switchS;
        struct {struct EXP* lvalues;    // weed to ensure that all exps are lvalues
                struct EXP* exps;} regAssignS;
        struct {struct EXP* lvalue; // weed to ensure that exp is an lvalue (and that there is just one of them? should already only be one since matches with primaryExp)
                OperationKind opKind;
                struct EXP* exp;} binOpAssignS;
        struct EXP* incS;   // weed to ensure exp is an lvalue
        struct EXP* decS;   // weed to ensure exp is an lvalue
        struct EXP* printS;     // linked-list of expressions
        struct EXP* printlnS;   // linked-list of expressions
        struct VARDECLARATION* varDeclS;
        struct TYPEDECLARATION* typeDeclS;
        struct {struct EXP* ids;    // needs to be weeded (to ensure only ids). also, both need to be weeded for length
                struct EXP* exps;} shortDeclS;
    } val;
    // points to the next statement at the same level as this one
    // nested statements are pointed to by the appropriate statement structs in val
    struct STATEMENT* next;
} STATEMENT;

/*
 * print switch cases at given level and print a new line after (prettySTATEMENT prints a new line after)
 */
void prettySWITCHCASE(SWITCHCASE* s, int level) {
    if (s == NULL) return;
    printTabsToFile(level, emitFILE);
    switch (s->kind) {
        case caseK:
            fprintf(emitFILE, "case ");
            prettyEXP(s->val.caseC.exps);
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
    if (param->next == NULL) {
        prettyID(param->ids);
        fprintf(emitFILE, " ");
        prettyTYPE(param->type, -1);
    } else {
        prettyID(param->ids);
        fprintf(emitFILE, " ");
        prettyTYPE(param->type, -1);
        fprintf(emitFILE, ", ");
        // recurse
        prettyPARAMETER(param->next);
    }
}

/*
 * prints the expression list at the current rw-pointer location
 * does not print a new line
 */
void prettyEXP(EXP* exp) {
    if (exp == NULL) return;
    if (exp->next == NULL) {
        // just print the exp
        prettyEXP(exp);
    } else {
        // print the exp, a comma, and a space
        prettyEXP(exp);
        fprintf(emitFILE, ", ");
        // recurse
        prettyEXP(exp->next);
    }
}
