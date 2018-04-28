#pragma once

#include "tool.h"
#include "weaves/weave.h"

class WeaveAddTool : public Tool
{
public:
    WeaveAddTool(int icon, std::shared_ptr<Weave> weaveManager,
                 std::shared_ptr<Eigen::Vector3f> fgcolor);
    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay);

private:
    std::shared_ptr<Weave> weaveManager;
    std::shared_ptr<Eigen::Vector3f> fgcolor;
};
