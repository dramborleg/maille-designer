#pragma once

#include <nanogui/common.h>

class Torus {
public:
    Torus(float radius, float thickness, unsigned numSamplesRadius = 128,
          unsigned numSamplesCrossSection = 32);

    const nanogui::MatrixXf & get_positions() const { return positions; }
    const nanogui::MatrixXf & get_colors() const { return colors; }
    const nanogui::MatrixXu & get_indices() const { return indices; }

private:
    float radius;
    float thickness;
    unsigned nTheta;
    unsigned nAlpha;
    nanogui::MatrixXf positions;
    nanogui::MatrixXu indices;
    nanogui::MatrixXf colors;
};
