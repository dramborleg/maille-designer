#include <GLFW/glfw3.h>
#include <nanogui/checkbox.h>
#include <nanogui/slider.h>

#include "colorselecttool.h"
#include "common.h"

ColorSelectTool::ColorSelectTool(int icon, std::shared_ptr<Weave> weaveManager,
                                 nanogui::Slider *distance,
                                 nanogui::CheckBox *adjacent)
    : SelectionTool(icon, weaveManager)
    , distance(distance)
    , adjacent(adjacent)
{
}

bool ColorSelectTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                       bool down, int modifiers,
                                       const Eigen::Vector2f &worldPos,
                                       MailleInlay &inlay)
{
    if (inlay.rings.empty() || down)
        return true;

    if (!ctrlDown)
        setSelection(inlay, false);

    auto nearest = findNearestRing(inlay, worldPos);
    if (!withinSelectDistance(nearest.second, *nearest.first))
    {
        setSelection(inlay, false);
        return true;
    }

    if (adjacent->checked())
    {
        weaveManager->selectAdjacentColors(worldPos, distance->value(), inlay);
        return true;
    }

    // don't need weave manager to select by color if we don't care about
    // whether rings are adjacent or not
    Maille::Color color = nearest.first->get_color();
    float dist = distance->value();
    for (auto &r : inlay.rings)
    {
        Maille::Color c = r->get_color();
        if (Maille::colorDistance(color, c) <= dist)
            r->set_selected(true);
    }

    return true;
}

bool ColorSelectTool::mouseDragEvent(const Eigen::Vector2i &p,
                                     const Eigen::Vector2i &rel, int button,
                                     int modifiers,
                                     const Eigen::Vector2f &worldPos,
                                     MailleInlay &inlay)
{
    return false;
}
