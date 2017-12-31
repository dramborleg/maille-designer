#include <nanogui/glcanvas.h>
#include <nanogui/screen.h>
#include <nanogui/glutil.h>
#include <nanogui/object.h>
#include <nanogui/window.h>
#include <nanogui/common.h>

#include <iostream>
#include "torus.h"
#include <string>

using std::endl;


class RingGLCanvas : public nanogui::GLCanvas {
public:
    RingGLCanvas(Widget *parent) : nanogui::GLCanvas(parent), mRotation(nanogui::Vector3f(0.25f, 0.5f, 0.33f)) {
        using namespace nanogui;

        mShader.init(
            /* An identifying name */
            "a_simple_shader",

            /* Vertex shader */
            "#version 330\n"
            "uniform mat4 modelViewProj;\n"
            "in vec3 position;\n"
            "in vec3 color;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "    frag_color = 3.0 * modelViewProj * vec4(color, 1.0);\n"
            "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
            "}",

            /* Fragment shader */
            "#version 330\n"
            "out vec4 color;\n"
            "in vec4 frag_color;\n"
            "void main() {\n"
            "    color = frag_color;\n"
            "}"
        );

        Torus t(1.2, 0.4, 32, 8);
        MatrixXf positions = t.get_positions();
        MatrixXf colors = t.get_colors();
        MatrixXu indices = t.get_indices();

        mShader.bind();
        mShader.uploadIndices(indices);

        mShader.uploadAttrib("position", positions);
        mShader.uploadAttrib("color", colors);
    }

    ~RingGLCanvas() {
        mShader.free();
    }

    void setRotation(nanogui::Vector3f vRotation) {
        mRotation = vRotation;
    }

    virtual void drawGL() override {
        using namespace nanogui;

        mShader.bind();

        Matrix4f mvp;
        mvp.setIdentity();
        float fTime = (float)glfwGetTime();
        mvp.topLeftCorner<3,3>() = 0.25f * Eigen::Matrix3f(
            Eigen::AngleAxisf(mRotation[0]*fTime, Vector3f::UnitX()) *
            Eigen::AngleAxisf(mRotation[1]*fTime,  Vector3f::UnitY()) *
            Eigen::AngleAxisf(mRotation[2]*fTime, Vector3f::UnitZ()));

        mShader.setUniform("modelViewProj", mvp);

        glEnable(GL_DEPTH_TEST);
        /* Draw 12 triangles starting at index 0 */
        mShader.drawIndexed(GL_TRIANGLES, 0, 16 * 32);
        glDisable(GL_DEPTH_TEST);
    }

private:
    nanogui::GLShader mShader;
    Eigen::Vector3f mRotation;
};


class MailleScreen : public nanogui::Screen {
public:
    MailleScreen() : nanogui::Screen(Eigen::Vector2i(800, 600), "Maille Designer", false) {
        using namespace nanogui;

        Window *window = new Window(this, "Design");
        window->setPosition(Vector2i(100, 0));
        //window->setLayout(new GroupLayout());

        mCanvas = new RingGLCanvas(window);
        mCanvas->setBackgroundColor({100, 100, 100, 255});
        mCanvas->setSize({600, 600});

        //performLayout();
    }

    virtual void draw(NVGcontext *ctx) {
        /* Draw the user interface */
        Screen::draw(ctx);
    }
private:
    RingGLCanvas *mCanvas;
};

int main(int /* argc */, char ** /* argv */) {
    try {
        nanogui::init();

        /* scoped variables */ {
            nanogui::ref<MailleScreen> app = new MailleScreen();
            app->drawAll();
            app->setVisible(true);
            nanogui::mainloop();
        }

        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }

    return 0;
}
