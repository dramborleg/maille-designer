#pragma once

#include "common.h"
#include "tool.h"
#include "weaves/weave.h"

class SelectionTool : public Tool
{
public:
    SelectionTool(int icon, std::shared_ptr<Weave> weaveManager);

    void load(MailleInlay &inlay);
    void unload(MailleInlay &inlay);
    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay);
    bool mouseDragEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel,
                        int button, int modifiers,
                        const Eigen::Vector2f &worldPos, MailleInlay &inlay);
    bool keyboardEvent(int key, int scancode, int action, int modifiers,
                       MailleInlay &inlay);
    void deleteSelection(MailleInlay &inlay);
    void setSelectionColor(MailleInlay &inlay, const Maille::Color &color);

private:
    std::pair<std::shared_ptr<Torus>, float>
    findNearestRing(const MailleInlay &inlay,
                    const Eigen::Vector2f &worldPos) const;

    bool handleDragEvent(const Eigen::Vector2f &worldPos, MailleInlay &inlay);
    void setSelection(MailleInlay &inlay, bool selected);

    bool dragEvent;
    Eigen::Vector2f dragBeginWorld;
    std::shared_ptr<Weave> weaveManager;
    bool ctrlDown;
};
