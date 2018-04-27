#include "weaveaddtool.h"

WeaveAddTool::WeaveAddTool(int icon, std::shared_ptr<Weave> weaveManager)
    : Tool(icon)
    , weaveManager(weaveManager)
{
}

bool WeaveAddTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                    bool down, int modifiers,
                                    const Eigen::Vector2f &worldPos,
                                    MailleInlay &inlay)
{
    if (!down)
        return true;

    weaveManager->addRing(worldPos, inlay);
    return true;
}
