/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 14th March 2010.
****    Description: camera frame routines.
****
********************************************************************************************************************************/
#include "System\CrusSystem.h"
#include "System\CrusInput.h"
#include "System\CrusWindow.h"

#include "Camera\CameraBehaviour.h"
#include "Camera\CrusCamera.h"

extern isle::Input::Controller controller;

namespace isle {
math::Vector const Camera::kWORLD_AXIS_X{1.0f, 0.0f, 0.0f};
math::Vector const Camera::kWORLD_AXIS_Y{0.0f, 1.0f, 0.0f};
math::Vector const Camera::kWORLD_AXIS_Z{0.0f, 0.0f, 1.0f};

Camera::Camera() : behavior{std::make_shared<__hidden::ICameraEditorBehaviour>()}, pitch_{0.0f}, yaw_{0.0f}, view_{math::Matrix::Identity()} { }
Camera::~Camera() { };

void Camera::Create(Camera::eCAM_BEHAVIOR _behavior)
{
    switch (_behavior) {
        case eCAM_BEHAVIOR::nFIRST:
            break;

        case eCAM_BEHAVIOR::nFREE:
            //Input::Mouse::main().HideCursor();
            ::ShowCursor(FALSE);
            break;

        case eCAM_BEHAVIOR::nTHIRD:
            //Input::Mouse::main().HideCursor();
            break;

        case eCAM_BEHAVIOR::nSDK:
            //Input::Mouse::main().ShowCursor();
            break;
    }

    behavior_ = _behavior;
}

void Camera::SetBehavior(Camera::eCAM_BEHAVIOR _behavior)
{
    if (behavior_ == _behavior)
        return;

    math::Vector aim;

    if (behavior_ == eCAM_BEHAVIOR::nFREE) {
        aim = aim_;
        aim_ = pos_;
        pos_ = aim;
    }

    else if (_behavior == eCAM_BEHAVIOR::nFREE && (behavior_ == eCAM_BEHAVIOR::nTHIRD || behavior_ == eCAM_BEHAVIOR::nSDK)) {
        pos_ = aim_;
        aim = -view_.zAxis();
    }

    Create(_behavior);

    pitch_ = -math::RadToDeg(std::acos(view_.yAxis().y()));     // Extract the pitch angle.
    yaw_ = -math::RadToDeg(std::acos(view_.xAxis().x()));       // Extract the yaw angle.
}

void Camera::LookAt(math::Vector const &_aim)
{
    aim_ = _aim;
    view_.zAxis() = pos_ - aim_;

    if (math::IsTooSmall(view_.zAxis().x()) && math::IsTooSmall(view_.zAxis().z()))
        view_.zAxis().z() += math::kEPSILON;

    view_.zAxis().Normalize();

    view_.xAxis() = (kWORLD_AXIS_Y ^ view_.zAxis()).Normalize();
    view_.yAxis() = (view_.zAxis() ^ view_.xAxis()).Normalize();

    view_.xOrigin() = -view_.xAxis() * pos_;
    view_.yOrigin() = -view_.yAxis() * pos_;
    view_.zOrigin() = -view_.zAxis() * pos_;

    pitch_ = -math::RadToDeg(std::acos(view_.yAxis().y()));     // Extract the pitch angle.
    yaw_ = -math::RadToDeg(std::acos(view_.xAxis().x()));       // Extract the yaw angle.

    // :TODO: remove.
    //rot_.FromMatrix4x4(view_.m());
}

// :TODO: temporary solution - must be deleted.
/*static*/ __declspec(noinline) Camera &Camera::inst()
{
    static Camera camera;
    return camera;
}

float Camera::RestricPitch(float _pitch)
{
    pitch_ += _pitch;

    if (pitch_ > 90.0f)
        pitch_ = 90.0f;

    else if (pitch_ < -90.0f)
        pitch_ = -90.0f;

    return pitch_;
}

float Camera::RestricYaw(float _yaw)
{
    yaw_ += _yaw;

    if (yaw_ > 360.0f)
        yaw_ -= 360.0f;

    else if (yaw_ < -360.0f)
        yaw_ += 360.0f;

    return yaw_;
}

void Camera::UpdateView()
{
    RECT ws;
    GetWindowRect(Window::main().hWnd(), &ws);

    POINT pt;
    GetCursorPos(&pt);

    float const x = ws.left + (ws.right - ws.left) / 2.0f - pt.x;
    float const y = ws.top + (ws.bottom - ws.top) / 2.0f - pt.y;

    SetCursorPos(ws.left + ((ws.right - ws.left) >> 1), ws.top + ((ws.bottom - ws.top) >> 1));

    RestricYaw(x * 0.05f);
    RestricPitch(y * 0.05f);

    //RestricYaw(controller.x() * 0.05f);
    //RestricPitch(controller.y() * 0.05f);

    rot_.FromAxisAngle(kWORLD_AXIS_Y.v().data(), yaw_);
    rot_ ^= math::Quaternion::GetFromAxisAngle(kWORLD_AXIS_X.v().data(), pitch_);

    view_.FromQuaternion(rot_.q());

    /*auto m = view_.Rotate(kWORLD_AXIS_Y, yaw_);
    view_ = view_.Rotate(kWORLD_AXIS_X, pitch_) * m;*/


    view_.xOrigin() = -view_.xAxis().Normalize() * pos_;
    view_.yOrigin() = -view_.yAxis().Normalize() * pos_;
    view_.zOrigin() = -view_.zAxis().Normalize() * pos_;
}

void Camera::Update()
{
    UpdateView();
}
};