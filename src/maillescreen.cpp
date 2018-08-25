#include <fstream>
#include <math.h>

#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/nanogui.h>
#include <nanogui/screen.h>
#include <nanogui/slider.h>
#include <nanogui/window.h>

#include "common.h"
#include "maillescreen.h"
#include "ringglcanvas.h"
#include "tools/colorpickertool.h"
#include "tools/selectiontool.h"
#include "tools/translationtool.h"
#include "tools/weaveaddtool.h"
#include "weaves/european4in1.h"

MailleScreen::MailleScreen()
    : nanogui::Screen(Eigen::Vector2i(800, 600), "Maille Designer", true)
{
    using namespace nanogui;

    // Initialize inlay
    inlay = std::make_shared<MailleInlay>();
    inlay->ambientIntensity = 0.5;

    // Initialize foreground color
    fgcolor = std::make_shared<Maille::Color>(255, 0, 0);

    // Initialize selection and adder tools
    auto weaveManager = std::make_shared<European4in1>();
    adderTool =
        std::make_shared<WeaveAddTool>(ENTYPO_ICON_PLUS, weaveManager, fgcolor);
    selectionTool = std::make_shared<SelectionTool>(ENTYPO_ICON_MOUSE_POINTER,
                                                    weaveManager);
    translationTool = std::make_shared<TranslationTool>(ENTYPO_ICON_HAND);
    curTool = adderTool;

    // Ring Canvas
    mCanvas = new RingGLCanvas(this, inlay, curTool);
    mCanvas->setPosition(Vector2i(200, 0));
    mCanvas->setBackgroundColor({100, 100, 100, 255});
    mCanvas->resize({600, 600});

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
    b->setCallback([this]() {
        mCanvas->setTool(adderTool);
        curTool = adderTool;
    });
    // Ring selection tool button
    b = new Button(toolsWidget, "", selectionTool->getIcon());
    b->setFlags(Button::RadioButton);
    b->setTooltip("Select Rings");
    b->setCallback([this]() {
        mCanvas->setTool(selectionTool);
        curTool = selectionTool;
    });
    // View translation tool button
    b = new Button(toolsWidget, "", translationTool->getIcon());
    b->setFlags(Button::RadioButton);
    b->setTooltip("Move Camera View");
    b->setCallback([this]() {
        mCanvas->setTool(translationTool);
        curTool = translationTool;
    });
    // Brightness slider widget
    new Label(palette, "Brightness Slider", "sans-bold");
    Slider *brightness = new Slider(palette);
    brightness->setTooltip("Set ambient brightness multiplier");
    brightness->setValue(mCanvas->getAmbientIntensityFactor());
    brightness->setCallback(
        [this](float val) { mCanvas->setAmbientIntensityFactor(val); });
    // Global foreground color selector
    ColorWheel *wheel = new ColorWheel(palette);
    wheel->setCallback([this](const Color &col) {
        Maille::Color color(round(255 * col(0)), round(255 * col(1)),
                            round(255 * col(2)));
        *fgcolor = color;
    });

    // Initialize color picker tool and button
    colorPickerTool =
        std::make_shared<ColorPickerTool>(ENTYPO_ICON_DROP, wheel);
    b = new Button(toolsWidget, "", colorPickerTool->getIcon());
    b->setFlags(Button::RadioButton);
    b->setTooltip("Select Color Based on Existing Ring");
    b->setCallback([this]() {
        mCanvas->setTool(colorPickerTool);
        curTool = colorPickerTool;
    });

    // Tool specific buttons
    new Label(palette, "Tool operations", "sans-bold");
    Widget *toolOperations = new Widget(palette);
    toolOperations->setLayout(
        new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 0));
    // Selection tool delete button
    Button *selectionToolDelete =
        new Button(toolOperations, "", ENTYPO_ICON_TRASH);
    selectionToolDelete->setTooltip("Delete Selection");
    selectionToolDelete->setCallback([this]() {
        if (curTool == selectionTool)
            selectionTool->deleteSelection(*inlay);
    });
    // Selection tool set color button
    Button *selectionToolColor =
        new Button(toolOperations, "", ENTYPO_ICON_PALETTE);
    selectionToolColor->setTooltip("Set Selection Color");
    selectionToolColor->setCallback([this]() {
        if (curTool == selectionTool)
            selectionTool->setSelectionColor(*inlay, *fgcolor);
    });
    // View rotation buttons
    Button *rotateClockwise = new Button(toolOperations, "", ENTYPO_ICON_CW);
    rotateClockwise->setTooltip("Rotate Clockwise");
    rotateClockwise->setCallback([this]() {
        Eigen::Matrix4f rot = Eigen::Matrix4f::Identity();
        rot(0, 0) = rot(1, 1) = 0.0;
        rot(1, 0) = -1.0;
        rot(0, 1) = 1.0;
        mCanvas->applyViewTransformation(rot);
    });
    Button *rotateCounterClockwise =
        new Button(toolOperations, "", ENTYPO_ICON_CCW);
    rotateCounterClockwise->setTooltip("Rotate Counter Clockwise");
    rotateCounterClockwise->setCallback([this]() {
        Eigen::Matrix4f rot = Eigen::Matrix4f::Identity();
        rot(0, 0) = rot(1, 1) = 0.0;
        rot(0, 1) = -1.0;
        rot(1, 0) = 1.0;
        mCanvas->applyViewTransformation(rot);
    });

    Button *colorReport = new Button(palette, "Color Report", 0);
    colorReport->setTooltip("Export Ring Color Counts");
    colorReport->setCallback([this]() { exportColorReport(); });

    performLayout();
}

bool MailleScreen::resizeEvent(const Eigen::Vector2i &size)
{
    auto position = mCanvas->position();
    mCanvas->resize({size(0) - position(0), size(1)});
    return true;
}

void MailleScreen::exportColorReport() const
{
    std::string fpath = nanogui::file_dialog({{"txt", "Textfile"}}, true);
    std::string report = mCanvas->getColorReport();

    fpath += ".txt";
    std::ofstream f(fpath);
    f.write(report.data(), report.size());
}
