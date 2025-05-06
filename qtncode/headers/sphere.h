#pragma once

#include <vector>

class Sphere
{
public:

    Sphere(float radius, int slices, int stacks);

    void draw() const;

private:
    float radius;
    int slices;
    int stacks;
};
