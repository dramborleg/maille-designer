#pragma once

#include <nanogui/popupbutton.h>

class OptionalPopupButton : public nanogui::PopupButton
{
public:
    OptionalPopupButton(Widget *parent, const std::string &caption = "Untitled",
                        int buttonIcon = 0)
        : nanogui::PopupButton(parent, caption, buttonIcon)
        , enable(false)
    {
        mChevronIcon = 0;
    }

    void enablePopup(bool enable) { this->enable = enable; }

    virtual void draw(NVGcontext *ctx) override
    {
        PopupButton::draw(ctx);
        if (!enable)
            mPopup->setVisible(false);
    }

    virtual Eigen::Vector2i preferredSize(NVGcontext *ctx) const override
    {
        return Button::preferredSize(ctx);
    }

protected:
    bool enable;
};
