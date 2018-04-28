#include "selectiontool.h"

SelectionTool::SelectionTool(int icon, std::shared_ptr<Weave> weaveManager)
    : Tool(icon)
    , weaveManager(weaveManager)
{
}

void SelectionTool::unload(MailleInlay &inlay) { inlay.selectedRings.clear(); }

bool SelectionTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                     bool down, int modifiers,
                                     const Eigen::Vector2f &worldPos,
                                     MailleInlay &inlay)
{
    if (!down || inlay.rings.empty())
        return true;

    bool isSelected = false;
    float nearDist = std::numeric_limits<float>::max();
    std::shared_ptr<Torus> nearest;
    std::vector<std::shared_ptr<Torus>>::iterator rSelected;

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

    if (!ctrlDown)
    {
        inlay.selectedRings.clear();
        inlay.selectedRings.push_back(nearest);
        return true;
    }

    for (auto r = inlay.selectedRings.begin(); r != inlay.selectedRings.end();
         r++)
    {
        if ((*r)->hasSameCenter(*nearest))
        {
            isSelected = true;
            rSelected = r;
            break;
        }
    }
    if (isSelected)
        inlay.selectedRings.erase(rSelected);
    else
        inlay.selectedRings.push_back(nearest);

    return true;
}

bool SelectionTool::keyboardEvent(int key, int scancode, int action,
                                  int modifiers, MailleInlay &inlay)
{
    bool ret = false;

    if (key == 261 && scancode == 119)
    {
        if (!action)
            return true;

        while (!inlay.selectedRings.empty())
            weaveManager->deleteRing(inlay.selectedRings[0]->get_center(),
                                     inlay);
        ret = true;
    }
    else if ((key == 345 && scancode == 105) || (key == 341 && scancode == 37))
    {
        if (action)
            ctrlDown = true;
        else
            ctrlDown = false;

        ret = true;
    }
    else if (key == 65 && scancode == 38)
    {
        if (action && ctrlDown)
        {
            inlay.selectedRings.clear();

            for (const auto &ring : inlay.rings)
                inlay.selectedRings.push_back(ring);
        }

        ret = true;
    }

    return ret;
}
