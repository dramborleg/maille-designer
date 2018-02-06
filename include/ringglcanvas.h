#pragma once

#include <nanogui/glcanvas.h>

#include "torus.h"

class RingGLCanvas : public nanogui::GLCanvas
{
public:
    RingGLCanvas(Widget *parent);
    ~RingGLCanvas() { mShader.free(); }

    void setAmbientIntensityFactor(float iFactor) { aiFactor = iFactor; }
    float getAmbientIntensityFactor() const { return aiFactor; }
    virtual bool mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                  bool down, int modifiers) override;
    virtual bool mouseDragEvent(const Eigen::Vector2i &p,
                                const Eigen::Vector2i &rel, int button,
                                int modifiers) override;
    virtual bool keyboardEvent(int key, int scancode, int action,
                               int modifiers) override;

    virtual void drawGL() override;

private:
    nanogui::GLShader mShader;
    // rings to draw
    std::vector<Torus> rings;
    // direction for our directional light
    Eigen::Vector3f lDirection;
    // ambient, diffuse, and specular light intensities
    float aIntensity, dIntensity, sIntensity;
    float aiFactor;
};
