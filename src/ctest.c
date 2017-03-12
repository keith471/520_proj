#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

typedef struct TEST {
    int val;
} TEST;

typedef struct OUTER {
    struct TEST* test;
} OUTER;

void updateOuter(TEST** test) {
    TEST* t = NEW(TEST);
    t->val = 1;
    *test = t;
}

int main() {
    TEST* t1 = NEW(TEST);
    t1->val = 0;

    OUTER* o = NEW(OUTER);
    o->test = t1;

    updateOuter(&(o->test));

    printf("%d\n", o->test->val);
}
