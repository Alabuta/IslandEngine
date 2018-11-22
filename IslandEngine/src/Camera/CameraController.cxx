#include "Camera/CameraController.hxx"
#include "Camera/MouseHandler.hxx"



namespace isle
{
OrbitController::OrbitController(std::shared_ptr<Camera> camera, InputManager &inputManager) : camera_{camera}
{
    mouseHandler_ = std::make_shared<isle::MouseHandler>(*this);
    inputManager.mouse().connect(mouseHandler_);
}

void OrbitController::update()
{
    ;
}
}
