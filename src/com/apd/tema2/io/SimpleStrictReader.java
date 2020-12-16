package com.apd.tema2.io;

import java.util.Map;
import java.util.concurrent.Semaphore;

public class SimpleStrictReader {

    private int time;
    private int cars;
    private Map<Integer, Semaphore> semaphoreMap;
    private Semaphore sem;

    public SimpleStrictReader(int cars, int time) {
        this.time = time;
        this.cars = cars;
        sem = new Semaphore(cars);
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

    public Map<Integer, Semaphore> getSemaphoreMap() {
        return semaphoreMap;
    }

    public void setSemaphoreMap(Map<Integer, Semaphore> semaphoreMap) {
        this.semaphoreMap = semaphoreMap;
    }

    public Semaphore getSem() {
        return sem;
    }

    public void setSem(Semaphore sem) {
        this.sem = sem;
    }
}
