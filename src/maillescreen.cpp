#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/screen.h>
#include <nanogui/slider.h>
#include <nanogui/window.h>

#include "maillescreen.h"
#include "ringglcanvas.h"

MailleScreen::MailleScreen()
    : nanogui::Screen(Eigen::Vector2i(800, 600), "Maille Designer", false)
{
    using namespace nanogui;

    mCanvas = new RingGLCanvas(this);
    mCanvas->setPosition(Vector2i(200, 0));
    mCanvas->setBackgroundColor({100, 100, 100, 255});
    mCanvas->setSize({600, 600});

    Window *controls = new Window(this, "Controls");
    controls->setPosition(Vector2i(16, 16));
    controls->setLayout(new GroupLayout());
    new Label(controls, "Brightness Slider", "sans-bold");
    Slider *brightness = new Slider(controls);
    brightness->setTooltip("Set ambient brightness multiplier");
    brightness->setValue(mCanvas->getAmbientIntensityFactor());
    brightness->setCallback(
        [this](float val) { mCanvas->setAmbientIntensityFactor(val); });

    performLayout();
}

void MailleScreen::draw(NVGcontext *ctx)
{
    /* Draw the user interface */
    Screen::draw(ctx);
}
