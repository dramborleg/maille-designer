#pragma once

#include <Eigen/Core>
#include <memory>

namespace Maille
{
typedef Eigen::Matrix<unsigned char, 3, 1> Color;

struct ColorHash
{
    size_t operator()(const Color &c) const
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

// clang-format off
const std::string POV_TEMPLATE =
    "// Example command to render a png image\n"
    "//\n"
    "// povray example.pov Height=1024 Width=1024 Antialias=true Sampling_Method=2\n"
    "\n"
    "background { color 0.4 * <1, 1, 1> }\n"
    "\n"
    "light_source {\n"
    "  <0, 50, 0>\n"
    "  color rgb 0.3 * <1, 1, 1>\n"
    "}\n"
    "\n"
    "light_source {\n"
    "  <-100, 500, 100>\n"
    "  color rgb 0.2 * <1, 1, 1>\n"
    "}\n"
    "\n"
    "#declare F_Metal =\n"
    "finish {\n"
    "  ambient 0.25\n"
    "  brilliance 4\n"
    "  diffuse 0.5\n"
    "  metallic\n"
    "  specular 0.80\n"
    "  roughness 1/80\n"
    "  reflection 0.5\n"
    "}\n\n";
// clang-format on
} // namespace Maille
