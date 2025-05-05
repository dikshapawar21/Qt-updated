#include "cylinder.h"
#include <GL/gl.h>
#include <cmath>

Cylinder::Cylinder(float radius, float height, int slices, int stacks)
    : radius(radius), height(height), slices(slices), stacks(stacks)
{
}

void Cylinder::draw() const
{
    float halfHeight = height / 2.0f;

    // Draw horizontal rings (stacks)
    for (int i = 0; i <= stacks; ++i)
    {
        float z = -halfHeight + (height * i / stacks);
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < slices; ++j)
        {
            float angle = 2 * M_PI * j / slices;
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    // Draw vertical lines (slices)
    for (int j = 0; j < slices; ++j)
    {
        float angle = 2 * M_PI * j / slices;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= stacks; ++i)
        {
            float z = -halfHeight + (height * i / stacks);
            glVertex3f(x, y, z);
        }
        glEnd();
    }
}
