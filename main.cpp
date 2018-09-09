#include <nanogui/common.h>
#include <nanogui/glcanvas.h>
#include <nanogui/glutil.h>
#include <nanogui/object.h>

#include <iostream>
#include <string>

#include "maillescreen.h"

#if defined(_WIN32)
#if defined(APIENTRY)
#undef APIENTRY
#endif
#include <windows.h>
#endif

using std::endl;

int main(int /* argc */, char ** /* argv */)
{
    try
    {
        nanogui::init();

        /* scoped variables */ {
            nanogui::ref<MailleScreen> app = new MailleScreen();
            app->drawAll();
            app->setVisible(true);
            nanogui::mainloop();
        }

        nanogui::shutdown();
    }
    catch (const std::runtime_error &e)
    {
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

#if defined(_WIN32)
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { return main(0, NULL); }
#endif
