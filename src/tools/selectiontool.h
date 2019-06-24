#pragma once

#include "common.h"
#include "tool.h"
#include "weaves/weave.h"

class SelectionTool : public Tool
{
public:
    SelectionTool(int icon, std::shared_ptr<Weave> weaveManager);

    void load(MailleInlay &inlay) override;
    void unload(MailleInlay &inlay) override;
    virtual bool mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                  bool down, int modifiers,
                                  const Eigen::Vector2f &worldPos,
                                  MailleInlay &inlay) override;
    virtual bool mouseDragEvent(const Eigen::Vector2i &p,
                                const Eigen::Vector2i &rel, int button,
                                int modifiers, const Eigen::Vector2f &worldPos,
                                MailleInlay &inlay) override;
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers,
                               MailleInlay &inlay) override;
    void deleteSelection(MailleInlay &inlay);
    void setSelectionColor(MailleInlay &inlay, const Maille::Color &color);
    void setWeaveManager(std::shared_ptr<Weave> weaveManager);

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:
    void setSelection(MailleInlay &inlay, bool selected);

    bool ctrlDown = false;

private:
    bool completeDragEvent(const Eigen::Vector2f &worldPos, MailleInlay &inlay);
    bool coordinateInBox(const Eigen::Vector2f &p0, const Eigen::Vector2f &p1,
                         const Eigen::Vector2f &location);

    bool dragEvent = false;
    // initial click position of a drag event
    Eigen::Vector2f dragBeginWorld;
    // location of cursor during most recent drag event that was processed
    Eigen::Vector2f dragPrevWorld;
    std::shared_ptr<Weave> weaveManager;
};
