#include "bezier.h"
#include <GL/gl.h>

Bezier::Bezier(const std::vector<std::vector<double>>& controlPoints, int numInterpolated)
    : controlPoints(controlPoints), interpolatedPoints(numInterpolated)
{
}

std::vector<double> Bezier::deCasteljau(double t) const
{
    std::vector<std::vector<double>> temp = controlPoints;
    int n = controlPoints.size();
    for (int r = 1; r < n; ++r)
    {
        for (int i = 0; i < n - r; ++i)
        {
            temp[i][0] = (1 - t) * temp[i][0] + t * temp[i + 1][0];
            temp[i][1] = (1 - t) * temp[i][1] + t * temp[i + 1][1];
        }
    }
    return temp[0];
}

void Bezier::draw() const
{
    if (controlPoints.empty() || interpolatedPoints <= 0)
        return;

    // Interpolated Bezier curve (yellow)
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= interpolatedPoints; ++i)
    {
        double t = static_cast<double>(i) / interpolatedPoints;
        std::vector<double> point = deCasteljau(t);
        glVertex3f(point[0], point[1], 0.0f);
    }
    glEnd();

    // Control polygon (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& pt : controlPoints)
    {
        glVertex3f(pt[0], pt[1], 0.0f);
    }
    glEnd();

    // Control points (green)
    glPointSize(5.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    for (const auto& pt : controlPoints)
    {
        glVertex3f(pt[0], pt[1], 0.0f);
    }
    glEnd();
}
