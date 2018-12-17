#include "config.h"

#include <iostream>

#include <string>
using namespace std::string_literals;

#include <string_view>
using namespace std::string_view_literals;

#include "main.hxx"


int main()
try {
    auto width = 800;
    auto height = 600;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//#if defined(_DEBUG)
//	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
//#else
//    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
//#endif

    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    // glfwSetErrorCallback(error_callback);

    isle::Window window{"IslandEngine"sv, width, height};

    /*auto resizeHandler = std::make_shared<ResizeHandler>(app);
    window.connectEventHandler(resizeHandler);*/

    auto inputManager = std::make_shared<isle::InputManager>();
    window.connectInputHandler(inputManager);

    glfwMakeContextCurrent(window.handle());
    glfwSwapInterval(1);

    glClearColor(.5f, .5f, .5f, 1.f);

    window.update([&width, &height] (auto &&window)
    {
        glfwPollEvents();

        glfwGetFramebufferSize(window.handle(), &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window.handle());
    });

    glfwTerminate();

    return 0;

} catch (std::exception const &ex) {
    std::cout << ex.what() << std::endl;
    std::cin.get();
}
