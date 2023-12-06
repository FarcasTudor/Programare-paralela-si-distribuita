import Utils.Participant;

import java.io.*;

class Reader extends Thread {
    private final String directoryPath;
    static final Object lock = new Object();
    private final int start;
    private final int end;

    public Reader(String directoryPath, int startIndex, int endIndex) {
        this.directoryPath = directoryPath;
        this.start = startIndex;
        this.end = endIndex;
    }

    @Override
    public void run() {
        File directory = new File(directoryPath);
        File[] files = directory.listFiles();

        if (files != null) {
            for (int i = start; i < end; i++) {
                File file = files[i];
                try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
                    String line;
                    while ((line = reader.readLine()) != null) {
                        String[] participantFromFile = line.split(",");
                        String id = participantFromFile[0];
                        int score = Integer.parseInt(participantFromFile[1]);
                        Participant participant = new Participant(id, score);
                        synchronized (lock) {
                            Main.queue.add(participant);
                            lock.notify();
                        }                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        synchronized (lock) {
            lock.notifyAll();
        }
    }
    public void writeRankingToFile(String filePath) {
        Main.writeLinkedListToFile(filePath);
    }
}
