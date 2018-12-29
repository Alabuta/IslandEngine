#include "config.h"

#if defined(_MSC_VER) && defined(DEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include "main.hxx"

struct per_object_t {
    glm::mat4 world{1};
    glm::mat4 normal{1};  // Transposed of the inversed of the upper left 3x3 sub-matrix of model(world)-view matrix.
};

struct application_t {
    isle::ResourceManager resourceManager;

    isle::CameraSystem cameraSystem;
    std::shared_ptr<isle::Camera> camera;

    std::unique_ptr<isle::OrbitController> cameraController;

    per_object_t object;

    std::int32_t width{800}, height{1080};
};

struct WindowEventHandler final : isle::Window::IEventHandler {
    WindowEventHandler(application_t &app) : app{app} { }
    
    void onResize(std::int32_t width, std::int32_t height) override
    {
        app.width = width;
        app.height = height;

        app.camera->aspect = static_cast<float>(width) / static_cast<float>(height);
    }

    application_t &app;
};

void initContext(isle::Window const &window)
{
    glfwMakeContextCurrent(window.handle());
    glfwSwapInterval(-1);

    glewExperimental = true;

    if (auto result = glewInit(); result != GLEW_OK)
        throw std::runtime_error("failed to init GLEW"s);
}

void update(application_t &app)
{
    app.cameraController->update();
    app.cameraSystem.update();
}

void drawFrame(application_t &app)
{
    glViewport(0, 0, app.width, app.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


int main()
try {
#if defined(_MSC_VER) && defined(DEBUG)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(84);
#endif

    if (auto result = glfwInit(); result != GLFW_TRUE)
        throw std::runtime_error("failed to init GLFW"s);

    // glfwSetErrorCallback([] (auto code, auto description)
    // {
    //     std::cerr << code << ' ' << description << '\n';
    // });

    application_t app;

    isle::Window window{"IslandEngine"sv, app.width, app.height};

    initContext(window);

    auto windowEventHandler = std::make_shared<WindowEventHandler>(app);
    window.connectEventHandler(windowEventHandler);

    auto inputManager = std::make_shared<isle::InputManager>();
    window.connectInputHandler(inputManager);

    app.camera = app.cameraSystem.createCamera();
    app.camera->aspect = static_cast<float>(app.width) / static_cast<float>(app.height);

    app.cameraController = std::make_unique<isle::OrbitController>(app.camera, *inputManager);
    app.cameraController->lookAt(glm::vec3{8, 24, 24}, {0, 8, 0});

    glClearColor(.5f, .5f, .5f, 1.f);

    auto width = 512, height = 512;

    auto fbo = app.resourceManager.CreateObject(isle::eOBJECT_TYPE::FRAMEBUFFER);
    auto rt_cubemap = app.resourceManager.CreateObject(isle::eOBJECT_TYPE::TEXTURE_CUBE_MAP);

    glTextureStorage2D(rt_cubemap->handle(), 1, GL_RGBA32F, width, height);

#if 0

    auto fbo = 0u;
    glCreateFramebuffers(1, &fbo);

    auto rt_cubemap = 0u;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &rt_cubemap);

    glTextureStorage2D(rt_cubemap, 1, GL_RGBA32F, width, height);

    std::vector<float> buffer(static_cast<std::size_t>(width * height * 4), .16f);

    for (auto face : { 0, 1, 2, 3, 4, 5 })
        glTextureSubImage3D(rt_cubemap, 0, 0, 0, face, width, height, 1, GL_RGBA, GL_FLOAT, std::data(buffer));

    glNamedFramebufferTextureLayer(fbo, GL_COLOR_ATTACHMENT0, rt_cubemap, 0, 0);

    std::array<std::uint32_t, 1> constexpr drawBuffers{ GL_COLOR_ATTACHMENT0 };
    glNamedFramebufferDrawBuffers(fbo, static_cast<std::int32_t>(std::size(drawBuffers)), std::data(drawBuffers));

    if (auto result = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER); result != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("framebuffer: "s + std::to_string(result));
#endif

    window.update([&app] (auto &&window)
    {
        glfwPollEvents();

        // glfwGetFramebufferSize(window.handle(), &app.width, &app.height);

        drawFrame(app);

        glfwSwapBuffers(window.handle());
    });

    glfwTerminate();

    return 0;

} catch (std::exception const &ex) {
    std::cout << ex.what() << std::endl;
    std::cin.get();
}
