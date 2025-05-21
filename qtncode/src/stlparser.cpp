#include "stlparser.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool loadSTLFile(const std::string& filename, std::vector<Triangle>& triangles) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << "\n";
        return false;
    }

    std::string line;
    POINT v[3];
    int vertexCount = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string word;
        iss >> word;

        if (word == "vertex") {
            float x, y, z;
            iss >> x >> y >> z;
            v[vertexCount++] = POINT(x, y, z);

            if (vertexCount == 3) {
                triangles.emplace_back(v[0], v[1], v[2]);
                vertexCount = 0;
            }
        }
    }

    file.close();
    return true;
}
