#include "simpleaddtool.h"

bool SimpleAddTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                     bool down, int modifiers,
                                     std::shared_ptr<MailleInlay> inlay)
{
    if (!down)
        return true;

    inlay->rings.insert(
        inlay->rings.begin(),
        Torus(1.2, 0.4, 128, 32, 8.0, Eigen::Vector3f(0.8, 0.0, 0.0)));
    inlay->rings[0].set_center(lastX, 0.0);
    lastX += 1.0;
    inlay->ringsModified = true;

    return true;
}

bool SimpleAddTool::mouseDragEvent(const Eigen::Vector2i &p,
                                   const Eigen::Vector2i &rel, int button,
                                   int modifiers,
                                   std::shared_ptr<MailleInlay> inlay)
{
    return false;
}
