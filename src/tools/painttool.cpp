#include "painttool.h"

PaintTool::PaintTool(int icon, std::shared_ptr<Maille::Color> fgcolor)
    : Tool(icon)
    , fgcolor(std::move(fgcolor))
{
}

bool PaintTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                 bool down, int modifiers,
                                 const Eigen::Vector2f &worldPos,
                                 MailleInlay &inlay)
{
    if (inlay.rings.empty() || !down)
        return true;

    auto nearest = findNearestRing(inlay, worldPos);

    if (nearest.second > 1.5 * nearest.first->get_radius())
        return true;

    nearest.first->set_color(*fgcolor);

    return true;
}

bool PaintTool::mouseDragEvent(const Eigen::Vector2i &p,
                               const Eigen::Vector2i &rel, int button,
                               int modifiers, const Eigen::Vector2f &worldPos,
                               MailleInlay &inlay)
{
    return mouseButtonEvent(p, button, true, modifiers, worldPos, inlay);
}
