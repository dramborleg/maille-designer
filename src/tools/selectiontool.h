#pragma once

#include "tool.h"

class SelectionTool : public Tool
{
public:
    SelectionTool(int icon)
        : Tool(icon)
    {
    }

    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay);
};
