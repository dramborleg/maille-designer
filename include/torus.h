#pragma once

#include <nanogui/common.h>

class Torus
{
public:
    Torus() {}
    Torus(float radius, float thickness, unsigned numSamplesRadius = 128,
          unsigned numSamplesCrossSection = 32, float shininess = 1.0,
          nanogui::Vector3f color = nanogui::Vector3f(0.0, 0.0, 0.0));

    const nanogui::MatrixXf &get_positions() const { return positions; }
    const nanogui::MatrixXf &get_normals() const { return normals; }
    const nanogui::MatrixXu &get_indices() const { return indices; }
    const nanogui::Vector3f &get_color() const { return color; }
    float get_shininess() const { return shininess; }
    unsigned get_num_samples_radius() const { return nTheta; }
    unsigned get_num_samples_cross_section() const { return nAlpha; }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    float radius;
    float shininess;
    float thickness;
    unsigned nTheta;
    unsigned nAlpha;
    nanogui::MatrixXf positions;
    nanogui::MatrixXf normals;
    nanogui::MatrixXu indices;
    nanogui::Vector3f color;
};
