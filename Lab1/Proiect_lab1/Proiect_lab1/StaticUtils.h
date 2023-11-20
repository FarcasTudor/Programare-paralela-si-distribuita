#ifndef STATIC_UTILS_H
#define STATIC_UTILS_H

const int MAX_N = 10000;
const int MAX_M = 10000;
const int MAX_K = 10000;

void convolutionSequentialStatic(int n, int m, int k, const int matrix_F[MAX_N][MAX_M], const int matrix_C[MAX_K][MAX_K], int matrix_V[MAX_N][MAX_M]);
void convolutionParallelVerticalStatic(int n, int m, int k, const int matrix_F[MAX_N][MAX_M], const int matrix_C[MAX_K][MAX_K], int matrix_V[MAX_N][MAX_M], int numberOfThreads);
void convolutionParallelHorizontalStatic(int n, int m, int k, const int matrix_F[MAX_N][MAX_M], const int matrix_C[MAX_K][MAX_K], int matrix_V[MAX_N][MAX_M], int numberOfThreads);
void allocateStaticMatrix(int n, int m, int matrix[MAX_N][MAX_M]);
void writeStaticMatrixToFile(const int matrix[MAX_N][MAX_M], int numRows, int numCols, const string& fileName);
void readStaticMatrixFromFile(const string& fileName, int numRows, int numCols, int matrix[MAX_N][MAX_M]);

#endif