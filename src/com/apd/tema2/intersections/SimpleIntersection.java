package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;
import com.apd.tema2.utils.Constants;

import static java.lang.Thread.sleep;


public abstract class SimpleIntersection implements Intersection {

    public void runSemaphore(Car car) {

        System.out.println(Constants.CAR  + car.getId() + " has " +
                "reached the semaphore, now waiting...");
        try {
            sleep(car.getWaitingTime());
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println(Constants.CAR  + car.getId() +
                " has waited enough, now driving...");
    }

}
