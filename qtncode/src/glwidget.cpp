// glwidget.cpp
#include "glwidget.h"
#include <QDebug>

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent) {}

GLWidget::~GLWidget() {}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.2, 0.2, 0.2, 1.0);
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-100, 100, -100, 100, -1, 1); // simple 2D orthographic view
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (drawCircle) {
        circle.draw(); 
    }

    if (drawRectangle) {
        rectangle.draw(); 
    }
}

void GLWidget::setCircleData(float cx, float cy, float r) {
    circle = Circle(cx, cy, r);  
    drawCircle = true;
    update();
}

void GLWidget::setRectangleData(float x1, float y1, float p1, float q1) {
    rectangle = Rectangle(x1, y1, p1, q1);  
    drawRectangle = true;
    update();
}
