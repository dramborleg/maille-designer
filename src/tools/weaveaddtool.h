#pragma once

#include "tool.h"
#include "weaves/european4in1.h"

class WeaveAddTool : public Tool
{
public:
    WeaveAddTool(int icon)
        : Tool(icon)
    {
        weaveManager = std::unique_ptr<Weave>(new European4in1());
    }

    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay);

private:
    std::unique_ptr<Weave> weaveManager;
};
