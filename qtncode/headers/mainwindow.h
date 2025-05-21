#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QInputDialog>
#include <QComboBox>
#include <QOpenGLWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include "openglwidget.h"
#include "revolvebezier.h"
#include "glwidget.h"
#include "stlwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onAddShapeButtonClicked();
    void onAddBezierInput();
    void onAddSphereRadius();
    void onExtrudeButtonClicked();
    void onAddCylinderSpecs();
    void onAddBezierIntersection();

    void onImportSTL();
    void onFindIntersection();

private:
    OpenGLWidget* glWidget;
    BezierWidget* bezierWidget;
    QPushButton* pushButton;
    QPushButton* extrudeButton;
    QComboBox* comboBox;
    QString selectedShape = "";

    QPushButton *importButton;
    QPushButton *intersectionButton;
    STLWidget* stlwidget;
};
