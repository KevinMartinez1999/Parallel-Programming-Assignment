#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define DEBUG_MODE

static pthread_mutex_t mutex;

int matrixSize;
int nmats;

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
} ThreadData;

ThreadArgs *thread_args;

double **allocateMatrix();
void saveResult(int current_matrix);
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
    char *fname = "matrices_large.dat";
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
        // Allocate matrices
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
    }

    fclose(fh);

    // Create threads
    for (t = 0; t < nthreads; t++)
    {
        thread_data[t].thread_id = t;
        thread_data[t].start = t * n_row_per_thread;
        thread_data[t].end = (t + 1) * n_row_per_thread - 1;

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

    // Guardar resultado en un archivo .txt solo en modo debug
#ifdef DEBUG_MODE
    // Remover el archivo result.txt si existe
    char filename[50];
    sprintf(filename, "check_data/result_finegrain.txt");
    remove(filename);

    for (t = 0; t < nmats; t++)
    {
        saveResult(t);
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

void saveResult(int current_matrix)
{
    char filename[50];
    sprintf(filename, "check_data/result_finegrain.txt");
    FILE *fh = fopen(filename, "a");
    if (fh == NULL)
    {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    for (int i = 0; i < matrixSize; i++)
    {
        for (int j = 0; j < matrixSize; j++)
        {
            fprintf(fh, "%f ", thread_args[current_matrix].c[i][j]);
        }
        fprintf(fh, "\n");
    }

    fclose(fh);
}

void *mm(void *data)
{
    // Extract thread data
    ThreadData *thread_data = (ThreadData *)data;

    int i, j, k, x;

    for (x = 0; x < nmats; x++)
    {
        pthread_mutex_lock(&mutex);
        double **a = thread_args[x].a;
        double **b = thread_args[x].b;
        double **c = thread_args[x].c;
        pthread_mutex_unlock(&mutex);

        // matrix multiplication
        for (i = thread_data->start; i <= thread_data->end; i++)
        {
            for (j = 0; j < matrixSize; j++)
            {
                double sum = 0.0;
                // dot product
                for (k = 0; k < matrixSize; k++)
                {
                    sum = sum + a[i][k] * b[k][j];
                }
                c[i][j] = sum;
            }
        }

        // Guardar cc en la estructura de datos global
        pthread_mutex_lock(&mutex);
        thread_args[x].c = c;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
