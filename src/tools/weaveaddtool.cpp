#include <cmath>

#include "weaveaddtool.h"

WeaveAddTool::WeaveAddTool(int icon, std::shared_ptr<Weave> weaveManager,
                           std::shared_ptr<Eigen::Vector3f> fgcolor)
    : Tool(icon)
    , weaveManager(std::move(weaveManager))
    , fgcolor(std::move(fgcolor))
{
}

bool WeaveAddTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                    bool down, int modifiers,
                                    const Eigen::Vector2f &worldPos,
                                    MailleInlay &inlay)
{
    if (down)
    {
        dragBeginWorld = worldPos;
        dragBeginPixel = p;
        return true;
    }

    if (dragEvent)
    {
        int xDiff = std::max(p(0), dragBeginPixel(0)) -
                    std::min(p(0), dragBeginPixel(0));
        int yDiff = std::max(p(1), dragBeginPixel(1)) -
                    std::min(p(1), dragBeginPixel(1));
        int dragArea = xDiff * yDiff;
        if (dragArea > 16)
            weaveManager->addRingsInArea(dragBeginWorld, worldPos, *fgcolor,
                                         inlay);
        else
            dragEvent = false;
    }

    if (!dragEvent)
        weaveManager->addRing(worldPos, *fgcolor, inlay);

    dragEvent = false;
    return true;
}

bool WeaveAddTool::mouseDragEvent(const Eigen::Vector2i &p,
                                  const Eigen::Vector2i &rel, int button,
                                  int modifiers,
                                  const Eigen::Vector2f &worldPos,
                                  MailleInlay &inlay)
{
    dragEvent = true;
    return true;
}
