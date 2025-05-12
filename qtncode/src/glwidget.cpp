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
        glBegin(GL_POLYGON);  // or GL_LINE_LOOP if outline only
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
    {
        rectangle.draw();
    }
}


void GLWidget::setCircleData(float cx, float cy, float r)
{
    circle = Circle(cx, cy, r);
    drawCircle = true;

    // generate circle.points here
    const int segments = 100;
    const float angle_increment = 2.0f * 3.14159265f / segments;
    circle.points.clear();
    for (int i = 0; i < segments; ++i) {
        float angle = i * angle_increment;
        float x = cx + r * std::cos(angle);
        float y = cy + r * std::sin(angle);
        circle.points.push_back({x, y});
    }

    update();
}


void GLWidget::setRectangleData(float x1, float y1, float p1, float q1)
{
    rectangle = Rectangle(x1, y1, p1, q1);
    drawRectangle = true;

    float halfLen = p1 / 2.0f;
    float halfWid = q1 / 2.0f;
    float lx1 = x1 - halfLen;
    float lx2 = x1 + halfLen;
    float ly1 = y1 - halfWid;
    float ly2 = y1 + halfWid;

    rectangle.vertices.clear();
    rectangle.vertices.push_back({lx1, ly1});
    rectangle.vertices.push_back({lx2, ly1});
    rectangle.vertices.push_back({lx2, ly2});
    rectangle.vertices.push_back({lx1, ly2});

    update();
}

void GLWidget::computeUnion()
{
    resultShape.clear();
    // drawCircle = false;
    // drawRectangle = false;
    Union u;
    resultShape = u.compute(circle.points, rectangle.vertices);
    showResult = true;
    update();
}

void GLWidget::computeIntersection()
{
    resultShape.clear();
    // drawCircle = false;
    // drawRectangle = false;
    Intersection i;
    resultShape = i.compute(circle.points, rectangle.vertices);
    showResult = true;
    update();
}

void GLWidget::computeSubtraction()
{
    resultShape.clear();
    // drawCircle = false;
    // drawRectangle = false;
    Subtraction s;
    resultShape = s.compute(circle.points, rectangle.vertices);
    showResult = true;
    update();
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

        auto inside = [&](const std::vector<float> &p)
        {
            return (B[0] - A[0]) * (p[1] - A[1]) > (B[1] - A[1]) * (p[0] - A[0]);
        };

        auto intersection = [&](const std::vector<float> &p1, const std::vector<float> &p2)
        {
            float A1 = p2[1] - p1[1];
            float B1 = p1[0] - p2[0];
            float C1 = A1 * p1[0] + B1 * p1[1];

            float A2 = B[1] - A[1];
            float B2 = A[0] - B[0];
            float C2 = A2 * A[0] + B2 * A[1];

            float det = A1 * B2 - A2 * B1;
            if (fabs(det) < 1e-6)
                return p2;
            float x = (B2 * C1 - B1 * C2) / det;
            float y = (A1 * C2 - A2 * C1) / det;
            return std::vector<float>{x, y};
        };

        for (size_t j = 0; j < input.size(); ++j)
        {
            const std::vector<float> &current = input[j];
            const std::vector<float> &prev = input[(j + input.size() - 1) % input.size()];
            bool currIn = inside(current);
            bool prevIn = inside(prev);

            if (currIn && prevIn)
                output.push_back(current);
            else if (prevIn && !currIn)
                output.push_back(intersection(prev, current));
            else if (!prevIn && currIn)
            {
                output.push_back(intersection(prev, current));
                output.push_back(current);
            }
        }
    }

    return output;
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
    // Subtract poly2 from poly1 using reverse Sutherland–Hodgman
    std::vector<std::vector<float>> inside = clipPolygon(poly1, poly2);
    std::vector<std::vector<float>> outside;
    for (const auto &pt : poly1)
    {
        if (std::find(inside.begin(), inside.end(), pt) == inside.end())
            outside.push_back(pt);
    }

    return outside.empty() ? inside : outside;
}
