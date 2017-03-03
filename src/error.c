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
}

/**
 * this is called after each phase of the compiler
 */
void terminateIfErrors() {
    if (errors != 0) {
        fprintf(stderr, "*** compilation terminated due to %i errors\n", errors);
        exit(1);
    }
}
