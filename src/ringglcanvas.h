#pragma once

#include <Eigen/Core>
#include <nanogui/glcanvas.h>

#include "mailleinlay.h"

class Tool;
class TranslationTool;

class RingGLCanvas : public nanogui::GLCanvas
{
public:
    RingGLCanvas(Widget *parent, std::shared_ptr<MailleInlay> inlay,
                 std::shared_ptr<Tool> tool,
                 std::shared_ptr<TranslationTool> translate,
                 nanogui::Slider *zoomSensitivity);
    ~RingGLCanvas() { mShader.free(); }

    void resetState();
    void applyViewTransformation(const Eigen::Matrix4f &transform);
    void setTool(std::shared_ptr<Tool> t);
    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down,
                          int modifiers) override;
    bool mouseMotionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel,
                          int button, int modifiers) override;
    bool mouseDragEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel,
                        int button, int modifiers) override;
    bool keyboardEvent(int key, int scancode, int action,
                       int modifiers) override;
    bool scrollEvent(const Eigen::Vector2i &p,
                     const Eigen::Vector2f &rel) override;
    void resize(const Eigen::Vector2i &size);

    void drawGL() override;

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
    // zoom level/sensitivity
    float zoom = 8.0;
    nanogui::Slider *zoomSensitivity;
    // the inlay being displayed on this canvas
    std::shared_ptr<MailleInlay> inlay;
    // currently selected tool
    std::shared_ptr<Tool> tool;
    // use translation tool for middle click + drag
    std::shared_ptr<TranslationTool> translate;
};
