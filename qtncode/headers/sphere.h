#pragma once

#include <vector>

class Sphere
{
public:
    Sphere(float radius = 30.0f, int slices = 20, int stacks = 20);

    void draw() const;

private:
    float radius;
    int slices;
    int stacks;
};
