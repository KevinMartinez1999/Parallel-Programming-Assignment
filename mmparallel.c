#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 20

int matrixSize;

static pthread_mutex_t mutex;

typedef struct
{
	double **a;
	double **b;
	double **c;
} ThreadDataMatrix;

typedef struct
{
	int thread_id;
	int start;
	int end;
} ThreadDataRange;

ThreadDataMatrix *thread_data_matrix;

double **allocateMatrix()
{
	int i;
	double *vals, **temp;

	// allocate space for values of a matrix
	vals = (double *)malloc(matrixSize * matrixSize * sizeof(double));

	// allocate vector of pointers to create the 2D array
	temp = (double **)malloc(matrixSize * sizeof(double *));

	for (i = 0; i < matrixSize; i++)
		temp[i] = &(vals[i * matrixSize]);

	return temp;
}

void *mm(void *data)
{
	// Obtener los datos del hilo
	ThreadDataRange *thread_data_range = (ThreadDataRange *)data;

	int i, j, k, l;
	double sum;

	for (l = thread_data_range->start; l <= thread_data_range->end; l++)
	{
		// Multiplicación de matrices
		for (i = 0; i < matrixSize; i++)
		{
			for (j = 0; j < matrixSize; j++)
			{
				sum = 0.0;
				// Producto punto
				for (k = 0; k < matrixSize; k++)
				{
					sum = sum + thread_data_matrix[l].a[i][k] * thread_data_matrix[l].b[k][j];

				}
				thread_data_matrix[l].c[i][j] = sum;
			}
		}

		// pthread_mutex_lock(&mutex);
		// // Mostrar el resultado de la multiplicación
		// for (i = 0; i < matrixSize; i++)
		// {
		// 	for (j = 0; j < matrixSize; j++)
		// 	{
		// 		printf("%lf ", thread_data_matrix[l].c[i][j]);
		// 	}
		// 	printf("\n");
		// }
		// pthread_mutex_unlock(&mutex);
	}

	pthread_exit(NULL);
}

int main(void)
{
	clock_t start_time = clock();
	int i, j;
	int nmats;
	int nthreads = NUM_THREADS;
	char *fname = "matrices_large.dat";
	FILE *fp;

	fp = fopen(fname, "r");
	if (fp == NULL)
	{
		printf("Could not open file %s\n", fname);
		exit(1);
	}
	fscanf(fp, "%d %d\n", &nmats, &matrixSize);

	if (NUM_THREADS > nmats)
	{
		nthreads = nmats;
	}

	// printf("Number of matrices: %d\n", nmats);
	// printf("Matrix size: %d\n", matrixSize);

	pthread_t threads[nthreads];
	int rc;
	long t;

	ThreadDataRange thread_data_range[nthreads];

	thread_data_matrix = (ThreadDataMatrix *)malloc(nmats * sizeof(ThreadDataMatrix));

	double **a, **b, **c;
	a = allocateMatrix();
	b = allocateMatrix();
	c = allocateMatrix();

	for (t = 0; t < nmats; t++)
	{

		for (i = 0; i < matrixSize; i++)
		{
			for (j = 0; j < matrixSize; j++)
			{
				fscanf(fp, "%lf", &a[i][j]);
			}
		}
		for (i = 0; i < matrixSize; i++)
		{
			for (j = 0; j < matrixSize; j++)
			{
				fscanf(fp, "%lf", &b[i][j]);
			}
		}

		thread_data_matrix[t].a = a;
		thread_data_matrix[t].b = b;
		thread_data_matrix[t].c = c;
	}

	for (t = 0; t < nthreads; t++)
	{
		thread_data_range[t].thread_id = t;
		thread_data_range[t].start = t * (nmats / nthreads);
		thread_data_range[t].end = (t + 1) * (nmats / nthreads) - 1;

		// El último hilo se encarga de las sumas restantes si num_sumas no es divisible por num_hilos
		if (t == nthreads - 1 && nmats % nthreads != 0)
		{
			thread_data_range[t].end += nmats % nthreads;
		}

		rc = pthread_create(&threads[t], NULL, mm, (void *)&thread_data_range[t]);
		if (rc)
		{
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	// Esperar a que todos los hilos terminen
	for (t = 0; t < nthreads; t++)
	{
		pthread_join(threads[t], NULL);
	}

	// free memory
	free(*a);
	free(a);
	free(*b);
	free(b);
	free(*c);
	free(c);

	fclose(fp);

	clock_t end_time = clock();
	double elapsed_time = ((double)(end_time - start_time)) * 1000.0 / CLOCKS_PER_SEC;
	printf("Tiempo de ejecución: %f milisegundos\n", elapsed_time);

	pthread_exit(NULL);
}
