#pragma once

#include "tool.h"

class TranslationTool : public Tool
{
public:
    TranslationTool(int icon);
    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay) override;
    bool mouseMotionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel,
                          int button, int modifiers,
                          const Eigen::Vector2f &worldPos, MailleInlay &inlay);
    bool mouseDragEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel,
                        int button, int modifiers,
                        const Eigen::Vector2f &worldPos,
                        MailleInlay &inlay) override;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    bool motionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel,
                     int button, int modifiers, const Eigen::Vector2f &worldPos,
                     MailleInlay &inlay);

    Eigen::Vector2f dragBeginWorld;
};
