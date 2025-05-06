#include "mainwindow.h"
#include "openglwidget.h"
#include "sphere.h"
#include "bezier.h"
#include "cylinder.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    qDebug() << "MainWindow constructor called.";
    resize(1024, 768);
    setWindowTitle("OpenGL & QT");

    auto* mainWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(mainWidget);

    glWidget = new OpenGLWidget(this);
    layout->addWidget(glWidget);

    comboBox = new QComboBox(this);
    comboBox->setPlaceholderText("Select Shape");

    comboBox->addItems({"bezier", "sphere", "cube", "cylinder", "revolution"});
    layout->addWidget(comboBox);
    
    pushButton = new QPushButton("Add Shape", this);
    layout->addWidget(pushButton);

    extrudeButton = new QPushButton("Extrude", this);
    layout->addWidget(extrudeButton);


    connect(pushButton, &QPushButton::clicked, this, &MainWindow::onAddShapeButtonClicked);
    connect(extrudeButton, &QPushButton::clicked, this, &MainWindow::onExtrudeButtonClicked);
    selectedShape = comboBox->currentText();

    connect(comboBox, &QComboBox::currentTextChanged, this, [this](const QString& text) {
        selectedShape = text;
    });

    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);
    qDebug() << "MainWindow setup complete.";
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
    else if (selectedShape == "revolution")
    {
        qDebug() << "Opening BezierWidget for revolution...";
        BezierWidget* bezierWidget = new BezierWidget(); // No parent to make it a top-level window
        bezierWidget->setAttribute(Qt::WA_DeleteOnClose); // Automatically delete when closed
        bezierWidget->resize(800, 600); // Optional: Set a default size
        bezierWidget->show();
        update();
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
