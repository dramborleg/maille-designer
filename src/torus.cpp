#include "torus.h"

Torus::Torus(float radius, float thickness, unsigned numSamplesRadius,
             unsigned numSamplesCrossSection)
    : radius(radius)
    , thickness(thickness)
    , nTheta(numSamplesRadius)
    , nAlpha(numSamplesCrossSection)
{
    positions = nanogui::MatrixXf(3, nTheta * nAlpha);
    normals = nanogui::MatrixXf(3, nTheta * nAlpha);
    indices = nanogui::MatrixXu(3, 2 * nAlpha * nTheta);

    float alpha, theta = 0.0;
    for (size_t i = 0; i < nTheta; i++) {
        // find center coordinates of torus for calculating normal
        float xcenter = radius * cos(theta);
        float ycenter = radius * sin(theta);
        float zcenter = 0.0;

        for (size_t j = 0; j < nAlpha; j++) {
            size_t cur_idx = i * nAlpha + j;

            // Torus points
            float dist = radius + thickness * cos(alpha);
            float x = dist * cos(theta);
            float y = dist * sin(theta);
            float z = thickness * sin(alpha);
            positions.col(cur_idx) << x, y, z;

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
