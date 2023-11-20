#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <thread>

using namespace std;

const int MAX_N = 10000;
const int MAX_M = 10000;
const int MAX_K = 10000;
int matrix_F_static[MAX_N][MAX_M];
int matrix_C_static[MAX_K][MAX_K];
int matrix_V_static[MAX_N][MAX_M];

int** allocateMatrix(int rows, int cols) {
    int** matrix = new int* [rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new int[cols];
    }
    return matrix;
}

void convolutionSequential(int n, int m, int k, int** matrix_F, int** matrix_C, int** matrix_V) {
    auto startTime = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            matrix_V[i][j] = 0;

            for (int x = 0; x < k; x++) {
                for (int y = 0; y < k; y++) {
                    int fRow = i - k / 2 + x;
                    int fCol = j - k / 2 + y;

                    // Pentru bordurile virtuale
                    if (fRow < 0) {
                        fRow = 0;
                    }
                    else if (fRow >= n) {
                        fRow = n - 1;
                    }

                    if (fCol < 0) {
                        fCol = 0;
                    }
                    else if (fCol >= m) {
                        fCol = m - 1;
                    }

                    matrix_V[i][j] += matrix_F[fRow][fCol] * matrix_C[x][y];
                }
            }
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() << endl;
}

void convolutionParallelVertical(int n, int m, int k, int** matrix_F, int** matrix_C, int** matrix_V, int numberOfThreads) {
    auto startTime = chrono::high_resolution_clock::now();

    auto convolution = [&matrix_F, &matrix_C, &matrix_V, n, m, k](int start, int finish) {
        for (int j = start; j < finish; j++) {
            for (int i = 0; i < m; i++) {
                int result = 0;
                for (int x = 0; x < k; x++) {
                    for (int y = 0; y < k; y++) {
                        int fRow = i - k / 2 + x;
                        int fCol = j - k / 2 + y;

                        if (fRow < 0) {
                            fRow = 0;
                        }
                        else if (fRow >= n) {
                            fRow = n - 1;
                        }

                        if (fCol < 0) {
                            fCol = 0;
                        }
                        else if (fCol >= m) {
                            fCol = m - 1;
                        }

                        result += matrix_F[fRow][fCol] * matrix_C[x][y];
                    }
                }
                matrix_V[i][j] = result;
            }
        }
    };

    thread* threads = new thread[numberOfThreads];
    int start = 0;
    int cat = n / numberOfThreads;
    int rest = n % numberOfThreads;

    for (int i = 0; i < numberOfThreads; i++) {
        int finish = start + cat;
        if (rest > 0) {
            finish++;
            rest--;
        }
        threads[i] = thread(convolution, start, finish);
        start = finish;
    }

    for (int i = 0; i < numberOfThreads; i++) {
        threads[i].join();
    }

    auto endTime = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() << endl;
}

void convolutionParallelHorizontal(int n, int m, int k, int** matrix_F, int** matrix_C, int** matrix_V, int numberOfThreads) {
    auto startTime = chrono::high_resolution_clock::now();

    thread* threads = new thread[numberOfThreads];
    int start = 0;
    int cat = n / numberOfThreads;
    int rest = n % numberOfThreads;

    auto convolution = [&matrix_F, &matrix_C, &matrix_V, n, m, k](int start, int finish) {
        for (int i = start; i < finish; i++) {
            for (int j = 0; j < m; j++) {
                int result = 0;
                for (int x = 0; x < k; x++) {
                    for (int y = 0; y < k; y++) {
                        int fRow = i - k / 2 + x;
                        int fCol = j - k / 2 + y;

                        if (fRow < 0) {
                            fRow = 0;
                        }
                        else if (fRow >= n) {
                            fRow = n - 1;
                        }

                        if (fCol < 0) {
                            fCol = 0;
                        }
                        else if (fCol >= m) {
                            fCol = m - 1;
                        }

                        result += matrix_F[fRow][fCol] * matrix_C[x][y];
                    }
                }
                matrix_V[i][j] = result;
            }
        }
    };

    for (int i = 0; i < numberOfThreads; i++) {
        int finish = start + cat;
        if (rest > 0) {
            finish++;
            rest--;
        }
        threads[i] = thread(convolution, start, finish);
        start = finish;
    }

    for (int i = 0; i < numberOfThreads; i++) {
        threads[i].join();
    }

    auto endTime = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() << endl;
}

void writeMatrixToFile(int** matrix, int numRows, int numCols, const string& fileName) {
    ofstream outFile(fileName);
    if (!outFile.is_open()) {
        cerr << "Failed to open the file for writing: " << fileName << endl;
        return;
    }

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            outFile << matrix[i][j] << ' ';
        }
        outFile << '\n'; // End the line
    }

    outFile.close();
}

void readMatrixFromFile(const string& fileName, int** matrix, int numRows, int numCols) {
    ifstream file(fileName);

    if (file.is_open()) {
        string line;
        int row = 0;

        while (getline(file, line) && row < numRows) {
            istringstream iss(line);
            int col = 0;
            int value;
            while (iss >> value && col < numCols) {
                matrix[row][col] = value;
                col++;
            }
            row++;
        }
        file.close();
    }
}

void convolutionSequentialStatic(int n, int m, int k, const int matrix_F[MAX_N][MAX_M], const int matrix_C[MAX_K][MAX_K], int matrix_V[MAX_N][MAX_M]) {
    auto startTime = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            matrix_V[i][j] = 0;

            for (int x = 0; x < k; x++) {
                for (int y = 0; y < k; y++) {
                    int fRow = i - k / 2 + x;
                    int fCol = j - k / 2 + y;

                    // Pentru bordurile virtuale
                    if (fRow < 0) {
                        fRow = 0;
                    }
                    else if (fRow >= n) {
                        fRow = n - 1;
                    }

                    if (fCol < 0) {
                        fCol = 0;
                    }
                    else if (fCol >= m) {
                        fCol = m - 1;
                    }

                    matrix_V[i][j] += matrix_F[fRow][fCol] * matrix_C[x][y];
                }
            }
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() << endl;
}
void convolutionParallelVerticalStatic(int n, int m, int k, const int matrix_F[MAX_N][MAX_M], const int matrix_C[MAX_K][MAX_K], int matrix_V[MAX_N][MAX_M], int numberOfThreads) {
    auto startTime = chrono::high_resolution_clock::now();

    auto convolution = [&matrix_F, &matrix_C, &matrix_V, n, m, k](int start, int finish) {
        for (int j = start; j < finish; j++) {
            for (int i = 0; i < m; i++) {
                int result = 0;
                for (int x = 0; x < k; x++) {
                    for (int y = 0; y < k; y++) {
                        int fRow = i - k / 2 + x;
                        int fCol = j - k / 2 + y;

                        if (fRow < 0) {
                            fRow = 0;
                        }
                        else if (fRow >= n) {
                            fRow = n - 1;
                        }

                        if (fCol < 0) {
                            fCol = 0;
                        }
                        else if (fCol >= m) {
                            fCol = m - 1;
                        }

                        result += matrix_F[fRow][fCol] * matrix_C[x][y];
                    }
                }
                matrix_V[i][j] = result;
            }
        }
    };

    thread* threads = new thread[numberOfThreads];
    int start = 0;
    int cat = n / numberOfThreads;
    int rest = n % numberOfThreads;

    for (int i = 0; i < numberOfThreads; i++) {
        int finish = start + cat;
        if (rest > 0) {
            finish++;
            rest--;
        }
        threads[i] = thread(convolution, start, finish);
        start = finish;
    }

    for (int i = 0; i < numberOfThreads; i++) {
        threads[i].join();
    }

    auto endTime = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() << endl;
}
void convolutionParallelHorizontalStatic(int n, int m, int k, const int matrix_F[MAX_N][MAX_M], const int matrix_C[MAX_K][MAX_K], int matrix_V[MAX_N][MAX_M], int numberOfThreads) {
    auto startTime = chrono::high_resolution_clock::now();

    thread* threads = new thread[numberOfThreads];
    int start = 0;
    int cat = n / numberOfThreads;
    int rest = n % numberOfThreads;

    auto convolution = [&matrix_F, &matrix_C, &matrix_V, n, m, k](int start, int finish) {
        for (int i = start; i < finish; i++) {
            for (int j = 0; j < m; j++) {
                int result = 0;
                for (int x = 0; x < k; x++) {
                    for (int y = 0; y < k; y++) {
                        int fRow = i - k / 2 + x;
                        int fCol = j - k / 2 + y;

                        if (fRow < 0) {
                            fRow = 0;
                        }
                        else if (fRow >= n) {
                            fRow = n - 1;
                        }

                        if (fCol < 0) {
                            fCol = 0;
                        }
                        else if (fCol >= m) {
                            fCol = m - 1;
                        }

                        result += matrix_F[fRow][fCol] * matrix_C[x][y];
                    }
                }
                matrix_V[i][j] = result;
            }
        }
    };

    for (int i = 0; i < numberOfThreads; i++) {
        int finish = start + cat;
        if (rest > 0) {
            finish++;
            rest--;
        }
        threads[i] = thread(convolution, start, finish);
        start = finish;
    }

    for (int i = 0; i < numberOfThreads; i++) {
        threads[i].join();
    }

    auto endTime = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() << endl;
}


static void writeStaticMatrixToFile(const int matrix[MAX_N][MAX_M], int numRows, int numCols, const string& fileName) {
    ofstream outFile(fileName);
    if (!outFile.is_open()) {
        cerr << "Failed to open the file for writing: " << fileName << endl;
        return;
    }

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            outFile << matrix[i][j] << ' ';
        }
        outFile << '\n';
    }

    outFile.close();
}
static void readStaticMatrixFromFile(const string& fileName, int numRows, int numCols, int matrix[MAX_N][MAX_M]) {
    ifstream file(fileName);

    if (file.is_open()) {
        string line;
        int row = 0;

        while (getline(file, line) && row < numRows) {
            istringstream iss(line);
            int col = 0;
            int value;
            while (iss >> value && col < numCols) {
                matrix[row][col] = value;
                col++;
            }
            row++;
        }
        file.close();
    }
}
int main(int argc, char* argv[]) {
    int n = stoi(argv[1]);
    int m = stoi(argv[2]);
    int k = stoi(argv[3]);
    int numberOfThreads = stoi(argv[4]);
    int methodOfRunning = stoi(argv[5]);
    int tipAlocare = stoi(argv[6]);

    int** matrix_F;
    int** matrix_C;
    int** matrix_V;

    if (tipAlocare == 0) {
        // Alocare dinamica
        matrix_F = allocateMatrix(n, m);
        matrix_C = allocateMatrix(k, k);
        matrix_V = allocateMatrix(n, m);

        readMatrixFromFile("data.txt", matrix_F, n, m);
        readMatrixFromFile("convolutionData.txt", matrix_C, k, k);

        switch (methodOfRunning) {
        case 0:
            convolutionSequential(n, m, k, matrix_F, matrix_C, matrix_V);
            writeMatrixToFile(matrix_V, n, m, "output1.txt");
            break;
        case 1:
            convolutionParallelVertical(n, m, k, matrix_F, matrix_C, matrix_V, numberOfThreads);
            writeMatrixToFile(matrix_V, n, m, "output2.txt");
            break;
        case 2:
            convolutionSequential(n, m, k, matrix_F, matrix_C, matrix_V);
            writeMatrixToFile(matrix_V, n, m, "output1.txt");
            convolutionParallelVertical(n, m, k, matrix_F, matrix_C, matrix_V, numberOfThreads);
            writeMatrixToFile(matrix_V, n, m, "output2.txt");
            break;
        default:
            cerr << "Invalid methodOfRunning value." << endl;
            return 1;
        }
    }
    else {
        // Static allocation
        readStaticMatrixFromFile("data.txt", n, m, matrix_F_static);
        readStaticMatrixFromFile("convolutionData.txt", k, k, matrix_C_static);

        switch (methodOfRunning) {
        case 0:
            convolutionSequentialStatic(n, m, k, matrix_F_static, matrix_C_static, matrix_V_static);
            writeStaticMatrixToFile(matrix_V_static, n, m, "output1.txt");
            break;
        case 1:
            //convolutionParallelVerticalStatic(n, m, k, matrix_F_static, matrix_C_static, matrix_V_static, numberOfThreads);
            convolutionParallelHorizontalStatic(n, m, k, matrix_F_static, matrix_C_static, matrix_V_static, numberOfThreads);
            writeStaticMatrixToFile(matrix_V_static, n, m, "output2.txt");
            break;
        case 2:
            convolutionSequentialStatic(n, m, k, matrix_F_static, matrix_C_static, matrix_V_static);
            writeStaticMatrixToFile(matrix_V_static, n, m, "output1.txt");
            convolutionParallelVerticalStatic(n, m, k, matrix_F_static, matrix_C_static, matrix_V_static, numberOfThreads);
            writeStaticMatrixToFile(matrix_V_static, n, m, "output2.txt");
            break;
        default:
            cerr << "Invalid methodOfRunning value." << endl;
            return 1;
        }
    }
    return 0;
}
