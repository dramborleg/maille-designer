#pragma once

#include "common.h"
#include "tool.h"

class PaintTool : public Tool
{
public:
    PaintTool(int icon, std::shared_ptr<Maille::Color> fgcolor);
    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay);
    bool mouseDragEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel,
                        int button, int modifiers,
                        const Eigen::Vector2f &worldPos, MailleInlay &inlay);

private:
    std::shared_ptr<Maille::Color> fgcolor;
};
