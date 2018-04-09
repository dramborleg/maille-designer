#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/nanogui.h>
#include <nanogui/screen.h>
#include <nanogui/slider.h>
#include <nanogui/window.h>

#include "maillescreen.h"
#include "ringglcanvas.h"
#include "tools/simpleaddtool.h"

MailleScreen::MailleScreen()
    : nanogui::Screen(Eigen::Vector2i(800, 600), "Maille Designer", false)
{
    using namespace nanogui;

    inlay = createSampleInlay();
    std::shared_ptr<Tool> tool(new SimpleAddTool(ENTYPO_ICON_ADD_TO_LIST));
    mCanvas = new RingGLCanvas(this, inlay, tool);
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

std::shared_ptr<MailleInlay> MailleScreen::createSampleInlay()
{
    std::shared_ptr<MailleInlay> inlay(new MailleInlay);

    inlay->rings.push_back(
        Torus(1.2, 0.4, 128, 32, 8.0, Eigen::Vector3f(0.8, 0.0, 0.0)));
    inlay->rings[0].set_center(-2.5, 0.0);
    inlay->rings.push_back(
        Torus(1.2, 0.4, 128, 32, 8.0, Eigen::Vector3f(0.8, 0.4, 0.0)));
    inlay->rings[1].set_center(1.5, 0.0);
    Eigen::Matrix4f rot = Eigen::Matrix4f::Identity();
    rot(1, 1) = cos(M_PI * 0.25);
    rot(2, 2) = cos(M_PI * 0.25);
    rot(1, 2) = -sin(M_PI * 0.25);
    rot(2, 1) = sin(M_PI * 0.25);
    inlay->rings[1].set_rotation(rot);
    inlay->rings.push_back(
        Torus(1.2, 0.4, 128, 32, 8.0, Eigen::Vector3f(0.8, 0.4, 0.2)));
    inlay->rings[2].set_center(5.5, 0.0);
    rot.setIdentity();
    rot(1, 1) = cos(M_PI * -0.25);
    rot(2, 2) = cos(M_PI * -0.25);
    rot(1, 2) = -sin(M_PI * -0.25);
    rot(2, 1) = sin(M_PI * -0.25);
    inlay->rings[2].set_rotation(rot);

    inlay->ambientIntensity = 0.5;
    inlay->ringsModified = true;

    return inlay;
}
