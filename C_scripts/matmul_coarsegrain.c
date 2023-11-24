/**
 * Matrix multiplication (coarse-grain) using pthreads
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// #define DEBUG_MODE

int matrixSize;

// Structure for thread arguments
typedef struct
{
    double **a;
    double **b;
    double **c;
} ThreadArgs;

// Structure for thread ranges
typedef struct
{
    int thread_id;
    int start;
    int end;
} ThreadRange;

// Global variables
ThreadArgs *thread_args;

// Function prototypes
double **allocateMatrix();
void *mm(void *data);
void saveResult(int current_matrix);

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
    int i, j;
    int nmats;
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
    printf("Grano GRUESO en modo DEBUG\n");
#endif

    // Limit the number of threads to the number of matrices
    if (nthreads > nmats)
    {
        nthreads = nmats;
    }

    // Create an array of threads
    pthread_t threads[nthreads];
    int rc;
    long t;

    // Create thread arguments
    ThreadRange thread_ranges[nthreads];
    thread_args = (ThreadArgs *)malloc(sizeof(ThreadArgs) * nmats);

    // Read the matrices from file
    for (t = 0; t < nmats; t++)
    {
        // Allocate memory for matrices
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

        // Save the matrices in the thread arguments
        thread_args[t].a = a;
        thread_args[t].b = b;
        thread_args[t].c = c;
    }

    // Close the file
    fclose(fh);

    // Calculate the number of operations per thread
    int n_op_per_thread = nmats / nthreads;
    int op_remainder = nmats % nthreads;

    // Create threads and assign tasks
    for (t = 0; t < nthreads; t++)
    {
        // Assign tasks to threads
        thread_ranges[t].thread_id = t;
        thread_ranges[t].start = t * n_op_per_thread;
        thread_ranges[t].end = (t + 1) * n_op_per_thread - 1;

        // Assign the remainder to the first threads
        if (op_remainder > 0)
        {
            thread_ranges[t].end++;
            op_remainder--;
        }

        // Create threads and pass arguments
        rc = pthread_create(&threads[t], NULL, mm, (void *)&thread_ranges[t]);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Join threads and wait until they complete
    for (t = 0; t < nthreads; t++)
    {
        rc = pthread_join(threads[t], NULL);
        if (rc)
        {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

    // Save the result in a file
#ifdef DEBUG_MODE
    // Create the directory check_data
    system("mkdir -p check_data");

    // Remove the file if it already exists
    char filename[50];
    sprintf(filename, "check_data/result_coarsegrain.txt");
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

void *mm(void *data)
{
    // Get the thread arguments
    ThreadRange *thread_range = (ThreadRange *)data;

    int i, j, k, x;
    double sum;

    for (x = thread_range->start; x <= thread_range->end; x++)
    {
        double **a = thread_args[x].a;
        double **b = thread_args[x].b;
        double **c = thread_args[x].c;

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
    }
}

void saveResult(int current_matrix)
{
    char filename[50];
    sprintf(filename, "check_data/result_coarsegrain.txt");
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
