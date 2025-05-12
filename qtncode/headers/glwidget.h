#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "circle.h"
#include "rectangle.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    void setCircleData(float cx, float cy, float r);
    void setRectangleData(float p, float q, float l, float w);

    void computeUnion();
    void computeIntersection();
    void computeSubtraction();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    Circle circle;
    bool drawCircle = false;

    Rectangle rectangle;
    bool drawRectangle = false;

    bool showResult = false;

    std::vector<std::vector<float>> resultShape;
};

class Union
{
public:
    std::vector<std::vector<float>> compute(const std::vector<std::vector<float>> &poly1,
                                            const std::vector<std::vector<float>> &poly2);
};

class Intersection
{
public:
    std::vector<std::vector<float>> compute(const std::vector<std::vector<float>> &poly1,
                                            const std::vector<std::vector<float>> &poly2);
};

class Subtraction
{
public:
    std::vector<std::vector<float>> compute(const std::vector<std::vector<float>> &poly1,
                                            const std::vector<std::vector<float>> &poly2);
};
