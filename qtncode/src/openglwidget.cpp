#include "openglwidget.h"
#include <cmath>
#include <vector>
#include <GL/gl.h>
#include <QMouseEvent>

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

OpenGLWidget::~OpenGLWidget()
{
    delete sphere;
    delete bezier;
    delete cube;
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(0.1f,1.0f,0.0f);

    // if (shouldDrawCube || shouldDrawSphere || shouldDrawCylinder) {
    //     glTranslatef(0.0f, 0.0f, -200.0f);
    //     glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    //     glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
    // }

    if(set3DAngle){
        glTranslatef(0.0f, 0.0f, -200.0f);
        glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
    }

    glPointSize(5.0f); // Set point size
    glColor3f(1.0f, 1.0f, 0.0f); // Set point color to yellow
    glPointSize(5.0f);
    glColor3f(1.0f, 1.0f, 0.0f);

    // Draw temporary vertices
    if (!tempVertices.empty()) {
        glBegin(GL_POINTS);
        for (const auto& vertex : tempVertices) {
            glVertex3f(vertex.getX(), vertex.getY(), vertex.getZ());
        }
        glEnd();

        // Draw lines between the vertices
        glBegin(GL_LINE_LOOP);
        for (const auto& vertex : tempVertices) {
            glVertex3f(vertex.getX(), vertex.getY(), vertex.getZ());
        }
        glEnd();
    }

    // Draw the cube (2D face or 3D shape)
    if (shouldDrawCube && cube) {
        cube->draw();
    }

    if (shouldDrawSphere && sphere)
    {
        sphere->draw();
    }

    if (shouldDrawCylinder && cylinder)
    {
        cylinder->draw();
    }

    if (shouldDrawBezier && bezier)
    {
        //qDebug() << "Drawing Bezier with" << bezier->getInterpolatedPoints().size() << "points";
        bezier->draw();
    }
    if (shouldDrawTwoBeziers) {
    // Draw first Bezier (red)
    Bezier b1(bezier1Points, bezier1Interp);
    glColor3f(1.0f, 0.0f, 0.0f);
    b1.draw();

    // Draw second Bezier (blue)
    Bezier b2(bezier2Points, bezier2Interp);
    glColor3f(0.0f, 0.0f, 1.0f);
    b2.draw();

    // Draw intersection points: white border + cyan inner point
    for (const auto& pt : intersectionPoints) {
        // Draw white border
        glPointSize(14.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);
        glVertex3f(pt[0], pt[1], 0.0f);
        glEnd();

        // Draw cyan inner point
        glPointSize(8.0f);
        glColor3f(0.0f, 1.0f, 1.0f); // Cyan for intersection
        glBegin(GL_POINTS);
        glVertex3f(pt[0], pt[1], 0.0f);
        glEnd();
    }
}
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width()/2, width()/2, -height()/2, height()/2, -500, 500);
    glMatrixMode(GL_MODELVIEW);
}

void OpenGLWidget::addSphere()
{
    delete sphere;
    sphere = new Sphere(radsp, 20, 20);
    shouldDrawSphere = true;
    update();
}

void OpenGLWidget::addCylinder()
{
    delete cylinder;
    cylinder = new Cylinder(radcy,htcy, 20,10);
    shouldDrawCylinder = true;
    update();
}

void OpenGLWidget::addBezier()
{
    if (!controlPoints.empty() && interpolatedPoints > 0) {
        qDebug() << "Rebuilding Bezier...";
        delete bezier; // Delete the old object if it exists
        bezier = new Bezier(controlPoints, interpolatedPoints);
    }
    shouldDrawBezier = true;
    shouldDrawCube = false; // Disable cube functionality
    shouldDrawSphere = false;
    update();
    qDebug() << "addBezier called. Points:" << controlPoints.size()<< " Interpolated:" << interpolatedPoints;
}

void OpenGLWidget::addCube()
{
    if (tempVertices.size() < 3) {
        qDebug() << "At least 3 vertices are required to create a polygon.";
        return;
    }

    delete cube;
    cube = new Cube();

    // Use the collected vertices as the base points
    for (const auto& vertex : tempVertices) {
        cube->addBasePoint(vertex);
    }
    shouldDrawCube = true;
    shouldDrawBezier = false; // Disable Bezier functionality
    shouldDrawSphere = false;
    tempVertices.clear(); // Clear the temporary vertices

    // Build edges for the 2D face
    cube->build2DFace();
    update();
}

void OpenGLWidget::setBezierData(const std::vector<std::vector<double>>& points, int numInterpolated)
{

    controlPoints = points;
    interpolatedPoints = numInterpolated;
    shouldDrawBezier = true;
    update();
}

void OpenGLWidget::setSphereRadius(float r){
    radsp = r;
    shouldDrawSphere = true;
    update();
}

void OpenGLWidget::setCylinderSpecs(float r, float h){
    radcy = r;
    htcy = h;
    shouldDrawCylinder = true;
    update();
}

void OpenGLWidget::setTwoBeziers(const std::vector<std::vector<double>>& points1, int numInterp1,
                                 const std::vector<std::vector<double>>& points2, int numInterp2)
{
    bezier1Points = points1;
    bezier1Interp = numInterp1;
    bezier2Points = points2;
    bezier2Interp = numInterp2;
    shouldDrawTwoBeziers = true;
    shouldDrawBezier = false;
    shouldDrawCube = false;
    shouldDrawSphere = false;
    shouldDrawCylinder = false;

    // Find intersection points
    intersectionPoints.clear();
    std::vector<std::vector<double>> curve1, curve2;
    Bezier b1(points1, numInterp1), b2(points2, numInterp2);
    for (int i = 0; i <= numInterp1; ++i) {
        double t = static_cast<double>(i) / numInterp1;
        curve1.push_back(b1.deCasteljau(t));
    }
    for (int i = 0; i <= numInterp2; ++i) {
        double t = static_cast<double>(i) / numInterp2;
        curve2.push_back(b2.deCasteljau(t));
    }
    double tol = 2.0; // intersection tolerance
    for (const auto& p1 : curve1) {
        for (const auto& p2 : curve2) {
            double dx = p1[0] - p2[0];
            double dy = p1[1] - p2[1];
            if (dx * dx + dy * dy < tol * tol) {
                // Avoid duplicate intersection points
                bool alreadyExists = false;
                for (const auto& ipt : intersectionPoints) {
                    double ddx = ipt[0] - (p1[0]+p2[0])/2;
                    double ddy = ipt[1] - (p1[1]+p2[1])/2;
                    if (ddx*ddx + ddy*ddy < 0.5*0.5) { // tighter tolerance for duplicates
                        alreadyExists = true;
                        break;
                    }
                }
                if (!alreadyExists)
                    intersectionPoints.push_back({(p1[0]+p2[0])/2, (p1[1]+p2[1])/2});
            }
        }
    }
    update();
}


void OpenGLWidget::extrudeCube(double height)
{
    if (shouldDrawCube && cube && cube->basePoints.size() >= 3) {
        cube->updateExtrusion(height); // Update the extrusion height
        cube->finalizeExtrusion();    // Build 3D edges
        qDebug() << "Cube extruded to height:" << height;
        set3DAngle= true;
        update(); // Trigger repaint
    } else {
        qDebug() << "Cube is not ready for extrusion.";
    }
}

// Helper to map mouse position to OpenGL coordinates
QPointF OpenGLWidget::mapToOpenGLCoordinates(const QPoint &mousePos) {
    float x = mousePos.x() - width() / 2;
    float y = height() / 2 - mousePos.y();
    return QPointF(x, y);
}


void OpenGLWidget::mousePressEvent(QMouseEvent *event) {
    QPointF mapped = mapToOpenGLCoordinates(event->pos());
    if (event->button() == Qt::LeftButton) {
        // Add a new base point for the cube
        tempVertices.emplace_back(mapped.x(), mapped.y(), 0.0f);
        update();
    } else if (event->button() == Qt::RightButton) {
        // Select a point for dragging
        draggedPointIndex = -1;
        for (int i = 0; i < static_cast<int>(tempVertices.size()); ++i) {
            float dx = mapped.x() - tempVertices[i].getX();
            float dy = mapped.y() - tempVertices[i].getY();
            if (std::sqrt(dx * dx + dy * dy) < 10.0f) {
                draggedPointIndex = i;
                break;
            }
        }
    }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (draggedPointIndex != -1) {
        QPointF mapped = mapToOpenGLCoordinates(event->pos());
        tempVertices[draggedPointIndex].setX(mapped.x());
        tempVertices[draggedPointIndex].setY(mapped.y());
        update();
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *) {
    draggedPointIndex = -1;
}
