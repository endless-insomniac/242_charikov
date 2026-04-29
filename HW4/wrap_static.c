#include <stdio.h>

void __real_fred(int);
void __real_john(double);

void __wrap_fred(int arg)
{
    printf("[link-time interposition] fred is called\n");
    __real_fred(arg);
}

void __wrap_john(double x)
{
    printf("[link-time interposition] john is called\n");
    __real_john(x);
}