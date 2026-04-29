#include <stdio.h>

void bill(char *arg)
{
    printf("[load-time interposition] bill is called\n");
}

void sam(double x)
{
    printf("[load-time interposition] sam is called\n");
}