#include "sphere.h"
#include <GL/gl.h>
#include <cmath>

Sphere::Sphere(float radius, int slices, int stacks)
    : radius(radius), slices(slices), stacks(stacks)
{
}

void Sphere::draw() const
{
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white for visibility

    // Draw lines of latitude (horizontal rings)
    for (int i = 1; i < stacks; ++i) {
        float lat = M_PI * (-0.5f + (float)(i) / stacks);
        float z = sin(lat);
        float zr = cos(lat);

        glBegin(GL_LINE_LOOP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2 * M_PI * (float)(j) / slices;
            float x = cos(lng);
            float y = sin(lng);
            glVertex3f(x * zr * radius, y * zr * radius, z * radius);
        }
        glEnd();
    }

    // Draw lines of longitude (vertical lines)
    for (int j = 0; j < slices; ++j) {
        float lng = 2 * M_PI * (float)(j) / slices;
        float x = cos(lng);
        float y = sin(lng);

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= stacks; ++i) {
            float lat = M_PI * (-0.5f + (float)(i) / stacks);
            float z = sin(lat);
            float zr = cos(lat);
            glVertex3f(x * zr * radius, y * zr * radius, z * radius);
        }
        glEnd();
    }
}
