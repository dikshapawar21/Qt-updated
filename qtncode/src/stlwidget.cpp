#include "STLWidget.h"
#include "intersection.h"
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QColor>
#include <QDebug>
 
STLWidget::STLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
}
 
STLWidget::~STLWidget()
{
}
 
void STLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}
 
void STLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    float aspect = float(w) / float(h ? h : 1);
    projection.setToIdentity();
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);
    // No gluPerspective, no glMatrixMode/glLoadIdentity for projection
}
 
void STLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view.setToIdentity();
    view.translate(0.0f, 0.0f, -3.0f * zoom);
    model.setToIdentity();
    model.rotate(rotationX, 1.0f, 0.0f, 0.0f);
    model.rotate(rotationY, 0.0f, 1.0f, 0.0f);
    QMatrix4x4 mvp = projection * view * model;
    glLoadMatrixf(mvp.constData());
 
    glColor3f(0.2f, 0.8f, 0.0f);
    for (const auto& tri : trianglesA) {
        glBegin(GL_LINE_LOOP);
        glVertex3f(tri.p1.x, tri.p1.y, tri.p1.z);
        glVertex3f(tri.p2.x, tri.p2.y, tri.p2.z);
        glVertex3f(tri.p3.x, tri.p3.y, tri.p3.z);
        glEnd();
    }
 

    glColor3f(0.0f, 0.2f, 0.9f);
    for (const auto& tri : trianglesB) {
        glBegin(GL_LINE_LOOP);
        glVertex3f(tri.p1.x, tri.p1.y, tri.p1.z);
        glVertex3f(tri.p2.x, tri.p2.y, tri.p2.z);
        glVertex3f(tri.p3.x, tri.p3.y, tri.p3.z);
        glEnd();
    }

 
    // For each pair of triangles, handle coplanar and non-coplanar cases
    std::vector<std::pair<POINT, POINT>> intersectionSegments;
    for (const auto& triA : trianglesA) {
        for (const auto& triB : trianglesB) {
            if (trianglesCoplanar(triA, triB)) {
                // Coplanar: collect intersection points along edges
                auto pointInTriangle = [](const POINT& p, const Triangle& t) {
                    float x = p.x, y = p.y;
                    float x1 = t.p1.x, y1 = t.p1.y;
                    float x2 = t.p2.x, y2 = t.p2.y;
                    float x3 = t.p3.x, y3 = t.p3.y;
                    float denom = (y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3);
                    float a = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / denom;
                    float b = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / denom;
                    float c = 1.0f - a - b;
                    return a >= 0 && b >= 0 && c >= 0 && a <= 1 && b <= 1 && c <= 1;
                };
                float step = 0.01f;
                // For each edge of triA, check if inside triB
                const POINT* ptsA[3] = { &triA.p1, &triA.p2, &triA.p3 };
                for (int i = 0; i < 3; ++i) {
                    const POINT& p0 = *ptsA[i];
                    const POINT& p1 = *ptsA[(i+1)%3];
                    std::vector<POINT> segment;
                    for (float t = 0; t <= 1.0f; t += step) {
                        float x = p0.x + t * (p1.x - p0.x);
                        float y = p0.y + t * (p1.y - p0.y);
                        POINT pt(x, y, 0);
                        if (pointInTriangle(pt, triB)) {
                            segment.push_back(pt);
                        } else if (!segment.empty()) {
                            // End of a segment inside
                            if (segment.size() > 1)
                                intersectionSegments.emplace_back(segment.front(), segment.back());
                            segment.clear();
                        }
                    }
                    if (segment.size() > 1)
                        intersectionSegments.emplace_back(segment.front(), segment.back());
                }
                // For each edge of triB, check if inside triA
                const POINT* ptsB[3] = { &triB.p1, &triB.p2, &triB.p3 };
                for (int i = 0; i < 3; ++i) {
                    const POINT& p0 = *ptsB[i];
                    const POINT& p1 = *ptsB[(i+1)%3];
                    std::vector<POINT> segment;
                    for (float t = 0; t <= 1.0f; t += step) {
                        float x = p0.x + t * (p1.x - p0.x);
                        float y = p0.y + t * (p1.y - p0.y);
                        POINT pt(x, y, 0);
                        if (pointInTriangle(pt, triA)) {
                            segment.push_back(pt);
                        } else if (!segment.empty()) {
                            if (segment.size() > 1)
                                intersectionSegments.emplace_back(segment.front(), segment.back());
                            segment.clear();
                        }
                    }
                    if (segment.size() > 1)
                        intersectionSegments.emplace_back(segment.front(), segment.back());
                }
            } else {
                // Non-coplanar: collect intersection segment endpoints as a line
                POINT segA, segB;
                if (triangleTriangleIntersectionSegment(triA, triB, segA, segB)) {
                    intersectionSegments.emplace_back(segA, segB);
                }
            }
        }
    }
    // Draw all intersection segments as lines in white
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(3.0f);
    for (const auto& seg : intersectionSegments) {
        glBegin(GL_LINES);
        glVertex3f(seg.first.x, seg.first.y, seg.first.z);
        glVertex3f(seg.second.x, seg.second.y, seg.second.z);
        glEnd();
    }
    glLineWidth(1.0f);
}
 
void STLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
}
 
void STLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastMousePos.x();
    int dy = event->y() - lastMousePos.y();
    if (event->buttons() & Qt::LeftButton) {
        rotationX += dy;
        rotationY += dx;
        update();
    }
    lastMousePos = event->pos();
}
 
void STLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
        zoom *= 0.9f;
    else
        zoom *= 1.1f;
    update();
}