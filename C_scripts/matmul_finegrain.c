/**
 * Matrix multiplication (fine-grain) using pthreads
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// #define DEBUG_MODE

// Structure for thread arguments
typedef struct
{
    double **a;
    double **b;
    double **c;
} ThreadArgs;

// Structure for thread data
typedef struct
{
    int thread_id;
    int start;
    int end;
} ThreadData;

// Global variables
ThreadArgs *thread_args;
int matrixSize;
int nmats;

// Function prototypes
double **allocateMatrix();
void saveResult(int current_matrix);
void *mm(void *data);

int main(int argc, char *argv[])
{
    // Check the number of command-line arguments
    if (argc != 2)
    {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        exit(1);
    }

    // Get the matrix size
    char *param = argv[1];
    int nthreads = atoi(param);

    // Get the start time
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    // Read the matrices from file
    int i, j, k;
    char *fname = "../dat_files/matrices_large.dat";
    FILE *fh;

    fh = fopen(fname, "r");
    if (fh == NULL)
    {
        printf("Error opening file %s\n", fname);
        exit(1);
    }

    fscanf(fh, "%d %d\n", &nmats, &matrixSize);

#ifdef DEBUG_MODE
    printf("Grano FINO en modo DEBUG\n");
#endif

    // Limit the number of threads to the number of matrices
    if (nthreads > nmats)
    {
        nthreads = nmats;
    }

    // Create an array of threads
    pthread_t threads[nthreads];
    int rc;
    int t;

    // Create thread arguments
    ThreadData thread_data[nthreads];
    thread_args = (ThreadArgs *)malloc(nmats * sizeof(ThreadArgs));

    // read matrices from file
    for (k = 0; k < nmats; k++)
    {
        // Allocate matrices
        double **a = allocateMatrix();
        double **b = allocateMatrix();
        double **c = allocateMatrix();

        // Read matrices a and b from file
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

        // Save matrices in thread arguments
        thread_args[k].a = a;
        thread_args[k].b = b;
        thread_args[k].c = c;
    }

    // Close the file
    fclose(fh);

    // Calculate the number of rows per thread
    int n_row_per_thread = matrixSize / nthreads;
    int row_remiander = matrixSize % nthreads;

    // Create threads and assign tasks
    for (t = 0; t < nthreads; t++)
    {
        // Assign tasks to threads
        thread_data[t].thread_id = t;
        thread_data[t].start = t * n_row_per_thread;
        thread_data[t].end = (t + 1) * n_row_per_thread - 1;

        // Assign the remainder to the first threads
        if (row_remiander > 0)
        {
            thread_data[t].end++;
            row_remiander--;
        }

        // Create threads and pass arguments
        rc = pthread_create(&threads[t], NULL, mm, (void *)&thread_data[t]);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(1);
        }
    }

    // Join threads and wait until they finish
    for (t = 0; t < nthreads; t++)
    {
        rc = pthread_join(threads[t], NULL);
        if (rc)
        {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(1);
        }
    }

    // Save the result in a file
#ifdef DEBUG_MODE
    // Create the directory check_data if it does not exist
    system("mkdir -p check_data");

    // Remove the file if it already exists
    char filename[50];
    sprintf(filename, "check_data/result_finegrain.txt");
    remove(filename);

    // Save the result in a file
    for (t = 0; t < nmats; t++)
    {
        saveResult(t);
    }
#endif

    // Free memory
    free(thread_args);

    // Get the end time
    gettimeofday(&end_time, NULL);

    // Print the elapsed time
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                          (end_time.tv_usec - start_time.tv_usec) / 1000.0;
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
    double sum;

    for (x = 0; x < nmats; x++)
    {
        double **a = thread_args[x].a;
        double **b = thread_args[x].b;
        double **c = thread_args[x].c;

        // matrix multiplication
        for (i = thread_data->start; i <= thread_data->end; i++)
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
    }
}
