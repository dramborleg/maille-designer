#include <nanogui/common.h>
#include <nanogui/glcanvas.h>
#include <nanogui/glutil.h>
#include <nanogui/object.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>

#include "ringglcanvas.h"
#include <iostream>
#include <string>

using std::endl;

class MailleScreen : public nanogui::Screen
{
public:
    MailleScreen()
        : nanogui::Screen(Eigen::Vector2i(800, 600), "Maille Designer", false)
    {
        using namespace nanogui;

        Window *window = new Window(this, "Design");
        window->setPosition(Vector2i(100, 0));
        // window->setLayout(new GroupLayout());

        mCanvas = new RingGLCanvas(window);
        mCanvas->setBackgroundColor({100, 100, 100, 255});
        mCanvas->setSize({600, 600});

        // performLayout();
    }

    virtual void draw(NVGcontext *ctx)
    {
        /* Draw the user interface */
        Screen::draw(ctx);
    }

private:
    RingGLCanvas *mCanvas;
};

int main(int /* argc */, char ** /* argv */)
{
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
        std::string error_msg =
            std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
        MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
        std::cerr << error_msg << endl;
#endif
        return -1;
    }

    return 0;
}
