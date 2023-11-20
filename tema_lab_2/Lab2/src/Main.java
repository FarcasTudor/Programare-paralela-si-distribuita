import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

import static java.lang.Math.max;
import static java.lang.Math.min;

public class Main {
    private static int n;
    private static int m;
    private static int k;
    private static int numberOfThreads;
    private static int[][] matrix;
    private static int[][] kernel;
    private static CyclicBarrier barrier;

    private static int[][] readMatrixFromFile(String fileName, int rows, int cols) {
        int[][] matrix = new int[rows][cols];
        try (BufferedReader br = new BufferedReader(new FileReader(fileName))) {
            String line;
            int row = 0;

            while ((line = br.readLine()) != null && row < rows) {
                String[] elements = line.split(" ");
                for (int col = 0; col < cols && col < elements.length; col++) {
                    matrix[row][col] = Integer.parseInt(elements[col]);
                }
                row++;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return matrix;
    }
    private static void writeMatrixToFile(int[][] matrix, String fileName) {
        try {
            BufferedWriter bufferedWriter = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(fileName), StandardCharsets.UTF_8));
            for (int[] row : matrix) {
                for (int elem : row) {
                    bufferedWriter.write(elem + " ");
                }
                bufferedWriter.newLine();
            }
            bufferedWriter.close();
        } catch (IOException ioException) {
            ioException.printStackTrace();
        }
    }
    private static void convolutionSequential() {
        long startTime = System.nanoTime();

        int[] previousLine = new int[m];
        int[] currentLine = new int[m];

        System.arraycopy(matrix[0], 0, previousLine, 0, m);
        System.arraycopy(matrix[0], 0, currentLine, 0, m);

        for (int i = 0; i < n; i++) {
            int[] buffer = new int[m];
            for (int j = 0; j < m; j++) {
                int output = compute(previousLine, j, 0) + compute(currentLine, j, 1) + compute(matrix[min(n - 1, i + 1)], j, 2);
                buffer[j] = output;
            }

            System.arraycopy(buffer, 0, matrix[i], 0, m);
            System.arraycopy(currentLine, 0, previousLine, 0, currentLine.length);
            System.arraycopy(matrix[min(n - 1, i + 1)], 0, currentLine, 0, currentLine.length);
        }

        long endTime = System.nanoTime();
        System.out.println((double)(endTime - startTime) / 1E6);
    }

    public static void main(String[] args) throws InterruptedException {
        n = Integer.parseInt(args[0]);
        m = Integer.parseInt(args[1]);
        k = Integer.parseInt(args[2]);
        numberOfThreads = Integer.parseInt(args[3]);
        int runningMethod = Integer.parseInt(args[4]);

        if(runningMethod != 0) {
            barrier = new CyclicBarrier(numberOfThreads);
        }

        matrix = readMatrixFromFile("data.txt", n, m);
        kernel = readMatrixFromFile("convolutionData.txt", k, k);

        switch (runningMethod) {
            case 0 -> {
                convolutionSequential();
                writeMatrixToFile(matrix, "output1.txt");
            }
            case 1 -> {
                convolutionParallel();
                writeMatrixToFile(matrix, "output2.txt");
            }
            case 2 -> {
                convolutionSequential();
                writeMatrixToFile(matrix, "output1.txt");
                matrix = readMatrixFromFile("data.txt", n, m);
                convolutionParallel();
                writeMatrixToFile(matrix, "output2.txt");
            }
        }
    }

    public static void convolutionParallel() throws InterruptedException {
        Thread[] t = new Worker[numberOfThreads];

        int start, end = 0;
        int cat = n / numberOfThreads;
        int rest = n % numberOfThreads;
        long startTime = System.nanoTime();

        for (int i = 0; i < t.length; i++) {
            start = end;
            end = start + cat;
            if (rest > 0) {
                end++;
                rest--;
            }
            t[i] = new Worker(start, end);
            t[i].start();
        }
        for (Thread thread : t) {
            thread.join();
        }
        long stopTime = System.nanoTime();
        System.out.println((double)(stopTime - startTime) / 1E6);
    }

    public static class Worker extends Thread {
        int start, end;

        public Worker(int start, int end) {
            this.start = start;
            this.end = end;
        }

        public void run() {
            int[] previousLine = new int[m];
            int[] currentLine = new int[m];

            System.arraycopy(matrix[Math.max(start - 1, 0)], 0, previousLine, 0, m);
            System.arraycopy(matrix[start], 0, currentLine, 0, m);

            int[] bufferUp = new int[m];
            int[] bufferDown = new int[m];

            for (int i = start; i < end; i++) {
                for (int j = 0; j < m; j++) {
                    int output = compute(previousLine, j, 0) + compute(currentLine, j, 1) + compute(matrix[min(n - 1, i + 1)], j, 2);

                    if (i == start) {
                        bufferUp[j] = output;
                    } else if (i == end - 1) {
                        bufferDown[j] = output;
                    } else {
                        matrix[i][j] = output;
                    }
                }

                System.arraycopy(currentLine, 0, previousLine, 0, currentLine.length);
                System.arraycopy(matrix[min(n - 1, i + 1)], 0, currentLine, 0, currentLine.length);
            }

            try {
                barrier.await();
            } catch (InterruptedException | BrokenBarrierException ignored) {}

            System.arraycopy(bufferUp, 0, matrix[start], 0, m);
            System.arraycopy(bufferDown, 0, matrix[end - 1], 0, m);
        }
    }
    private static int compute(int[] rowValues, int j, int kernelRow) {
        return rowValues[max(j - 1, 0)] * kernel[kernelRow][0] + rowValues[j] * kernel[kernelRow][1] + rowValues[min(m - 1, j + 1)] * kernel[kernelRow][2];
    }

}