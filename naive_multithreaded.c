#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int m, n, p;
    int **A, **B, **C;
    int i, j;
} ThreadData;

void *multiply_element(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int sum = 0;
    for (int k = 0; k < data->n; k++) {
        sum += data->A[data->i][k] * data->B[k][data->j];
    }
    data->C[data->i][data->j] = sum;
    return NULL;
}

void sequential_multiplication(int **A, int **B, int **C, int m, int n, int p) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

int main() {
    int m, n, p;

    printf("Enter the dimensions of Matrix A (m x n): ");
    scanf("%d %d", &m, &n);

    int **A = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        A[i] = (int *)malloc(n * sizeof(int));
    }

    printf("Enter the values of Matrix A (row-wise):\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &A[i][j]);
        }
    }

    printf("Enter the dimensions of Matrix B (n x p): ");
    scanf("%d %d", &n, &p);

    int **B = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        B[i] = (int *)malloc(p * sizeof(int));
    }

    printf("Enter the values of Matrix B (row-wise):\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            scanf("%d", &B[i][j]);
        }
    }

    int **C = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        C[i] = (int *)malloc(p * sizeof(int));
    }

    clock_t startS = clock();
    sequential_multiplication(A, B, C, m, n, p);
    clock_t endS = clock();
    double sequential_time = (double)(endS - startS) / CLOCKS_PER_SEC;
    printf("Sequential matrix multiplication completed in %f seconds\n", sequential_time);

    pthread_t threads[m * p];
    ThreadData thread_data[m * p];

    clock_t startP = clock();
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            thread_data[i * p + j] = (ThreadData){m, n, p, A, B, C, i, j};
            pthread_create(&threads[i * p + j], NULL, multiply_element, &thread_data[i * p + j]);
        }
    }

    for (int i = 0; i < m * p; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t endP = clock();
    double parallel_time = (double)(endP - startP) / CLOCKS_PER_SEC;
    printf("Parallel matrix multiplication completed in %f seconds\n", parallel_time);

    double speedup = sequential_time / parallel_time;
    printf("Speedup: %f\n", speedup);

    printf("Resultant Matrix C (Parallel):\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
}
