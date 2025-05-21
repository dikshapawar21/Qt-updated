#ifndef STLPARSER_H
#define STLPARSER_H

#include <vector>
#include <string>
#include "triangle.h"

bool loadSTLFile(const std::string& filename, std::vector<Triangle>& triangles);

#endif
