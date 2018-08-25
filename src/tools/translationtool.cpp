#include "translationtool.h"

TranslationTool::TranslationTool(int icon)
    : Tool(icon)
{
}

bool TranslationTool::mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                       bool down, int modifiers,
                                       const Eigen::Vector2f &worldPos,
                                       MailleInlay &inlay)
{
    if (!down)
    {
        inlay.mvp *= inlay.transformation;
        inlay.transformation.setIdentity();
        return true;
    }

    dragBeginWorld = worldPos;

    return true;
}

bool TranslationTool::mouseDragEvent(const Eigen::Vector2i &p,
                                     const Eigen::Vector2i &rel, int button,
                                     int modifiers,
                                     const Eigen::Vector2f &worldPos,
                                     MailleInlay &inlay)
{
    inlay.transformation(0, 3) = worldPos(0) - dragBeginWorld(0);
    inlay.transformation(1, 3) = worldPos(1) - dragBeginWorld(1);

    return true;
}
