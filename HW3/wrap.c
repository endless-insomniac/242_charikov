#include <stdio.h>

void __real_fred(int arg);
void __real_john(double x);

void __wrap_fred(int arg) {
    printf("interposed at link time: fred is called\n");
    __real_fred(arg);
}

void __wrap_john(double x) {
    printf("interposed at link time: john is called\n");
    __real_john(x);
}