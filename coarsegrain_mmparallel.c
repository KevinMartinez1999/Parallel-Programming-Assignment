#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// #define DEBUG_MODE

static pthread_mutex_t mutex;

int matrixSize;

typedef struct
{
    double **a;
    double **b;
    double **c;
} ThreadArgs;

typedef struct
{
    int thread_id;
    int start;
    int end;
} ThreadRange;

ThreadArgs *thread_args;

double **allocateMatrix();
void *mm(void *data);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        exit(1);
    }

    char *param = argv[1];
    int nthreads = atoi(param);

    clock_t start = clock();

    int i, j;
    int nmats;
    char *fname = "matrices_dev.dat";
    FILE *fh;

    fh = fopen(fname, "r");
    if (fh == NULL)
    {
        printf("Error opening file %s\n", fname);
        exit(1);
    }

    fscanf(fh, "%d %d\n", &nmats, &matrixSize);

// Mostrar numero de matrices y tamaño solo en modo debug
#ifdef DEBUG_MODE
    printf("Number of matrices: %d\n", nmats);
    printf("Matrix size: %d\n", matrixSize);
#endif

    if (nthreads > nmats)
    {
        nthreads = nmats;
    }

    pthread_t threads[nthreads];
    int rc;
    long t;

    ThreadRange thread_ranges[nthreads];
    thread_args = (ThreadArgs *)malloc(sizeof(ThreadArgs) * nmats);

    for (t = 0; t < nmats; t++)
    {
        double **a = allocateMatrix();
        double **b = allocateMatrix();
        double **c = allocateMatrix();

        // Read the a, b matrices from file
        for (i = 0; i < matrixSize; i++)
        {
            for (j = 0; j < matrixSize; j++)
            {
                fscanf(fh, "%lf", &a[i][j]);
            }
        }
        for (i = 0; i < matrixSize; i++)
        {
            for (j = 0; j < matrixSize; j++)
            {
                fscanf(fh, "%lf", &b[i][j]);
            }
        }

        thread_args[t].a = a;
        thread_args[t].b = b;
        thread_args[t].c = c;
    }

    fclose(fh);

    int n_op_per_thread = nmats / nthreads;
    int op_remainder = nmats % nthreads;

    // Create threads
    for (t = 0; t < nthreads; t++)
    {
        thread_ranges[t].thread_id = t;
        thread_ranges[t].start = t * n_op_per_thread;
        thread_ranges[t].end = (t + 1) * n_op_per_thread - 1;

        if (t == nthreads - 1)
        {
            thread_ranges[t].end += op_remainder;
        }

        rc = pthread_create(&threads[t], NULL, mm, (void *)&thread_ranges[t]);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Join threads
    for (t = 0; t < nthreads; t++)
    {
        rc = pthread_join(threads[t], NULL);
        if (rc)
        {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

    // Free memory
    free(thread_args);

    clock_t end = clock();
    double elapsed_time = ((double)(end - start)) * 1000.0 / CLOCKS_PER_SEC;
    printf("%f\n", elapsed_time);

    pthread_exit(NULL);

    return 0;
}

double **allocateMatrix()
{
    int i;
    double *vals, **temp;

    // allocate space for values of a matrix
    vals = (double *)malloc(matrixSize * matrixSize * sizeof(double));

    // allocate vector of pointers to create the 2D array
    temp = (double **)malloc(matrixSize * sizeof(double *));

    // initialize pointers to rows
    for (i = 0; i < matrixSize; i++)
        temp[i] = &(vals[i * matrixSize]);

    return temp;
}

void *mm(void *data)
{
    // Get the thread arguments
    ThreadRange *thread_range = (ThreadRange *)data;

    int i, j, k, x;
    double sum;

    for (x = thread_range->start; x <= thread_range->end; x++)
    {
        pthread_mutex_lock(&mutex);
        double **a = thread_args[x].a;
        double **b = thread_args[x].b;
        double **c = thread_args[x].c;
        pthread_mutex_unlock(&mutex);

        // matrix multiplication
        for (i = 0; i < matrixSize; i++)
        {
            for (j = 0; j < matrixSize; j++)
            {
                sum = 0.0;
                // dot product
                for (k = 0; k < matrixSize; k++)
                {
                    sum = sum + a[i][k] * b[k][j];
                }
                c[i][j] = sum;
            }
        }

// Show the result of the multiplication in debug mode
#ifdef DEBUG_MODE
        pthread_mutex_lock(&mutex);
        for (i = 0; i < matrixSize; i++)
        {
            for (j = 0; j < matrixSize; j++)
            {
                printf("%lf ", c[i][j]);
            }
            printf("\n");
        }
        printf("\n");
        pthread_mutex_unlock(&mutex);
#endif
    }

    pthread_exit(NULL);
}
