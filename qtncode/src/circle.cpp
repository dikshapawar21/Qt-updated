#include "circle.h"
#include <GL/gl.h>
#include <cmath>

Circle::Circle() : c1(0), c2(0), radius(0) {};

Circle::Circle(float x, float y, float r) : c1(x), c2(y), radius(r) {};

void Circle::draw() {
    const int segments = 100; 
    const float angle_increment = 2.0f * 3.14159265f / segments;

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_LOOP);

    points.clear(); // Clear any previously stored points

    for (int i = 0; i < segments; ++i) {
        float angle = i * angle_increment;
        float x = c1 + radius * std::cos(angle);
        float y = c2 + radius * std::sin(angle);
        glVertex2f(x, y);
        points.push_back({x, y});
    }

    glEnd();

}
