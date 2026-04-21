#include <stdio.h>
#include "lib.h"

int main(void) {
    printf("Program started\n");

    fred(10);
    john(25.0);
    bill("Hello from bill");
    sam(1.57079632679);

    printf("Program finished\n");
    return 0;
}