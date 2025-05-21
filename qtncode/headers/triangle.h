#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <iostream>
#include "point.h"

class Triangle {
public:
    POINT p1, p2, p3;
    Triangle(POINT p1 = POINT(), POINT p2 = POINT(), POINT p3 = POINT()) : p1(p1), p2(p2), p3(p3) {}
};

#endif