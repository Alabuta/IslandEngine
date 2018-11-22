#pragma once

#define GLM_FORCE_CXX17
#include <glm/glm.hpp>


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

    std::function<void()> updateHandler_{[] { }};

    glm::vec2 delta{0.f, 0.f};
    glm::vec2 last{0.f, 0.f};

    glm::vec2 dollyDirection{0, -1};

    void onMove(i64 x, i64 y) override;
    void onWheel(i16 delta) override;
    void onDown(IHandler::buttons_t buttons) override;
    void onUp(IHandler::buttons_t buttons) override;
};
}
