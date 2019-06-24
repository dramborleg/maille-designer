#pragma once

#include <Eigen/Core>
#include <memory>

namespace Maille
{
typedef Eigen::Matrix<unsigned char, 3, 1> Color;

struct ColorHash
{
    std::size_t operator()(const Color &c) const
    {
        return c(0) << 16 | c(1) << 8 | c(2);
    }
};

float colorDistance(const Maille::Color &c0, const Maille::Color &c1);
} // namespace Maille
