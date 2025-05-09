#include "openglwidget.h"
#include <cmath>
#include <vector>
#include <GL/gl.h>

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
    //new
    //glTranslatef(0.0f, 0.0f, -100.0f);

    //glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    //glRotatef(45.0f, 0.0f, 1.0f, 0.0f);

    glTranslatef(0.0f, 0.0f, -200.0f); // Move the camera back
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f); // Rotate around the X-axis
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f); // Rotate around the Y-axis

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
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //glOrtho(-100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f);
    
    float aspectRatio = static_cast<float>(w) / static_cast<float>(h);
    float nearPlane = 1.0f;
    float farPlane = 1000.0f;
    float fovY = 45.0f; // Field of view in degrees

    // Calculate the top, bottom, left, and right for the frustum
    float top = nearPlane * tan(fovY * M_PI / 360.0f); // Convert FOV to radians
    float bottom = -top;
    float right = top * aspectRatio;
    float left = -right;

    // Set the perspective projection using glFrustum
    glFrustum(left, right, bottom, top, nearPlane, farPlane);
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
    //delete bezier;
    //bezier = new Bezier(points, numInterpolated);

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


// QPointF OpenGLWidget::screenToWorld(const QPoint &mousePos) {
//     float x = mousePos.x() - width() / 2;
//     float y = (height() / 2) - mousePos.y();
//     return QPointF(x, y);
// }

// ------------------------------------------- CUBE ------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    // Convert screen coordinates to world coordinates
    auto world = screenToWorld(event->position().toPoint().x(), event->position().toPoint().y());
    float worldX = world.first;
    float worldY = world.second;
    
    if (shouldDrawBezier)
    {
        mousePressBezier(event);
        return;
    }

    if (!shouldDrawCube) {
        // Add vertices for the polygon
        Point newPoint(worldX, worldY, 0.0);
        tempVertices.push_back(newPoint);
        qDebug() << "Added vertex:" << worldX << worldY;

        update(); // Trigger repaint to show the points
    }

    if (!shouldDrawCube || !cube) return;

    if (cube->basePoints.size() < 4) {
        // Convert screen coordinates to world coordinates
        auto world = screenToWorld(event->position().toPoint().x(), event->position().toPoint().y());
        float worldX = world.first;
        float worldY = world.second;
        
        cube->addBasePoint(Point(worldX, worldY, 0.0));
        qDebug() << "Added base point:" << worldX << worldY;

        if (cube->basePoints.size() == 4) {
            qDebug() << "Base points complete. Ready for extrusion.";
        }

        update();
    } else if (event->button() == Qt::LeftButton) {
        // Start extrusion
        isDragging = true;
        startDragPos = event->position().toPoint(); // Convert QPointF to QPoint
        qDebug() << "Started extrusion at:" << startDragPos;
    }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (shouldDrawBezier)
    {
        mouseMoveBezier(event);
        return;
    }
        
    if (isDragging && shouldDrawCube && cube && cube->basePoints.size() == 4) {
        int dy = event->position().toPoint().y() - startDragPos.y(); // Convert QPointF to QPoint
        double extrusionHeight = static_cast<double>(dy) / height() * 200.0; // Scale extrusion height
        cube->updateExtrusion(extrusionHeight);

        qDebug() << "Extruding cube to height:" << extrusionHeight;
        update();
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (shouldDrawBezier)
    {
        mouseReleaseBezier(event);
        return;
    }
        
    if (isDragging && shouldDrawCube && cube && cube->basePoints.size() == 4) {
        cube->finalizeExtrusion();
        isDragging = false;

        qDebug() << "Extrusion finalized.";
        update();
    }
}

void OpenGLWidget::extrudeCube(double height)
{
    if (shouldDrawCube && cube && cube->basePoints.size() >= 3) {
        cube->updateExtrusion(height); // Update the extrusion height
        cube->finalizeExtrusion();    // Build 3D edges
        qDebug() << "Cube extruded to height:" << height;
        update(); // Trigger repaint
    } else {
        qDebug() << "Cube is not ready for extrusion.";
    }
}

// ----------------------------------------------BEZIER ------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

std::pair<float, float> OpenGLWidget::screenToWorld(int x, int y)
{
   float fx = static_cast<float>(x);
   float fy = static_cast<float>(y);

    int w = width();
    int h = height();

    // Get the current ortho projection (same as resizeGL)
    float worldX = (fx / w) * 200.0f - 100.0f;
    float worldY = ((h - fy) / h) * 200.0f - 100.0f;

    return { worldX, worldY };
}

void OpenGLWidget::mousePressBezier(QMouseEvent* event)
{
    auto world = screenToWorld(event->position().toPoint().x(), event->position().toPoint().y());
    float worldX = world.first;
    float worldY = world.second;

    const float tolerance = 5.0f;
    selectedPointIndex = -1;

    for (size_t i = 0; i < controlPoints.size(); ++i)
    {
        float dx = worldX - controlPoints[i][0];
        float dy = worldY - controlPoints[i][1];
        if (dx * dx + dy * dy < tolerance * tolerance)
        {
            selectedPointIndex = static_cast<int>(i);
            isDragging = true;
            break;
        }
    }
}

void OpenGLWidget::mouseMoveBezier(QMouseEvent* event)
{
    if (isDragging && selectedPointIndex != -1)
    {
        auto world = screenToWorld(event->position().toPoint().x(), event->position().toPoint().y());
        float worldX = world.first;
        float worldY = world.second;

        controlPoints[selectedPointIndex][0] = worldX;
        controlPoints[selectedPointIndex][1] = worldY;

        // Recreate the bezier curve with updated points
        delete bezier;
        bezier = new Bezier(controlPoints, interpolatedPoints);

        update(); // Trigger repaint
        qDebug() << "Dragging" << selectedPointIndex;
    }
}

void OpenGLWidget::mouseReleaseBezier(QMouseEvent* event)
{
    Q_UNUSED(event);
    isDragging = false;
    selectedPointIndex = -1;

    // Recreate the bezier curve after finalizing the drag
    delete bezier;
    bezier = new Bezier(controlPoints, interpolatedPoints);

    update();
}
