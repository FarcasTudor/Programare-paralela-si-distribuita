#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <time.h>
#include <barrier>
#include <sstream>

using namespace std;
using namespace std::chrono;

class MyBarrier {
private:
    mutex m;
    condition_variable cv;
    int counter;
    int waiting;
    int thread_count;
public:
    MyBarrier(int count) : thread_count(count), counter(0), waiting(0) {}
    void wait() {
        unique_lock<mutex> lk(m);
        ++counter;
        ++waiting;
        cv.wait(lk, [&] {return counter >= thread_count; });
        cv.notify_one();
        --waiting;
        if (waiting == 0) {
            counter = 0;
        }
        lk.unlock();
    }
};

int n, m, k, numberOfThreads, runningMethod;

vector<vector<int>> matrix;
vector<vector<int>> kernel;

int compute(vector<int> values, int j, int kernelRow) {
    return values[max(j - 1, 0)] * kernel[kernelRow][0] + values[j] * kernel[kernelRow][1] + values[min(m - 1, j + 1)] * kernel[kernelRow][2];
}

void writeMatrixToFile(vector<vector<int>> matrix, int numRows, int numCols, const string& fileName) {
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

void readMatrixFromFile(const string& fileNameData, const string& fileNameConvolutionData, int n, int m, int k) {
    ifstream file(fileNameData);
    matrix = vector<std::vector<int>>(n, std::vector<int>(m));
    if (file.is_open()) {
        string line;
        int row = 0;

        while (getline(file, line) && row < n) {
            istringstream iss(line);
            int col = 0;
            int value;
            while (iss >> value && col < m) {
                matrix[row][col] = value;
                col++;
            }
            row++;
        }
        file.close();
    }

    ifstream fileConvolution(fileNameConvolutionData);
    kernel = vector<std::vector<int>>(k, std::vector<int>(k));
    if (fileConvolution.is_open()) {
        string line;
        int row = 0;

        while (getline(file, line) && row < k) {
            istringstream iss(line);
            int col = 0;
            int value;
            while (iss >> value && col < k) {
                kernel[row][col] = value;
                col++;
            }
            row++;
        }
        file.close();
    }
}


void sequentialConvolution() {
    vector<int> previousLine;
    vector<int> currentLine;

    previousLine = matrix[0];
    currentLine = matrix[0];

    vector<int> buffer;

    auto startTime = high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        buffer.clear();
        for (int j = 0; j < m; j++) {
            int output = 0;
            output = compute(previousLine, j, 0) + compute(currentLine, j, 1) + compute(matrix[min(n - 1, i + 1)], j, 2);

            buffer.push_back(output);
        }

        matrix[i] = buffer;

        previousLine = currentLine;
        currentLine = matrix[min(n - 1, i + 1)];
    }

    auto endTime = high_resolution_clock::now();

    cout << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() / 1e6 << endl;
}

void parallelConvolution(MyBarrier& barrier, int start, int end) {
    vector<int> previousLine;
    vector<int> currentLine;

    if (start > 0) {
        previousLine = matrix[start - 1];
    }
    else {
        previousLine = matrix[start];
    }
    currentLine = matrix[start];

    vector<int> bufferUp;
    vector<int> bufferDown;
    bufferUp.clear();
    bufferDown.clear();

    for (int i = start; i < end; i++) {
        for (int j = 0; j < m; j++) {
            int output = 0;
            output = compute(previousLine, j, 0) + compute(currentLine, j, 1) + compute(matrix[min(n - 1, i + 1)], j, 2);

            if (i == end - 1) {
                bufferDown.push_back(output);
            }
            else if (i == start) {
                bufferUp.push_back(output);
            }
            else {
                matrix[i][j] = output;
            }
        }

        previousLine = currentLine;
        currentLine = matrix[min(n - 1, i + 1)];
    }

    barrier.wait();

    matrix[start] = bufferUp;
    matrix[end - 1] = bufferDown;
}

void parallelization(MyBarrier& barrier) {
    vector<thread> threads;

    int start = 0, end = 0;
    int cat = n / numberOfThreads;
    int rest = n % numberOfThreads;


    for (size_t i = 0; i < numberOfThreads; i++) {
        start = end;
        end = start + cat;
        if (rest > 0)
        {
            end++;
            rest--;
        }
        thread t = thread(parallelConvolution, ref(barrier), start, end);
        threads.push_back(std::move(t));
    }

    for (auto& th : threads)
    {
        if (th.joinable())
            th.join();
    }
}

int main(int argc, char** argv) {
    n = stoi(argv[1]);
    m = stoi(argv[2]);
    k = stoi(argv[3]);
    numberOfThreads = stoi(argv[4]);
    runningMethod = stoi(argv[5]);
    MyBarrier barrier{ numberOfThreads };

    readMatrixFromFile("data.txt", "dataConvolution", n, m, k);

    int offset = (n - 1) / 2;

    switch (runningMethod) {
    case 0:
    {
        auto startTime = high_resolution_clock::now();
        sequentialConvolution();
        auto endTime = high_resolution_clock::now();
        double difference = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() / 1e6;
        cout << difference << endl;

        writeMatrixToFile(matrix, n, m, "output1.txt");
    }
    break;
    case 1:
    {
        auto startTime = high_resolution_clock::now();
        parallelization(barrier);
        auto endTime = high_resolution_clock::now();
        double difference = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() / 1e6;
        cout << difference << endl;

        writeMatrixToFile(matrix, n, m, "output2.txt");
    }
    break;
    case 2:
    {
        auto startTime = high_resolution_clock::now();
        sequentialConvolution();
        auto endTime = high_resolution_clock::now();
        double difference = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() / 1e6;
        cout << difference << endl;

        writeMatrixToFile(matrix, n, m, "output1.txt");

        readMatrixFromFile("data.txt", "dataConvolution", n, m, k);
        parallelization(barrier);
        writeMatrixToFile(matrix, n, m, "output2.txt");
    }
    break;
    }
}