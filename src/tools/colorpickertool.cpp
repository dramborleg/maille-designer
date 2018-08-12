#include "colorpickertool.h"
#include <nanogui/colorwheel.h>
#include <nanogui/common.h>

ColorPickerTool::ColorPickerTool(int icon, nanogui::ColorWheel *wheel)
    : Tool(icon)
    , wheel(wheel)
{
}

bool ColorPickerTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                       bool down, int modifiers,
                                       const Eigen::Vector2f &worldPos,
                                       MailleInlay &inlay)
{
    if (inlay.rings.empty() || down)
        return true;

    auto nearest = findNearestRing(inlay, worldPos);
    Maille::Color color = nearest.first->get_color();
    wheel->setColor(
        nanogui::Color(Eigen::Vector3i(color(0), color(1), color(2)), 1.0));
    wheel->callback()(wheel->color());

    return true;
}
