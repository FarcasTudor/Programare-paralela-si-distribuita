import javax.swing.plaf.basic.BasicInternalFrameTitlePane;

public class Main {

    public static class ThreadSum2 extends Thread {
        private int a[], b[], c[], n, p, startPos;
        public ThreadSum2(int a[], int b[], int c[], int n, int p, int startPos) {
            this.a = a;
            this.b = b;
            this.c = c;
            this.n = n;
            this.p = p;
            this.startPos = startPos;
        }
        @Override
        public void run() {
            for(int i = startPos; i < n; i += p) {
                c[i] = (int) (Math.pow(a[i],3) + Math.pow(b[i],3));// = a[i] + b[i];
            }
        }
    }

    private static void sum_paralel_cyclic(int n, int[] v1, int[] v2, int[] v3, int p) throws InterruptedException {
        ThreadSum2[] threads = new ThreadSum2[p];
        for(int i = 0; i < p; i++) {
            ThreadSum2 t = new ThreadSum2(v1,v2,v3,n,p,i);
            t.start();
            threads[i] = t;
        }
        for(int i = 0; i < p; i++) {
            threads[i].join();
        }

    }
    public static class ThreadSum extends Thread {
        private int a[], b[], c[], start, finish;
        public ThreadSum(int a[], int b[], int c[], int start, int finish) {
            this.a = a;
            this.b = b;
            this.c = c;
            this.start = start;
            this.finish = finish;
        }
        @Override
        public void run() {
            for(int i = start; i < finish; i++) {
                c[i] = (int) (Math.pow(a[i],3) + Math.pow(b[i],3));// = a[i] + b[i];
            }
        }
    }
    public static void main(String[] args) throws InterruptedException {
        int n = 20000000;
        int p = 16;
        int v1[] = new int[n];
        int v2[] = new int[n];
        int v3[] = new int[n];

        for(int i = 0; i < n; i++) {
            v1[i] = i*i;
            v2[i] = -i;
        }
        var startTime = System.nanoTime();
        sum(n, v1, v2, v3);
        var endTime = System.nanoTime();
        System.out.println("Secvential: " + (double)(endTime-startTime));


        startTime = System.nanoTime();
        sum_paralel(n, v1, v2, v3, p);
        endTime = System.nanoTime();
        System.out.println("Paralel: " + (double)(endTime - startTime));

        startTime = System.nanoTime();
        sum_paralel_cyclic(n, v1, v2, v3, p);
        endTime = System.nanoTime();
        System.out.println("Paralel cyclic: " + (double)(endTime - startTime));

    }

    private static void sum_paralel(int n, int[] v1, int[] v2, int[] v3, int p) throws InterruptedException {
        int start = 0;
        int cat = n / p;
        int rest = n % p;
        ThreadSum tSum[] = new ThreadSum[p];
        for(int i = 0; i < p; i++) {
            int finish = start + cat;
            if(rest > 0) {
                finish++;
                rest--;
            }
            tSum[i] = new ThreadSum(v1, v2, v3, start, finish);
            tSum[i].start();

            start = finish;
        }
        for(int i = 0; i < p; i++ ) {
            tSum[i].join();
        }
    }

    private static void sum(int n, int[] v1, int[] v2, int[] v3) {
        for(int i = 0; i < n; i++) {
            v3[i] = (int) (Math.pow(v2[i],3) + Math.pow(v1[i],3));// = a[i] + b[i];
        }
    }
}