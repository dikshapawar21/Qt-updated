#include "mainwindow.h"
#include "openglwidget.h"
#include "sphere.h"
#include "bezier.h"
#include "cylinder.h"
#include "intersection.h"
#include "stlwidget.h"
#include "stlparser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << "MainWindow constructor called.";
    resize(1024, 768);
    setWindowTitle("OpenGL & QT");

    auto *mainWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(mainWidget);

    glWidget = new OpenGLWidget(this);
    layout->addWidget(glWidget);

    comboBox = new QComboBox(this);
    comboBox->setPlaceholderText("Select Shape");

    comboBox->addItems({"Bezier", "Sphere", "Cube", "Cylinder", "Revolution", "Circle", "Rectangle", "Boolean operations", "Bezier Intersection", "STL-STL Intersection"});
    layout->addWidget(comboBox);

    pushButton = new QPushButton("Add Shape", this);
    layout->addWidget(pushButton);

    extrudeButton = new QPushButton("Extrude", this);
    layout->addWidget(extrudeButton);

    connect(pushButton, &QPushButton::clicked, this, &MainWindow::onAddShapeButtonClicked);
    connect(extrudeButton, &QPushButton::clicked, this, &MainWindow::onExtrudeButtonClicked);
    selectedShape = comboBox->currentText();

    connect(comboBox, &QComboBox::currentTextChanged, this, [this](const QString &text)
            { selectedShape = text; });

    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);
    qDebug() << "MainWindow setup complete.";
}

MainWindow::~MainWindow()
{
}

void MainWindow::onExtrudeButtonClicked()
{
    if (selectedShape == "Cube" && glWidget)
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

    if (selectedShape == "Sphere")
    {
        onAddSphereRadius();
        glWidget->addSphere();
    }
    else if (selectedShape == "Bezier")
    {
        onAddBezierInput();
        glWidget->addBezier();
    }
    else if (selectedShape == "Cube")
    {
        qDebug() << "Drawing cube...";
        glWidget->addCube();
    }
    else if (selectedShape == "Cylinder")
    {
        onAddCylinderSpecs();
        glWidget->addCylinder();
    }
    else if (selectedShape == "Bezier Intersection")
    {
        onAddBezierIntersection();
    }
    else if (selectedShape == "Revolution")
    {
        qDebug() << "Opening BezierWidget for revolution...";
        BezierWidget *bezierWidget = new BezierWidget();  // No parent to make it a top-level window
        bezierWidget->setAttribute(Qt::WA_DeleteOnClose); // Automatically delete when closed
        bezierWidget->resize(800, 600);                   // Optional: Set a default size
        bezierWidget->show();
        update();
    }
    else if (selectedShape == "STL-STL Intersection")
    {

        QWidget *centralWidget = new QWidget(this);
        QHBoxLayout *layout = new QHBoxLayout(centralWidget);

        importButton = new QPushButton("Import STL File", this);
        intersectionButton = new QPushButton("Intersect Shapes", this);
        stlwidget = new STLWidget(this);

        // Create a vertical layout for the buttons
        QVBoxLayout *buttonLayout = new QVBoxLayout();
        buttonLayout->addWidget(importButton);
        buttonLayout->addWidget(intersectionButton);

        layout->addWidget(stlwidget, 1);
        layout->addLayout(buttonLayout); // Add the vertical layout to the horizontal layout

        setCentralWidget(centralWidget);

        connect(importButton, &QPushButton::clicked, this, &MainWindow::onImportSTL);
        connect(intersectionButton, &QPushButton::clicked, this, &MainWindow::onFindIntersection);

        stlwidget->setAttribute(Qt::WA_DeleteOnClose);
        stlwidget->resize(800, 600);
        stlwidget->show();
        update();
    }
    else if (selectedShape == "Circle")
    {
        bool ok;
        float cx = QInputDialog::getDouble(this, "Circle Center X", "Enter X coordinate of center:", 0.0, -1000.0, 1000.0, 2, &ok);
        if (!ok)
            return;

        float cy = QInputDialog::getDouble(this, "Circle Center Y", "Enter Y coordinate of center:", 0.0, -1000.0, 1000.0, 2, &ok);
        if (!ok)
            return;

        float radius = QInputDialog::getDouble(this, "Circle Radius", "Enter radius of the circle:", 30.0, 0.1, 1000.0, 2, &ok);
        if (!ok)
            return;

        GLWidget *glwidget = new GLWidget(); // No parent: standalone window
        glwidget->setAttribute(Qt::WA_DeleteOnClose);
        glwidget->resize(800, 600);
        glwidget->setCircleData(cx, cy, radius);
        glwidget->show();
    }

    else if (selectedShape == "Rectangle")
    {
        bool ok;
        float x = QInputDialog::getDouble(this, "Rectangle Center X", "Enter X coordinate:", 0.0, -1000.0, 1000.0, 2, &ok);
        if (!ok)
            return;
        float y = QInputDialog::getDouble(this, "Rectangle Center Y", "Enter Y coordinate:", 0.0, -1000.0, 1000.0, 2, &ok);
        if (!ok)
            return;

        float length = QInputDialog::getDouble(this, "Rectangle Length", "Enter length:", 40.0, 0.1, 1000.0, 2, &ok);
        if (!ok)
            return;

        float width = QInputDialog::getDouble(this, "Rectangle Width", "Enter width:", 30.0, 0.1, 1000.0, 2, &ok);
        if (!ok)
            return;

        GLWidget *glwidget = new GLWidget();
        glwidget->setAttribute(Qt::WA_DeleteOnClose);
        glwidget->resize(800, 600);
        glwidget->setRectangleData(x, y, length, width);
        glwidget->show();
    }

    else if (selectedShape == "Boolean operations")
    {
        bool ok;
        float cx = QInputDialog::getDouble(this, "Rectangle1 Center X", "Enter X coordinate of center:", 0.0, -1000.0, 1000.0, 2, &ok);
        if (!ok)
            return;

        float cy = QInputDialog::getDouble(this, "Rectangle Center Y", "Enter Y coordinate of center:", 0.0, -1000.0, 1000.0, 2, &ok);
        if (!ok)
            return;

        // float radius = QInputDialog::getDouble(this, "Circle Radius", "Enter radius of the circle:", 30.0, 0.1, 1000.0, 2, &ok);
        // if (!ok)
        //     return;

        float x = QInputDialog::getDouble(this, "Rectangle Center X", "Enter X coordinate:", 0.0, -1000.0, 1000.0, 2, &ok);
        if (!ok)
            return;
        float y = QInputDialog::getDouble(this, "Rectangle Center Y", "Enter Y coordinate:", 0.0, -1000.0, 1000.0, 2, &ok);
        if (!ok)
            return;

        float length = QInputDialog::getDouble(this, "Rectangle Length", "Enter length:", 40.0, 0.1, 1000.0, 2, &ok);
        if (!ok)
            return;

        float width = QInputDialog::getDouble(this, "Rectangle Width", "Enter width:", 30.0, 0.1, 1000.0, 2, &ok);
        if (!ok)
            return;

        QWidget *container = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(container);

        GLWidget *glwidget = new GLWidget();
        QPushButton *btn = new QPushButton("Boolean Operation");

        layout->addWidget(glwidget);
        layout->addWidget(btn);

        glwidget->setAttribute(Qt::WA_DeleteOnClose);
        glwidget->resize(800, 600);

        // glwidget->setCircleData(cx, cy, radius);
        glwidget->setRectangleData(cx, cy, length, width);
        glwidget->setRectangleData(x, y, length, width);

        container->setLayout(layout);
        container->resize(800, 650);
        container->show();

        // Connect button to boolean operation dialog
        connect(btn, &QPushButton::clicked, this, [=]()
                {
                    QStringList options = {"Union", "Intersection", "Subtraction"};
                    bool okOp;
                    QString selectedOp = QInputDialog::getItem(container, "Boolean Operation", "Select operation:", options, 0, false, &okOp);

                    if (okOp)
                    {
                        if (selectedOp == "Union")
                            glwidget->computeUnion();
                        else if (selectedOp == "Intersection")
                            glwidget->computeIntersection();
                        else if (selectedOp == "Subtraction")
                            glwidget->computeSubtraction();

                        glwidget->update(); // trigger redraw if needed
                    } });
    }

    else
    {
        QMessageBox::information(this, "Info", "Please select a valid shape.");
    }
}
void MainWindow::onAddBezierIntersection()
{
    // Collect control points for first Bezier
    bool ok;
    int numControl1 = QInputDialog::getInt(this, "Bezier 1", "Enter number of control points:", 3, 2, 10, 1, &ok);
    if (!ok)
        return;
    std::vector<std::vector<double>> points1;
    for (int i = 0; i < numControl1; ++i)
    {
        double x = QInputDialog::getDouble(this, "Bezier 1", QString("Enter X for point %1:").arg(i + 1), 0, -1000, 1000, 2, &ok);
        if (!ok)
            return;
        double y = QInputDialog::getDouble(this, "Bezier 1", QString("Enter Y for point %1:").arg(i + 1), 0, -1000, 1000, 2, &ok);
        if (!ok)
            return;
        points1.push_back({x, y});
    }
    int numInterpolated1 = QInputDialog::getInt(this, "Bezier 1", "Enter number of interpolated points:", 50, 1, 1000, 1, &ok);
    if (!ok)
        return;

    // Collect control points for second Bezier
    int numControl2 = QInputDialog::getInt(this, "Bezier 2", "Enter number of control points:", 3, 2, 10, 1, &ok);
    if (!ok)
        return;
    std::vector<std::vector<double>> points2;
    for (int i = 0; i < numControl2; ++i)
    {
        double x = QInputDialog::getDouble(this, "Bezier 2", QString("Enter X for point %1:").arg(i + 1), 0, -1000, 1000, 2, &ok);
        if (!ok)
            return;
        double y = QInputDialog::getDouble(this, "Bezier 2", QString("Enter Y for point %1:").arg(i + 1), 0, -1000, 1000, 2, &ok);
        if (!ok)
            return;
        points2.push_back({x, y});
    }
    int numInterpolated2 = QInputDialog::getInt(this, "Bezier 2", "Enter number of interpolated points:", 50, 1, 1000, 1, &ok);
    if (!ok)
        return;

    if (glWidget)
    {
        glWidget->setTwoBeziers(points1, numInterpolated1, points2, numInterpolated2);
    }
}

void MainWindow::onAddBezierInput()
{
    bool ok;
    int numControl = QInputDialog::getInt(this, "Control Points", "Enter number of control points:", 3, 2, 10, 1, &ok);
    if (!ok)
        return;

    std::vector<std::vector<double>> points;

    for (int i = 0; i < numControl; ++i)
    {
        double x = QInputDialog::getDouble(this, "Control Point", QString("Enter X for point %1:").arg(i + 1), 0, -1000, 1000, 2, &ok);
        if (!ok)
            return;

        double y = QInputDialog::getDouble(this, "Control Point", QString("Enter Y for point %1:").arg(i + 1), 0, -1000, 1000, 2, &ok);
        if (!ok)
            return;

        points.push_back({x, y});
    }

    int numInterpolated = QInputDialog::getInt(this, "Interpolation", "Enter number of interpolated points:", 50, 1, 1000, 1, &ok);
    if (!ok)
        return;

    if (glWidget)
    {
        glWidget->setBezierData(points, numInterpolated);
    }
}

void MainWindow::onAddSphereRadius()
{
    bool ok;
    float radius = QInputDialog::getDouble(
        this,
        "Sphere",
        "Enter radius of sphere",
        40.0,
        0.1,
        1000.0,
        2,
        &ok);
    if (!ok)
        return;

    if (glWidget)
    {
        glWidget->setSphereRadius(radius);
    }
}

void MainWindow::onAddCylinderSpecs()
{
    bool ok;
    float radius = QInputDialog::getDouble(
        this,
        "Cylinder",
        "Enter radius of Cylinder",
        40.0,
        0.1,
        1000.0,
        2,
        &ok);
    if (!ok)
        return;

    float height = QInputDialog::getDouble(
        this,
        "Cylinder",
        "Enter height of Cylinder",
        80.0,
        0.1,
        1000.0,
        2,
        &ok);
    if (!ok)
        return;

    if (glWidget)
    {
        glWidget->setCylinderSpecs(radius, height);
    }
}

void MainWindow::onImportSTL()
{
    static bool loadToA = true;
    QString fileName = QFileDialog::getOpenFileName(this, "Open STL File", "", "STL Files (*.stl)");
    if (!fileName.isEmpty())
    {
        std::vector<Triangle> tris;
        if (loadSTLFile(fileName.toStdString(), tris))
        {
            if (loadToA)
            {
                trianglesA = tris;
                QMessageBox::information(this, "Import STL", "Loaded as A: " + fileName);
            }
            else
            {
                trianglesB = tris;
                QMessageBox::information(this, "Import STL", "Loaded as B: " + fileName);
            }
            loadToA = !loadToA;
            stlwidget->update();
        }
        else
        {
            QMessageBox::warning(this, "Import STL", "Failed to load STL file.");
        }
    }
}

void MainWindow::onFindIntersection()
{
    // Just update the GLWidget to show intersection (if any) between trianglesA and trianglesB
    stlwidget->update();
    QMessageBox::information(this, "Find Intersection", "Intersection (if any) is now shown in the view.");
}