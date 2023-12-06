import Utils.MyLinkedList;
import Utils.Participant;

import java.io.*;
import java.util.*;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Main {
    static final String RANKING_PARALLEL_FILE_PATH = "D:\\ANUL_3\\PPD\\tema_lab_4\\Lab4\\src\\outputs\\rankingParallel.txt";
    static final String RANKING_Sequential_FILE_PATH = "D:\\ANUL_3\\PPD\\tema_lab_4\\Lab4\\src\\outputs\\rankingSequential.txt";
    private static final int READER_THREADS = 2;
    private static final int WORKER_THREADS = 16;
    static final Queue<Participant> queue = new LinkedList<>();
    static final MyLinkedList ranking = new MyLinkedList();
    static Reader[] readerThreads;
    static Worker[] workerThreads;
    static Set<String> cheaters = new HashSet<>();

    static Participant findPairInRanking(List<Participant> ranking, Participant pair) {
        return ranking.stream()
                .filter((Participant pairToFind) -> pairToFind.getId().equals(pair.getId()))
                .findFirst()
                .orElse(null);
    }

    public static void readFilesFromDirectorySequential(String directoryPath) {
        List<Participant> ranking = new LinkedList<>();
        Set<String> cheaters = new HashSet<>();

        File directory = new File(directoryPath);
        File[] files = directory.listFiles();

        if (files != null) {
            for (File file : files) {
                try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
                    String line;
                    while ((line = reader.readLine()) != null) {

                        String[] participantFromFile = line.split(",");
                        String id = participantFromFile[0];
                        int score = Integer.parseInt(participantFromFile[1]);

                        if (cheaters.contains(id)) continue;

                        if (score == -1) {
                            cheaters.add(id);
                            ranking.remove(findPairInRanking(ranking, new Participant(id, score)));
                            continue;
                        }

                        Participant participant = new Participant(id, score);
                        Participant existingParticipant = findPairInRanking(ranking, participant);

                        if (existingParticipant != null)
                            existingParticipant.setPunctaj(existingParticipant.getPunctaj() + score);
                        else ranking.add(participant);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            ranking.sort((o1, o2) -> o2.getPunctaj() - o1.getPunctaj());
            writeListToFile(RANKING_Sequential_FILE_PATH, ranking);
        }
    }

    private static void radFilesFromDirectoryParallel(String directoryPath, int numFiles) {
        readerThreads = new Reader[READER_THREADS];
        workerThreads = new Worker[WORKER_THREADS];

        int start = 0;
        int cat = numFiles / READER_THREADS;
        int rest = numFiles % READER_THREADS;
        for (int i = 0; i < READER_THREADS; i++) {
            int end = start + cat;
            if (rest > 0) {
                end++;
                rest--;
            }
            readerThreads[i] = new Reader(directoryPath, start, end);
            readerThreads[i].start();
            start = end;
        }

        for (int i = 0; i < WORKER_THREADS; i++) {
            workerThreads[i] = new Worker();
            workerThreads[i].start();
        }

        for (int i = 0; i < READER_THREADS; i++) {
            try {
                readerThreads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for(Worker worker : workerThreads) {
            // worker.stopProcessing();
            worker.interrupt();
        }

        for (int i = 0; i < WORKER_THREADS; i++) {
            try {
                workerThreads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        readerThreads[0].writeRankingToFile(RANKING_PARALLEL_FILE_PATH);
    }

    public static void writeListToFile(String fileName, List<Participant> ranking) {
        try (PrintWriter writer = new PrintWriter(fileName)) {
            ranking.forEach(participant -> writer.printf("ID: %s, Punctaj: %s%n", participant.getId(), participant.getPunctaj()));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        String directoryPath = "D:\\ANUL_3\\PPD\\tema_lab_4\\Lab4\\out\\production\\Lab4\\results";

        long startTime = System.currentTimeMillis();

        readFilesFromDirectorySequential(directoryPath);

        int numFiles = Objects.requireNonNull(new File(directoryPath).listFiles()).length;
        radFilesFromDirectoryParallel(directoryPath, numFiles);

        long endTime = System.currentTimeMillis();
        System.out.println("Total execution time: " + (endTime - startTime) + "ms");
    }

    public static void writeLinkedListToFile(String filePath) {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filePath))) {
            ranking.printList(writer);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}