
void yyerror(char *s);
void reportWeedError(char* s, int lineno);
void reportStrError(char* errType, char *s, char *name, int lineno);
void reportDoubleStrError(char* errType, char *s, char *n, char* m, int lineno);
void reportFloatError(char* errType, char *s, float f, int lineno);
void reportCharError(char* errType, char *s, char c, int lineno);
//void reportTypeError(char *s, int lineno);
void reportSymbolError(char *s, char *name, int lineno);
void reportRedeclError(char *msg, char* name, int prevLineno, int lineno);
void reportError(char* errType, char* msg, int lineno);
void reportGeneralError(char* errType, char* msg);
void terminateIfErrors();
