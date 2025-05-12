#pragma once
#include <vector>

class Circle{
    private:
    float c1, c2, radius;

    public:

    Circle();
    Circle(float c1, float c2, float r);

    std::vector<std::vector<float>>points;
    void draw();

};