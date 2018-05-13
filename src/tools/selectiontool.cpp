#include <GLFW/glfw3.h>

#include "selectiontool.h"

SelectionTool::SelectionTool(int icon, std::shared_ptr<Weave> weaveManager)
    : Tool(icon)
    , weaveManager(std::move(weaveManager))
{
}

void SelectionTool::unload(MailleInlay &inlay) { inlay.selectedRings.clear(); }

bool SelectionTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                     bool down, int modifiers,
                                     const Eigen::Vector2f &worldPos,
                                     MailleInlay &inlay)
{
    if (inlay.rings.empty())
        return true;

    if (down)
    {
        dragBeginWorld = worldPos;
        return true;
    }

    if (!ctrlDown)
        inlay.selectedRings.clear();

    if (dragEvent)
        return handleDragEvent(worldPos, inlay);

    auto nearest = findNearestRing(inlay, worldPos);

    if (nearest.second > 2 * nearest.first->get_radius())
    {
        inlay.selectedRings.clear();
        return true;
    }

    if (ctrlDown)
    {
        auto rSelected = findSelectedRing(inlay, *nearest.first);
        if (rSelected != inlay.selectedRings.cend())
            inlay.selectedRings.erase(rSelected);
        else
            inlay.selectedRings.push_back(nearest.first);
    }
    else
    {
        inlay.selectedRings.push_back(nearest.first);
    }

    return true;
}

bool SelectionTool::mouseDragEvent(const Eigen::Vector2i &p,
                                   const Eigen::Vector2i &rel, int button,
                                   int modifiers,
                                   const Eigen::Vector2f &worldPos,
                                   MailleInlay &inlay)
{
    dragEvent = true;
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

std::vector<std::shared_ptr<Torus>>::const_iterator
SelectionTool::findSelectedRing(const MailleInlay &inlay,
                                const Torus &ring) const
{
    auto r = inlay.selectedRings.cbegin();

    for (; r != inlay.selectedRings.cend(); r++)
    {
        if ((*r)->hasSameCenter(ring))
            break;
    }

    return r;
}

std::pair<std::shared_ptr<Torus>, float>
SelectionTool::findNearestRing(const MailleInlay &inlay,
                               const Eigen::Vector2f &worldPos) const
{
    if (inlay.rings.empty())
        throw std::domain_error("Nearest ring undefined\n");

    float nearDist = std::numeric_limits<float>::max();
    std::shared_ptr<Torus> nearest;

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

    return std::make_pair(nearest, nearDist);
}

bool SelectionTool::handleDragEvent(const Eigen::Vector2f &worldPos,
                                    MailleInlay &inlay)
{
    dragEvent = false;

    Eigen::Vector2f maxCoord(std::max(dragBeginWorld(0), worldPos(0)),
                             std::max(dragBeginWorld(1), worldPos(1)));
    Eigen::Vector2f minCoord(std::min(dragBeginWorld(0), worldPos(0)),
                             std::min(dragBeginWorld(1), worldPos(1)));
    for (const auto &r : inlay.rings)
    {
        Eigen::Vector2f loc = r->get_center();
        if (loc(0) > minCoord(0) && loc(0) < maxCoord(0) &&
            loc(1) > minCoord(1) && loc(1) < maxCoord(1))
        {
            auto rSelected = findSelectedRing(inlay, *r);
            if (rSelected != inlay.selectedRings.cend())
                inlay.selectedRings.erase(rSelected);
            else
                inlay.selectedRings.push_back(r);
        }
    }

    return true;
}
