#pragma once 

#include <cstdint>
#include <string>
#include <string_view>

#include <boost/signals2.hpp>

#include <GLFW/glfw3.h>

#include "input/inputDataTypes.hxx"


namespace isle
{
class Window final {
public:

    Window(std::string_view name, std::int32_t width, std::int32_t height);

    ~Window();

    void update(std::function<void(Window &)> &&callback);

    GLFWwindow *handle() noexcept { return handle_; }

    struct IEventHandler {
        virtual ~IEventHandler() = default;

        virtual void onResize(std::int32_t width, std::int32_t height) = 0;
    };

    void connectEventHandler(std::shared_ptr<IEventHandler> handler);

    struct IInputHandler {
        virtual ~IInputHandler() = default;

        virtual void onUpdate(input::RawData &data) = 0;
    };

    void connectInputHandler(std::shared_ptr<IInputHandler> handler);

private:
    GLFWwindow *handle_;

    std::int32_t width_{0}, height_{0};

    std::string name_;

    boost::signals2::signal<void(std::int32_t, std::int32_t)> resizeCallback_;

    boost::signals2::signal<void(input::RawData &)> inputUpdateCallback_;

    void setCallbacks();
};
}
