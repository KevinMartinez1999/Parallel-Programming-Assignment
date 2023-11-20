// Matrix multiplication

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #define DEBUG_MODE

double **a, **b, **c;
int matrixSize;

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
void printResult(void)
{
	int i, j;
	for (i = 0; i < matrixSize; i++)
	{
		for (j = 0; j < matrixSize; j++)
		{
			printf("%lf ", c[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	clock_t start = clock();
	int i, j, k;
	int nmats;
	char *fname = "matrices_large.dat"; // Change to matrices_large.dat for performance evaluation
	FILE *fh;

	// printf("Start\n");
	fh = fopen(fname, "r");
	// First line indicates how many pairs of matrices there are and the matrix size
	fscanf(fh, "%d %d\n", &nmats, &matrixSize);

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
		printf("Multiplying two matrices...\n"); // Remove this line for performance tests
		printResult();							 // Remove this line for performance tests
#endif
	}
	fclose(fh);

	clock_t end = clock();
	double elapsed_time = ((double)(end - start)) * 1000.0 / CLOCKS_PER_SEC;
	// printf("Tiempo de ejecución: %f milisegundos\n", elapsed_time);
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
