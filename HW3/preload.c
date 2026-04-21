#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

void bill(const char *arg) {
    printf("interposed at load time: bill is called\n");

    void (*real_bill)(const char *) = dlsym(RTLD_NEXT, "bill");
    if (real_bill) {
        real_bill(arg);
    }
}

void sam(double x) {
    printf("interposed at load time: sam is called\n");

    void (*real_sam)(double) = dlsym(RTLD_NEXT, "sam");
    if (real_sam) {
        real_sam(x);
    }
}