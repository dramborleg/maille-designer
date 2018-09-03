#pragma once

#include "tool.h"

class ColorPickerTool : public Tool
{
public:
    ColorPickerTool(int icon, nanogui::ColorWheel *wheel);
    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay) override;

private:
    nanogui::ColorWheel *wheel;
};
