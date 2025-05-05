// app.h
#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QPushButton>
#include <QInputDialog>
#include <QComboBox>
#include <QOpenGLWidget>
#include <QMessageBox>
#include <QVBoxLayout>
// #include "ui_mainwindow.h"
#include <QComboBox>
#include "openglwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onAddShapeButtonClicked();
    void onAddBezierInput();
    void onExtrudeButtonClicked(); // New slot for extrusion

private:
    OpenGLWidget* glWidget;
    QPushButton* pushButton;
    QPushButton* extrudeButton; // New button for extrusion
    QComboBox* comboBox;
    QString selectedShape = "";
};
