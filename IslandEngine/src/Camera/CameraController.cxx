#include "Camera/CameraController.hxx"
#include "Camera/MouseHandler.hxx"



namespace isle
{
OrbitController::OrbitController(std::shared_ptr<Camera> camera, InputManager &inputManager) : camera_{camera}
{
    mouseHandler_ = std::make_shared<MouseHandler>(*this);
    inputManager.mouse().connect(mouseHandler_);
}

void OrbitController::rotate(float x, float y)
{
    log::Debug() << __FUNCTION__ << ' ' << x << '\t' << y;
}

void OrbitController::pan(float x, float y)
{
    log::Debug() << __FUNCTION__ << ' ' << x << '\t' << y;
}

void OrbitController::dolly(float delta)
{
    log::Debug() << __FUNCTION__ << ' ' << delta;
}

void OrbitController::update()
{
    ;
}
}
