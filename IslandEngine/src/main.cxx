#include "config.h"

#include <iostream>
#include <sstream>
#include <array>
#include <vector>

#include <string>
using namespace std::string_literals;

#include <string_view>
using namespace std::string_view_literals;

#include "main.hxx"


int main()
try {
    auto width = 512;
    auto height = 512;

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


    auto fbo = 0u;
    glCreateFramebuffers(1, &fbo);

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("1: "s + std::to_string(result));

    auto rt_cubemap = 0u;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &rt_cubemap);

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("2: "s + std::to_string(result));

    glTextureStorage2D(rt_cubemap, 1, GL_RGBA16F, width, height);

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("3: "s + std::to_string(result));

    std::vector<float> buffer(static_cast<std::size_t>(width * height * 4), .16f);

    for (auto face : {0, 1, 2, 3, 4, 5})
        glTextureSubImage3D(rt_cubemap, 0, 0, 0, face, width, height, 1, GL_RGBA, GL_FLOAT, std::data(buffer));

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("4: "s + std::to_string(result));

    glNamedFramebufferTextureLayer(fbo, GL_COLOR_ATTACHMENT0, rt_cubemap, 0, 0);

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("5: "s + std::to_string(result));

    std::array<std::uint32_t, 1> constexpr drawBuffers{ GL_COLOR_ATTACHMENT0 };
    glNamedFramebufferDrawBuffers(fbo, static_cast<std::int32_t>(std::size(drawBuffers)), std::data(drawBuffers));

    if (auto result = glGetError(); result != GL_NO_ERROR)
        throw std::runtime_error("6: "s + std::to_string(result));

    if (auto result = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE) {
        std::string error;
        switch (result) {
            case GL_FRAMEBUFFER_UNDEFINED: error = "GL_FRAMEBUFFER_UNDEFINED"s; break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"s; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"s; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"s; break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"s; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: error = "GL_FRAMEBUFFER_UNSUPPORTED"s; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"s; break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: error = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"s; break;
            default: {
                std::stringstream stream;
                stream << std::hex << result;
                error = stream.str();

                if (auto xresult = glGetError(); xresult != GL_NO_ERROR) {
                    switch (xresult) {
                        case GL_INVALID_OPERATION: error = "GL_INVALID_OPERATION"s; break;
                        case GL_INVALID_ENUM: error = "GL_INVALID_ENUM"s; break;
                        case GL_INVALID_VALUE: error = "GL_INVALID_VALUE"s; break;
                        default: break;
                    }
                }
            } break;
        }

        throw std::runtime_error("framebuffer: "s + error);
    }

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
