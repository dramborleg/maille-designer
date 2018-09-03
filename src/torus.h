#pragma once

#include <nanogui/common.h>

#include "common.h"

class Torus
{
public:
    Torus(const Maille::Color &color);

    nanogui::MatrixXf get_positions() const;
    nanogui::MatrixXf get_normals() const;
    const nanogui::MatrixXu &get_indices() const { return indices; }
    const Maille::Color &get_color() const { return color; }
    float get_radius() const { return r; }
    float get_shininess() const { return shininess; }
    unsigned get_num_samples_radius() const { return nTheta; }
    unsigned get_num_samples_cross_section() const { return nAlpha; }
    bool get_selected() const { return selected; }
    void set_rotation(nanogui::Matrix4f rotation);
    void set_center(float x, float y);
    void set_color(const Maille::Color &color) { this->color = color; }
    void set_selected(bool sel) { selected = sel; }
    nanogui::Vector2f get_center() const;
    bool hasSameCenter(const Torus &t) const;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    void init();
    void compute_points();
    void compute_normals();

    bool selected = true;
    float r = 1.0;
    float shininess = 1.0;
    float thickness = 0.278;
    unsigned nTheta = 32;
    unsigned nAlpha = 16;
    nanogui::MatrixXf positions;
    nanogui::MatrixXf normals;
    nanogui::MatrixXu indices;
    nanogui::Matrix4f rotation = nanogui::Matrix4f::Identity();
    nanogui::Matrix4f translation = nanogui::Matrix4f::Identity();
    Maille::Color color;

    // pre-calculated normals and positions
    nanogui::MatrixXf transformed_points;
    nanogui::MatrixXf transformed_normals;
};
