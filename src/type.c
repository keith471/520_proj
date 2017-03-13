


typedef struct SYMBOL {
    int lineno;
    char* name;
    SymbolKind kind;
    union {
        struct TYPE* typeS;
        struct TYPE* varS;
        struct FUNCTIONDECLARATION *functionDeclS;
        struct STATEMENT* shortDeclS;
        struct {struct TYPEDECLARATION * typeDecl;
                struct TYPE* type;} typeDeclS;
        struct {struct VARDECLARATION* varDecl;
                struct TYPE* type; /* could be NULL */} varDeclS;
        struct {struct PARAMETER * param;
                struct TYPE* type;} parameterS;
        struct {struct FIELD* field;
                struct TYPE* type;} fieldS;
    } val;
    // this is a linked list in the SymbolTable hashmap, so we have to have this next field.
    // it doesn't actually have anything to do with the 'current' symbol
    struct SYMBOL *next;
} SYMBOL;


// ...after having given everything a type and added the types to the symbol table...

// where e is an expression of type selectorK
void checkSelectors(EXP* e) {
    SymbolTable* t = getStructScope(e->val.selectorK.receiver, FUCK);
    // seach t for e->val.selectorK.lastSelector
}

SymbolTable* getStructScope(EXP* receivingExp, SymbolTable* t) {
    SymbolTable* structTable;
    SymbolTable* structTable;
    SYMBOL* s;
    TYPE* type;
    switch (receivingExp->kind) {
        case selectorK:
            t = symSELECTOR(e->val.selectorE.receiver->receivingExp);
            break;
        case identifierK:
            // we need to seach the symbol table for the identifier
            s = getSymbol(t, e->val.idE->name, e->lineno);
            // the symbol should have type struct
            switch (s->kind) {
                case typeDeclSym:
                    type = s->val.typeDeclS.type;
                    break;
                case varDeclSym:
                    type = s->val.varDeclS.type;
                    break;
                case shortDeclSym:
                    type = s->val.shortDeclS.type;
                    break;
                case parameterSym:
                    type = s->val.parameterS.type;
                    break;
                default:
                    // error
                    reportStrError("SYMBOL", "%s is not a reference to a struct", e->val.idE->name, e->lineno);
                    return NULL;
            }
            // we have the type of the symbol --> now check if it is a struct
            // type should NOT be NULL since we should have inferred all types by this time
            // the type could be an idK with an underlying type that is eventually a struct
            // make sure to check this!
            if (type->kind == structK) {

            } else if (type->kind == idK) {

            } else {
                // error --> not a struct
            }
            break;
        case indexK:
            // find the id of the array, get its type, and check that the elementType of its type is a struct
            break;
        case argumentsK;
            // function call could return a struct
            // find the function in the symbol table
            // use the reference to the FUNCTIONDECLARATION to get the return type of the function
            // the return type should be a struct
            // if it is a struct, then get access to its symbolTable field and use that
            break;
        default:
            // this is an error
            break;
    }
    return NULL;
}
