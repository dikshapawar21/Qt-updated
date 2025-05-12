#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "circle.h"
#include "rectangle.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget* parent = nullptr);
    ~GLWidget();

    void setCircleData(float cx, float cy, float r);
    void setRectangleData(float p, float q, float l, float w);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    Circle circle;
    bool drawCircle = false;

    Rectangle rectangle;
    bool drawRectangle = false;
};