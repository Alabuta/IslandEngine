#pragma once

#include <memory>
#include <variant>
#include <tuple>

#if NOT_YET_IMPLEMENTED
#include "platform/window.hxx"
#include "mouseInput.hxx"


class InputManager final : public Window::IInputHandler {
public:

    MouseInput &mouse() noexcept { return mouse_; }

private:

    MouseInput mouse_;

    void onUpdate(input::RawData &data) override;

};
#endif
