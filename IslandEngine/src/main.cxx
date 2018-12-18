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

void initContext(isle::Window const &window)
{
    glfwMakeContextCurrent(window.handle());
    glfwSwapInterval(1);

    glewExperimental = true;

    if (auto result = glewInit(); result != GLEW_OK)
        throw std::runtime_error("failed to init GLEW"s);
}


int main()
try {
    auto width = 512;
    auto height = 512;

    if (auto result = glfwInit(); result != GLFW_TRUE)
        throw std::runtime_error("failed to init GLFW"s);

    glfwSetErrorCallback([] (auto code, auto description)
    {
        std::cerr << code << ' ' << description << '\n';
    });

    isle::Window window{"IslandEngine"sv, width, height};

    initContext(window);

    /*auto resizeHandler = std::make_shared<ResizeHandler>(app);
    window.connectEventHandler(resizeHandler);*/

    auto inputManager = std::make_shared<isle::InputManager>();
    window.connectInputHandler(inputManager);

    glClearColor(.5f, .5f, .5f, 1.f);

#if 0
    auto fbo = 0u;
    glCreateFramebuffers(1, &fbo);

    auto rt_cubemap = 0u;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &rt_cubemap);

    glTextureStorage2D(rt_cubemap, 1, GL_RGBA16F, width, height);

    std::vector<float> buffer(static_cast<std::size_t>(width * height * 4), .16f);

    for (auto face : {0, 1, 2, 3, 4, 5})
        glTextureSubImage3D(rt_cubemap, 0, 0, 0, face, width, height, 1, GL_RGBA, GL_FLOAT, std::data(buffer));

    glNamedFramebufferTextureLayer(fbo, GL_COLOR_ATTACHMENT0, rt_cubemap, 0, 0);

    std::array<std::uint32_t, 1> constexpr drawBuffers{ GL_COLOR_ATTACHMENT0 };
    glNamedFramebufferDrawBuffers(fbo, static_cast<std::int32_t>(std::size(drawBuffers)), std::data(drawBuffers));

    if (auto result = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("framebuffer: "s + std::to_string(result));
#endif

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
