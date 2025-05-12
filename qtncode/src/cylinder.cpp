#include "cylinder.h"
#include <vector>
#include <GL/gl.h>
#include <cmath>

Cylinder::Cylinder(float radius, float height, int slices, int stacks)
    : radius(radius), height(height), slices(slices), stacks(stacks)
{
}

void Cylinder::draw() const
{
    float halfHeight = height / 2.0f;

    // Data structures to store points and lines
    std::vector<std::pair<float, float>> bottomRingPoints;
    std::vector<std::pair<float, float>> topRingPoints;
    std::vector<std::pair<std::pair<float, float>, std::pair<float, float>>> verticalEdges;

    // Draw the bottom ring (stack) and store points
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j < slices; ++j)
    {
        float angle = 2 * M_PI * j / slices;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        bottomRingPoints.emplace_back(x, y); // Store bottom ring points
        glVertex3f(x, y, -halfHeight);       // Bottom ring at -halfHeight
    }
    glEnd();

    // Draw the top ring (stack) and store points
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j < slices; ++j)
    {
        float angle = 2 * M_PI * j / slices;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        topRingPoints.emplace_back(x, y); // Store top ring points
        glVertex3f(x, y, halfHeight);     // Top ring at +halfHeight
    }
    glEnd();

    // Draw vertical lines (slices) connecting the top and bottom rings and store edges
    for (int j = 0; j < slices; ++j)
    {
        float angle = 2 * M_PI * j / slices;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        // Store vertical edges as pairs of points
        verticalEdges.emplace_back(
            std::make_pair(x, y), // Bottom point
            std::make_pair(x, y)  // Top point
        );

        glBegin(GL_LINES);
        glVertex3f(x, y, -halfHeight); // Bottom ring
        glVertex3f(x, y, halfHeight);  // Top ring
        glEnd();
    }
}
