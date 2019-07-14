#include "common.h"

namespace Maille
{
float colorDistance(const Maille::Color &c0, const Maille::Color &c1)
{
    using std::abs;
    using std::max;

    return max({abs(c0(0) - c1(0)), abs(c0(1) - c1(1)), abs(c0(2) - c1(2))});
}
} // namespace Maille
