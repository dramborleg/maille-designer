#include <cpptoml.h>
#include <fstream>
#include <math.h>

#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/messagedialog.h>
#include <nanogui/nanogui.h>
#include <nanogui/screen.h>
#include <nanogui/slider.h>
#include <nanogui/window.h>

#include "common.h"
#include "maillescreen.h"
#include "ringglcanvas.h"
#include "tools/colorpickertool.h"
#include "tools/painttool.h"
#include "tools/selectiontool.h"
#include "tools/translationtool.h"
#include "tools/weaveaddtool.h"
#include "weaves/european4in1.h"

MailleScreen::MailleScreen()
    : nanogui::Screen(Eigen::Vector2i(1200, 900), "Maille Designer", true)
{
    using namespace nanogui;

    // Initialize inlay
    inlay = std::make_shared<MailleInlay>();
    inlay->ambientIntensity = 0.5;

    // Initialize foreground color
    fgcolor = std::make_shared<Maille::Color>(255, 0, 0);

    // Initialize selection and adder tools
    weaveSettings = new Window(this, "Weave Settings");
    weaveManager = std::make_shared<European4in1>(weaveSettings, inlay);
    adderTool =
        std::make_shared<WeaveAddTool>(ENTYPO_ICON_PLUS, weaveManager, fgcolor);
    selectionTool = std::make_shared<SelectionTool>(ENTYPO_ICON_MOUSE_POINTER,
                                                    weaveManager);
    translationTool = std::make_shared<TranslationTool>(ENTYPO_ICON_HAND);
    paintTool = std::make_shared<PaintTool>(ENTYPO_ICON_ROUND_BRUSH, fgcolor);
    curTool = adderTool;

    // Ring Canvas
    mCanvas = new RingGLCanvas(this, inlay, curTool, translationTool);
    mCanvas->setPosition(Vector2i(200, 0));
    mCanvas->setBackgroundColor({100, 100, 100, 255});
    mCanvas->resize({width() - 200, height()});

    // Tool Palette and Widget Window
    Window *palette = new Window(this, "Tools");
    palette->setPosition(Vector2i(0, 0));
    palette->setLayout(new GroupLayout());
    // Palette label and widget for containing buttons
    new Label(palette, "Tools", "sans-bold");
    Widget *toolsWidget = new Widget(palette);
    toolsWidget->setLayout(new GridLayout(Orientation::Horizontal, 4));

    // Tool buttons
    adderButton = addToolButton(toolsWidget, adderTool, "'A'dd Rings");
    adderButton->setPushed(true);
    selectionButton =
        addToolButton(toolsWidget, selectionTool, "'S'elect Rings");
    translationButton =
        addToolButton(toolsWidget, translationTool, "'M'ove Camera View");
    paintButton = addToolButton(toolsWidget, paintTool, "'B'rush: Paint Rings");

    // Brightness slider widget
    new Label(palette, "Brightness Slider", "sans-bold");
    Slider *brightness = new Slider(palette);
    brightness->setTooltip("Set ambient brightness multiplier");
    brightness->setValue(inlay->ambientIntensity);
    brightness->setCallback(
        [this](float val) { inlay->ambientIntensity = val; });
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
    colorPickerButton =
        addToolButton(toolsWidget, colorPickerTool, "'C'olor Picker");

    // put tool buttons in same radio button group
    adderButton->setButtonGroup(toolButtons);
    selectionButton->setButtonGroup(toolButtons);
    translationButton->setButtonGroup(toolButtons);
    paintButton->setButtonGroup(toolButtons);
    colorPickerButton->setButtonGroup(toolButtons);

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

    // export color report button
    Button *colorReport = new Button(palette, "Color Report", 0);
    colorReport->setTooltip("Export Ring Color Counts");
    colorReport->setCallback([this]() { exportColorReport(); });

    // save file and load file buttons
    Button *saveFileButton = new Button(palette, "Save", ENTYPO_ICON_SAVE);
    saveFileButton->setCallback([this]() { saveFile(); });
    Button *loadFileButton = new Button(palette, "Load", ENTYPO_ICON_FOLDER);
    loadFileButton->setCallback([this]() { loadFile(); });

    performLayout();
    weaveSettings->setPosition(Vector2i(0, palette->height() + 10));
}

bool MailleScreen::resizeEvent(const Eigen::Vector2i &size)
{
    auto position = mCanvas->position();
    mCanvas->resize({size(0) - position(0), size(1)});
    return true;
}

bool MailleScreen::keyboardEvent(int key, int scancode, int action,
                                 int modifiers)
{
    if (mCanvas->keyboardEvent(key, scancode, action, modifiers))
        return true;

    bool ret = false;

    if (key == GLFW_KEY_A && action)
        ret = simulateButtonClick(adderButton);
    else if (key == GLFW_KEY_S && action)
        ret = simulateButtonClick(selectionButton);
    else if (key == GLFW_KEY_B && action)
        ret = simulateButtonClick(paintButton);
    else if (key == GLFW_KEY_C && action)
        ret = simulateButtonClick(colorPickerButton);
    else if (key == GLFW_KEY_M && action)
        ret = simulateButtonClick(translationButton);

    return ret;
}

bool MailleScreen::simulateButtonClick(nanogui::Button *b)
{
    auto siblings = b->buttonGroup();
    b->callback()();
    for (auto &s : siblings)
        s->setPushed(false);
    b->setPushed(true);

    return true;
}

void MailleScreen::setTool(std::shared_ptr<Tool> tool)
{
    mCanvas->setTool(tool);
    curTool = tool;
}

nanogui::Button *MailleScreen::addToolButton(nanogui::Widget *parent,
                                             std::shared_ptr<Tool> tool,
                                             const std::string &tooltip)
{
    nanogui::Button *b = new nanogui::Button(parent, "", tool->getIcon());
    b->setFlags(nanogui::Button::RadioButton);
    b->setTooltip(tooltip);
    b->setCallback([this, tool]() { setTool(tool); });
    toolButtons.push_back(b);

    return b;
}

void MailleScreen::exportColorReport() const
{
    std::unordered_map<Maille::Color, unsigned, Maille::ColorHash> hist;
    std::string report;
    std::string fpath = nanogui::file_dialog({{"txt", "Textfile"}}, true);

    if (fpath == "")
        return;

    if (fpath.size() < 4 || fpath.substr(fpath.size() - 4, 4) != ".txt")
        fpath += ".txt";

    for (const std::shared_ptr<Torus> &ring : inlay->rings)
    {
        Maille::Color c = ring->get_color();
        if (hist.count(c))
            hist[c] += 1;
        else
            hist[c] = 1;
    }

    report = "r, g, b: count";
    for (auto i = hist.begin(); i != hist.end(); i++)
        report += "\n" + std::to_string(i->first(0)) + ", " +
                  std::to_string(i->first(1)) + ", " +
                  std::to_string(i->first(2)) + ": " +
                  std::to_string(i->second);

    std::ofstream f(fpath);
    f.write(report.data(), report.size());
}

void MailleScreen::saveFile() const
{
    std::string fpath =
        nanogui::file_dialog({{"midf", "Maille Inlay Designer File"}}, true);

    if (fpath == "")
        return;

    if (fpath.size() < 5 || fpath.substr(fpath.size() - 5, 5) != ".midf")
        fpath += ".midf";

    auto saveContents = weaveManager->generateSaveFile(*inlay);
    std::ofstream f(fpath);
    f << *saveContents;
}

void MailleScreen::loadFile()
{
    std::string fpath =
        nanogui::file_dialog({{"midf", "Maille Inlay Designer File"}}, false);

    if (fpath == "")
        return;

    std::shared_ptr<cpptoml::table> design = cpptoml::parse_file(fpath);
    auto weave = design->get_as<std::string>("WeaveID");
    if (!weave)
    {
        new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Warning,
                                   "File Error",
                                   "File does not appear to be a Maille Inlay "
                                   "Designer File, aborting file load.");
        return;
    }

    // first figure out weave so we can ensure tools have proper weave manager
    if (*weave != European4in1::getWeaveID())
    {
        new nanogui::MessageDialog(
            this, nanogui::MessageDialog::Type::Warning, "Weave Error",
            "Weave type " + *weave + " is not supported, aborting file load.");
        return;
    }

    // If we call performLayout() in this class twice, then for some reason the
    // tools palette gets wider after the 2nd call (but not after the third
    // call). To prevent this happening, we try to avoid calling performLayout
    // here while still giving the opportunity to reset the weaveSettings
    // elements, which we do by deleting all children of weaveSettings, letting
    // the weave add its new elements, and then calling performLayout
    // specifically on weaveSettings (rather than on this).
    int nChildren = weaveSettings->childCount();
    for (int i = 0; i < nChildren; i++)
        weaveSettings->removeChild(0);
    weaveManager = std::make_shared<European4in1>(weaveSettings, inlay);
    weaveSettings->performLayout(this->nvgContext());
    adderTool->setWeaveManager(weaveManager);
    selectionTool->setWeaveManager(weaveManager);

    mCanvas->resetState();
    weaveManager->importSaveFile(this, design, *inlay);
}
