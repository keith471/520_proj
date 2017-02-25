#include <stdio.h>

void printTabsToFile(int n, FILE* emitFILE) {
    int i;
    for (i = 0; i < n; i++) {
        fprintf(emitFILE, "    ");
    }
}

void newLineInFile(FILE* emitFILE) {
    fprintf(emitFILE, "\n");
}
