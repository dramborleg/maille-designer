#include <cpptoml.h>
#include <fstream>
#include <math.h>

#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/messagedialog.h>
#include <nanogui/nanogui.h>
#include <nanogui/screen.h>
#include <nanogui/slider.h>
#include <nanogui/textbox.h>
#include <nanogui/window.h>

#include "common.h"
#include "maillescreen.h"
#include "nanogui/optionalpopupbutton.h"
#include "ringglcanvas.h"
#include "tools/colorpickertool.h"
#include "tools/colorselecttool.h"
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

    // Tool Palette and Widget Window
    Window *palette = new Window(this, "Tools");
    palette->setPosition(Vector2i(0, 0));
    palette->setLayout(new GroupLayout());
    // Palette label and widget for containing buttons
    new Label(palette, "Tools", "sans-bold");

    // Tool palette widget
    Widget *toolsWidget = new Widget(palette);
    toolsWidget->setLayout(new GridLayout(Orientation::Horizontal, 4));

    // Brightness slider widget
    new Label(palette, "Brightness Slider", "sans-bold");
    Slider *brightness = new Slider(palette);
    brightness->setTooltip("Set ambient brightness multiplier");
    brightness->setValue(inlay->ambientIntensity);
    brightness->setCallback(
        [this](float val) { inlay->ambientIntensity = val; });
    // zoom sensitivity slider to widget
    new Label(palette, "Zoom Sensitivity", "sans-bold");
    Slider *zoom = new Slider(palette);
    zoom->setTooltip("How quickly zoom changes when scrolling");
    zoom->setValue(2);
    zoom->setRange({0.3, 6});
    // Global foreground color selector
    ColorWheel *wheel = new ColorWheel(palette);
    wheel->setCallback([this](const Color &col) {
        Maille::Color color(round(255 * col(0)), round(255 * col(1)),
                            round(255 * col(2)));
        *fgcolor = color;
    });

    // Tool buttons

    // Add tool button
    adderButton = addToolButton(toolsWidget, adderTool, "'A'dd Rings");
    adderButton->setPushed(true);

    // Selection button
    selectionButton =
        addToolButton(toolsWidget, selectionTool, "'S'elect Rings");
    selectionButton->enablePopup(true);
    Popup *sPopup = selectionButton->popup();
    sPopup->setLayout(new GroupLayout());
    // Selection tool delete button
    Button *selectionToolDelete = new Button(sPopup, "", ENTYPO_ICON_TRASH);
    selectionToolDelete->setTooltip("Delete Selection");
    selectionToolDelete->setCallback(
        [this]() { selectionTool->deleteSelection(*inlay); });
    Button *selectionToolColor = new Button(sPopup, "", ENTYPO_ICON_PALETTE);
    selectionToolColor->setTooltip("Set Selection Color");
    selectionToolColor->setCallback(
        [this]() { selectionTool->setSelectionColor(*inlay, *fgcolor); });

    // translate and paint buttons
    translationButton =
        addToolButton(toolsWidget, translationTool, "'M'ove Camera View");
    paintButton = addToolButton(toolsWidget, paintTool, "'B'rush: Paint Rings");

    // color picker tool and button
    colorPickerTool =
        std::make_shared<ColorPickerTool>(ENTYPO_ICON_DROP, wheel);
    colorPickerButton =
        addToolButton(toolsWidget, colorPickerTool, "'C'olor Picker");

    // fuzzy color select button
    Slider *threshold = new Slider(nullptr);
    CheckBox *adjColors = new CheckBox(nullptr, "Require adjacency");
    colorSelectTool = std::make_shared<ColorSelectTool>(
        ENTYPO_ICON_BOWL, weaveManager, threshold, adjColors);
    colorSelectButton =
        addToolButton(toolsWidget, colorSelectTool, "Select by Color");
    colorSelectButton->enablePopup(true);
    sPopup = colorSelectButton->popup();
    sPopup->setLayout(new GroupLayout());
    // sPopup buttons widget
    Widget *sPButtons = new Widget(sPopup);
    sPButtons->setLayout(
        new BoxLayout(Orientation::Horizontal, Alignment::Minimum));
    // color select tool buttons
    Button *colorSelToolDelete = new Button(sPButtons, "", ENTYPO_ICON_TRASH);
    colorSelToolDelete->setTooltip("Delete Selection");
    colorSelToolDelete->setCallback(
        [this]() { colorSelectTool->deleteSelection(*inlay); });
    Button *colorSelToolColor = new Button(sPButtons, "", ENTYPO_ICON_PALETTE);
    colorSelToolColor->setTooltip("Set Selection Color");
    colorSelToolColor->setCallback(
        [this]() { colorSelectTool->setSelectionColor(*inlay, *fgcolor); });
    // color select tool distance slider/textbox
    new Label(sPopup, "Threshold", "sans-bold");
    sPopup->addChild(threshold);
    threshold->setValue(5);
    threshold->setRange({0, 255});
    TextBox *thresholdText = new TextBox(sPopup);
    thresholdText->setValue(std::to_string((int)threshold->value()));
    threshold->setCallback([thresholdText](float val) {
        thresholdText->setValue(std::to_string((int)val));
    });
    // color select tool require adjacency checkbox
    sPopup->addChild(adjColors);
    adjColors->setTooltip("Selects only adjacent chunks of rings, rings which "
                          "are not part of the selected island are excluded");

    // put tool buttons in same radio button group
    adderButton->setButtonGroup(toolButtons);
    selectionButton->setButtonGroup(toolButtons);
    translationButton->setButtonGroup(toolButtons);
    paintButton->setButtonGroup(toolButtons);
    colorPickerButton->setButtonGroup(toolButtons);
    colorSelectButton->setButtonGroup(toolButtons);

    // export color report button
    Button *colorReport = new Button(palette, "Color Report", 0);
    colorReport->setTooltip("Export Ring Color Counts");
    colorReport->setCallback([this]() { exportColorReport(); });

    // import image button
    Button *importImageButton = new Button(palette, "Import Image", 0);
    importImageButton->setTooltip("Create a design from an input image");
    importImageButton->setCallback([this]() { importImage(); });

    // export POV button
    Button *exportPOVButton = new Button(palette, "Export POV", 0);
    exportPOVButton->setCallback([this]() { exportPOV(); });

    // save file and load file buttons
    Button *saveFileButton = new Button(palette, "Save", ENTYPO_ICON_SAVE);
    saveFileButton->setCallback([this]() { saveFile(); });
    Button *loadFileButton = new Button(palette, "Load", ENTYPO_ICON_FOLDER);
    loadFileButton->setCallback([this]() { loadFile(); });

    // Ring Canvas
    mCanvas = new RingGLCanvas(this, inlay, curTool, translationTool, zoom);
    mCanvas->setPosition(Vector2i(200, 0));
    mCanvas->setBackgroundColor({100, 100, 100, 255});
    mCanvas->resize({width() - 200, height()});

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


    if(key == GLFW_KEY_RIGHT_SHIFT || key== GLFW_KEY_LEFT_SHIFT)
        shift_key = action == GLFW_PRESS;

    if (key == GLFW_KEY_A && action)
        simulateButtonClick(adderButton);
    else if (key== GLFW_KEY_S && action && shift_key)
        simulateButtonClick(colorSelectButton);
    else if (key == GLFW_KEY_S && action)
        simulateButtonClick(selectionButton);
    else if (key == GLFW_KEY_B && action)
        simulateButtonClick(paintButton);
    else if (key == GLFW_KEY_C && action)
        simulateButtonClick(colorPickerButton);
    else if (key == GLFW_KEY_M && action)
        simulateButtonClick(translationButton);
    return Screen::keyboardEvent(key, scancode, action, modifiers);
}

void MailleScreen::simulateButtonClick(nanogui::Button *b)
{
    auto siblings = b->buttonGroup();
    b->callback()();
    for (auto &s : siblings)
        s->setPushed(false);
    b->setPushed(true);
}

void MailleScreen::setTool(std::shared_ptr<Tool> tool)
{
    mCanvas->setTool(tool);
    curTool = tool;
}

OptionalPopupButton *MailleScreen::addToolButton(nanogui::Widget *parent,
                                                 std::shared_ptr<Tool> tool,
                                                 const std::string &tooltip)
{
    OptionalPopupButton *b =
        new OptionalPopupButton(parent, "", tool->getIcon());
    b->setFlags(nanogui::Button::RadioButton);
    b->setTooltip(tooltip);
    b->setCallback([this, tool]() { setTool(tool); });
    toolButtons.push_back(b);

    return b;
}

std::unordered_map<Maille::Color, unsigned, Maille::ColorHash>
    MailleScreen::getColorReport() const
{
    std::unordered_map<Maille::Color, unsigned, Maille::ColorHash> hist;

    for (const std::shared_ptr<Torus> &ring : inlay->rings)
    {
        Maille::Color c = ring->get_color();
        if (hist.count(c))
            hist[c] += 1;
        else
            hist[c] = 1;
    }

    return hist;
}

void MailleScreen::exportColorReport()
{
    using namespace nanogui;

    auto hist = getColorReport();
    std::string report = "r, g, b: count";
    // Cap max number of colors which can (somewhat) be displayed on a 1200x900
    // screen. This means ~24 rows and ~16 color columns, and 16 color columns
    // would be equivalent to 32 color + textbox columns
    int maxRows = 24;
    int maxCols = 16 * 2;
    int ncols = 2 + 2 * (hist.size() / maxRows);
    int maxEntries = maxRows * maxCols / 2;
    bool overflow = false;

    if (ncols > maxCols)
    {
        overflow = true;
        ncols = maxCols;
    }

    Window *reportWin = new Window(this, "Color Report");
    GridLayout *layout =
        new GridLayout(Orientation::Horizontal, ncols, Alignment::Middle, 10);
    layout->setColAlignment({Alignment::Fill, Alignment::Fill});
    reportWin->setLayout(layout);
    int numEntries = 0;
    unsigned char r, g, bl;
    for (auto i = hist.begin(); i != hist.end(); i++)
    {
        r = i->first(0);
        g = i->first(1);
        bl = i->first(2);
        numEntries++;

        report += "\n" + std::to_string(r) + ", " + std::to_string(g) + ", " +
                  std::to_string(bl) + ": " + std::to_string(i->second);

        if (overflow && numEntries > maxEntries)
            continue;

        Button *b = new Button(reportWin, "");
        b->setBackgroundColor(Color(Eigen::Vector3i(r, g, bl)));
        new TextBox(reportWin, std::to_string(i->second));
    }

    Button *b = new Button(reportWin, "close");
    b->setCallback([reportWin] { reportWin->dispose(); });

    b = new Button(reportWin, "save txt");
    std::function<void()> exportText = [reportWin, report]() {
        std::string fpath = nanogui::file_dialog({{"txt", "Textfile"}}, true);

        if (fpath == "")
        {
            reportWin->dispose();
            return;
        }

        if (fpath.size() < 4 || fpath.substr(fpath.size() - 4, 4) != ".txt")
            fpath += ".txt";

        std::ofstream f(fpath);
        f.write(report.data(), report.size());
        reportWin->dispose();
    };
    b->setCallback(exportText);

    reportWin->center();
}

void MailleScreen::importImage()
{
    std::string fpath = nanogui::file_dialog(
        {{"jpg", "jpg"}, {"png", "png"}, {"gif", "gif"}, {"bmp", "bmp"}},
        false);

    if (fpath == "")
        return;

    if (!inlay->rings.empty())
    {
        auto dlg = new nanogui::MessageDialog(
            this, nanogui::MessageDialog::Type::Question, "Import from Image",
            "Importing from an image will overwrite any existing rings in the "
            "design, continue import?",
            "continue", "cancel", true);
        dlg->setCallback([this, fpath](int cancel) {
            if (!cancel)
                weaveManager->importImage(this, fpath, *inlay);
        });
    }
    else
        weaveManager->importImage(this, fpath, *inlay);
}

void MailleScreen::exportPOV() const
{
    std::string fpath = nanogui::file_dialog({{"pov", ""}}, true);

    if (fpath == "")
        return;

    if (fpath.size() < 4 || fpath.substr(fpath.size() - 4, 4) != ".pov")
        fpath += ".pov";

    std::string out = weaveManager->exportPOV(*inlay);
    std::ofstream f(fpath);
    f << out;
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
    colorSelectTool->setWeaveManager(weaveManager);
    mCanvas->resetState();
    weaveManager->importSaveFile(this, design, *inlay);
}
