#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "memory.h"
#include "cppType.h"

CPPTYPE *intCPPTYPE, *doubleCPPTYPE, *charCPPTYPE, *boolCPPTYPE, *stringCPPTYPE;

CPPTYPE* head = NULL;

void initCppTypes() {
    intCPPTYPE = NEW(CPPTYPE);
    intCPPTYPE->kind = cppIntK;
    doubleCPPTYPE = NEW(CPPTYPE);
    doubleCPPTYPE->kind = cppDoubleK;
    charCPPTYPE = NEW(CPPTYPE);
    charCPPTYPE->kind = cppCharK;
    boolCPPTYPE = NEW(CPPTYPE);
    boolCPPTYPE->kind = cppBoolK;
    stringCPPTYPE = NEW(CPPTYPE);
    stringCPPTYPE->kind = cppStringK;
}

/*
 * traverses through the list headed at head in search of a C++ type equivalent to t
 */
CPPTYPE* getCppType(CPPTYPE* t) {
    CPPTYPE* c;
    if (head == NULL) {
        return NULL;
    }
    c = head;
    while (c != NULL) {
        // check if c and t are equivalent
        if (equivalent(c, t)) {
            return c;
        }
        c = c->next;
    }
    return NULL;
}

int equivalent(CPPTYPE* c, CPPTYPE* t) {
    switch (c->kind) {
        case cppIntK:
            return t->kind == cppIntK;
            break;
        case cppDoubleK:
            return t->kind == cppDoubleK;
            break;
        case cppBoolK:
            return t->kind == cppBoolK;
            break;
        case cppCharK:
            return t->kind == cppCharK;
            break;
        case cppStringK:
            return t->kind == cppStringK;
            break;
        case cppArrayK:
            if (t->kind == cppArrayK) {
                return equivalentArrays(c, t);
            }
            return 0;
            break;
        case cppVectorK:
            if (t->kind == cppVectorK) {
                return equivalentVectors(c, t);
            }
            return 0;
            break;
        case cppStructK:
            if (t->kind == cppArrayK) {
                return equivalentStructs(c, t);
            }
            return 0;
            break;
    }
}

/*
 * Two C++ vectors are equivalent if they have equivalent element types
 */
int equivalentArrays(CPPTYPE* c, CPPTYPE* t) {
    return equivalent(c->val.vectorT, t->val.vectorT);
}

/*
 * Two C++ arrays are equivalent if they have equal sizes and equivalent element types
 */
int equivalentArrays(CPPTYPE* c, CPPTYPE* t) {
    if (c->val.arrayT.size != t->val.arrayT.size) {
        return 0;
    }
    return equivalent(c->val.arrayT.elementType, t->val.arrayT.elementType);
}

/*
 * two C++ structs are equivalent if they have equivalent fields
 */
int equivalentStructs(CPPTYPE* c, CPPTYPE* t) {
    equivalentFIELDs(c->val.structT.structType->fields, c->val.structT.structType->fields, t->val.structT.structType->fields, t->val.structT.structType->fields);
}

int equivalentFIELDs(FIELD* currExpected, FIELD* expected, FIELD* currActual, FIELD* actual) {
    if (currExpected == NULL && currActual == NULL) return 1;
    if (currExpected == NULL) {
        return 0;
    }
    if (currActual == NULL) {
        return 0;
    }

    // check that the names are the same
    if (strcmp(currExpected->id->name, currActual->id->name) != 0) {
        return 0;
    }
    // check that the C++ types are equivalent
    if (!equivalent(currExpected->type->cppType, currActual->type->cppType)) {
        return 0;
    }

    // prepare for recursive case
    if (currExpected->nextId == NULL) {
        currExpected = expected->nextFieldSet;
        expected = expected->nextFieldSet;
    } else {
        currExpected = currExpected->nextId;
    }

    if (currActual->nextId == NULL) {
        currActual = actual->nextFieldSet;
        actual = actual->nextFieldSet;
    } else {
        currActual = currActual->nextId;
    }

    // recurse
    return equivalentFIELDs(currExpected, expected, currActual, actual);
}

/*
 * puts a struct or array C++ type at the end of the list headed at head
 */
void putCPPTYPE(CPPTYPE* t) {
    CPPTYPE* c;
    if (head == NULL) {
        head = t;
        return;
    }
    c = head;
    while (c->next != NULL) c = c->next;
    // append t to the end of the list
    c->next = t;
}

void cppTypePROGRAM(PROGRAM* p) {
    // generate the basic CPPTYPEs
    initCppTypes();
    // set the C++ type of relevant type nodes
    cppTypeTOPLEVELDECLARATION(p->topLevelDeclaration);
}

void cppTypeTOPLEVELDECLARATION(TOPLEVELDECLARATION* tld) {
    if (tld == NULL) return;
    switch (tld->kind) {
        case vDeclK:
            cppTypeVARDECLARATION(tld->val.varDeclTLD);
            break;
        case tDeclK:
            cppTypeTYPEDECLARATION(tld->val.typeDeclTLD);
            break;
        case functionDeclK:
            cppTypeFUNCTIONDECLARATION(tld->val.functionDeclTLD);
            break;
    }
    cppTypeTOPLEVELDECLARATION(tld->next);
}

void cppTypeVARDECLARATION(VARDECLARATION* vd) {
    if (vd == NULL) return;
    cppTypeVARDECLARATIONlist(vd);
    cppTypeVARDECLARATION(vd->nextDistributed);
}

void cppTypeVARDECLARATIONlist(VARDECLARATION* vd) {
    if (vd == NULL) return;
    switch (vd->kind) {
        case typeOnlyK:
            cppTypeTYPE(vd->val.typeVD);
            break;
        case expOnlyK:
            cppTypeTYPE(vd->val.expVD.exp->type);
            break;
        case typeAndExpK:
            cppTypeTYPE(vd->val.typeAndExpVD.type);
            break;
    }
    cppTypeVARDECLARATIONlist(vd->next, level);
}

void cppTypeTYPEDECLARATION(TYPEDECLARATION* td) {

}

void cppTypeFUNCTIONDECLARATION(FUNCTIONDECLARATION* fd) {

}

/*
 * the purpose of this function is to set the C++ type on a GoLite TYPE
 */
void cppTypeTYPE(TYPE* t) {
    CPPTYPE* cppType;
    CPPTYPE* other;
    switch (t->kind) {
        case intK:
            t->cppType = intCPPTYPE;
            break;
        case float64K:
            t->cppType = doubleCPPTYPE;
            break;
        case runeK:
            t->cppType = charCPPTYPE;
            break;
        case boolK:
            t->cppType = boolCPPTYPE;
            break;
        case stringK:
            t->cppType = stringCPPTYPE;
            break;
        // these are the interesting ones
        case sliceK:
            cppType = NEW(CPPTYPE);
            cppType->kind = cppVectorK;
            // set the C++ type of the elements in this slice
            cppTypeTYPE(t->val.sliceT);
            // copy the C++ type of the elements over to the vectorT property
            cppType->val.vectorT = t->val.sliceT->cppType;
            t->cppType = cppType;
            break;
        case idK:
            // recurse on the underlying type
            cppTypeTYPE(t->val.idT.underlyingType);
            t->cppType = t->val.idT.underlyingType->cppType;
            break;
        case structK:
            cppType = NEW(CPPTYPE);
            cppType->kind = cppStructK;
            // to C++ type a struct, we just C++ type all its fields
            cppTypeSTRUCTTYPE(t->val.structT);
            cppType->val.structT.structType = t->val.structT;
            // now, it is important to see if an equivalent C++ struct has already been defined
            other = getCPPTYPE(cppType);
            if (other == NULL) {
                // this is a new struct type, so give it a name and add it
                cppType->val.structT.name = getStructName();
                putCPPTYPE(cppType);
                t->cppType = cppType;
            } else {
                t->cppType = other;
            }
            break;
        case arrayK:
            cppType = NEW(CPPTYPE);
            cppType->kind = cppArrayK;
            cppTypeTYPE(t->val.arrayT.elementType);
            cppType->val.arrayT.elementType = t->val.arrayT.elementType->cppType;
            cppType->val.arrayT.size = t->val.arrayT.size->val.intLiteralE.decValue;
            // check if an equivalent C++ array has already been defined
            other = getCPPTYPE(cppType);
            if (other == NULL) {
                // this is a new array type, so give it a name and add it
                cppType->val.arrayT.name = getArrayName();
                t->cppType = cppType;
            } else {
                t->cppType = other;
            }
            break;
    }
}

typedef struct CPPTYPE {
    enum { cppIntK, cppDoubleK, cppBoolK, cppCharK, cppStringK, cppArrayK, cppVectorK, cppStructK } kind;
    union {
        struct {char* name;
                int size;
                struct CPPTYPE* elementType;} arrayT;
        struct CPPTYPE* vectorT; // the type of the elements in the vector
        struct {char* name;
                struct STRUCTTYPE* structType;} structT;
    } val;
}

typedef struct TYPE {
    int lineno;
    enum { idK, structK, sliceK, arrayK, intK, float64K, runeK, boolK, stringK } kind;
    struct CPPTYPE* cppType; // the equivalent C++ type, set in the codegen phase
    union {
        struct {struct ID* id;
                struct TYPEDECLARATION* typeDecl; // a reference to this type's type declaration, set in symbol phase
                struct TYPE* underlyingType; /* set in symbol phase */} idT;
        struct {struct EXP* size; // this will be an int literal expression
                struct TYPE* elementType;} arrayT;
        //struct FIELD* structT;  // the fields in the struct
        struct STRUCTTYPE* structT;
        struct TYPE* sliceT;    // the type of the elements in the slice
    } val;
} TYPE;
