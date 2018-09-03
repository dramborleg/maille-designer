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
        dragPrevWorld = worldPos;
        return true;
    }

    if (!ctrlDown)
        setSelection(inlay, false);

    if (dragEvent)
        return completeDragEvent(worldPos, inlay);

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
    bool inCurrent, inPrevious;

    for (auto &r : inlay.rings)
    {
        auto center = r->get_center();
        inCurrent = coordinateInBox(dragBeginWorld, worldPos, center);
        inPrevious = coordinateInBox(dragBeginWorld, dragPrevWorld, center);
        if (inCurrent)
            r->set_selected(true);
        else if (inPrevious)
            r->set_selected(false);
    }

    dragPrevWorld = worldPos;
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
        if (r->get_selected())
            r->set_color(color);
}

bool SelectionTool::completeDragEvent(const Eigen::Vector2f &worldPos,
                                      MailleInlay &inlay)
{
    dragEvent = false;

    if (ctrlDown)
        return true;

    for (const auto &r : inlay.rings)
    {
        if (coordinateInBox(dragBeginWorld, worldPos, r->get_center()))
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

void SelectionTool::setWeaveManager(std::shared_ptr<Weave> weaveManager)
{
    this->weaveManager = weaveManager;
}

bool SelectionTool::coordinateInBox(const Eigen::Vector2f &p0,
                                    const Eigen::Vector2f &p1,
                                    const Eigen::Vector2f &location)
{
    Eigen::Vector2f maxCoord(std::max(p0(0), p1(0)), std::max(p0(1), p1(1)));
    Eigen::Vector2f minCoord(std::min(p0(0), p1(0)), std::min(p0(1), p1(1)));

    return (location(0) > minCoord(0) && location(0) < maxCoord(0) &&
            location(1) > minCoord(1) && location(1) < maxCoord(1));
}
