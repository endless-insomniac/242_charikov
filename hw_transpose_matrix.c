#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n, m;
    scanf("%d %d", &n, &m);

    if (n < 0 || m < 0) {
        return 1;
    }

    int **a = (int **)malloc(sizeof(int *) * n);
    int **t = (int **)malloc(sizeof(int *) * m);

    if ((n > 0 && a == NULL) || (m > 0 && t == NULL)) {
        return 1;
    }

    for (int i = 0; i < n; i++) {
        a[i] = (int *)malloc(sizeof(int) * m);
        if (m > 0 && a[i] == NULL) {
            return 1;
        }
    }

    for (int i = 0; i < m; i++) {
        t[i] = (int *)malloc(sizeof(int) * n);
        if (n > 0 && t[i] == NULL) {
            return 1;
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            scanf("%d", &a[i][j]);
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            t[j][i] = a[i][j];
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (j > 0) {
                printf(" ");
            }
            printf("%d", t[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < n; i++) {
        free(a[i]);
    }
    free(a);

    for (int i = 0; i < m; i++) {
        free(t[i]);
    }
    free(t);

    return 0;
}