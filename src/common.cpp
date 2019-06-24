#include "common.h"

namespace Maille
{
float colorDistance(const Maille::Color &c0, const Maille::Color &c1)
{
    int total0 = c0(0) * c0(0) + c0(1) * c0(1) + c0(2) * c0(2);
    int total1 = c1(0) * c1(0) + c1(1) * c1(1) + c1(2) * c1(2);

    return std::sqrt(std::abs(total0 - total1));
}
} // namespace Maille
