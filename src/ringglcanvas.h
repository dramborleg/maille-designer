#pragma once

#include <Eigen/Core>
#include <nanogui/glcanvas.h>

#include "mailleinlay.h"

class Tool;

class RingGLCanvas : public nanogui::GLCanvas
{
public:
    RingGLCanvas(Widget *parent, std::shared_ptr<MailleInlay> inlay,
                 std::shared_ptr<Tool> tool);
    ~RingGLCanvas() { mShader.free(); }

    void resetState(std::shared_ptr<Tool> tool);
    void applyViewTransformation(Eigen::Matrix4f transform);
    void setTool(std::shared_ptr<Tool> t);
    virtual bool mouseButtonEvent(const Eigen::Vector2i &p, int button,
                                  bool down, int modifiers) override;
    virtual bool mouseMotionEvent(const Eigen::Vector2i &p,
                                  const Eigen::Vector2i &rel, int button,
                                  int modifiers) override;
    virtual bool mouseDragEvent(const Eigen::Vector2i &p,
                                const Eigen::Vector2i &rel, int button,
                                int modifiers) override;
    virtual bool keyboardEvent(int key, int scancode, int action,
                               int modifiers) override;
    virtual bool scrollEvent(const Eigen::Vector2i &p,
                             const Eigen::Vector2f &rel) override;
    void resize(const Eigen::Vector2i &size);

    virtual void drawGL() override;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    void uploadRingData();
    Eigen::Vector2f canvasToWorld(const Eigen::Vector2i &p);
    void setZoom();

    nanogui::GLShader mShader;
    // direction for our directional light
    Eigen::Vector3f lDirection;
    // ambient, diffuse, and specular light intensities
    float aIntensity, dIntensity, sIntensity;
    // zoom level
    float zoom;
    // the inlay being displayed on this canvas
    std::shared_ptr<MailleInlay> inlay;
    // currently selected tool
    std::shared_ptr<Tool> tool;
};
