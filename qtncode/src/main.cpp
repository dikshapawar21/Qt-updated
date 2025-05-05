#include "mainwindow.h"
// #include "./ui_mainwindow.h"
#include <QApplication>
#include <QDebug>

#include <bits/stdc++.h>
using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;

    // BezierWidget widget;
    // widget.setWindowTitle("Interactive Bezier Curve");
    // widget.resize(800, 600);
    // widget.show();
 
    //mainWindow.resize(800,600);
    mainWindow.show();  
    return app.exec();
}
