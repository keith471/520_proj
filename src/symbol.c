#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "symbol.h"
#include "error.h"

FILE* emitFILE;
extern int dumpsymtab;
int currLineno = 0;  // the current lineno we are at

/////////////////////////////////////////////////////////////////////////////////
// SYMBOL TABLE PRINTING
/////////////////////////////////////////////////////////////////////////////////

/*
 * called when a scope is exited to print the scope to file
 */
void scopeExit(SymbolTable* t) {
    t->endLineno = currLineno;
    if (dumpsymtab) {
        dumpFrame(t);
    }
}

/*
 * dumps only what is in the current symbol table to file
 * lineno is the line at which the scope exited
 */
void dumpFrame(SymbolTable* scope) {
    fprintf(emitFILE, "____________________________________________________________________________________\n");
    if (scope->isUniverseBlock) {
        fprintf(emitFILE, "the universe block:\n");
    } else {
        fprintf(emitFILE, "the scope beginning at line %d and ending at line %d:\n", scope->startLineno, scope->endLineno);
    }
    fprintf(emitFILE, "____________________________________________________________________________________\n");
    fprintf(emitFILE, "lineno\t\tname\t\tkind\t\ttype\n");
    fprintf(emitFILE, "---------------------------------------------------------\n");
    printSymbolTable(scope->table);
    fprintf(emitFILE, "\n");
}

void printSymbolTable(SYMBOL* table[]) {
    SYMBOL* s;
    int i;
    for (i = 0; i < HashSize; i++) {
        s = table[i];
        while (s != NULL) {
            printSymbol(s);
            s = s->next;
        }
    }
}

/*
 * each symbol has the following relevant properties
 *  lineno - the line at which the symbol was declared
 *  name - the name of the symbol
 *  kind - the kind of the symbol
 *  type - the type of the symbol, or the type that the symbol represents
 */
void printSymbol(SYMBOL* s) {
    switch (s->kind) {
        case typeSym:
            fprintf(emitFILE, "%d\t\t%s\t\t%s\t\t%s\n", s->lineno, s->name, "predeclared type", getTypeAsString(s->val.typeS));
            break;
        case typeDeclSym:
            fprintf(emitFILE, "%d\t\t%s\t\t%s\t\t%s\n", s->lineno, s->name, "type declaration", getTypeAsString(s->val.typeDeclS.type));
            break;
        case varSym:
            fprintf(emitFILE, "%d\t\t%s\t\t%s\t\t%s\n", s->lineno, s->name, "predeclared variable", getTypeAsString(s->val.varS));
            break;
        case varDeclSym:
            fprintf(emitFILE, "%d\t\t%s\t\t%s\t\t%s\n", s->lineno, s->name, "var declaration", getTypeAsString(s->val.varDeclS.type));
            break;
        case shortDeclSym:
            fprintf(emitFILE, "%d\t\t%s\t\t%s\t\t%s\n", s->lineno, s->name, "short var declaration", "<tbd>");
            break;
        case functionDeclSym:
            fprintf(emitFILE, "%d\t\t%s\t\t%s\t\t%s\n", s->lineno, s->name, "function declaration", "<n/a>");
            break;
        case parameterSym:
            fprintf(emitFILE, "%d\t\t%s\t\t%s\t\t%s\n", s->lineno, s->name, "parameter", getTypeAsString(s->val.parameterS.type));
            break;
        case fieldSym:
            fprintf(emitFILE, "%d\t\t%s\t\t%s\t\t%s\n", s->lineno, s->name, "field", getTypeAsString(s->val.fieldS.type));
            break;
        default:
            break;
    }
}

char* getTypeAsString(TYPE* t) {
    if (t == NULL) {
        return "<no type>";
    }
    switch (t->kind) {
        case idK:
            // TODO make fancier
            // returns id (alias to id2 (alias to id3(...)))
            return t->val.idT.id->name;
            break;
        case structK:
            // TODO should we print the fields? Probably not --> could get very messy!
            return "struct";
            break;
        case sliceK:
            // TODO make fancier
            // returns slice[type of slice elements]
            return "slice";
            break;
        case arrayK:
            // TODO make fancier
            // returns array[type of array elements]
            return "array";
            break;
        case intK:
            return "int";
            break;
        case float64K:
            return "float64";
            break;
        case runeK:
            return "rune";
            break;
        case boolK:
            return "bool";
            break;
        case stringK:
            return "string";
            break;
    }
}

/////////////////////////////////////////////////////////////////////////////////
// SYMBOL TABLE LOGIC
/////////////////////////////////////////////////////////////////////////////////

/*
 * Hash an identifier into an index
 */
int Hash(char *str) {
    unsigned int hash = 0;
    while (*str) hash = (hash << 1) + *str++;
    return hash % HashSize;
}

/*
 * creates the outermost symbol table, the universe block
 */
SymbolTable* createUniverseBlock() {
    SymbolTable *t;
    int i;
    t = NEW(SymbolTable);
    t->isUniverseBlock = 1;
    t->startLineno = -1;
    t->endLineno = -1;
    for (i=0; i < HashSize; i++) t->table[i] = NULL;
    // add all the defaults!
    addDefault(intD, typeSym, t);
    addDefault(float64D, typeSym, t);
    addDefault(runeD, typeSym, t);
    addDefault(stringD, typeSym, t);
    addDefault(boolD, typeSym, t);
    addDefault(trueD, varSym, t);
    addDefault(falseD, varSym, t);
    t->next = NULL;     // the universe block is the outermost block, so the "next" block is null
    return t;
}

/*
 * for adding a predeclared variable or type to the universe block
 */
void addDefault(DefaultSymbol defSym, SymbolKind kind, SymbolTable* symbolTable) {
    int i;
    char* name;
    SYMBOL *s;
    TYPE* t;
    s = NEW(SYMBOL);
    t = NEW(TYPE);
    switch (defSym) {
        case intD:
            name = "int";
            t->kind = intK;
            s->val.typeS = t;
            break;
        case float64D:
            name = "float64";
            t->kind = float64K;
            s->val.typeS = t;
            break;
        case runeD:
            name = "rune";
            t->kind = runeK;
            s->val.typeS = t;
            break;
        case boolD:
            name = "bool";
            t->kind = boolK;
            s->val.typeS = t;
            break;
        case stringD:
            name = "string";
            t->kind = stringK;
            s->val.typeS = t;
            break;
        case trueD:
            name = "true";
            t->kind = boolK;
            s->val.varS = t;
            break;
        case falseD:
            name = "false";
            t->kind = boolK;
            s->val.varS = t;
            break;
        default:
            break;
    }
    // finish initializing the fields of t
    t->lineno = -1; // avoid any accidental seg fault

    // finish initializing the fields of s
    s->lineno = -1; // avoid any accidental seg fault
    s->name = name;
    s->kind = kind;

    // add s to the hashtable
    i = Hash(name);
    s->next = symbolTable->table[i];
    symbolTable->table[i] = s;
}

/*
 * Initialize a fresh symbol table
 */
SymbolTable* initSymbolTable(int startLineno) {
    SymbolTable *t;
    int i;
    t = NEW(SymbolTable);
    t->isUniverseBlock = 0;
    t->startLineno = startLineno;
    t->endLineno = startLineno;
    for (i=0; i < HashSize; i++) t->table[i] = NULL;
    t->next = NULL;
    return t;
}

/*
 * adds an inner scope to the symbol table
 * s is the outer scope
 * i.e. creates the new inner scope t, and sets its next pointer to s
 * returns the newly created inner scope
 */
SymbolTable* scopeSymbolTable(SymbolTable *s, int startLineno) {
    SymbolTable *t;
    // create a new inner scope
    t = initSymbolTable(startLineno);
    // set the inner scope's outer scope to s
    t->next = s;
    // return the inner scope
    return t;
}

/*
 * this is called in response to the declaration of some new identifier
 * check that the identifier does not already exist AT THE CURRENT LEVEL ONLY (it's ok if it exists at a higher level)
 * if it does, then report an error
 * the exception to this rule is for short var declarations, so we need a flag and extra logic for these
 * if the symbol already exists and it is not in a short variable declaration, we'll still create a new
 * symbol for it so that we can provide more useful error messages
 */
PutSymbolWrapper* putSymbol(SymbolTable *t, char *name, SymbolKind kind, int lineno, int isShortVarDecl) {
    int i = Hash(name);
    PutSymbolWrapper* p;
    p = NEW(PutSymbolWrapper);
    SYMBOL* curr;
    SYMBOL* prevDeclSym = NULL;

    int addNewSymbol = 1;

    // check if the symbol already exists at the current level
    int isRedecl = 0;
    for (curr = t->table[i]; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            // redeclarations are ok for short declarations, but only if at least one variable is new
            // we need to indicate that this is a redeclaration so that the caller can determine if it is valid
            if (!isShortVarDecl) {
                // invalid redeclaration
                reportRedeclError("%s redeclared in this block; previous declaration at line %d", name, curr->lineno, lineno);
            } else {
                // this short variable declaration is a redeclaration
                // we DO NOT want to add a new symbol to the table for it!!!
                addNewSymbol = 0;
            }
            if (prevDeclSym == NULL) {
                // capture only the closest previous declaration in prevDeclSym
                // while continuing to iterate curr in case there are more redelcarations that we can announce as errors
                isRedecl = 1;
                prevDeclSym = curr;
            }
        }
    }

    // create a new symbol
    if (addNewSymbol) {
        SYMBOL* s;
        s = NEW(SYMBOL);
        s->lineno = lineno;
        s->name = name;
        s->kind = kind;
        s->next = t->table[i];
        t->table[i] = s; // put the new symbol at the start of the linked list
        p->symbol = s;
        p->isRedecl = isRedecl;
        p->prevDeclSym = prevDeclSym;
    } else {
        // must be a short variable re-declaration
        p->symbol = NULL;
        p->isRedecl = 1;
        p->prevDeclSym = prevDeclSym;
    }
    return p;
}

/*
 * this is called in response to the use of an identifier
 * search the hash table from top to bottom (most recent to least recently declared symbols)
 * to ensure that we get the most recently declared one
 * this means that at each index of the hash table, we should have a linked list:
 *   most recently used -> least recently used
 * if the identifier is not found in the current symbol table, then we try the one a level up, etc.
 * if the identifier is never found, then we report an error
 */
SYMBOL *getSymbol(SymbolTable *t, char *name, int lineno) {
    int i = Hash(name);
    SYMBOL *s;
    for (s = t->table[i]; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    if (t->next == NULL) {
        // the symbol doesn't exist :( --> error
        reportStrError("SYMBOL", "undefined: %s", name, lineno);
        return NULL;
    }
    return getSymbol(t->next, name, lineno);
}

/////////////////////////////////////////////////////////////////////////////////
// SYMBOL TABLE PRODUCTION (AST TRAVERSAL)
/////////////////////////////////////////////////////////////////////////////////

/*
 * program traversal begins here
 */
void symPROGRAM(PROGRAM* p, char* filePath) {
    if (dumpsymtab) {
        emitFILE = fopen(filePath, "w");
    }

    symbolTable = createUniverseBlock(); // create the universe block
    scopeExit(symbolTable);
    symbolTable = scopeSymbolTable(symbolTable, 0); // create the outermost scope for the program
    symTOPLEVELDECLARATION(p->topLevelDeclaration, symbolTable);
    scopeExit(symbolTable);

    if (dumpsymtab) {
        fclose(emitFILE);
    }
}

/*
 * does nothing interesting
 * just delegates based on type of the top level declaration
 */
void symTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld, SymbolTable* t) {
    if (tld == NULL) return;
    currLineno = tld->lineno;
    switch (tld->kind) {
        case vDeclK:
            symVARDECLARATION(tld->val.varDeclTLD, t);
            break;
        case tDeclK:
            symTYPEDECLARATION(tld->val.typeDeclTLD, t);
            break;
        case functionDeclK:
            symFUNCTIONDECLARATION(tld->val.functionDeclTLD, t);
            break;
        default:
            break;
    }
    symTOPLEVELDECLARATION(tld->next, t);
}

/*
 * a VARDECLARATION is the start of a linked list of variables that were declared on one line
 * moreover, a VARDECLARATION could have a pointer to another VARDECLARATION if declared in
 * a distributed variable declaration statement
 */
void symVARDECLARATION(VARDECLARATION* v, SymbolTable* t) {
    if (v == NULL) return;  // no more distributed variable declarations
    currLineno = v->lineno;
    if (v->isEmpty) return;
    symVARDECLARATIONlist(v, t, 0);
    symVARDECLARATION(v->nextDistributed, t);
}

/*
 * recurses through the linked list of VARDECLARATIONs headed at v
 * for each one, we need to create a symbol!
 * we also need to ensure that the type exists in the symble table, if there is a type for the var decl
 * all var decls in the list have the same type so we only need to check the type the first time
 */
void symVARDECLARATIONlist(VARDECLARATION* v, SymbolTable* t, int checkedType) {
    if (v == NULL) return;
    PutSymbolWrapper* p;
    SYMBOL* s;
    switch (v->kind) {
        case typeOnlyK:
            if (!checkedType) {
                // MUST check the type first!!! (in case the var has the same name as the type)
                verifyType(v->val.typeVD, t);
                checkedType = 1;
            }
            // create a symbol for the varDecl (if it is not blank)
            if (notBlank(v->id->name)) {
                p = putSymbol(t, v->id->name, varDeclSym, v->lineno, 0);
                s = p->symbol;
                s->val.varDeclS.varDecl = v;
                s->val.varDeclS.type = v->val.typeVD;
            } else {
                v->isBlank = 1;
            }
            break;
        case expOnlyK:
            // MUST check the expression first!
            symEXP(v->val.expVD.exp, t);
            // create a symbol for the varDecl (if it is not blank)
            if (notBlank(v->id->name)) {
                p = putSymbol(t, v->id->name, varDeclSym, v->lineno, 0);
                s = p->symbol;
                v->val.expVD.symbol = s; // important so we can set the type in the type checking phase
                s->val.varDeclS.varDecl = v;
                s->val.varDeclS.type = NULL;
            } else {
                v->isBlank = 1;
            }
            break;
        case typeAndExpK:
            // check the expression
            symEXP(v->val.typeAndExpVD.exp, t);
            // then check the type
            if (!checkedType) {
                verifyType(v->val.typeAndExpVD.type, t);
                checkedType = 1;
            }
            // then create a symbol for the var decl (if it is not blank)
            if (notBlank(v->id->name)) {
                p = putSymbol(t, v->id->name, varDeclSym, v->lineno, 0);
                s = p->symbol;
                s->val.varDeclS.varDecl = v;
                s->val.varDeclS.type = v->val.typeAndExpVD.type;
            } else {
                v->isBlank = 1;
            }
            break;
        default:
            break;
    }
    symVARDECLARATIONlist(v->next, t, checkedType);
}

/*
 * validates and creates a symbol table entry for a type declaration
 */
void symTYPEDECLARATION(TYPEDECLARATION* td, SymbolTable* t) {
    if (td == NULL) return;
    currLineno = td->lineno;
    if (td->isEmpty) return;
    // verify the type
    verifyType(td->type, t);
    // create a symbol for the id
    PutSymbolWrapper* p;
    // the id of a type cannot be _, so we don't need to check that here
    p = putSymbol(t, td->id->name, typeDeclSym, td->lineno, 0);
    SYMBOL* s = p->symbol;
    // set val on the symbol
    s->val.typeDeclS.typeDecl = td;
    // create a new type with kind idK
    TYPE* idType = NEW(TYPE);
    idType->lineno = td->lineno;
    idType->kind = idK;
    idType->val.idT.id = td->id;
    idType->val.idT.typeDecl = td;
    idType->val.idT.underlyingType = td->type;
    // this id type is the type we save on the symbol for this type declaration
    s->val.typeDeclS.type = idType;
    symTYPEDECLARATION(td->nextDistributed, t);
}

/*
 * sym a function - the order in which we sym things is important
 */
void symFUNCTIONDECLARATION(FUNCTIONDECLARATION* f, SymbolTable* t) {
    currLineno = f->lineno;

    // sym the return type in the current scope (could be NULL)
    if (f->returnType != NULL) {
        verifyType(f->returnType, t);
    }

    // sym the name of the function in the current scope
    PutSymbolWrapper* p;
    SYMBOL* s;
    if (notBlank(f->id->name)) {
        p = putSymbol(t, f->id->name, functionDeclSym, f->lineno, 0);
        s = p->symbol;
        s->val.functionDeclS = f;
    }
    // don't need to do anything if the function name is blank as the weeder ensures
    // that we cannot call blank functions. Technically, we could mark this function and not
    // generate code for it though

    // create a new scope
    SymbolTable* funcScope = scopeSymbolTable(t, currLineno);

    // sym the parameters in the new scope
    symPARAMETER(f->parameters, funcScope);

    // sym the statements in the new scope
    symSTATEMENT(f->statements, funcScope);

    // print out the scope
    scopeExit(funcScope);
}

void symPARAMETER(PARAMETER* p, SymbolTable* t) {
    if (p == NULL) return;
    currLineno = p->lineno;
    symPARAMETERlist(p, t, 0);
    symPARAMETER(p->nextParamSet, t);
}

/*
 * creates symbols for all the parameters in the list
 * only checks the type for the first parameter in the list
 */
void symPARAMETERlist(PARAMETER* p, SymbolTable* t, int checkedType) {
    if (p == NULL) return;
    PutSymbolWrapper* psw;
    SYMBOL* s;
    if (!checkedType) {
        // first, check the type!
        verifyType(p->type, t);
        checkedType = 1;
    }
    // create a symbol for the parameter (if it is not blank)
    if (notBlank(p->id->name)) {
        psw = putSymbol(t, p->id->name, parameterSym, p->lineno, 0);
        s = psw->symbol;
        s->val.parameterS.param = p;
        s->val.parameterS.type = p->type;
    }
    // we can code-generate a blank parameter later on, it simply won't every be used

    // recurse
    symPARAMETERlist(p->nextId, t, checkedType);
}

void symSTATEMENT(STATEMENT* s, SymbolTable* symbolTable) {
    if (s == NULL) return;
    currLineno = s->lineno;
    SymbolTable* ifScope;
    SymbolTable* bodyScope;
    SymbolTable* ifElseScope;
    SymbolTable* ifBody;
    SymbolTable* elseBody;
    SymbolTable* switchScope;
    SymbolTable* whileScope;
    SymbolTable* forScope;
    SymbolTable* blockScope;
    switch (s->kind) {
        case emptyK:
            // nothing to do
            break;
        case expK:
            // sym the exp
            symEXP(s->val.expS, symbolTable);
            break;
        case incK:
            symEXP(s->val.incS, symbolTable);
            break;
        case decK:
            symEXP(s->val.decS, symbolTable);
            break;
        case regAssignK:
            // if the lvalue is the blank identifier, then mark it as such
            if (s->val.regAssignS.lvalue->kind == identifierK) {
                if (!notBlank(s->val.regAssignS.lvalue->val.idE.id->name)) {
                    s->val.regAssignS.isBlank = 1;
                }
            }
            // sym the lvalue
            symEXP(s->val.regAssignS.lvalue, symbolTable);
            // sym the exp
            symEXP(s->val.regAssignS.exp, symbolTable);
            // sym the next assignment
            symSTATEMENT(s->val.regAssignS.next, symbolTable);
            break;
        case binOpAssignK:
            // sym the lvalue
            symEXP(s->val.binOpAssignS.lvalue, symbolTable);
            // sym the exp
            symEXP(s->val.binOpAssignS.exp, symbolTable);
            break;
        case shortDeclK:
            // short variable declarations!
            symSTATEMENTshortvardecl(s, symbolTable);
            break;
        case varDeclK:
            // simply sym the variable declaration
            symVARDECLARATION(s->val.varDeclS, symbolTable);
            break;
        case typeDeclK:
            symTYPEDECLARATION(s->val.typeDeclS, symbolTable);
            break;
        case printK:
            // sym the expressions in the list
            symEXPs(s->val.printS, symbolTable);
            break;
        case printlnK:
            symEXPs(s->val.printlnS, symbolTable);
            break;
        case returnK:
            symEXP(s->val.returnS, symbolTable);
            break;
        case ifK:
            // an if statement takes place within a new scope, including the init statement
            // create a new scope
            ifScope = scopeSymbolTable(symbolTable, currLineno);
            // sym the initStatement within the new scope
            symSTATEMENT(s->val.ifS.initStatement, ifScope);
            // sym the condition within the new scope
            symEXP(s->val.ifS.condition, ifScope);
            // create another new scope for the body of the if statement
            bodyScope = scopeSymbolTable(ifScope, currLineno);
            // sym the body within the body scope
            symSTATEMENT(s->val.ifS.body, bodyScope);
            // scope exits
            scopeExit(bodyScope);
            scopeExit(ifScope);
            break;
        case ifElseK:
            // again, all of this takes place within a new scope
            ifElseScope = scopeSymbolTable(symbolTable, currLineno);
            // sym the initStatement and condition within the new scope
            symSTATEMENT(s->val.ifElseS.initStatement, ifElseScope);
            symEXP(s->val.ifElseS.condition, ifElseScope);
            // the if body and else body have their own scopes
            ifBody = scopeSymbolTable(ifElseScope, currLineno);
            symSTATEMENT(s->val.ifElseS.thenPart, ifBody);
            // the scope of the if body ends here
            scopeExit(ifBody);
            elseBody = scopeSymbolTable(ifElseScope, currLineno);
            symSTATEMENT(s->val.ifElseS.elsePart, elseBody);
            // the scope of the else body ends here
            scopeExit(elseBody);
            // the scope of the entire if/else ends here
            scopeExit(ifElseScope);
            break;
        case switchK:
            // again, all of this takes place within a new scope
            switchScope = scopeSymbolTable(symbolTable, currLineno);
            // sym the init statement and condition within the new scope
            symSTATEMENT(s->val.switchS.initStatement, switchScope);
            symEXP(s->val.switchS.condition, switchScope);
            // sym all the switch cases
            symSWITCHCASE(s->val.switchS.cases, switchScope);
            // the scope of the entire switch-case block ends here
            scopeExit(switchScope);
            break;
        case whileK:
            // again, all of this happens within a new scope
            // technically, we can sym the condition within the current scope but it works either way
            whileScope = scopeSymbolTable(symbolTable, currLineno);
            symEXP(s->val.whileS.condition, whileScope);
            // the body has its own scope
            bodyScope = scopeSymbolTable(whileScope, currLineno);
            symSTATEMENT(s->val.whileS.body, bodyScope);
            // scope exits
            scopeExit(bodyScope);
            scopeExit(whileScope);
            break;
        case infiniteLoopK:
            // again, we nest this within some scopes, although it seems awfully silly to do so
            // we do it because the reference compiler does and to be consistent with the other kinds of for loops
            //SymbolTable* infLoopScope = scopeSymbolTable(symbolTable, currLineno);
            //SymbolTable* bodyScope = scopeSymbolTable(infLoopScope, currLineno);
            //symSTATEMENT(s->val.infiniteLoopS, bodyScope);

            // ^^ actually, fuck that, let's use only what we need
            bodyScope = scopeSymbolTable(symbolTable, currLineno);
            symSTATEMENT(s->val.infiniteLoopS, bodyScope);
            scopeExit(bodyScope);
            break;
        case forK:
            // create a new scope for the initStatement, condition, and postStatement
            forScope = scopeSymbolTable(symbolTable, currLineno);
            symSTATEMENT(s->val.forS.initStatement, forScope);
            symEXP(s->val.forS.condition, forScope);
            symSTATEMENT(s->val.forS.postStatement, forScope);
            // create a new scope for the body
            bodyScope = scopeSymbolTable(forScope, currLineno);
            symSTATEMENT(s->val.forS.body, bodyScope);
            // scope exits
            scopeExit(bodyScope);
            scopeExit(forScope);
            break;
        case breakK:
            // nothing to do
            break;
        case continueK:
            // nothing to do
            break;
        case blockK:
            // create a new scope
            blockScope = scopeSymbolTable(symbolTable, currLineno);
            // sym all the statements within the new scope
            symSTATEMENT(s->val.blockS, blockScope);
            scopeExit(blockScope);
            break;
        default:
            break;
    }
    symSTATEMENT(s->next, symbolTable);
}

void symSWITCHCASE(SWITCHCASE* sc, SymbolTable* switchScope) {
    if (sc == NULL) return;
    currLineno = sc->lineno;
    // each case takes place within its own scope
    SymbolTable* caseScope = scopeSymbolTable(switchScope, currLineno);
    // sym everything within the new scope
    switch (sc->kind) {
        case caseK:
            symEXPs(sc->val.caseC.exps, caseScope);
            symSTATEMENT(sc->val.caseC.statements, caseScope);
            break;
        case defaultK:
            symSTATEMENT(sc->val.defaultStatementsC, caseScope);
            break;
    }
    // scope exit
    scopeExit(caseScope);
    // sym the next switch case within the switchScope
    symSWITCHCASE(sc->next, switchScope);
}

/*
 * variables can be redeclared, but if they are, then there must be at least one
 * new non-blank variable and any redeclared variables must have the same type(s)
 * as they originally had
 * we can't confirm the type part here (we leave that to type checking), but we
 * can check the rest
 */
void symSTATEMENTshortvardecl(STATEMENT* stmt, SymbolTable* symbolTable) {
    int newCount = 0;   // count of the number of new variables encountered
    int lineno = 0;
    PutSymbolWrapper* p;
    SYMBOL* s;
    STATEMENT* t;
    while (stmt != NULL) {
        lineno = stmt->lineno;
        // check that this variable isn't repeated
        t = stmt->val.shortDeclS.next;
        while (t != NULL) {
            if (strcmp(t->val.shortDeclS.id->val.idE.id->name, stmt->val.shortDeclS.id->val.idE.id->name) == 0) {
                reportStrError("SYMBOL", "%s repeated on left side of :=", t->val.shortDeclS.id->val.idE.id->name, lineno);
            }
            t = t->val.shortDeclS.next;
        }
        // first, sym the expression
        symEXP(stmt->val.shortDeclS.exp, symbolTable);
        // then, create a symbol for the id (provided it is not blank)
        if (notBlank(stmt->val.shortDeclS.id->val.idE.id->name)) {
            p = putSymbol(symbolTable, stmt->val.shortDeclS.id->val.idE.id->name, shortDeclSym, stmt->lineno, 1);
            // check if the id was a redeclaration
            if (p->isRedecl) {
                stmt->val.shortDeclS.isRedecl = 1;
                stmt->val.shortDeclS.prevDeclSym = p->prevDeclSym;
            } else {
                s = p->symbol;
                s->val.shortDeclS.statement = stmt;
                s->val.shortDeclS.type = NULL;
                stmt->val.shortDeclS.symbol = s;
                newCount += 1;
            }
        } else {
            stmt->val.shortDeclS.isBlank = 1;
        }

        // move onto the next decl in the short decl statement
        stmt = stmt->val.shortDeclS.next;
    }

    // error check
    if (newCount == 0) {
        reportError("SYMBOL", "no new variables on left side of :=", lineno);
    }
}

void symFIELD(FIELD* f, SymbolTable* t) {
    if (f == NULL) return;
    currLineno = f->lineno;
    symFIELDlist(f, t, 0);
    symFIELD(f->nextFieldSet, t);
}

void symFIELDlist(FIELD* f, SymbolTable* t, int checkedType) {
    if (f == NULL) return;
    PutSymbolWrapper* p;
    SYMBOL* s;
    if (!checkedType) {
        // first, check the type
        verifyType(f->type, t);
        checkedType = 1;
    }
    // create a symbol for the field, if it is not blank
    if (notBlank(f->id->name)) {
        p = putSymbol(t, f->id->name, fieldSym, f->lineno, 0);
        s = p->symbol;
        s->val.fieldS.field = f;
        s->val.fieldS.type = f->type;
    }

    // recurse
    symFIELDlist(f->nextId, t, checkedType);
}

/*
 * syms all expressions in the linked list of expressions headed by e
 */
void symEXPs(EXP* e, SymbolTable* t) {
    if (e == NULL) return;
    symEXP(e, t);
    symEXPs(e->next, t);
}

/*
 * just syms the current expression, regardless of whether or not its next pointer
 * points to another expression
 * more or less just
 */
void symEXP(EXP* e, SymbolTable* t) {
    if (e == NULL) return;
    currLineno = e->lineno;
    SYMBOL* s;
    switch (e->kind) {
        case identifierK:
            // if the identifier is not blank, we need to check that a symbol exists for it
            if (notBlank(e->val.idE.id->name)) {
                s = getSymbol(t, e->val.idE.id->name, e->lineno);
                // store the symbol on the exp for the type phase
                e->val.idE.symbol = s;
            }
            // identifiers in expressions had better be either fields, parameters, variables, or functions
            /*
            if (s != NULL) {
                if ((s->kind != fieldSym) && (s->kind != parameterSym) && (s->kind != varDeclSym) && (s->kind != varSym) && (s->kind != shortDeclSym) && (s->kind != functionDeclSym)) {
                    reportStrError("SYMBOL", "%s is not a variable or function as expected", e->val.idE.id->name, e->lineno);
                }
            }
            */
            break;
        case intLiteralK:
            // nothing to do for literals
            break;
        case floatLiteralK:
            break;
        case runeLiteralK:
            break;
        case stringLiteralK:
            break;
        case rawStringLiteralK:
            break;
        case plusK:
            symEXP(e->val.plusE.left, t);
            symEXP(e->val.plusE.right, t);
            break;
        case minusK:
            symEXP(e->val.minusE.left, t);
            symEXP(e->val.minusE.right, t);
            break;
        case timesK:
            symEXP(e->val.timesE.left, t);
            symEXP(e->val.timesE.right, t);
            break;
        case divK:
            symEXP(e->val.divE.left, t);
            symEXP(e->val.divE.right, t);
            break;
        case modK:
            symEXP(e->val.modE.left, t);
            symEXP(e->val.modE.right, t);
            break;
        case bitwiseOrK:
            symEXP(e->val.bitwiseOrE.left, t);
            symEXP(e->val.bitwiseOrE.right, t);
            break;
        case bitwiseAndK:
            symEXP(e->val.bitwiseAndE.left, t);
            symEXP(e->val.bitwiseAndE.right, t);
            break;
        case xorK:
            symEXP(e->val.xorE.left, t);
            symEXP(e->val.xorE.right, t);
            break;
        case ltK:
            symEXP(e->val.ltE.left, t);
            symEXP(e->val.ltE.right, t);
            break;
        case gtK:
            symEXP(e->val.gtE.left, t);
            symEXP(e->val.gtE.right, t);
            break;
        case eqK:
            symEXP(e->val.eqE.left, t);
            symEXP(e->val.eqE.right, t);
            break;
        case neqK:
            symEXP(e->val.neqE.left, t);
            symEXP(e->val.neqE.right, t);
            break;
        case leqK:
            symEXP(e->val.leqE.left, t);
            symEXP(e->val.leqE.right, t);
            break;
        case geqK:
            symEXP(e->val.geqE.left, t);
            symEXP(e->val.geqE.right, t);
            break;
        case orK:
            symEXP(e->val.orE.left, t);
            symEXP(e->val.orE.right, t);
            break;
        case andK:
            symEXP(e->val.andE.left, t);
            symEXP(e->val.andE.right, t);
            break;
        case leftShiftK:
            symEXP(e->val.leftShiftE.left, t);
            symEXP(e->val.leftShiftE.right, t);
            break;
        case rightShiftK:
            symEXP(e->val.rightShiftE.left, t);
            symEXP(e->val.rightShiftE.right, t);
            break;
        case bitClearK:
            symEXP(e->val.bitClearE.left, t);
            symEXP(e->val.bitClearE.right, t);
            break;
        case appendK:
            symEXP(e->val.appendE.slice, t);
            symEXP(e->val.appendE.expToAppend, t);
            break;
        case castK:
            // nothing to do here because function-call (arguments) expressions that are actually casts
            // won't have been converted to casts at this stage
            break;
        case selectorK:
            // there is a lot to do here, but we aren't fully equipped to do it at this stage
            // we need to wait until the type checking phase
            symRECEIVER(e->val.selectorE.receiver, t);
            break;
        case indexK:
            symEXP(e->val.indexE.rest, t);
            symEXP(e->val.indexE.lastIndex, t);
            break;
        case argumentsK:
            symEXP(e->val.argumentsE.rest, t);
            symEXPs(e->val.argumentsE.args, t);
            break;
        case uPlusK:
            symEXP(e->val.uPlusE, t);
            break;
        case uMinusK:
            symEXP(e->val.uMinusE, t);
            break;
        case uNotK:
            symEXP(e->val.uNotE, t);
            break;
        case uXorK:
            symEXP(e->val.uXorE, t);
            break;
        default:
            break;
    }
}

void symRECEIVER(RECEIVER* r, SymbolTable* t) {
    symEXP(r->receivingStruct, t);
}

/*
 * if the type is an idK, we need to search the table for the id to make sure
 *   it exists
 *   it is a type
 * if the symbol we find is a typeSym, then we update the type to have the same kind as the symbol's type
 * if the symbol we find is a typeDeclSym, then we set the underlying type on the type AND we set the
 * reference to the type declaration on the type
 * else, we report an error
 * if the type is a composite type, then we verify the components of the type separately
*/
void verifyType(TYPE* type, SymbolTable* t) {
    SYMBOL* s;
    SymbolTable* structScope;
    switch (type->kind) {
        case idK:
            // check that the id of the type has been defined (will produce an error if not)
            s = getSymbol(t, type->val.idT.id->name, type->lineno);
            // ensure that the symbol we found is for a type
            if (s != NULL) {
                switch (s->kind) {
                    case typeSym:
                        // primitive!
                        // this type immediately refers to a predeclared primitive, so we update TYPE
                        // to have the same kind as the primitive --> this is sufficient
                        type->kind = s->val.typeS->kind;
                        break;
                    case typeDeclSym:
                        // alias!
                        // set the given type's underlying type to this alias
                        //type->val.idT.underlyingType = s->val.typeDeclS.type;
                        // set the given type's underlying type to the type of the typeDeclaration
                        type->val.idT.underlyingType = s->val.typeDeclS.typeDecl->type;
                        // and the given type's reference to the type declaration
                        type->val.idT.typeDecl = s->val.typeDeclS.typeDecl;
                        break;
                    default:
                        // error
                        reportStrError("SYMBOL", "%s is not a type", s->name, type->lineno);
                        break;
                }
            }
            break;
        case sliceK:
            // sym the type of the slice elements
            verifyType(type->val.sliceT, t);
            break;
        case arrayK:
            // no need to sym the size expression since we know it is an int literal
            // sym the type of the array elements
            verifyType(type->val.arrayT.elementType, t);
            break;
        case structK:
            symSTRUCTTYPE(type->val.structT, t);
            break;
        default:
            break;
    }
}

void symSTRUCTTYPE(STRUCTTYPE* s, SymbolTable *t) {
    SymbolTable* structScope;
    // create a new scope for the struct!
    structScope = scopeSymbolTable(t, currLineno);
    // save this scope on the struct so that we can use it later for field lookups
    s->symbolTable = structScope;
    // sym the fields of the struct within the new scope
    symFIELD(s->fields, structScope);
    // scope exit
    scopeExit(structScope);
}

/////////////////////////////////////////////////////////////////////////////////
// HELPERS
/////////////////////////////////////////////////////////////////////////////////

int notBlank(char* name) {
    if (strcmp("_", name) == 0) {
        return 0;
    }
    return 1;
}
