#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/nanogui.h>
#include <nanogui/screen.h>
#include <nanogui/slider.h>
#include <nanogui/window.h>

#include "maillescreen.h"
#include "ringglcanvas.h"
#include "tools/selectiontool.h"
#include "tools/weaveaddtool.h"

MailleScreen::MailleScreen()
    : nanogui::Screen(Eigen::Vector2i(800, 600), "Maille Designer", false)
{
    using namespace nanogui;

    inlay = std::shared_ptr<MailleInlay>(new MailleInlay);
    inlay->ambientIntensity = 0.5;
    inlay->ringsModified = true;

    // Initialize tools
    adderTool = std::make_shared<WeaveAddTool>(ENTYPO_ICON_PLUS);
    selectionTool = std::make_shared<SelectionTool>(ENTYPO_ICON_MOUSE_POINTER);

    // Ring Canvas
    mCanvas = new RingGLCanvas(this, inlay, adderTool);
    mCanvas->setPosition(Vector2i(200, 0));
    mCanvas->setBackgroundColor({100, 100, 100, 255});
    mCanvas->setSize({600, 600});

    // Tool Palette and Widget Window
    Window *palette = new Window(this, "Tools");
    palette->setPosition(Vector2i(0, 0));
    palette->setLayout(new GroupLayout());
    // Palette label and widget for containing buttons
    new Label(palette, "Tools", "sans-bold");
    Widget *toolsWidget = new Widget(palette);
    toolsWidget->setLayout(
        new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 0));
    // Ring adder tool button
    Button *b = new Button(toolsWidget, "", adderTool->getIcon());
    b->setFlags(Button::RadioButton);
    b->setTooltip("Add Rings");
    b->setPushed(true);
    b->setCallback([this]() { mCanvas->setTool(adderTool); });
    // Ring selection tool button
    b = new Button(toolsWidget, "", selectionTool->getIcon());
    b->setFlags(Button::RadioButton);
    b->setTooltip("Select Rings");
    b->setCallback([this]() { mCanvas->setTool(selectionTool); });
    // Brightness slider widget
    new Label(palette, "Brightness Slider", "sans-bold");
    Slider *brightness = new Slider(palette);
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
        std::shared_ptr<Torus>(new Torus(Eigen::Vector3f(0.8, 0.0, 0.0))));
    inlay->rings[0]->set_center(-2.5, 0.0);
    inlay->rings.push_back(
        std::shared_ptr<Torus>(new Torus(Eigen::Vector3f(0.0, 0.8, 0.0))));
    inlay->rings[1]->set_center(1.5, 0.0);
    Eigen::Matrix4f rot = Eigen::Matrix4f::Identity();
    rot(1, 1) = cos(M_PI * 0.25);
    rot(2, 2) = cos(M_PI * 0.25);
    rot(1, 2) = -sin(M_PI * 0.25);
    rot(2, 1) = sin(M_PI * 0.25);
    inlay->rings[1]->set_rotation(rot);
    inlay->rings.push_back(
        std::shared_ptr<Torus>(new Torus(Eigen::Vector3f(0.0, 0.0, 0.8))));
    inlay->rings[2]->set_center(5.5, 0.0);
    rot.setIdentity();
    rot(1, 1) = cos(M_PI * -0.25);
    rot(2, 2) = cos(M_PI * -0.25);
    rot(1, 2) = -sin(M_PI * -0.25);
    rot(2, 1) = sin(M_PI * -0.25);
    inlay->rings[2]->set_rotation(rot);

    inlay->ambientIntensity = 0.5;
    inlay->ringsModified = true;

    return inlay;
}
