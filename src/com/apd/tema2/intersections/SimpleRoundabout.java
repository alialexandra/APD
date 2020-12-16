package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;
import com.apd.tema2.utils.Constants;

import java.util.Map;
import java.util.concurrent.Semaphore;

import static java.lang.Thread.sleep;

public final class SimpleRoundabout implements Intersection{

    public void solveRound(Car car, Semaphore sem, int time) {


        System.out.println(Constants.CAR  +car.getId()
                + " has reached the roundabout, now waiting...");
        try {
            sem.acquire();

        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println(Constants.CAR + car.getId() +
                " has entered the roundabout");
        try {
            sleep(time);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        try {
            System.out.println(Constants.CAR  + car.getId()
                    + " has exited the roundabout after "
                    + time / 1000 + " seconds");
        }
        finally {
            sem.release();
        }

    }

    @Override
    public void solveStrictRound(Car car, Semaphore sem, int time) {

    }

    @Override
    public void runSemaphore(Car car) {

    }

}
