#include "torus.h"

Torus::Torus(nanogui::Vector3f color, float radius, float thickness)
    : r(radius)
    , shininess(1.0)
    , thickness(thickness)
    , nTheta(128)
    , nAlpha(32)
    , rotation(nanogui::Matrix4f::Identity())
    , translation(nanogui::Matrix4f::Identity())
    , color(color)
{
    init();
}

Torus::Torus(float radius, float thickness, unsigned numSamplesRadius,
             unsigned numSamplesCrossSection, float shininess,
             nanogui::Vector3f color)
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
}

nanogui::MatrixXf Torus::get_positions() const
{
    nanogui::MatrixXf transformed_points(4, nTheta * nAlpha);
    nanogui::Matrix4f transformation = translation * rotation;

    for (size_t i = 0; i < positions.cols(); i++)
    {
        transformed_points.col(i) = transformation * positions.col(i);
    }

    return transformed_points;
}

nanogui::MatrixXf Torus::get_normals() const
{
    nanogui::MatrixXf transformed_normals(3, nTheta * nAlpha);

    for (size_t i = 0; i < normals.cols(); i++)
    {
        transformed_normals.col(i) =
            rotation.topLeftCorner(3, 3) * normals.col(i);
    }

    return transformed_normals;
}

void Torus::set_center(float x, float y)
{
    translation(0, 3) = x;
    translation(1, 3) = y;
}

nanogui::Vector2f Torus::get_center() const
{
    nanogui::Vector2f center(translation(0, 3), translation(1, 3));
    return center;
}
