#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t taskQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t taskAvailable = PTHREAD_COND_INITIALIZER;
pthread_mutex_t resultMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t taskAddMutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int m, n, p;
    int **A, **B, **D;
    int i, j;
} Task;

Task *taskQueue[5000];
int front = 0, rear = 0, taskCount = 0;

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

void add_task_to_queue(Task *task) {
    pthread_mutex_lock(&taskQueueMutex);
    if (taskCount < 5000) {
        taskQueue[rear] = task;
        rear = (rear + 1) % 5000;
        taskCount++;
        pthread_cond_signal(&taskAvailable);
    }
    pthread_mutex_unlock(&taskQueueMutex);
}

Task* get_task_from_queue() {
    pthread_mutex_lock(&taskQueueMutex);
    while (taskCount == 0) {
        pthread_cond_wait(&taskAvailable, &taskQueueMutex);
    }
    Task *task = taskQueue[front];
    front = (front + 1) % 5000;
    taskCount--;
    pthread_mutex_unlock(&taskQueueMutex);
    return task;
}

void* process_task(void* arg) {
    while (1) {
        Task *task = get_task_from_queue();
        if (task != NULL) {
            int sum = 0;
            for (int k = 0; k < task->n; k++) {
                sum += task->A[task->i][k] * task->B[k][task->j];
            }
            pthread_mutex_lock(&resultMutex);
            task->D[task->i][task->j] = sum;
            pthread_mutex_unlock(&resultMutex);
            free(task);
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

    int **D = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        D[i] = (int *)malloc(p * sizeof(int));
    }

    clock_t startS = clock();
    sequential_multiplication(A, B, C, m, n, p);
    clock_t endS = clock();
    double sequential_time = (double)(endS - startS) / CLOCKS_PER_SEC;
    printf("Sequential matrix multiplication completed in %f seconds\n", sequential_time);

    pthread_mutex_lock(&taskAddMutex);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            Task *task = (Task *)malloc(sizeof(Task));
            task->m = m;
            task->n = n;
            task->p = p;
            task->A = A;
            task->B = B;
            task->D = D;
            task->i = i;
            task->j = j;
            add_task_to_queue(task);
        }
    }
    pthread_mutex_unlock(&taskAddMutex);

    clock_t startP = clock();
    pthread_t threads[6];
    for (int i = 0; i < 6; i++) {
        pthread_create(&threads[i], NULL, process_task, NULL);
    }

    // No join here; threads run indefinitely

    clock_t endP = clock();
    double parallel_time = (double)(endP - startP) / CLOCKS_PER_SEC;
    printf("Parallel execution time: %f seconds\n", parallel_time);

    double speedup = sequential_time / parallel_time;
    printf("Speedup: %f\n", speedup);

    printf("Resultant Matrix D (Parallel):\n");
    for (int i = 0; i < m; i++) {
        for (int j =
