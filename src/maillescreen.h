#pragma once

#include <nanogui/screen.h>

class RingGLCanvas;

class MailleScreen : public nanogui::Screen
{
public:
    MailleScreen();

    virtual void draw(NVGcontext *ctx);

private:
    RingGLCanvas *mCanvas;
};

