
void yyerror(char *s);
void reportWeedError(char* s, int lineno);
void reportStrError(char* errType, char *s, char *name, int lineno);
void reportFloatError(char* errType, char *s, float f, int lineno);
void reportCharError(char* errType, char *s, char c, int lineno);
//void reportTypeError(char *s, int lineno);
//void reportIdentifierError(char *s, char *name, int lineno);
void terminateIfErrors();
