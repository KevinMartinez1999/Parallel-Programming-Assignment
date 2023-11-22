#include <stdio.h>
#include <stdlib.h>

void mm(double **a, double **b, double **c, int matrixSize);
void mm(double **a, double **b, double **c, int matrixSize) {
    int i, j, k;
    double sum;
    // matrix multiplication
    for (i = 0; i < matrixSize; i++) {
        for (j = 0; j < matrixSize; j++) {
            sum = 0.0;
            // dot product
            for (k = 0; k < matrixSize; k++) {
                sum = sum + a[i][k] * b[k][j];
            }
            c[i][j] = sum;
        }
    }
}
