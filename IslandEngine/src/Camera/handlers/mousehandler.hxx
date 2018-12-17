#pragma once

#include <functional>

#define GLM_FORCE_CXX17
#include <glm/glm.hpp>


#include "input/inputManager.hxx"
#include "input/mouseInput.hxx"

#include "camera/cameraController.hxx"


namespace isle
{
class MouseHandler final : public MouseInput::IHandler {
public:

    MouseHandler(OrbitController &controller) : controller_{controller} { }

private:

    OrbitController &controller_;

    std::function<void(MouseHandler &)> updateHandler_{[](auto &&) { }};

    glm::vec2 delta{0.f, 0.f};
    glm::vec2 last{0.f, 0.f};

    glm::vec2 dollyDirection{0, -1};

    void onMove(float x, float y) override;
    void onWheel(float xoffset, float yoffset) override;
    void onDown(IHandler::buttons_t buttons) override;
    void onUp(IHandler::buttons_t buttons) override;
};
}
