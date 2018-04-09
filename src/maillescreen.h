#pragma once

#include <nanogui/screen.h>

#include "mailleinlay.h"

class RingGLCanvas;

class MailleScreen : public nanogui::Screen
{
public:
    MailleScreen();

    virtual void draw(NVGcontext *ctx);

private:
    std::shared_ptr<MailleInlay> createSampleInlay();

    RingGLCanvas *mCanvas;
};
