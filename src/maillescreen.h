#pragma once

#include <nanogui/screen.h>

#include "mailleinlay.h"

class RingGLCanvas;
class SelectionTool;
class Tool;
class WeaveAddTool;

class MailleScreen : public nanogui::Screen
{
public:
    MailleScreen();

private:
    RingGLCanvas *mCanvas;
    // Global inlay state
    std::shared_ptr<MailleInlay> inlay;
    // Tools
    std::shared_ptr<WeaveAddTool> adderTool;
    std::shared_ptr<SelectionTool> selectionTool;
    // Global foreground color
    std::shared_ptr<Eigen::Vector3f> fgcolor;
};
