package test;

import java.io.*;
import java.nio.charset.StandardCharsets;

public class TestMain {
    static class ThreadMatrix extends Thread {
        private int row;
        private int n, m, k;
        private int[][] matrix_F, matrix_C, matrix_V;

        public ThreadMatrix(int row, int n, int m, int k, int[][] matrix_F, int[][] matrix_C, int[][] matrix_V) {
            this.row = row;
            this.n = n;
            this.m = m;
            this.k = k;
            this.matrix_F = matrix_F;
            this.matrix_C = matrix_C;
            this.matrix_V = matrix_V;
        }

        @Override
        public void run() {
            for (int j = 0; j < m; j++) {
                matrix_V[row][j] = 0;

                // Formula convolutionala
                for (int x = 0; x < k; x++) {
                    for (int y = 0; y < k; y++) {
                        int fRow = row - k / 2 + x;
                        int fCol = j - k / 2 + y;

                        // Pentru bordurile virtuale
                        if (fRow < 0) {
                            fRow = 0;
                        } else if (fRow >= n) {
                            fRow = n - 1;
                        }

                        if (fCol < 0) {
                            fCol = 0;
                        } else if (fCol >= m) {
                            fCol = m - 1;
                        }

                        matrix_V[row][j] += matrix_F[fRow][fCol] * matrix_C[x][y];
                    }
                }
            }
        }
    }

    public static int[][] convolutionParallel(int n, int m, int k, int[][] matrix_F, int[][] matrix_C, int[][] matrix_V, int numberOfThreads) {
        long startTime = System.nanoTime();

        ThreadMatrix[] threads = new ThreadMatrix[numberOfThreads];
        int rowsPerThread = n / numberOfThreads;
        int remainingRows = n % numberOfThreads;
        int startRow = 0;

        for (int i = 0; i < numberOfThreads; i++) {
            int rowsForThread = rowsPerThread + (i < remainingRows ? 1 : 0);
            threads[i] = new ThreadMatrix(startRow, rowsForThread, m, k, matrix_F, matrix_C, matrix_V);
            threads[i].start();
            startRow += rowsForThread;
        }

        // Wait for all threads to finish
        for (int i = 0; i < numberOfThreads; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        long endTime = System.nanoTime();
        System.out.println((double)(endTime - startTime) / 1e6);
        return matrix_V;
    }
    public static void main(String[] args) {

        int n = Integer.parseInt(args[0]);
        int m = Integer.parseInt(args[1]);
        int k = Integer.parseInt(args[2]);
        int numberOfThreads = Integer.parseInt(args[3]);
        int methodOfRunning = Integer.parseInt(args[4]);

        int[][] matrix_F = readMatrixFromFile("data.txt", n, m);
        int[][] matrix_C = readMatrixFromFile("convolutionData.txt", k, k);

        int[][] matrix_V = new int[n][m];

        switch (methodOfRunning) {
            case 0 -> writeToFile(convolutionSequential(n, m, k, matrix_F, matrix_C, matrix_V), "output1.txt");
            case 1 -> writeToFile(convolutionParallel(n, m, k, matrix_F, matrix_C, matrix_V, numberOfThreads), "output2.txt");
            case 2 ->  {
                writeToFile(convolutionSequential(n, m, k, matrix_F, matrix_C, matrix_V), "output1.txt");
                writeToFile(convolutionParallel(n, m, k, matrix_F, matrix_C, matrix_V,numberOfThreads), "output2.txt");
            }
        }
    }

    private static void writeToFile(int[][] matrix, String fileName) {
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
    private static int[][] convolutionSequential(int n, int m, int k, int[][] matrix_F, int[][] matrix_C, int[][] matrix_V) {
        long startTime = System.nanoTime();
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                matrix_V[i][j] = 0;

                for (int x = 0; x < k; x++) {
                    for (int y = 0; y < k; y++) {
                        int fRow = i - k / 2 + x;
                        int fCol = j - k / 2 + y;

                        // Pentru bordurile virtuale
                        if (fRow < 0) {
                            fRow = 0;
                        } else if (fRow >= n) {
                            fRow = n - 1;
                        }

                        if (fCol < 0) {
                            fCol = 0;
                        } else if (fCol >= m) {
                            fCol = m - 1;
                        }

                        matrix_V[i][j] += matrix_F[fRow][fCol] * matrix_C[x][y];
                    }
                }
            }
        }
        long endTime = System.nanoTime();
        System.out.println((double)(endTime - startTime) / 1e6);
        return matrix_V;
    }
    private static int[][] readMatrixFromFile(String fileName, int numRows, int numCols) {
        int[][] matrix = new int[numRows][numCols];
        try (BufferedReader br = new BufferedReader(new FileReader(fileName))) {
            String line;
            int row = 0;

            while ((line = br.readLine()) != null && row < numRows) {
                String[] elements = line.split(" ");
                for (int col = 0; col < numCols && col < elements.length; col++) {
                    matrix[row][col] = Integer.parseInt(elements[col]);
                }
                row++;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return matrix;
    }
}