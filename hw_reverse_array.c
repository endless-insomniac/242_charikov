#include <stdio.h>
#include <stdlib.h>

void reverse_array(int *a, int n) {
    for (int i = 0; i < n / 2; i++) {
        int t = a[i];
        a[i] = a[n - 1 - i];
        a[n - 1 - i] = t;
    }
}

int main(void) {
    int n;
    scanf("%d", &n);

    if (n < 0) {
        return 1;
    }

    int *a = (int *)malloc(sizeof(int) * n);
    if (n > 0 && a == NULL) {
        return 1;
    }

    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }

    reverse_array(a, n);

    for (int i = 0; i < n; i++) {
        if (i > 0) {
            printf(" ");
        }
        printf("%d", a[i]);
    }
    printf("\n");

    free(a);
    return 0;
}