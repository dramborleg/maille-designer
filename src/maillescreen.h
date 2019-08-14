#pragma once

#include <unordered_map>

#include <nanogui/screen.h>

#include "common.h"
#include "mailleinlay.h"

class ColorPickerTool;
class ColorSelectTool;
class OptionalPopupButton;
class PaintTool;
class RingGLCanvas;
class SelectionTool;
class Tool;
class TranslationTool;
class Weave;
class WeaveAddTool;

class MailleScreen : public nanogui::Screen
{
public:
    MailleScreen();

    bool resizeEvent(const Eigen::Vector2i &size) override;
    bool keyboardEvent(int key, int scancode, int action,
                       int modifiers) override;

private:
    void simulateButtonClick(nanogui::Button *b);
    void setTool(std::shared_ptr<Tool> tool);
    OptionalPopupButton *addToolButton(nanogui::Widget *parent,
                                       std::shared_ptr<Tool> tool,
                                       const std::string &tooltip);
    std::unordered_map<Maille::Color, unsigned, Maille::ColorHash>
        getColorReport() const;
    void exportColorReport();
    void importImage();
    void exportPOV() const;
    void saveFile() const;
    void loadFile();

    //indicates if the shift key is pressed for keyboard input
    bool shift_key;
    RingGLCanvas *mCanvas;
    // list of all the tool radio buttons so they can be reset on file load
    std::vector<nanogui::Button *> toolButtons;
    // weave manager used by certain tools as well as for creating save files
    std::shared_ptr<Weave> weaveManager;
    // weave settings window used by weaveManager
    nanogui::Window *weaveSettings = nullptr;
    // Global inlay state
    std::shared_ptr<MailleInlay> inlay;
    // Tools
    std::shared_ptr<ColorPickerTool> colorPickerTool;
    std::shared_ptr<ColorSelectTool> colorSelectTool;
    std::shared_ptr<WeaveAddTool> adderTool;
    std::shared_ptr<SelectionTool> selectionTool;
    std::shared_ptr<TranslationTool> translationTool;
    std::shared_ptr<PaintTool> paintTool;
    std::shared_ptr<Tool> curTool;
    // Tool buttons
    OptionalPopupButton *colorPickerButton;
    OptionalPopupButton *colorSelectButton;
    OptionalPopupButton *adderButton;
    OptionalPopupButton *selectionButton;
    OptionalPopupButton *translationButton;
    OptionalPopupButton *paintButton;
    // Global foreground color
    std::shared_ptr<Maille::Color> fgcolor;
};
