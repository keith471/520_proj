#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"
#include "error.h"
#include "tree.h"
#include "pretty.h"
#include "weed.h"
#include "symbol.h"
#include "type.h"
#include "cppType.h"
#include "codegen.h"
#include "memory.h"

// the compiler version
char* VERSION = "v1.0.0";

int yylex();
void yyparse();

PROGRAM* theprogram;

int dumpsymtab = 0; // if true, the symbol table will be dumped
int pptype = 0; // TODO if true, the program will be pretty printed with types
int requireMain = 0; // can require that the main function is present though the -reqmain flag

extern FILE* yyin;

/*
 * parse and return the program name (without the extension)
 */
char* getProgramName(char* programFilename) {
    int dotIndex = -1;
    int slashIndex = -1;
    int i;
    // get the index of the last '.' and the last '/' in the name of the program file
    for (i = strlen(programFilename) - 1; i >= 0; i--) {
        if (programFilename[i] == '.' && dotIndex == -1) {
            dotIndex = i;
        }
        if (programFilename[i] == '/') {
            slashIndex = i;
            break;
        }
    }

    char* name = malloc(strlen(programFilename)+1);

    int end;
    if (dotIndex == -1) {
        end = strlen(programFilename);
    } else {
        end = dotIndex;
    }

    strncpy(name, &programFilename[slashIndex + 1], (end - slashIndex - 1));
    name[(end - slashIndex - 1)] = '\0';

    return name;
}

void parsePROGRAM(char* programFilename) {
    // open a file handle to the file to scan
    FILE *infile = fopen(programFilename, "r");
    // make sure it's valid
    if (!infile) {
        printf("Error: can't open the file '%s'\n", programFilename);
        exit(1);
    }

    // set lex to read from the external file instead of defaulting to STDIN
    yyin = infile;

    // parse through the input until there is no more
    do {
        yyparse();
    } while (!feof(yyin));
    // we do some weeding in the parser; if there were any weeding errors, terminate
    fclose(infile);
}

int beginsWithDash(char* s) {
    if (s[0] == '-') {
        return 1;
    }
    return 0;
}

typedef struct OPTIONS {
    char* pathToFile;
    int help;
    int version;
    int prettyPrint;
    int dumpsymtab;
    int onTheFly;
} OPTIONS;

OPTIONS* getOptions(int nargs, char* args[]) {
    OPTIONS* opts = NEW(OPTIONS);
    opts->pathToFile = NULL;
    opts->help = 0;
    opts->version = 0;
    opts->prettyPrint = 0;
    opts->dumpsymtab = 0;
    opts->onTheFly = 0;

    int i;
    int pathSet = 0;
    char* currArg;
    for (i = 1; i < nargs; i++) {
        currArg = args[i];
        if (strcmp(currArg, "-dumpsymtab") == 0) {
            opts->dumpsymtab = 1;
            dumpsymtab = 1; // must set this here since it is an extern variable in symbol.c
        } else if (strcmp(currArg, "-h") == 0) {
            opts->help = 1;
        } else if (strcmp(currArg, "-v") == 0) {
            opts->version = 1;
        } else if (strcmp(currArg, "-pp") == 0) {
            opts->prettyPrint = 1;
        } else if (strcmp(currArg, "-onthefly") == 0) {
            opts->onTheFly = 1;
        } else if (strcmp(currArg, "-reqmain") == 0) {
            requireMain = 1; // must set this here since it is an extern variable in symbol.c and codegen.c
        } else {
            if (beginsWithDash(currArg)) {
                fprintf(stderr, "ERROR: unrecognized option '%s'. For proper usage, try the '-h' flag.\n", currArg);
                exit(1);
            } else if (!pathSet) {
                // should be the path to the file
                opts->pathToFile = currArg;
                pathSet = 1;
            } else {
                fprintf(stderr, "ERROR: too many filepaths provided. For proper usage, try the '-h' flag.\n");
                exit(1);
            }
        }
    }
    return opts;
}

void printVersion() {
    printf("GoLite Compiler, version %s\n", VERSION);
}

void printHelp() {
    printf("USAGE: <path_to_executable> <path_to_golite_file> [-h, -v, -pp, -onthefly, -dumpsymtab, -reqmain]\n");
    printf("-h:\tfor help\n");
    printf("-v:\tversion information\n");
    printf("-pp:\tto pretty print the input code\n");
    printf("-onTheFly:\tto compile code typed directly into the terminal\n");
    printf("-dumpsymtab:\tdump a file containing symbol table information\n");
    printf("-reqmain:\tset this to require that the main function be present - if not set and no main function is present, the compiled C++ code will give a linker error due to the lack of a main function\n");
}

int main(int argc, char* argv[]) {

    #ifdef FLEX_DEBUG
        // parse through the input until there is no more:
        do {
            yylex();
        } while (!feof(yyin));
    #endif

    #ifndef FLEX_DEBUG

        char* programName; // parsed from program name
        char* outputPath = "./src/output/"; // the path to the output folder
        char* prettyPath; // the path to the pretty printed file
        char* symbolPath; // the path to the symbol table file
        char* cppPath; // the path to the generated c++ code

        OPTIONS* opts = getOptions(argc, argv);

        if (opts->help) {
            printHelp();
        }

        if (opts->version) {
            printVersion();
        }

        if (opts->onTheFly) {
            printf("Type some GoLite code folowed by one or two ctrl-d's:\n");
            // parse the program and build an AST rooted at theprogram
            yyparse();
            // we do some weeding in the parser; if there were any weeding errors, terminate
            terminateIfErrors();
            printf("    >>> PARSED CODE SUCCESSFULLY\n");

            programName = "onthefly";
        } else if (opts->pathToFile != NULL) {
            printf("parsing program...\n");
            parsePROGRAM(opts->pathToFile);
            terminateIfErrors();
            printf("    >>> SUCCESS\n");

            programName = getProgramName(opts->pathToFile);
        } else {
            if (!opts->help && !opts->version) {
                fprintf(stderr, "USAGE: <path_to_executable> <path_to_golite_file> [-h, -v, -pp, -onthefly, -dumpsymtab, -reqmain]\n");
            }
            exit(1);
        }

        // finish any weeding that we didn't do in the parser
        printf("weeding the program...\n");
        weedPROGRAM(theprogram);
        terminateIfErrors();
        printf("    >>> SUCCESS\n");

        // optionally pretty print the program
        if (opts->prettyPrint) {
            printf("pretty printing program...\n");
            prettyPath = concat(outputPath, concat(programName, ".pretty.go"));
            prettyPROGRAM(theprogram, prettyPath);
            printf("    >>> SUCCESS\n");
            printf("    >>> pretty printed program to %s\n", prettyPath);
        }

        // create a symbol table for the program
        printf("making a symbol table...\n");
        symbolPath = concat(outputPath, concat(programName, ".symtab"));
        symPROGRAM(theprogram, symbolPath);
        terminateIfErrors();
        printf("    >>> SUCCESS\n");
        if (opts->dumpsymtab) {
            printf("    >>> wrote the symbol table to %s\n", symbolPath);
        }

        // type check the program
        printf("type checking program...\n");
        typePROGRAM(theprogram);
        terminateIfErrors();
        printf("    >>> SUCCESS\n");

        // save C++ versions of types
        printf("C++ typing the program...\n");
        cppTypePROGRAM(theprogram);
        printf("    >>> SUCCESS\n");

        // generate C++ code
        printf("generating C++ code...\n");
        cppPath = concat(outputPath, concat(programName, ".cpp"));
        genPROGRAM(theprogram, cppPath);
        printf("    >>> SUCCESS\n\n");
        printf(">>> successfully compiled the program as %s\n", cppPath);

    #endif

    exit(0);
}
