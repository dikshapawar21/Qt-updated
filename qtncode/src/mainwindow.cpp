
#include "mainwindow.h"
#include "openglwidget.h"
#include "sphere.h"
#include "bezier.h"
#include "cylinder.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(700, 700);
    setWindowTitle("OpenGL & QT");

    auto* mainWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(mainWidget);

    glWidget = new OpenGLWidget(this);
    layout->addWidget(glWidget);

    pushButton = new QPushButton("Add Shape", this);
    layout->addWidget(pushButton);

    extrudeButton = new QPushButton("Extrude", this); // New button for extrusion
    layout->addWidget(extrudeButton);

    comboBox = new QComboBox(this);
    comboBox->addItems({"bezier", "sphere", "cube", "cylinder"});
    layout->addWidget(comboBox);

    connect(pushButton, &QPushButton::clicked, this, &MainWindow::onAddShapeButtonClicked);
    connect(extrudeButton, &QPushButton::clicked, this, &MainWindow::onExtrudeButtonClicked); // Connect extrusion button
    selectedShape = comboBox->currentText();
    
    connect(comboBox, &QComboBox::currentTextChanged, this, [this](const QString& text) {
        selectedShape = text;
    });

    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);
}

MainWindow::~MainWindow()
{}

void MainWindow::onExtrudeButtonClicked()
{
    if (selectedShape == "cube" && glWidget)
    {
        bool ok;
        double height = QInputDialog::getDouble(this, "Extrusion Height", "Enter height to extrude:", 50.0, 1.0, 1000.0, 2, &ok);
        if (ok)
        {
            glWidget->extrudeCube(height); // Call the new method in OpenGLWidget
        }
    }
    else
    {
        QMessageBox::information(this, "Info", "Extrusion is only available for cubes.");
    }
}

void MainWindow::onAddShapeButtonClicked()
{
    if (!glWidget)
    {
        QMessageBox::warning(this, "Error", "OpenGL widget not found!");
        return;
    }

    if (selectedShape == "sphere")
    {
        glWidget->addSphere();
    }
    else if (selectedShape == "bezier")
    {
        onAddBezierInput();
        glWidget->addBezier();
    }
    else if (selectedShape == "cube")
    {
        qDebug() << "Drawing cube...";
        glWidget->addCube();
    }
    
    else if (selectedShape == "cylinder")
    {
        glWidget->addCylinder();
    }

    else
    {
        QMessageBox::information(this, "Info", "Please select a valid shape.");
    }
}

void MainWindow::onAddBezierInput()
{
    bool ok;
    int numControl = QInputDialog::getInt(this, "Control Points", "Enter number of control points:", 3, 2, 10, 1, &ok);
    if (!ok) return;
    
    std::vector<std::vector<double>> points;
    
    for (int i = 0; i < numControl; ++i)
    {
        double x = QInputDialog::getDouble(this, "Control Point", QString("Enter X for point %1:").arg(i + 1), 0, -1000, 1000, 2, &ok);
        if (!ok) return;
    
        double y = QInputDialog::getDouble(this, "Control Point", QString("Enter Y for point %1:").arg(i + 1), 0, -1000, 1000, 2, &ok);
        if (!ok) return;
    
        points.push_back({ x, y });
    }
    
    int numInterpolated = QInputDialog::getInt(this, "Interpolation", "Enter number of interpolated points:", 50, 1, 1000, 1, &ok);
    if (!ok) return;
    
    if (glWidget)
    {
        glWidget->setBezierData(points, numInterpolated);
    }
}
