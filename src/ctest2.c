#include <stdio.h>
#include <stdlib.h>

int i;

void changeInt() {
    i = 10;
}

int main(int argc, char* argv[]) {
    i = 3;
    changeInt();

}
