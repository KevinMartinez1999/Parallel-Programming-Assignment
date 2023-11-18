#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

int main(void)
{
    clock_t start = clock();

    printf("Hello, world!\n");

    clock_t end = clock();
    double elapsed_time = ((double)(end - start)) * 1000.0 / CLOCKS_PER_SEC;
    printf("%f\n", elapsed_time);

    return 0;
}