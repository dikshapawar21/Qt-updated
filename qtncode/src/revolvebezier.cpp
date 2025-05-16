#include "revolvebezier.h"
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QPushButton>
#include <QMatrix4x4>
#include <QtMath>
#include <QDebug>
 
BezierWidget::BezierWidget(QWidget *parent) : QOpenGLWidget(parent) {
}
 
void BezierWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glPointSize(4.0f);
    glLineWidth(1.5f);
}
 
void BezierWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width()/2, width()/2, -height()/2, height()/2, -500, 500);
    glMatrixMode(GL_MODELVIEW);
}
 
void BezierWidget::paintGL() {
    //Clears the screen, resets modelview, and draws the axes.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
 
    drawRevolutionAxis();
 
 
    //Draws the polyline between control points.
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_LINE_STRIP);
    for (const auto &pt : controlPoints)
        glVertex3f(pt.x(), pt.y(), 0.5f);
    glEnd();
 
    //Draws the Bezier curve.
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto &pt : bezierCurve)
        glVertex3f(pt.x(), pt.y(), 0.0f);
    glEnd();
 
    //Draws red points at control points.
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_POINTS);
    for (const auto &pt : controlPoints)
        glVertex3f(pt.x(), pt.y(), 0.0f);
    glEnd();
 
 
    //Draws the 3D solid of revolution using quads (rectangular faces between rotated curve segments).
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(0.6f, 0.6f, 0.6f);
    for (int i = 0; i < revolutionMesh.size() - 1; ++i) {
        for (int j = 0; j < revolutionMesh[i].size() - 1; ++j) {
            glBegin(GL_QUADS);
            glVertex3f(revolutionMesh[i][j].x(), revolutionMesh[i][j].y(), revolutionMesh[i][j].z());
            glVertex3f(revolutionMesh[i + 1][j].x(), revolutionMesh[i + 1][j].y(), revolutionMesh[i + 1][j].z());
            glVertex3f(revolutionMesh[i + 1][j + 1].x(), revolutionMesh[i + 1][j + 1].y(), revolutionMesh[i + 1][j + 1].z());
            glVertex3f(revolutionMesh[i][j + 1].x(), revolutionMesh[i][j + 1].y(), revolutionMesh[i][j + 1].z());
            glEnd();
        }
    }
}
 
 
QPointF BezierWidget::mapToOpenGLCoordinates(const QPoint &mousePos) {
    float x = mousePos.x() - width() / 2;
    float y = (height() / 2) - mousePos.y();
    return QPointF(x, y);
}
     
void BezierWidget::mousePressEvent(QMouseEvent *event) {
    QPointF mapped = mapToOpenGLCoordinates(event->pos());
    if (event->button() == Qt::LeftButton) {
        controlPoints.append(mapped);
        computeBezierCurve();
        update();
    } else if (event->button() == Qt::RightButton) {
        for (int i = 0; i < controlPoints.size(); ++i) {
            if (QLineF(mapped, controlPoints[i]).length() < 10.0f) {
                draggedPointIndex = i;
                break;
            }
        }
    }
}
 
//If a point is selected, this updates its position when the mouse is moved.
void BezierWidget::mouseMoveEvent(QMouseEvent *event) {
    if (draggedPointIndex != -1) {
        controlPoints[draggedPointIndex] = mapToOpenGLCoordinates(event->pos());
        computeBezierCurve();
        computeRevolution();
        update();
    }
}
 
void BezierWidget::mouseReleaseEvent(QMouseEvent *) {
    draggedPointIndex = -1;
}
 
void BezierWidget::handleRightClick() {
    computeRevolution();
    update();
}
 
// Generates a smooth curve by calling De Casteljau’s algorithm with different values of t.
void BezierWidget::computeBezierCurve() {
    bezierCurve.clear();
    if (controlPoints.size() < 2)
        return;
    for (float t = 0.0f; t <= 1.0f; t += 0.01f)
        bezierCurve.append(deCasteljau(t));
}

QPointF BezierWidget::deCasteljau(float t) {
    QVector<QPointF> pts = controlPoints;
    while (pts.size() > 1) {
        QVector<QPointF> next;
        for (int i = 0; i < pts.size() - 1; ++i) {
            float x = (1 - t) * pts[i].x() + t * pts[i + 1].x();
            float y = (1 - t) * pts[i].y() + t * pts[i + 1].y();
            next.append(QPointF(x, y));
        }
        pts = next;
    }
    return pts[0];
}
 

void BezierWidget::computeRevolution() {
    revolutionMesh.clear();
    if (bezierCurve.isEmpty()) return;
 
    const int steps = 72;
    const float angleStep = 360.0f / steps;
 
    for (int i = 0; i <= steps; ++i) {
        float angle = i * angleStep;
        QVector<QVector3D> ring;
        for (const auto &pt : bezierCurve) {
            QVector3D point3D(pt.x(), pt.y(), 0.0f);
            ring.append(rotatePointAroundAxis(point3D, angle, QVector3D(0, 1, 0)));
        }
        revolutionMesh.append(ring);
    }
}
 
//Rotates a point around the Y-axis by a given angle.
//Uses a QMatrix4x4 to perform the rotation.
QVector3D BezierWidget::rotatePointAroundAxis(const QVector3D &point, float angle, const QVector3D &axis) {
    QMatrix4x4 rot;
    rot.rotate(angle, axis);
 
    return rot.map(point);
}
 
//Draws the X and Y axes for reference.
//The Z-axis is not drawn as it is not visible in the current view.
void BezierWidget::drawRevolutionAxis() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(-500.0f, 0.0f, 0.5f);
    glVertex3f(500.0f, 0.0f, 0.5f);
    glVertex3f(0.0f, -500.0f, 0.5f);
    glVertex3f(0.0f, 500.0f, 0.5f);
    glEnd();
}