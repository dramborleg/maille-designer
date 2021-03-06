#pragma once

#include "tool.h"

class SimpleAddTool : public Tool
{
public:
    SimpleAddTool(int icon)
        : Tool(icon)
    {
    }

    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay) override;
};
