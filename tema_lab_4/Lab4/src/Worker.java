
import Utils.Participant;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Worker extends Thread {
    static final Lock listLock = new ReentrantLock();

    /*private volatile boolean running = true;

    public void stopProcessing() {
        running = false;
    }*/

    @Override
    public void run() {
        while (true) {
            Participant participant;
            synchronized (Reader.lock) {
                try {
                    while (Main.queue.isEmpty()) {
                        Reader.lock.wait();
                    }
                    participant = Main.queue.poll();
                } catch (InterruptedException e) {
                    return;
                }
            }
            listLock.lock();
            assert participant != null;
            if (Main.cheaters.contains(participant.getId())) {
                listLock.unlock();
                continue;
            }
            if (participant.getPunctaj() == -1) {
                Main.cheaters.add(participant.getId());
                Main.ranking.remove(participant.getId());
                listLock.unlock();
                continue;
            }
            Participant existingParticipant = Main.ranking.findPair(participant.getId());
            if (existingParticipant != null) {
                existingParticipant.setPunctaj(existingParticipant.getPunctaj() + participant.getPunctaj());
                Main.ranking.remove(participant.getId());
                Main.ranking.insert(existingParticipant);
            } else {
                Main.ranking.insert(participant);
            }
            listLock.unlock();
            /*if(!running) {
                break;
            }*/
        }
    }
}
