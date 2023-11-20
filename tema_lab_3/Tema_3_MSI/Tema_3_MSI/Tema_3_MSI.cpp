#include "mpi.h"
#include <random>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <string>

using namespace std;

std::ifstream inputFile("data.txt");
std::ifstream convolutionFile("dataConvolution.txt");

bool compareFiles(const std::string& p1, const std::string& p2) {
	std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
	std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

	if (f1.fail() || f2.fail()) {
		return false; //file problem
	}

	if (f1.tellg() != f2.tellg()) {
		return false; //size mismatch
	}

	//seek back to beginning and use std::equal to compare contents
	f1.seekg(0, std::ifstream::beg);
	f2.seekg(0, std::ifstream::beg);
	return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
		std::istreambuf_iterator<char>(),
		std::istreambuf_iterator<char>(f2.rdbuf()));
}

void readData(int** F, int n, int m, int start, int end) {

	for (int i = start; i < end; i++) {
		for (int j = 0; j < m; j++) {
			inputFile >> F[i][j];
		}
	}
}

void readConvolution(int** C, int K) {

	for (int i = 0; i < K; i++) {
		for (int j = 0; j < K; j++) {
			convolutionFile >> C[i][j];
		}
	}
}

void writeData(int** convolutionResult, const char* outputFileName, int n, int m) {
	std::ofstream outputFile(outputFileName);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			outputFile << convolutionResult[i][j] << ' ';
		}
		outputFile << std::endl;
	}
	outputFile.close();
}

int computeElement(int** F, int** C, int N, int M, int K, int i, int j, int x, int y, int start, int end, int* upFrontier, int* downFrontier) {
	int ii = i + x - K / 2;
	int jj = j + y - K / 2;

	if (jj < 0) {
		jj = 0;
	}
	else if (jj >= M) {
		jj = M - 1;
	}

	if (ii < start) {
		return upFrontier[jj] * C[x][y];
	}
	else if (ii >= end) {
		return downFrontier[jj] * C[x][y];
	}
	else {
		return F[ii][jj] * C[x][y];
	}
}

int** computeConvolution(int** F, int** C, int N, int M, int K, int start, int end, int* upFrontier, int* downFrontier) {
	int** V = new int* [N];
	for (int i = 0; i < N; i++) {
		V[i] = new int[M];
	}

	for (int i = start; i < end; i++) {
		for (int j = 0; j < M; j++) {
			int sum = 0;
			for (int x = 0; x < K; x++) {
				for (int y = 0; y < K; y++) {
					sum += computeElement(F, C, N, M, K, i, j, x, y, start, end, upFrontier, downFrontier);
				}
			}
			V[i][j] = sum;
		}
	}
	return V;
}


int main(int argc, char** argv) {
	std::chrono::steady_clock::time_point start_T1;
	int N = 1000, M = 1000, K = 3;

	int** F = new int* [N];
	int** C = new int* [K];
	int** V = new int* [N];
	for (int i = 0; i < K; i++) {
		C[i] = new int[K];
	}

	for (int i = 0; i < N; i++) {
		V[i] = new int[M];
		F[i] = new int[M];
	}

	int start, end;
	MPI_Status status;

	MPI_Init(NULL, NULL);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if (world_rank == 0) {
		start_T1 = chrono::steady_clock::now();
		readConvolution(C, 3);
		int* vectorC = new int[K * K];
		for (int i = 0; i < K; i++) {
			for (int j = 0; j < K; j++) {
				vectorC[i * K + j] = C[i][j];
			}
		}
		MPI_Bcast(vectorC, K * K, MPI_INT, 0, MPI_COMM_WORLD);
		delete[] vectorC;
	}

	if (world_rank == 0) {

		int numberOfLines = N / (world_size - 1);
		start = 0;
		for (int i = 1; i < world_size; i++) {
			end = start + numberOfLines;
			readData(F, N, M, start, end);
			MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			for (int j = start; j < end; j++) {
				MPI_Send(F[j], M, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
			start = end;
		}

		auto start_T2 = chrono::steady_clock::now();

		for (int i = 1; i < world_size; i++) {
			MPI_Recv(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			for (int j = start; j < end; j++) {
				MPI_Recv(F[j], M, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			}
		}
		auto end_T2 = chrono::steady_clock::now();

		writeData(F, "output.txt", N, M);
		auto end_T1 = chrono::steady_clock::now();

		cout << "T1: " << chrono::duration_cast<chrono::nanoseconds>(end_T1 - start_T1).count() / 1e6 << endl;
		cout << "T2: " << chrono::duration_cast<chrono::nanoseconds>(end_T2 - start_T2).count() / 1e6 << endl;

		string lab2Output = "D:\\ANUL_3\\PPD\\tema_lab_2\\Lab2\\out\\production\\Lab2\\output1.txt";
		if (world_size == 5) {
			if (compareFiles("output.txt", lab2Output)) {
				cout << "Files are the same" << endl;
			}
			else {
				cout << "Files are not the same" << endl;
			}
		}
	}
	else { // I am a worker
		int* vectorC = new int[K * K];
		MPI_Bcast(vectorC, K * K, MPI_INT, 0, MPI_COMM_WORLD);
		for (int i = 0; i < K; i++) {
			for (int j = 0; j < K; j++) {
				C[i][j] = vectorC[i * K + j];
			}
		}
		delete[] vectorC;

		MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		for (int j = start; j < end; j++) {
			MPI_Recv(F[j], M, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		}

		printf("I am worker %d: %d to %d\n", world_rank, start, end);

		//send the frontiers
		if (world_rank != 1) {
			MPI_Send(F[start], M, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD);
		}

		if (world_rank != world_size - 1) {
			MPI_Send(F[end - 1], M, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD);
		}

		int* bufferUp = (world_rank != 1) ? new int[M] : F[0];
		int* bufferDown = (world_rank != world_size - 1) ? new int[M] : F[N - 1];

		if (world_rank != 1) {
			MPI_Recv(bufferUp, M, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, &status);
		}

		if (world_rank != world_size - 1) {
			MPI_Recv(bufferDown, M, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD, &status);
		}

		// compute convolution
		F = computeConvolution(F, C, N, M, K, start, end, bufferDown, bufferDown);

		MPI_Send(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		for (int j = start; j < end; j++) {
			MPI_Send(F[j], M, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
		auto end_T2 = chrono::steady_clock::now();
	}

	MPI_Finalize();

	for (int i = 0; i < N; i++) {
		delete[] F[i];
		delete[] V[i];
	}
	for (int i = 0; i < K; i++) {
		delete[] C[i];
	}

	delete[] F;
	delete[] V;
	delete[] C;

	inputFile.close();
	convolutionFile.close();
}