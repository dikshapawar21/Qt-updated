#pragma once

#include <vector>

class Bezier
{
public:
    Bezier(const std::vector<std::vector<double>>& controlPoints, int numInterpolated);

    void draw() const;

private:
    std::vector<std::vector<double>> controlPoints;
    int interpolatedPoints;

    std::vector<double> deCasteljau(double t) const;
};
