#include "torus.h"
#include "common.h"

Torus::Torus(Maille::Color color, float radius, float thickness)
    : selected(true)
    , r(radius)
    , shininess(1.0)
    , thickness(thickness)
    , nTheta(32)
    , nAlpha(16)
    , rotation(nanogui::Matrix4f::Identity())
    , translation(nanogui::Matrix4f::Identity())
    , color(color)
{
    init();
}

Torus::Torus(float radius, float thickness, unsigned numSamplesRadius,
             unsigned numSamplesCrossSection, float shininess,
             Maille::Color color)
    : r(radius)
    , shininess(shininess)
    , thickness(thickness)
    , nTheta(numSamplesRadius)
    , nAlpha(numSamplesCrossSection)
    , rotation(nanogui::Matrix4f::Identity())
    , translation(nanogui::Matrix4f::Identity())
    , color(color)
{
    init();
}

void Torus::init()
{
    positions = nanogui::MatrixXf(4, nTheta * nAlpha);
    normals = nanogui::MatrixXf(3, nTheta * nAlpha);
    indices = nanogui::MatrixXu(3, 2 * nAlpha * nTheta);
    transformed_points = nanogui::MatrixXf(4, nTheta * nAlpha);
    transformed_normals = nanogui::MatrixXf(3, nTheta * nAlpha);

    float alpha = 0.0;
    float theta = 0.0;
    for (size_t i = 0; i < nTheta; i++)
    {
        // find center coordinates of torus for calculating normal
        float xcenter = r * cos(theta);
        float ycenter = r * sin(theta);
        float zcenter = 0.0;

        for (size_t j = 0; j < nAlpha; j++)
        {
            size_t cur_idx = i * nAlpha + j;

            // Torus points
            float dist = r + thickness * cos(alpha);
            float x = dist * cos(theta);
            float y = dist * sin(theta);
            float z = thickness * sin(alpha);
            positions.col(cur_idx) << x, y, z, 1.0;

            // Find normal
            nanogui::Vector3f n(x - xcenter, y - ycenter, z - zcenter);
            n.normalize();
            normals.col(cur_idx) = n;
            alpha += 2 * M_PI / nAlpha;

            // Triangle indices
            size_t next_idx = i * nAlpha + (j + 1) % nAlpha;
            size_t far_idx = (i + 1) % nTheta * nAlpha + j;
            size_t far_next_idx = (i + 1) % nTheta * nAlpha + (j + 1) % nAlpha;
            indices.col(2 * cur_idx) << cur_idx, next_idx, far_next_idx;
            indices.col(2 * cur_idx + 1) << cur_idx, far_next_idx, far_idx;
        }
        theta += 2 * M_PI / nTheta;
    }

    compute_points();
    compute_normals();
}

void Torus::compute_points()
{
    nanogui::Matrix4f transformation = translation * rotation;

    for (size_t i = 0; i < positions.cols(); i++)
    {
        transformed_points.col(i) = transformation * positions.col(i);
    }
}

void Torus::compute_normals()
{
    for (size_t i = 0; i < normals.cols(); i++)
    {
        transformed_normals.col(i) =
            rotation.topLeftCorner(3, 3) * normals.col(i);
    }
}

nanogui::MatrixXf Torus::get_positions() const { return transformed_points; }

nanogui::MatrixXf Torus::get_normals() const { return transformed_normals; }

void Torus::set_center(float x, float y)
{
    translation(0, 3) = x;
    translation(1, 3) = y;

    compute_points();
}

void Torus::set_rotation(nanogui::Matrix4f rotation)
{
    this->rotation = rotation;

    compute_points();
    compute_normals();
}

nanogui::Vector2f Torus::get_center() const
{
    nanogui::Vector2f center(translation(0, 3), translation(1, 3));
    return center;
}

bool Torus::hasSameCenter(const Torus &t) const
{
    Eigen::Vector2f center = get_center();
    Eigen::Vector2f otherCenter = t.get_center();
    float dist = (otherCenter - center).norm();

    if (dist < thickness / 10.0)
        return true;

    return false;
}
