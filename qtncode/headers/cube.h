#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <cmath>

class Point {
private:
    double x, y, z;

public:
    Point() : x(0), y(0), z(0) {}
    Point(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    double getX() const { return x; }
    double getY() const { return y; }
    double getZ() const { return z; }

    void setX(double p) { x = p; }
    void setY(double q) { y = q; }
    void setZ(double r) { z = r; }
};

class Line {
private:
    Point start, end;

public:
    Line() {}
    Line(Point s, Point e) : start(s), end(e) {}

    Point getStart() const { return start; }
    Point getEnd() const { return end; }

    void setStart(Point p1) { start = p1; }
    void setEnd(Point p2) { end = p2; }

    double getLength() const {
        double dx = start.getX() - end.getX();
        double dy = start.getY() - end.getY();
        double dz = start.getZ() - end.getZ();
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }
};

class Cube
{
public:
    // Cube(){

    // }
    // ~Cube(){
        
    // }
    std::vector<Point> basePoints;
    std::vector<Point> extrudedPoints;
    std::vector<Line> edges;
    double currentExtrusionHeight = 0.0;

    void addBasePoint(const Point& p);
    void updateExtrusion(double height);
    void buildEdges();
    void finalizeExtrusion();
    void build2DFace();
    void draw() const;
};


#endif //CUBE_H
