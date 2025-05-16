#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <vector>
#include "sphere.h"
#include "bezier.h"
#include "cube.h"
#include "cylinder.h"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void addSphere();
    void addBezier();
    void addCube();
    void addCylinder();

    void setBezierData(const std::vector<std::vector<double>> &points, int numInterpolated);
    void setTwoBeziers(const std::vector<std::vector<double>> &points1, int numInterp1,
                       const std::vector<std::vector<double>> &points2, int numInterp2);
    void setSphereRadius(float r);
    void setCylinderSpecs(float r, float h);
    void extrudeCube(double height);

private:
    Sphere *sphere = nullptr;
    Bezier *bezier = nullptr;
    Cube *cube = nullptr;
    Cylinder *cylinder = nullptr;

    bool shouldDrawSphere = false;
    bool shouldDrawBezier = false;
    bool shouldDrawCube = false;
    bool shouldDrawCylinder = false;

    // new
    std::vector<std::vector<double>> controlPoints;
    int interpolatedPoints = 0;
    int selectedPointIndex = -1;
    float radsp;
    float radcy;
    float htcy;

    bool isDragging = false;
    QPoint startDragPos;

    std::vector<Point> tempVertices; // Temporary vertices for the polygon
   
    std::vector<std::vector<double>> bezier1Points;
    int bezier1Interp = 0;
    std::vector<std::vector<double>> bezier2Points;
    int bezier2Interp = 0;
    std::vector<std::vector<double>> intersectionPoints;
    bool shouldDrawTwoBeziers = false;

    bool set3DAngle = false;

    int draggedPointIndex = -1;
    QPointF mapToOpenGLCoordinates(const QPoint &mousePos);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

};
