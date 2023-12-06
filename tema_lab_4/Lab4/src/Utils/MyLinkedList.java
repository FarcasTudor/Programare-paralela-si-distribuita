package Utils;
import java.io.BufferedWriter;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class MyLinkedList {
    private Node head;
    private final Lock lock;

    public MyLinkedList() {
        this.head = null;
        this.lock = new ReentrantLock();
    }

    public void insert(Participant newData) {
        lock.lock();
        if (head == null) {
            head = new Node(newData);
            lock.unlock();
            return;
        }
        Node current = head;
        Node prev = null;
        while (current.next != null && current.data.getPunctaj() >= newData.getPunctaj()) {
            prev = current;
            current = current.next;
        }
        if (prev == null){
            if (current.data.getPunctaj() < newData.getPunctaj()) {
                Node newNode = new Node(newData);
                newNode.next = current;
                head = newNode;
            } else {
                current.next = new Node(newData);
            }
            lock.unlock();
            return;
        }
        if (current.data.getPunctaj() < newData.getPunctaj()) {
            Node newNode = new Node(newData);
            prev.next = newNode;
            newNode.next = current;
        } else {
            current.next = new Node(newData);
        }
        lock.unlock();
    }

    public void remove(String id) {
        lock.lock();
        if (head == null) {
            lock.unlock();
            return;
        }
        Node current = head;
        Node prev = null;
        while (current != null) {
            if (current.data.getId().equals(id)) {
                if (prev == null) {
                    head = current.next;
                } else {
                    prev.next = current.next;
                }
                lock.unlock();
                return;
            }
            prev = current;
            current = current.next;
        }
        lock.unlock();
    }

    public Participant findPair(String id)
    {
        lock.lock();
        Node current = head;
        while (current != null) {
            if (current.data.getId().equals(id)) {
                lock.unlock();
                return current.data;
            }
            current = current.next;
        }
        lock.unlock();
        return null;
    }

    public void printList(BufferedWriter writer) {
        Node current = head;
        while (current != null) {
            try {
                writer.write("ID: " + current.data.getId() + ", Punctaj: " + current.data.getPunctaj() + "\n");
            } catch (Exception e) {
                e.printStackTrace();
            }
            current = current.next;
        }
    }
}
