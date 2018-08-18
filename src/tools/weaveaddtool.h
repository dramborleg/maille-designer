#pragma once

#include "common.h"
#include "tool.h"
#include "weaves/weave.h"

class WeaveAddTool : public Tool
{
public:
    WeaveAddTool(int icon, std::shared_ptr<Weave> weaveManager,
                 std::shared_ptr<Maille::Color> fgcolor);
    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay);
    bool mouseDragEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel,
                        int button, int modifiers,
                        const Eigen::Vector2f &worldPos, MailleInlay &inlay);

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    bool dragEvent;
    Eigen::Vector2f dragBeginWorld;
    Eigen::Vector2i dragBeginPixel;
    std::shared_ptr<Weave> weaveManager;
    std::shared_ptr<Maille::Color> fgcolor;
};
