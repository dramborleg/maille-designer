#pragma once

#include <nanogui/common.h>

class Torus
{
public:
    Torus(nanogui::Vector3f color = nanogui::Vector3f(1.0, 0.0, 0.0),
          float radius = 1.0, float thickness = 0.278);
    Torus(float radius, float thickness, unsigned numSamplesRadius = 32,
          unsigned numSamplesCrossSection = 16, float shininess = 1.0,
          nanogui::Vector3f color = nanogui::Vector3f(0.0, 0.0, 0.0));

    nanogui::MatrixXf get_positions() const;
    nanogui::MatrixXf get_normals() const;
    const nanogui::MatrixXu &get_indices() const { return indices; }
    const nanogui::Vector3f &get_color() const { return color; }
    float get_shininess() const { return shininess; }
    unsigned get_num_samples_radius() const { return nTheta; }
    unsigned get_num_samples_cross_section() const { return nAlpha; }
    void set_rotation(nanogui::Matrix4f rotation);
    void set_center(float x, float y);
    nanogui::Vector2f get_center() const;
    bool hasSameCenter(const Torus &t) const;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    void init();
    void compute_points();
    void compute_normals();

    float r;
    float shininess;
    float thickness;
    unsigned nTheta;
    unsigned nAlpha;
    nanogui::MatrixXf positions;
    nanogui::MatrixXf normals;
    nanogui::MatrixXu indices;
    nanogui::Matrix4f rotation;
    nanogui::Matrix4f translation;
    nanogui::Vector3f color;

    // pre-calculated normals and positions
    nanogui::MatrixXf transformed_points;
    nanogui::MatrixXf transformed_normals;
};
