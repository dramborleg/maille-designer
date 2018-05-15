#include <GLFW/glfw3.h>

#include "common.h"
#include "selectiontool.h"

SelectionTool::SelectionTool(int icon, std::shared_ptr<Weave> weaveManager)
    : Tool(icon)
    , weaveManager(std::move(weaveManager))
{
}

void SelectionTool::load(MailleInlay &inlay) { setSelection(inlay, false); }

void SelectionTool::unload(MailleInlay &inlay) { setSelection(inlay, true); }

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
        setSelection(inlay, false);

    if (dragEvent)
        return handleDragEvent(worldPos, inlay);

    auto nearest = findNearestRing(inlay, worldPos);

    if (nearest.second > 2 * nearest.first->get_radius())
    {
        setSelection(inlay, false);
        return true;
    }

    if (ctrlDown)
        nearest.first->set_selected(!nearest.first->get_selected());
    else
        nearest.first->set_selected(true);

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
            setSelection(inlay, true);
    }
    else
    {
        ret = false;
    }

    return ret;
}

void SelectionTool::deleteSelection(MailleInlay &inlay)
{
    bool deleted = true;

    // This while loop is used instead of a simple for loop because we are
    // deleting elements from the vector while looping through it, so care
    // must be taken to avoid corrupting the iterators.
    while (deleted)
    {
        deleted = false;

        for (const auto &r : inlay.rings)
        {
            if (r->get_selected())
            {
                weaveManager->deleteRing(r->get_center(), inlay);
                deleted = true;
                break;
            }
        }
    }
}

void SelectionTool::setSelectionColor(MailleInlay &inlay,
                                      const Maille::Color &color)
{
    for (auto &r : inlay.rings)
        r->set_color(color);
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
            r->set_selected(!r->get_selected());
        }
    }

    return true;
}

void SelectionTool::setSelection(MailleInlay &inlay, bool selected)
{
    for (auto &r : inlay.rings)
        r->set_selected(selected);
}
