#pragma once

#include "System/InputManager.hxx"
#include "System/MouseInput.hxx"

#include "Camera/CameraController.hxx"

namespace isle
{
class MouseHandler final : public isle::MouseInput::IHandler {
public:

    MouseHandler(OrbitController &controller);

private:

    OrbitController &controller_;

    void onMove(i64 x, i64 y) override;
    void onWheel(i16 delta) override;
    void onDown(IHandler::buttons_t buttons) override;
    void onUp(IHandler::buttons_t buttons) override;
};
}
