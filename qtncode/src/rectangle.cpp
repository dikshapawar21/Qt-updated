#include "rectangle.h"
#include <GL/gl.h>

Rectangle::Rectangle() : X(0), Y(0), length(0), width(0) {}

Rectangle::Rectangle(float cx, float cy, float len, float wid)
    : X(cx), Y(cy), length(len), width(wid) {}

void Rectangle::draw()
{
    float halfLen = length / 2.0f;
    float halfWid = width / 2.0f;
    float x1 = X - halfLen;
    float x2 = X + halfLen;
    float y1 = Y - halfWid;
    float y2 = Y + halfWid;

    vertices.clear();
    vertices.push_back({x1, y1});
    vertices.push_back({x2, y1});
    vertices.push_back({x2, y2});
    vertices.push_back({x1, y2});

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    for (const auto &v : vertices)
    {
        glVertex2f(v[0], v[1]);
    }
    glEnd();
}