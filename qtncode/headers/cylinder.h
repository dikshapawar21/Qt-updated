#pragma once

class Cylinder
{
public:
    Cylinder(float radius, float height, int slices, int stacks);
    void draw() const;

private:
    float radius;
    float height;
    int slices;
    int stacks;

};
