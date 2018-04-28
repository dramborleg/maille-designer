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
#include "weaves/european4in1.h"

MailleScreen::MailleScreen()
    : nanogui::Screen(Eigen::Vector2i(800, 600), "Maille Designer", false)
{
    using namespace nanogui;

    // Initialize inlay
    inlay = std::make_shared<MailleInlay>();
    inlay->ambientIntensity = 0.5;

    // Initialize foreground color
    fgcolor = std::make_shared<Eigen::Vector3f>(1.0, 0.0, 0.0);

    // Initialize tools
    auto weaveManager = std::make_shared<European4in1>();
    adderTool =
        std::make_shared<WeaveAddTool>(ENTYPO_ICON_PLUS, weaveManager, fgcolor);
    selectionTool = std::make_shared<SelectionTool>(ENTYPO_ICON_MOUSE_POINTER,
                                                    weaveManager);

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
    // Global foreground color selector
    ColorWheel *wheel = new ColorWheel(palette);
    wheel->setCallback([this](const Color &col) { *fgcolor = col.head(3); });

    // Tool specific buttons
    new Label(palette, "Tool operations", "sans-bold");
    Widget *toolOperations = new Widget(palette);
    toolOperations->setLayout(
        new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 0));
    // Selection tool delete button
    Button *selectionToolDelete =
        new Button(toolOperations, "", ENTYPO_ICON_TRASH);
    selectionToolDelete->setTooltip("Delete Selection");
    selectionToolDelete->setCallback(
        [this]() { selectionTool->deleteSelection(*inlay); });
    // Selection tool set color button
    Button *selectionToolColor =
        new Button(toolOperations, "", ENTYPO_ICON_PALETTE);
    selectionToolColor->setTooltip("Set Selection Color");
    selectionToolColor->setCallback(
        [this]() { selectionTool->setSelectionColor(*inlay, *fgcolor); });

    performLayout();
}

void MailleScreen::draw(NVGcontext *ctx)
{
    /* Draw the user interface */
    Screen::draw(ctx);
}
