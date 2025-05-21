#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>

class STLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit STLWidget(QWidget *parent = nullptr);
    ~STLWidget();
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    float rotationX = 0.0f;
    float rotationY = 0.0f;
    float zoom = 1.0f;
    QPoint lastMousePos;
    QMatrix4x4 projection;
    QMatrix4x4 view;
    QMatrix4x4 model;
};

#endif