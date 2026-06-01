#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int  *array;
static int   N;
static long  sum = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int start;
    int end;
} Range;

static void *workerFunc(void *arg) {
    Range *r = (Range *)arg;
    long local_sum = 0;

    for (int i = r->start; i < r->end; i++) {
        local_sum += array[i];
    }

    int s = pthread_mutex_lock(&mtx);
    if (s != 0) { perror("pthread_mutex_lock"); return NULL; }

    sum += local_sum;

    s = pthread_mutex_unlock(&mtx);
    if (s != 0) { perror("pthread_mutex_unlock"); return NULL; }

    return NULL;
}

int main(void) {
    printf("Введите N (>= 16): ");
    if (scanf("%d", &N) != 1 || N < 16) {
        fprintf(stderr, "N должно быть целым числом >= 16.\n");
        return EXIT_FAILURE;
    }

    array = (int *)malloc(N * sizeof(int));
    if (!array) { perror("malloc"); return EXIT_FAILURE; }

    srand((unsigned)time(NULL));
    for (int i = 0; i < N; i++) {
        array[i] = rand() % 201 - 100;
    }

    printf("\nМассив (%d элементов):\n", N);
    for (int i = 0; i < N; i++) {
        printf("%4d", array[i]);
        if ((i + 1) % 16 == 0 || i == N - 1) printf("\n");
    }

#define NTHREADS 4
    pthread_t threads[NTHREADS];
    Range     ranges[NTHREADS];

    int base  = N / NTHREADS;
    int extra = N % NTHREADS;

    int start = 0;
    for (int t = 0; t < NTHREADS; t++) {
        ranges[t].start = start;
        ranges[t].end   = start + base + (t < extra ? 1 : 0);
        start = ranges[t].end;
    }

    for (int t = 0; t < NTHREADS; t++) {
        int s = pthread_create(&threads[t], NULL, workerFunc, &ranges[t]);
        if (s != 0) { perror("pthread_create"); return EXIT_FAILURE; }
    }

    for (int t = 0; t < NTHREADS; t++) {
        int s = pthread_join(threads[t], NULL);
        if (s != 0) { perror("pthread_join"); return EXIT_FAILURE; }
    }

    printf("\nСумма всех элементов: %ld\n", sum);

    free(array);
    return EXIT_SUCCESS;
}