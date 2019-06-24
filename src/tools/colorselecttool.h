#pragma once

#include "selectiontool.h"

class ColorSelectTool : public SelectionTool
{
public:
    ColorSelectTool(int icon, std::shared_ptr<Weave> weaveManager,
                    nanogui::Slider *distance);
    virtual bool mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                  bool down, int modifiers,
                                  const Eigen::Vector2f &worldPos,
                                  MailleInlay &inlay) override;
    virtual bool mouseDragEvent(const Eigen::Vector2i &p,
                                const Eigen::Vector2i &rel, int button,
                                int modifiers, const Eigen::Vector2f &worldPos,
                                MailleInlay &inlay) override;

private:
    nanogui::Slider *distance;
};
