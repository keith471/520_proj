#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "tree.h"
#include "pretty.h"

int yylex();
void yyparse();

PROGRAM* theprogram;

extern FILE* yyin;

/*
 * concatenate two strings and return the result
 */
char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1)+strlen(s2)+1); // + 1 for the null-terminator
    // should check for malloc errors here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

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
    printf("parsing program...");
    do {
        yyparse();
    } while (!feof(yyin));
    printf("SUCCESS\n");
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

        if (argc == 2) {
            programPath = argv[1];
            parsePROGRAM(programPath);
        } else if (argc > 2) {
            printf("USAGE: <path_to_executable> <path_to_golite_file>\n");
            exit(1);
        } else {
            printf("Type some golite code folowed by one or two Ctrl-d's:\n");
            // parse the program and build an AST rooted at theprogram
            yyparse();
        }

        programName = getProgramName(programPath);

        // TODO weed the program

        // pretty print the program
        printf("pretty printing program...");
        prettyPath = concat(outputPath, concat(programName, ".pretty.go"));
        prettyPROGRAM(theprogram, prettyPath);
        printf("SUCCESS\n");
        printf(">>> pretty printed program to %s\n", prettyPath);

    #endif

    exit(0);
}
