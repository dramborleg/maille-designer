#pragma once

#include <nanogui/screen.h>

#include "mailleinlay.h"

class RingGLCanvas;
class Tool;

class MailleScreen : public nanogui::Screen
{
public:
    MailleScreen();

    virtual void draw(NVGcontext *ctx);

private:
    std::shared_ptr<MailleInlay> createSampleInlay();

    RingGLCanvas *mCanvas;
    // Global inlay state
    std::shared_ptr<MailleInlay> inlay;
    // Tools
    std::shared_ptr<Tool> adderTool;
    std::shared_ptr<Tool> selectionTool;
};
