#pragma once

#include "mailleinlay.h"

class Tool
{
public:
    Tool(int icon)
        : icon(icon)
    {
    }

    virtual void load(MailleInlay &inlay) {}
    virtual void unload(MailleInlay &inlay) {}
    virtual bool mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                  bool down, int modifiers,
                                  const Eigen::Vector2f &worldPos,
                                  MailleInlay &inlay)
    {
        return false;
    }
    virtual bool mouseDragEvent(const Eigen::Vector2i &p,
                                const Eigen::Vector2i &rel, int button,
                                int modifiers, const Eigen::Vector2f &worldPos,
                                MailleInlay &inlay)
    {
        return false;
    }
    int getIcon() { return icon; }

private:
    int icon;
};
