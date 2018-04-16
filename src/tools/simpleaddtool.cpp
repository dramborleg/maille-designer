#include "simpleaddtool.h"

bool SimpleAddTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                     bool down, int modifiers,
                                     const Eigen::Vector2f &worldPos,
                                     std::shared_ptr<MailleInlay> inlay)
{
    if (!down)
        return true;

    inlay->rings.push_back(Torus(Eigen::Vector3f(0.8, 0.0, 0.0)));
    inlay->rings.back().set_center(worldPos(0), worldPos(1));
    inlay->ringsModified = true;

    return true;
}
