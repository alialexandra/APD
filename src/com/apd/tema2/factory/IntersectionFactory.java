package com.apd.tema2.factory;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;
import com.apd.tema2.intersections.*;
import com.apd.tema2.intersections.SimpleRoundabout;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Semaphore;

/**
 * Prototype Factory: va puteti crea cate o instanta din fiecare tip de implementare de Intersection.
 */
public class IntersectionFactory {
    private static Map<String, Intersection> cache = new HashMap<>();


    static {
        cache.put("simpleIntersection", new SimpleIntersection() {

            @Override
            public void solveStrictRound(Car car, Semaphore sem, int time) {

            }

            @Override
            public void solveRound(Car car, Semaphore sem, int time) {

            }
        });
        cache.put("simpleRoundabout", new SimpleRoundabout());
        cache.put("strictRoundabout", new StrictRoundabout());
    }

    public static Intersection getIntersection(String handlerType) {
        return cache.get(handlerType);
    }

}
