#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"

// stuff from flex
extern char *yytext;
extern int yylineno;

int errors = 0;

/*
 * for errors encountered during parsing
 */
void yyerror(char *s) {
    fprintf(stderr, "PARSER ERROR:\n");
    fprintf(stderr, "    *** message: %s\n", s);
    fprintf(stderr, "    *** location: syntax error before %s at line %i\n", yytext, yylineno);
    fprintf(stderr, "*** compilation terminated\n");
    exit(1);
}

/**
 * called if error encountered during weeding
 */
void reportWeedError(char* s, int lineno) {
    fprintf(stderr, "WEED ERROR:\n");
    fprintf(stderr, "    *** message: %s\n", s);
    fprintf(stderr, "    *** location: line %i\n", lineno);
    errors++;
    exit(1);
}

void reportStrError(char* errType, char *s, char *name, int lineno) {
    char str[256];
    sprintf(str, s, name);
    fprintf(stderr, "%s ERROR:\n", errType);
    fprintf(stderr, "    *** message: %s\n", str);
    fprintf(stderr, "    *** location: line %i\n", lineno);
    errors++;
    exit(1);
}

void reportDoubleStrError(char* errType, char *s, char *n, char* m, int lineno) {
    char str[256];
    sprintf(str, s, n, m);
    fprintf(stderr, "%s ERROR:\n", errType);
    fprintf(stderr, "    *** message: %s\n", str);
    fprintf(stderr, "    *** location: line %i\n", lineno);
    errors++;
    exit(1);
}

void reportFloatError(char* errType, char *s, float f, int lineno) {
    char str[256];
    sprintf(str, s, f);
    fprintf(stderr, "%s ERROR:\n", errType);
    fprintf(stderr, "    *** message: %s\n", str);
    fprintf(stderr, "    *** location: line %i\n", lineno);
    errors++;
    exit(1);
}

void reportCharError(char* errType, char *s, char c, int lineno) {
    char str[256];
    sprintf(str, s, c);
    fprintf(stderr, "%s ERROR:\n", errType);
    fprintf(stderr, "    *** message: %s\n", str);
    fprintf(stderr, "    *** location: line %i\n", lineno);
    errors++;
    exit(1);
}

/*
 * for symbol errors
 */
void reportSymbolError(char *s, char *name, int lineno) {
    fprintf(stderr, "SYMBOL ERROR:\n");
    fprintf(stderr, "    *** message: %s\n", s);
    fprintf(stderr, "    *** identifier: %s\n", name);
    fprintf(stderr, "    *** location: line %i\n", lineno);
    errors++;
    exit(1);
}

void reportRedeclError(char *msg, char* name, int prevLineno, int lineno) {
    char str[256];
    sprintf(str, msg, name, prevLineno);
    fprintf(stderr, "SYMBOL ERROR:\n");
    fprintf(stderr, "    *** message: %s\n", str);
    fprintf(stderr, "    *** location: line %i\n", lineno);
    errors++;
    exit(1);
}

void reportError(char* errType, char* msg, int lineno) {
    fprintf(stderr, "%s ERROR:\n", errType);
    fprintf(stderr, "    *** message: %s\n", msg);
    fprintf(stderr, "    *** location: line %i\n", lineno);
    errors++;
    exit(1);
}

//==============================================================================
// Errors for which we terminate after printing
//==============================================================================

/**
 * this is called after each phase of the compiler
 */
void terminateIfErrors() {
    if (errors != 0) {
        fprintf(stderr, "*** compilation terminated due to %i errors\n", errors);
        exit(1);
    }
}
