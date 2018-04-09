#pragma once

#include <Eigen/Core>
#include <nanogui/glcanvas.h>

#include "mailleinlay.h"

class RingGLCanvas : public nanogui::GLCanvas
{
public:
    RingGLCanvas(Widget *parent, MailleInlay inlay);
    ~RingGLCanvas() { mShader.free(); }

    void setAmbientIntensityFactor(float iFactor);
    float getAmbientIntensityFactor() const { return inlay.ambientIntensity; }
    virtual bool mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                  bool down, int modifiers) override;
    virtual bool mouseDragEvent(const Eigen::Vector2i &p,
                                const Eigen::Vector2i &rel, int button,
                                int modifiers) override;
    virtual bool keyboardEvent(int key, int scancode, int action,
                               int modifiers) override;
    virtual bool scrollEvent(const Eigen::Vector2i &p,
                             const Eigen::Vector2f &rel) override;

    virtual void drawGL() override;

private:
    nanogui::GLShader mShader;
    // model view projection matrix
    Eigen::Matrix4f mvp;
    // direction for our directional light
    Eigen::Vector3f lDirection;
    // ambient, diffuse, and specular light intensities
    float aIntensity, dIntensity, sIntensity;
    // the inlay being displayed on this canvas
    MailleInlay inlay;
};
