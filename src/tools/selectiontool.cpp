#include <GLFW/glfw3.h>

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

    if (nearDist > 2 * nearest->get_radius())
    {
        inlay.selectedRings.clear();
        return true;
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
    bool ret = true;

    if (key == GLFW_KEY_DELETE && action)
    {
        deleteSelection(inlay);
    }
    else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)
    {
        if (action)
            ctrlDown = true;
        else
            ctrlDown = false;
    }
    else if (key == GLFW_KEY_A)
    {
        if (action && ctrlDown)
        {
            inlay.selectedRings.clear();

            for (const auto &ring : inlay.rings)
                inlay.selectedRings.push_back(ring);
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

void SelectionTool::deleteSelection(MailleInlay &inlay)
{
    while (!inlay.selectedRings.empty())
        weaveManager->deleteRing(inlay.selectedRings[0]->get_center(), inlay);
}

void SelectionTool::setSelectionColor(MailleInlay &inlay,
                                      const Eigen::Vector3f &color)
{
    for (auto &r : inlay.selectedRings)
        r->set_color(color);
}
