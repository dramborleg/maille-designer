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

struct ColorCompare
{
    bool operator()(const Color &c1, const Color &c2) const
    {
        size_t h1 = ColorHash()(c1);
        size_t h2 = ColorHash()(c2);
        return h1 < h2;
    }
};

float colorDistance(const Maille::Color &c0, const Maille::Color &c1);
} // namespace Maille
