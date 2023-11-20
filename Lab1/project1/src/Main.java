import java.io.*;
import java.nio.charset.StandardCharsets;

public class Main {
    static class ThreadMatrix extends Thread {
        private final int k, n, m, start, end;
        private final int[][] matrix_F,matrix_C,matrix_V;

        public ThreadMatrix(int start, int end, int n, int m, int k, int[][] matrix_F, int[][] matrix_C, int[][] matrix_V) {
            this.start = start;
            this.end = end;
            this.n = n;
            this.m = m;
            this.k = k;
            this.matrix_F = matrix_F;
            this.matrix_C = matrix_C;
            this.matrix_V = matrix_V;
        }

        @Override
        public void run() {
            for(int i = start; i < end; i++) {
                for (int j = 0; j < m; j++) {
                    int result = 0;

                    // Formula convolutionala
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

                            result += matrix_F[fRow][fCol] * matrix_C[x][y];
                        }
                    }

                    synchronized (matrix_V) {
                        matrix_V[i][j] = result;
                    }
                }
            }
        }
    }
    static class ThreadMatrixVertical extends Thread {
        private final int k, n, m, start, end;
        private final int[][] matrix_F,matrix_C,matrix_V;

        public ThreadMatrixVertical(int start, int end, int n, int m, int k, int[][] matrix_F, int[][] matrix_C, int[][] matrix_V) {
            this.start = start;
            this.end = end;
            this.n = n;
            this.m = m;
            this.k = k;
            this.matrix_F = matrix_F;
            this.matrix_C = matrix_C;
            this.matrix_V = matrix_V;
        }

        @Override
        public void run() {
            for(int j = start; j < end; j++) {
                for (int i = 0; i < n; i++) {
                    int result = 0;

                    // Formula convolutionala
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

                            result += matrix_F[fRow][fCol] * matrix_C[x][y];
                        }
                    }

                    synchronized (matrix_V) {
                        matrix_V[i][j] = result;
                    }
                }
            }
        }
    }

    public static int[][] convolutionParallelHorizontal(int n, int m, int k, int[][] matrix_F, int[][] matrix_C, int[][] matrix_V, int numberOfThreads) {
        long startTime = System.nanoTime();

        ThreadMatrix[] threads = new ThreadMatrix[numberOfThreads];
        int start = 0;
        int cat = n / numberOfThreads;
        int rest = n % numberOfThreads;

        for (int i = 0; i < numberOfThreads; i++) {
            int finish = start + cat;
            if(rest > 0) {
                finish++;
                rest--;
            }
            threads[i] = new ThreadMatrix(start, finish, n, m, k, matrix_F, matrix_C, matrix_V);
            threads[i].start();
            start = finish;
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
    public static int[][] convolutionParallelVertical(int n, int m, int k, int[][] matrix_F, int[][] matrix_C, int[][] matrix_V, int numberOfThreads) {
        long startTime = System.nanoTime();

        ThreadMatrixVertical[] threads = new ThreadMatrixVertical[numberOfThreads];
        int start = 0;
        int cat = n / numberOfThreads;
        int rest = n % numberOfThreads;

        for (int i = 0; i < numberOfThreads; i++) {
            int finish = start + cat;
            if(rest > 0) {
                finish++;
                rest--;
            }
            threads[i] = new ThreadMatrixVertical(start, finish, n, m, k, matrix_F, matrix_C, matrix_V);
            threads[i].start();
            start = finish;
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
            case 1 -> {
                //writeToFile(convolutionParallelHorizontal(n, m, k, matrix_F, matrix_C, matrix_V, numberOfThreads), "output2.txt");
                writeToFile(convolutionParallelVertical(n, m, k, matrix_F, matrix_C, matrix_V, numberOfThreads), "output2.txt");
            }
            case 2 ->  {
                writeToFile(convolutionSequential(n, m, k, matrix_F, matrix_C, matrix_V), "output1.txt");
                //writeToFile(convolutionParallelHorizontal(n, m, k, matrix_F, matrix_C, matrix_V,numberOfThreads), "output2.txt");
                writeToFile(convolutionParallelVertical(n, m, k, matrix_F, matrix_C, matrix_V,numberOfThreads), "output2.txt");
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