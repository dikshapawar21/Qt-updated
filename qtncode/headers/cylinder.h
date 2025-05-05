#pragma once

class Cylinder
{
public:
    Cylinder(float radius = 30.0f, float height = 60.0f, int slices = 20, int stacks = 10);
    void draw() const;

private:
    float radius;
    float height;
    int slices;
    int stacks;
};
