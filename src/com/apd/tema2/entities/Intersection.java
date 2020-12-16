package com.apd.tema2.entities;

import java.util.Map;
import java.util.concurrent.Semaphore;

/**
 * Utilizata pentru a uniformiza tipul de date ce ajuta la definirea unei intersectii / a unui task.
 * Implementarile acesteia vor contine variabile specifice task-ului, respectiv mecanisme de sincronizare.
 */
public interface Intersection {

    void solveRound(Car car, Semaphore sem, int time);
    void solveStrictRound(Car car, Semaphore sem, int time);
    void runSemaphore(Car car);

}
