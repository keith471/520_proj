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

int yylex();
void yyparse();

PROGRAM* theprogram;

int dumpsymtab = 0; // if true, the symbol table will be dumped
int pptype = 0; // if true, the program will be pretty printed with types

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
    printf("parsing program...\n");
    do {
        yyparse();
    } while (!feof(yyin));
    // we do some weeding in the parser; if there were any weeding errors, terminate
    terminateIfErrors();
    printf("    >>> SUCCESS\n");
    fclose(infile);
}

int main(int argc, char* argv[]) {

    #ifdef FLEX_DEBUG
        // parse through the input until there is no more:
        do {
            yylex();
        } while (!feof(yyin));
    #endif

    #ifndef FLEX_DEBUG

        char* programPath = "test.go";
        char* programName; // parsed from program name
        char* outputPath = "./src/output/"; // the path to the output folder
        char* prettyPath; // the path to the pretty printed file
        char* symbolPath; // the path to the symbol table file
        char* cppPath; // the path to the generated c++ code

        if (argc == 2) {
            programPath = argv[1];
            parsePROGRAM(programPath);
        } else if (argc == 3) {
            int pathSet = 0;
            int error = 0;

            char* arg = argv[1];

            if (strcmp(arg, "-dumpsymtab") == 0) {
                dumpsymtab = 1;
            } else if (strcmp(arg, "-pptype") == 0) {
                pptype = 1;
            } else {
                // must be the path to the file
                programPath = arg;
                pathSet = 1;
            }

            arg = argv[2];

            if (strcmp(arg, "-dumpsymtab") == 0) {
                dumpsymtab = 1;
            } else if (strcmp(arg, "-pptype") == 0) {
                pptype = 1;
            } else {
                if (!pathSet) {
                    // must be the path to the file
                    programPath = arg;
                    pathSet = 1;
                } else {
                    // two paths given?
                    error = 1;
                }
            }

            if (!pathSet || error) {
                printf("USAGE: <path_to_executable> <path_to_golite_file> [-dumpsymtab, -pptype]\n");
                exit(1);
            }

            parsePROGRAM(programPath);

        } else if (argc == 4) {
            int pathSet = 0;
            int error = 0;

            char* arg = argv[1];

            if (strcmp(arg, "-dumpsymtab") == 0) {
                dumpsymtab = 1;
            } else if (strcmp(arg, "-pptype") == 0) {
                pptype = 1;
            } else {
                // must be the path to the file
                programPath = arg;
                pathSet = 1;
            }

            arg = argv[2];

            if (strcmp(arg, "-dumpsymtab") == 0) {
                dumpsymtab = 1;
            } else if (strcmp(arg, "-pptype") == 0) {
                pptype = 1;
            } else {
                if (!pathSet) {
                    // must be the path to the file
                    programPath = arg;
                    pathSet = 1;
                } else {
                    // two paths given?
                    error = 1;
                }
            }

            arg = argv[3];

            if (strcmp(arg, "-dumpsymtab") == 0) {
                dumpsymtab = 1;
            } else if (strcmp(arg, "-pptype") == 0) {
                pptype = 1;
            } else {
                if (!pathSet) {
                    // must be the path to the file
                    programPath = arg;
                    pathSet = 1;
                } else {
                    // two or more paths given?
                    error = 1;
                }
            }

            if (!pathSet || error) {
                printf("USAGE: <path_to_executable> <path_to_golite_file> [-dumpsymtab, -pptype]\n");
                exit(1);
            }

            parsePROGRAM(programPath);
        } else if (argc > 4) {
            printf("USAGE: <path_to_executable> <path_to_golite_file> [-dumpsymtab, -pptype]\n");
            exit(1);
        } else {
            printf("Type some golite code folowed by one or two Ctrl-d's:\n");
            // parse the program and build an AST rooted at theprogram
            yyparse();
            // we do some weeding in the parser; if there were any weeding errors, terminate
            terminateIfErrors();
        }

        programName = getProgramName(programPath);

        // finish any weeding that we didn't do in the parser
        printf("weeding the program...\n");
        weedPROGRAM(theprogram);
        terminateIfErrors();
        printf("    >>> SUCCESS\n");

        // pretty print the program
        printf("pretty printing program...\n");
        prettyPath = concat(outputPath, concat(programName, ".pretty.go"));
        prettyPROGRAM(theprogram, prettyPath);
        printf("    >>> SUCCESS\n");
        printf("    >>> pretty printed program to %s\n", prettyPath);

        // create a symbol table for the program
        printf("making a symbol table...\n");
        symbolPath = concat(outputPath, concat(programName, ".symtab"));
        symPROGRAM(theprogram, symbolPath);
        terminateIfErrors();
        printf("    >>> SUCCESS\n");
        if (dumpsymtab) {
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
        printf("    >>> SUCCESS\n");
        printf("    >>> successfully compiled the program as %s\n", cppPath);

    #endif

    exit(0);
}
