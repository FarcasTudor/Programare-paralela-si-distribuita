package Utils;

import java.util.Objects;

public class Participant {
    private final String id;
    private int punctaj;

    public Participant(String id, int punctaj) {
        this.id = id;
        this.punctaj = punctaj;
    }

    public String getId() {
        return id;
    }

    public int getPunctaj() {
        return punctaj;
    }

    public void setPunctaj(int punctaj) {
        this.punctaj = punctaj;
    }

    @Override
    public String toString() {
        return "ID: " + id + ", Punctaj: " + punctaj;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Participant participant = (Participant) o;
        return punctaj == participant.punctaj && Objects.equals(id, participant.id);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, punctaj);
    }
}