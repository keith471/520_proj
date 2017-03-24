#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int structNo = 1;

char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1)+strlen(s2)+1); // + 1 for the null-terminator
    // should check for malloc errors here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* getStructName() {
    char number[100]; // more than we need
    sprintf(number, "%d", structNo);
    char* structName = concat("struct_", number);
    return structName;
}

int main() {
    char* structName = getStructName();
    printf("%s", structName);
}
