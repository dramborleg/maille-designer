#pragma once

#include "mailleinlay.h"

class Tool
{
public:
    Tool(int icon)
        : icon(icon)
    {
    }
    virtual bool mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                  bool down, int modifiers,
                                  const Eigen::Vector2f &worldPos,
                                  std::shared_ptr<MailleInlay> inlay) = 0;
    virtual bool mouseDragEvent(const Eigen::Vector2i &p,
                                const Eigen::Vector2i &rel, int button,
                                int modifiers, const Eigen::Vector2f &worldPos,
                                std::shared_ptr<MailleInlay> inlay) = 0;
    int getIcon() { return icon; }

private:
    int icon;
};
