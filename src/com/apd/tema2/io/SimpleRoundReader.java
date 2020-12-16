package com.apd.tema2.io;

import java.util.Map;
import java.util.concurrent.Semaphore;

public class SimpleRoundReader {

    private int time;
    private int cars;
    private Semaphore sem;
    //private Map<Integer, Semaphore> map;

    public SimpleRoundReader(int cars, int time) {
        this.time = time;
        this.cars = cars;
        this.sem = new Semaphore(cars);
    }

    public int getTime() {
        return time;
    }

    public void setTime(int time) {
        this.time = time;
    }

    public int getCars() {
        return cars;
    }

    public void setCars(int cars) {
        this.cars = cars;
    }

    public Semaphore getSem() {
        return sem;
    }

    public void setSem(Semaphore sem) {
        this.sem = sem;
    }
}
