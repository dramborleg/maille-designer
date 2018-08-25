#pragma once

#include "tool.h"

class TranslationTool : public Tool
{
public:
    TranslationTool(int icon);
    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay);
    bool mouseDragEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel,
                        int button, int modifiers,
                        const Eigen::Vector2f &worldPos, MailleInlay &inlay);

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    Eigen::Vector2f dragBeginWorld;
};
