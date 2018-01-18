#pragma once

#include <nanogui/glcanvas.h>

#include "torus.h"

class RingGLCanvas : public nanogui::GLCanvas
{
public:
    RingGLCanvas(Widget *parent);
    ~RingGLCanvas() { mShader.free(); }

    virtual void drawGL() override;

private:
    nanogui::GLShader mShader;
    // rings to draw
    std::vector<Torus> rings;
    // direction for our directional light
    Eigen::Vector3f lDirection;
    // ambient, diffuse, and specular light intensities
    float aIntensity, dIntensity, sIntensity;
};
