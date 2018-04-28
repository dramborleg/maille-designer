#pragma once

#include "tool.h"
#include "weaves/weave.h"

class SelectionTool : public Tool
{
public:
    SelectionTool(int icon, std::shared_ptr<Weave> weaveManager);

    void unload(MailleInlay &inlay);
    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers, const Eigen::Vector2f &worldPos,
                          MailleInlay &inlay);
    bool keyboardEvent(int key, int scancode, int action, int modifiers,
                       MailleInlay &inlay);

private:
    std::shared_ptr<Weave> weaveManager;
    bool ctrlDown;
};
