#include "selectiontool.h"

void SelectionTool::unload(MailleInlay &inlay) { inlay.selectedRings.clear(); }

bool SelectionTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                     bool down, int modifiers,
                                     const Eigen::Vector2f &worldPos,
                                     MailleInlay &inlay)
{
    if (!down || inlay.rings.empty())
        return true;

    std::shared_ptr<Torus> nearest;
    float nearDist = std::numeric_limits<float>::max();

    for (const auto &r : inlay.rings)
    {
        Eigen::Vector2f center = r->get_center();
        float dist = (worldPos - center).norm();

        if (dist < nearDist)
        {
            nearest = r;
            nearDist = dist;
        }
    }

    inlay.selectedRings.clear();
    inlay.selectedRings.push_back(nearest);
    return true;
}
