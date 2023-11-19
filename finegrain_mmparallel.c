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
    int current_matrix;
    int start;
    int end;
} ThreadData;

ThreadArgs *thread_args;

double **allocateMatrix();
void printResult(int current_matrix);
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

    // Allocate matrices
    double **a = allocateMatrix();
    double **b = allocateMatrix();
    double **c = allocateMatrix();

    ThreadData thread_data[nthreads];
    thread_args = (ThreadArgs *)malloc(nmats * sizeof(ThreadArgs));

    int n_row_per_thread = matrixSize / nthreads;
    int row_remiander = matrixSize % nthreads;

    // read matrices from file
    for (k = 0; k < nmats; k++)
    {
        pthread_t threads[nthreads];
        int rc;
        int t;

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
            thread_data[t].current_matrix = k;

            if (t == nthreads - 1)
            {
                thread_data[t].end += row_remiander;
            }

            rc = pthread_create(&threads[t], NULL, mm, (void *)&thread_data[t]);
            if (rc)
            {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(1);
            }
        }

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
    }

    fclose(fh);

    // Imprime la matriz 0 d ela estructura de datos
    printResult(0);

    // Free memory
    free(thread_args);
    free(*a);
    free(a);
    free(*b);
    free(b);
    free(*c);
    free(c);

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
    double **aa, **bb, **cc;
    aa = thread_args[thread_data->current_matrix].a;
    bb = thread_args[thread_data->current_matrix].b;
    cc = thread_args[thread_data->current_matrix].c;
    pthread_mutex_unlock(&mutex);

    int i, j, k;

    for (k = thread_data->start; k <= thread_data->end; k++)
    {
        // Extrae la fila k de la matriz a
        double *a = aa[k];

        // Crea un vector con la columna i de la matriz b
        double *b = (double *)malloc(matrixSize * sizeof(double));

        for (i = 0; i < matrixSize; i++)
        {
            // Extrae la columna i de la matriz b
            for (j = 0; j < matrixSize; j++)
            {
                b[j] = bb[j][i];
            }

            // Multiplica la fila k de a por la columna i de b
            double sum = 0.0;
            for (j = 0; j < matrixSize; j++)
            {
                sum += a[j] * b[j];
            }
            // printf("%lf ", sum);
            cc[k][i] = sum;
        }
    }
    pthread_mutex_lock(&mutex);
#ifdef DEBUG_MODE
    printf("Hola, soy el hilo %d y voy a multiplicar a la matriz %d desde %d hasta %d\n", thread_data->thread_id, thread_data->current_matrix, thread_data->start, thread_data->end);

    // Mostrar la matriz cc en modo debug
    for (k = 0; k < matrixSize; k++)
    {
        for (i = 0; i < matrixSize; i++)
        {
            printf("%lf ", cc[k][i]);
        }
        printf("\n");
    }
#endif
    // Guardar cc en la estructura de datos
    thread_args[thread_data->current_matrix].c = cc;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}
