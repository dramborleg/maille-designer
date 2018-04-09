#pragma once

#include "tool.h"

class SimpleAddTool : public Tool
{
public:
    SimpleAddTool(int icon)
        : Tool(icon)
        , lastX(0.0)
    {
    }
    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, std::shared_ptr<MailleInlay> inlay);
    bool mouseDragEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel,
                        int button, int modifiers,
                        std::shared_ptr<MailleInlay> inlay);

private:
    float lastX;
};
