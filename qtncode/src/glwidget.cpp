// glwidget.cpp
#include "glwidget.h"
#include <algorithm>
#include <QDebug>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {}

GLWidget::~GLWidget() {}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.2, 0.2, 0.2, 1.0);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-100, 100, -100, 100, -1, 1); // simple 2D orthographic view
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (!resultShape.empty())
    {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow color
        glBegin(GL_POLYGON);         // or GL_LINE_LOOP if outline only
        for (const auto &pt : resultShape)
        {
            glVertex2f(pt[0], pt[1]);
        }
        glEnd();
        return;
    }

    if (drawCircle)
    {
        circle.draw();
    }

    if (drawRectangle)
        for (auto &rect : rectangles)
            rect.draw();
}

void GLWidget::setCircleData(float cx, float cy, float r)
{
    circle = Circle(cx, cy, r);
    drawCircle = true;

    // generate circle.points here
    const int segments = 100;
    const float angle_increment = 2.0f * 3.14159265f / segments;
    circle.points.clear();
    for (int i = 0; i < segments; ++i)
    {
        float angle = i * angle_increment;
        float x = cx + r * std::cos(angle);
        float y = cy + r * std::sin(angle);
        circle.points.push_back({x, y});
    }

    update();
}

void GLWidget::setRectangleData(float x1, float y1, float p1, float q1)
{
    
    drawRectangle = true;
    Rectangle rect(x1, y1, p1, q1);
    rectangles.push_back(rect);
    update();
}

void GLWidget::computeUnion()
{
    resultShape.clear();
    if (rectangles.size() >= 2)
    {
        Union u;
        resultShape = u.compute(rectangles[0].vertices, rectangles[1].vertices);
        showResult = true;
        update();
    }
}

void GLWidget::computeIntersection()
{
    resultShape.clear();
    if (rectangles.size() >= 2)
    {
        Intersection i;
        resultShape = i.compute(rectangles[0].vertices, rectangles[1].vertices);
        showResult = true;
        update();
    }
}

void GLWidget::computeSubtraction()
{
    resultShape.clear();
    if (rectangles.size() >= 2)
    {
        Subtraction s;
        resultShape = s.compute(rectangles[0].vertices, rectangles[1].vertices);
        showResult = true;
        update();
    }
}



static bool pointInPolygon(const std::vector<std::vector<float>> &polygon, float x, float y)
{
    bool inside = false;
    size_t n = polygon.size();
    for (size_t i = 0, j = n - 1; i < n; j = i++)
    {
        const auto &pi = polygon[i];
        const auto &pj = polygon[j];
        if ((pi[1] > y) != (pj[1] > y) &&
            x < (pj[0] - pi[0]) * (y - pi[1]) / (pj[1] - pi[1] + 1e-6) + pi[0])
        {
            inside = !inside;
        }
    }
    return inside;
}

std::vector<std::vector<float>> clipPolygon(
    const std::vector<std::vector<float>> &subjectPolygon,
    const std::vector<std::vector<float>> &clipPolygon)
{
    std::vector<std::vector<float>> output = subjectPolygon;

    for (size_t i = 0; i < clipPolygon.size(); ++i)
    {
        std::vector<std::vector<float>> input = output;
        output.clear();

        std::vector<float> A = clipPolygon[i];
        std::vector<float> B = clipPolygon[(i + 1) % clipPolygon.size()];

        auto inside = [&](const std::vector<float> &p) -> bool
        {
            return (B[0] - A[0]) * (p[1] - A[1]) - (B[1] - A[1]) * (p[0] - A[0]) >= 0;
        };

        auto intersection = [&](const std::vector<float> &p1, const std::vector<float> &p2) -> std::vector<float>
        {
            float x1 = p1[0], y1 = p1[1];
            float x2 = p2[0], y2 = p2[1];
            float x3 = A[0], y3 = A[1];
            float x4 = B[0], y4 = B[1];

            float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
            if (fabs(denom) < 1e-6)
                return p2; // Lines are nearly parallel

            float xi = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denom;
            float yi = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / denom;
            return {xi, yi};
        };

        for (size_t j = 0; j < input.size(); ++j)
        {
            const auto &current = input[j];
            const auto &prev = input[(j + input.size() - 1) % input.size()];
            bool currIn = inside(current);
            bool prevIn = inside(prev);

            if (currIn && prevIn)
            {
                output.push_back(current);
            }
            else if (prevIn && !currIn)
            {
                output.push_back(intersection(prev, current));
            }
            else if (!prevIn && currIn)
            {
                output.push_back(intersection(prev, current));
                output.push_back(current);
            }
        }
    }

    // Remove duplicate points and ensure closed polygon
    std::vector<std::vector<float>> result;
    for (size_t i = 0; i < output.size(); ++i)
    {
        if (result.empty() || (fabs(result.back()[0] - output[i][0]) > 1e-6 || fabs(result.back()[1] - output[i][1]) > 1e-6))
        {
            result.push_back(output[i]);
        }
    }

    if (!result.empty() && (fabs(result.front()[0] - result.back()[0]) > 1e-6 || fabs(result.front()[1] - result.back()[1]) > 1e-6))
    {
        result.push_back(result.front());
    }

    return result;
}

std::vector<std::vector<float>> Intersection::compute(
    const std::vector<std::vector<float>> &poly1,
    const std::vector<std::vector<float>> &poly2)
{
    return clipPolygon(poly1, poly2);
}

static float cross(const std::vector<float> &O, const std::vector<float> &A, const std::vector<float> &B)
{
    return (A[0] - O[0]) * (B[1] - O[1]) - (A[1] - O[1]) * (B[0] - O[0]);
}

std::vector<std::vector<float>> convexHull(std::vector<std::vector<float>> P)
{
    int n = P.size(), k = 0;
    if (n <= 3)
        return P;

    std::sort(P.begin(), P.end(), [](const std::vector<float> &a, const std::vector<float> &b)
              { return a[0] < b[0] || (a[0] == b[0] && a[1] < b[1]); });

    std::vector<std::vector<float>> H(2 * n);

    // Lower hull
    for (int i = 0; i < n; ++i)
    {
        while (k >= 2 && cross(H[k - 2], H[k - 1], P[i]) <= 0)
            k--;
        H[k++] = P[i];
    }

    // Upper hull
    for (int i = n - 2, t = k + 1; i >= 0; --i)
    {
        while (k >= t && cross(H[k - 2], H[k - 1], P[i]) <= 0)
            k--;
        H[k++] = P[i];
    }

    H.resize(k - 1);
    return H;
}

std::vector<std::vector<float>> Union::compute(
    const std::vector<std::vector<float>> &poly1,
    const std::vector<std::vector<float>> &poly2)
{
    std::vector<std::vector<float>> combined = poly1;
    combined.insert(combined.end(), poly2.begin(), poly2.end());
    return convexHull(combined);
}


std::vector<std::vector<float>> Subtraction::compute(
    const std::vector<std::vector<float>> &poly1,
    const std::vector<std::vector<float>> &poly2)
{
    // Assuming both polygons are rectangles with 4 points:
    if (poly1.size() < 4 || poly2.size() < 4)
        return poly1;

    // Get bounding boxes
    float ax1 = std::min(poly1[0][0], poly1[2][0]);
    float ax2 = std::max(poly1[0][0], poly1[2][0]);
    float ay1 = std::min(poly1[0][1], poly1[2][1]);
    float ay2 = std::max(poly1[0][1], poly1[2][1]);

    float bx1 = std::min(poly2[0][0], poly2[2][0]);
    float bx2 = std::max(poly2[0][0], poly2[2][0]);
    float by1 = std::min(poly2[0][1], poly2[2][1]);
    float by2 = std::max(poly2[0][1], poly2[2][1]);

    // Compute intersection
    float ix1 = std::max(ax1, bx1);
    float iy1 = std::max(ay1, by1);
    float ix2 = std::min(ax2, bx2);
    float iy2 = std::min(ay2, by2);

    // If no overlap
    if (ix1 >= ix2 || iy1 >= iy2)
        return poly1;

    // Resulting rectangles (at most 4)
    std::vector<std::vector<std::vector<float>>> parts;

    // Top strip
    if (iy2 < ay2)
        parts.push_back({{ax1, iy2}, {ax2, iy2}, {ax2, ay2}, {ax1, ay2}});
    // Bottom strip
    if (iy1 > ay1)
        parts.push_back({{ax1, ay1}, {ax2, ay1}, {ax2, iy1}, {ax1, iy1}});
    // Left strip
    if (ix1 > ax1)
        parts.push_back({{ax1, iy1}, {ix1, iy1}, {ix1, iy2}, {ax1, iy2}});
    // Right strip
    if (ix2 < ax2)
        parts.push_back({{ix2, iy1}, {ax2, iy1}, {ax2, iy2}, {ix2, iy2}});

    // Combine all resulting strips into one polygon (can draw them separately)
    std::vector<std::vector<float>> result;
    for (const auto &r : parts)
        for (const auto &v : r)
            result.push_back(v);

    return result;
}
