#pragma once

#include <nanogui/screen.h>

#include "common.h"
#include "mailleinlay.h"

class ColorPickerTool;
class RingGLCanvas;
class SelectionTool;
class Tool;
class TranslationTool;
class WeaveAddTool;

class MailleScreen : public nanogui::Screen
{
public:
    MailleScreen();

    bool resizeEvent(const Eigen::Vector2i &size);

private:
    void exportColorReport() const;

    RingGLCanvas *mCanvas;
    // Global inlay state
    std::shared_ptr<MailleInlay> inlay;
    // Tools
    std::shared_ptr<ColorPickerTool> colorPickerTool;
    std::shared_ptr<WeaveAddTool> adderTool;
    std::shared_ptr<SelectionTool> selectionTool;
    std::shared_ptr<TranslationTool> translationTool;
    std::shared_ptr<Tool> curTool;
    // Global foreground color
    std::shared_ptr<Maille::Color> fgcolor;
};
