#include "matrix.h"

int** init(int value, int dimension)
{
    int** M;
    int i, j;

    // A matrix could be considered as an array of array.
    // We first allocate storage for an array of pointers
    M = (int **)malloc(dimension * sizeof(int *));

    // For each pointer, allocate storage for an array of int
    for (i = 0; i < dimension; i++)
        M[i] = malloc(dimension * sizeof(int));

    for(i = 0; i < dimension; i++) {
        for(j = 0; j < dimension; j++) {
            M[i][j] = value;
        }
    }

    return M;
}

int** multiply(int** A, int** B, int n)
{
    int** M;/*Result matrix*/
    int i,j,k;

    // Allocate memory for result matrix.
    // Allocate storage for an array of pointers.
    M = (int **)malloc(n * sizeof(int *));

    // For each pointer, allocate storage for an array of ints.
    for (i = 0; i < n; i++)
        M[i] = (int *)malloc(n * sizeof(int));

    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            M[i][j]=0;

            // Compute the sum:
            // M[i][j] = sum_{k=0..(n-1)} A[i][k]*B[k][j]
            for(k = 0; k < n; k++)
                M[i][j] += A[i][k] * B[k][j];
        }
    }

    return M;
}

void printMatrix(int** A, int n)
{
    int i, j;

    /*Print a line*/
    for(i = 0; i < n; i++)
        printf("-");

    printf("\n");

    for(i = 0; i < n; i++) {
        /*Print row i*/
        for(j = 0; j < n; j++)
            printf("%d \t ",A[i][j] );

        printf("\n");
    }
}

