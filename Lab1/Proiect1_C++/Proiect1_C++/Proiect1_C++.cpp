#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <thread>

using namespace std;

std::vector<std::vector<int>> convolutionSequential(int n, int m, int k, const std::vector<std::vector<int>>& matrix_F,
    const std::vector<std::vector<int>>& matrix_C) {
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<int>> matrix_V(n, std::vector<int>(m, 0));

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

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Execution time: " << std::chrono::duration<double, std::milli>(endTime - startTime).count() << " ms" << std::endl;
    return matrix_V;
}

void writeToFile(const std::vector<std::vector<int>>& matrix, const std::string& fileName) {
    std::ofstream outFile(fileName);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open the file for writing: " << fileName << std::endl;
        return;
    }

    for (const std::vector<int>& row : matrix) {
        for (int elem : row) {
            outFile << elem << ' ';
        }
        outFile << '\n'; // End the line
    }

    outFile.close();
}

std::vector<std::vector<int>> readMatrixFromFile(const std::string& fileName, int numRows, int numCols) {
    std::vector<std::vector<int>> matrix(numRows, std::vector<int>(numCols, 0));
    std::ifstream file(fileName);

    if (file.is_open()) {
        std::string line;
        int row = 0;

        while (std::getline(file, line) && row < numRows) {
            std::istringstream iss(line);
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
    return matrix;
}

int main(int argc, char* argv[]) {
    int n = std::atoi(argv[1]);
    int m = std::atoi(argv[2]);
    int k = std::atoi(argv[3]);
    int numberOfThreads = std::atoi(argv[4]);
    int methodOfRunning = std::atoi(argv[5]);

    std::vector<std::vector<int>> matrix_F = readMatrixFromFile("data.txt", n, m);
    std::vector<std::vector<int>> matrix_C = readMatrixFromFile("convolutionData.txt", k, k);

    switch (methodOfRunning) {
    case 0:
        writeToFile(convolutionSequential(n, m, k, matrix_F, matrix_C), "output1.txt");
        break;
    case 1:
        //writeToFile(convolutionParallel(n, m, k, matrix_F, matrix_C, numberOfThreads), "output2.txt");
        break;
    case 2:
        //writeToFile(convolutionSequential(n, m, k, matrix_F, matrix_C), "output1.txt");
        //writeToFile(convolutionParallel(n, m, k, matrix_F, matrix_C, numberOfThreads), "output2.txt");
        break;
    default:
        std::cerr << "Invalid methodOfRunning value." << std::endl;
        return 1;
    }

    return 0;
}

