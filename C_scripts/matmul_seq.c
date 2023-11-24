/**
 * @file matmul_seq.c
 * @brief This file contains the sequential implementation of matrix multiplication.
 */
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// #define DEBUG_MODE

double **a, **b, **c;
int matrixSize;

/**
 * @brief Allocates memory for a matrix.
 *
 * @return double** Pointer to the allocated matrix.
 */
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

/**
 * @brief Performs matrix multiplication.
 *
 * This function performs matrix multiplication using a sequential algorithm.
 *
 * @param None
 * @return None
 */
void mm(void)
{
	int i, j, k;
	double sum;
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

/**
 * @brief Saves the result of the matrix multiplication to a file.
 *
 * @return None
 */
void saveResult(void)
{
	char filename[50];
	sprintf(filename, "check_data/check.txt");
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
			fprintf(fh, "%f ", c[i][j]);
		}
		fprintf(fh, "\n");
	}

	fclose(fh);
}

/**
 * @brief Main function.
 *
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @return int 0 if successful.
 */
int main(void)
{
	struct timeval start_time, end_time;
	gettimeofday(&start_time, NULL);
	int i, j, k;
	int nmats;
	char *fname = "../dat_files/matrices_large.dat"; // Change to matrices_large.dat for performance evaluation
	FILE *fh;

	fh = fopen(fname, "r");
	// First line indicates how many pairs of matrices there are and the matrix size
	fscanf(fh, "%d %d\n", &nmats, &matrixSize);

#ifdef DEBUG_MODE
	printf("SECUENCIAL en modo DEBUG\n");

	// Crear la carpeta check_data si no existe
	system("mkdir -p check_data");

	// Remover el archivo result.txt si existe
	char filename[50];
	sprintf(filename, "check_data/check.txt");
	remove(filename);
#endif

	// Dynamically create matrices of the size needed
	a = allocateMatrix();
	b = allocateMatrix();
	c = allocateMatrix();

	// printf("Loading %d pairs of square matrices of size %d from %s...\n", nmats, matrixSize, fname);
	for (k = 0; k < nmats; k++)
	{
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

		mm();
#ifdef DEBUG_MODE
		// Save result to file
		saveResult();
#endif
	}
	fclose(fh);

	gettimeofday(&end_time, NULL);
	// Calcula la diferencia de tiempo en milisegundos
	double elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
						  (end_time.tv_usec - start_time.tv_usec) / 1000.0;
	printf("%f\n", elapsed_time);

	// Free memory
	free(*a);
	free(a);
	free(*b);
	free(b);
	free(*c);
	free(c);

	return 0;
}
