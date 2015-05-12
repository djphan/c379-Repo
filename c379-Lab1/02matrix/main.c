#include "matrix.h"

int main(int argc, char** argv)
{
    int i, n;
    int **A;
    int **B;
    int **C;

    if(argc!=2) {
        printf("usage: ./mult n(dimension of the square matrix) \n");
        exit(0);
    }

    /*Read n from the command line*/
    sscanf(argv[1], "%d", &n);

    A = init(1, n);
    printMatrix(A, n);

    B = init(2, n);
    printMatrix(B, n);

    C = multiply(A,B,n);
    printMatrix(C,n);

    // Free the allocated memory
    // Now for each pointer, free its array of ints */
    for (i = 0; i < n; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }

    /* now free the array of pointers */
    free(A);
    free(B);
    free(C);
}

