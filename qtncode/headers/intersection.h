#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include "triangle.h"

// Separate vectors for each triangle group (e.g., for two objects/meshes)
extern std::vector<Triangle> trianglesA;
extern std::vector<Triangle> trianglesB;

// Intersection function declaration
bool trianglesIntersect(const Triangle& t1, const Triangle& t2);
// Returns true if triangles are coplanar
bool trianglesCoplanar(const Triangle& t1, const Triangle& t2);
// Returns true if triangles intersect in 3D and sets segA, segB to the segment endpoints
bool triangleTriangleIntersectionSegment(const Triangle& t1, const Triangle& t2, POINT& segA, POINT& segB);

// Extensible: you can add more vectors like trianglesC, trianglesD, etc. in the future.

#endif
