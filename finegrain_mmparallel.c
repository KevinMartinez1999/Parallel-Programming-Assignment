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
    int nmatrix;
    int start;
    int end;
} ThreadData;

ThreadArgs *thread_args;

double **allocateMatrix();
void printResult(int current_matrix);
void *mm(void *data);

void *mm2(void *data)
{
    ThreadData *thread_data = (ThreadData *)data;

    int i, j, k;

    for (k = thread_data->start; k <= thread_data->end; k++)
    {
        // Extrae la fila k de la matriz a
        double *a = thread_args[thread_data->nmatrix].a[k];

        // Extrae la columna k de la matriz b
        double *b = (double *)malloc(matrixSize * sizeof(double));
        for (i = 0; i < matrixSize; i++)
        {
            b[i] = thread_args[thread_data->nmatrix].b[i][k];
        }

        // Muestra las filas y columnas extraidas
        printf("Thread %d: fila %d de a: ", thread_data->thread_id, k);
        for (i = 0; i < matrixSize; i++)
        {
            printf("%lf ", a[i]);
        }
        printf("\n");

        printf("Thread %d: columna %d de b: ", thread_data->thread_id, k);
        for (i = 0; i < matrixSize; i++)
        {
            printf("%lf ", b[i]);
        }
        printf("\n");
    }

}

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

    int i, j, k;
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
    int t;

    ThreadData thread_data[nthreads];
    thread_args = (ThreadArgs *)malloc(nmats * sizeof(ThreadArgs));

    int n_row_per_thread = matrixSize / nthreads;
    int row_remiander = matrixSize % nthreads;

    // read matrices from file
    for (k = 0; k < nmats; k++)
    {
        double **a = allocateMatrix();
        double **b = allocateMatrix();
        double **c = allocateMatrix();

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

        thread_args[k].a = a;
        thread_args[k].b = b;
        thread_args[k].c = c;

        for (t = 0; t < nthreads; t++)
        {
            thread_data[t].thread_id = t;
            thread_data[t].start = t * n_row_per_thread;
            thread_data[t].end = (t + 1) * n_row_per_thread - 1;
            thread_data[t].nmatrix = k;

            if (t == nthreads - 1)
            {
                thread_data[t].end += row_remiander;
            }

            rc = pthread_create(&threads[t], NULL, mm2, (void *)&thread_data[t]);
            if (rc)
            {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(1);
            }
        }
    }

    fclose(fh);

    // Join threads
    for (t = 0; t < nthreads; t++)
    {
        rc = pthread_join(threads[t], NULL);
        if (rc)
        {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(1);
        }
    }

#ifdef DEBUG_MODE
    for (k = 0; k < nmats; k++)
    {
        printResult(k);
    }
#endif

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

void printResult(int current_matrix)
{
    int i, j, k;

    pthread_mutex_lock(&mutex);
    double **c = thread_args[current_matrix].c;
    for (k = 0; k < matrixSize; k++)
    {
        for (i = 0; i < matrixSize; i++)
        {
            printf("%lf ", c[k][i]);
        }
        printf("\n");
    }
    pthread_mutex_unlock(&mutex);
}

void *mm(void *data)
{
    // Extract thread data
    ThreadData *thread_data = (ThreadData *)data;

    pthread_mutex_lock(&mutex);
    double **a = thread_args[thread_data->nmatrix].a;
    double **b = thread_args[thread_data->nmatrix].b;
    double **c = thread_args[thread_data->nmatrix].c;
    pthread_mutex_unlock(&mutex);

    int i, j, k;
    double sum;

    for (k = thread_data->start; k <= thread_data->end; k++)
    {
        // Multiplica la fila k de la matriz a por la columna k de la matriz b
        for (i = 0; i < matrixSize; i++)
        {
            sum = 0.0;
            for (j = 0; j < matrixSize; j++)
            {
                sum = sum + a[k][j] * b[j][i];
            }
            c[k][i] = sum;
        }

        // LLeva c a la estrutura de datos global
        pthread_mutex_lock(&mutex);
        thread_args[thread_data->nmatrix].c = c;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}
