#include "cube.h"
#include <GL/gl.h>

void Cube::addBasePoint(const Point& p)
{
    basePoints.push_back(p);
}

void Cube::updateExtrusion(double height)
{
    currentExtrusionHeight = height;

    extrudedPoints.clear();
    for (const auto& pt : basePoints) {
        Point extruded = pt;
        extruded.setZ(height);
        extrudedPoints.push_back(extruded);
    }
}

void Cube::buildEdges()
{
    edges.clear();

    if (basePoints.empty() || extrudedPoints.empty()) return;

    // Bottom face
    for (size_t i = 0; i < basePoints.size(); ++i) {
        edges.emplace_back(basePoints[i], basePoints[(i + 1) % basePoints.size()]);
    }

    // Top face
    for (size_t i = 0; i < extrudedPoints.size(); ++i) {
        edges.emplace_back(extrudedPoints[i], extrudedPoints[(i + 1) % extrudedPoints.size()]);
    }

    // Vertical edges
    for (size_t i = 0; i < basePoints.size(); ++i) {
        edges.emplace_back(basePoints[i], extrudedPoints[i]);
    }
}

void Cube::build2DFace()
{
    edges.clear();

    if (basePoints.empty()) return;

    // Create edges for the 2D face
    for (size_t i = 0; i < basePoints.size(); ++i) {
        edges.emplace_back(basePoints[i], basePoints[(i + 1) % basePoints.size()]);
    }
}



void Cube::finalizeExtrusion()
{
    buildEdges();
}


void Cube::draw() const
{
    if (edges.empty())
    {
        return;
    }

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);

    for (const auto& edge : edges)
    {
        glVertex3f(edge.getStart().getX(), edge.getStart().getY(), edge.getStart().getZ());
        glVertex3f(edge.getEnd().getX(), edge.getEnd().getY(), edge.getEnd().getZ());
    }

    glEnd();
}
