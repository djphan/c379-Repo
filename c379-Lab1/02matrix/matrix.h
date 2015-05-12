#include<stdio.h>
#include<stdlib.h>

/**
 * Create a N by N matrix (N=dimension), and initialize each cell with the given value.
 *
 * @param value The initial value of each cell in the matrix.
 * @param dimention  The dimention of the matrix.
 *
 * @return A NxN matrix.
 */
int** init(int value, int dimension);

/**
 * Multiply two given matrix.
 *
 * @param A  Matrix A
 * @param B  Matrix B
 * @param n  The dimension of matrix A and B.
 *
 * @return The result matrix of the multiplication.
 */
int** multiply(int** A, int** B, int n);

/**
 * Print out a matrix in a user friendly format. For example.
 * 3    1    2
 * 0    3    0
 * 1    2    4
 *
 * @param A  Matrix to be printed.
 * @param n  The dimension of the matrix.
 */
void printMatrix(int** A, int n);
