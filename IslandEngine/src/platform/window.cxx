#include <stdexcept>

#include "window.hxx"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace isle
{
Window::Window(std::string_view name, std::int32_t width, std::int32_t height)
    : handle_{nullptr}, width_{width}, height_{height}, name_{name}
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//#if defined(_DEBUG)
	// glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
//#else
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
//#endif

    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    handle_ = glfwCreateWindow(width_, height_, name_.c_str(), nullptr, nullptr);

    if (handle_ == nullptr)
        throw std::runtime_error("failed to create '"s + name_ + "' window"s);

    glfwSetWindowUserPointer(handle_, this);

    setCallbacks();
}

Window::~Window()
{
    if (handle_)
        glfwDestroyWindow(handle_);
}

void Window::connectEventHandler(std::shared_ptr<IEventHandler> handler)
{
    resizeCallback_.connect(decltype(resizeCallback_)::slot_type(
        &IEventHandler::onResize, handler.get(), _1, _2
    ).track_foreign(handler));
}

void Window::connectInputHandler(std::shared_ptr<IInputHandler> handler)
{
    inputUpdateCallback_.connect(decltype(inputUpdateCallback_)::slot_type(
        &IInputHandler::onUpdate, handler.get(), _1
    ).track_foreign(handler));
}

void Window::update(std::function<void(Window &)> &&callback)
{
    while (glfwWindowShouldClose(handle_) == GLFW_FALSE && glfwGetKey(handle_, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        callback(*this);
    }
}

void Window::setCallbacks()
{
    glfwSetWindowSizeCallback(handle_, [] (auto handle, auto width, auto height)
    {
        auto instance = reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        if (instance) {
            instance->width_ = width;
            instance->height_ = height;

            instance->resizeCallback_(width, height);
        }
    });

    glfwSetCursorPosCallback(handle_, [] (auto handle, auto x, auto y)
    {
        auto instance = reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        if (instance) {
            auto coords = input::mouse::RelativeCoords{
                static_cast<decltype(input::mouse::RelativeCoords::x)>(x),
                static_cast<decltype(input::mouse::RelativeCoords::y)>(y)
            };

            input::RawData data = input::mouse::RawData{std::move(coords)};

            instance->inputUpdateCallback_(data);
        }
    });

    glfwSetMouseButtonCallback(handle_, [] (auto handle, auto button, auto action, auto)
    {
        auto instance = reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        if (instance) {
            auto buttons = input::mouse::Buttons{};

            std::size_t offset = action == GLFW_PRESS ? 0 : 1;

            buttons.value[static_cast<std::size_t>(button) * 2 + offset] = 1;

            input::RawData data = input::mouse::RawData{std::move(buttons)};

            instance->inputUpdateCallback_(data);
        }
    });

    // TODO: replace to 'glfwSetMouseWheelCallback'
    glfwSetScrollCallback(handle_, [] (auto handle, auto xoffset, auto yoffset)
    {
        auto instance = reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        if (instance) {
            auto wheel = input::mouse::Wheel{
                static_cast<decltype(input::mouse::Wheel::xoffset)>(xoffset),
                static_cast<decltype(input::mouse::Wheel::yoffset)>(yoffset)
            };

            input::RawData data = input::mouse::RawData{std::move(wheel)};

            instance->inputUpdateCallback_(data);
        }
    });
}
}
