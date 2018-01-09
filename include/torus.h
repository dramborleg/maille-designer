#pragma once

#include <nanogui/common.h>

class Torus
{
public:
    Torus(float radius, float thickness, unsigned numSamplesRadius = 128,
          unsigned numSamplesCrossSection = 32);

    const nanogui::MatrixXf &get_positions() const { return positions; }
    const nanogui::MatrixXf &get_normals() const { return normals; }
    const nanogui::MatrixXu &get_indices() const { return indices; }
    const unsigned &get_num_samples_radius() const { return nTheta; }
    const unsigned &get_num_samples_cross_section() const { return nAlpha; }

private:
    float radius;
    float thickness;
    unsigned nTheta;
    unsigned nAlpha;
    nanogui::MatrixXf positions;
    nanogui::MatrixXf normals;
    nanogui::MatrixXu indices;
};
