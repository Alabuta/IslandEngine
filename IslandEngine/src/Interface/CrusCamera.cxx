/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2014 Crusoe's Island LLC.
****
****    Started at 14th March 2010.
****    Description: camera frame routines.
****
****************************************************************************************/
#include "System\CrusSystem.h"
#include "System\CrusInput.h"
#include "System\CrusWindow.h"

#include "Interface\CrusCamera.h"

extern isle::Input::Controller controller;

namespace isle
{
math::Vector const Camera::kWORLD_AXIS_X{1.0f, 0.0f, 0.0f};
math::Vector const Camera::kWORLD_AXIS_Y{0.0f, 1.0f, 0.0f};
math::Vector const Camera::kWORLD_AXIS_Z{0.0f, 0.0f, 1.0f};

Camera::Camera() : pitch_{0.0f}, yaw_{0.0f}, view_{math::Matrix::GetIdentity()} {}
Camera::~Camera() {};

void Camera::Create(Camera::eCAM_BEHAVIOR _behavior)
{
    switch(_behavior){
        case eCAM_BEHAVIOR::nFIRST:
            break;

        case eCAM_BEHAVIOR::nFREE:
            //Input::Mouse::inst().HideCursor();
            break;

        case eCAM_BEHAVIOR::nTHIRD:
            //Input::Mouse::inst().HideCursor();
            break;

        case eCAM_BEHAVIOR::nSDK:
            //Input::Mouse::inst().ShowCursor();
            break;
    }

    behavior_ = _behavior;
}

void Camera::SetBehavior(Camera::eCAM_BEHAVIOR _behavior)
{
    if(behavior_ == _behavior)
        return;

    math::Vector aim;

    if (behavior_ == eCAM_BEHAVIOR::nFREE) {
        aim  = aim_;
        aim_ = pos_;
        pos_ = aim;
    }

    else if (_behavior == eCAM_BEHAVIOR::nFREE && (behavior_ == eCAM_BEHAVIOR::nTHIRD || behavior_ == eCAM_BEHAVIOR::nSDK)) {
        pos_ = aim_;
        aim = -zAxis_;
    }

    Create(_behavior);

    pitch_ = -math::RadToDeg(acosf(yAxis_.y()));     // Extract the pitch angle.
    yaw_ = -math::RadToDeg(acosf(xAxis_.x()));       // Extract the yaw angle.
}

void Camera::LookAt(math::Vector const &_aim)
{
    aim_ = _aim;
    zAxis_ = pos_ - aim_;

    if(math::IsTooSmall(zAxis_.x()) && math::IsTooSmall(zAxis_.z()))
        zAxis_.z() += math::kEPSILON;

    zAxis_.Normalize();

    xAxis_ = (kWORLD_AXIS_Y ^ zAxis_).Normalize();
    yAxis_ = (zAxis_   ^ xAxis_).Normalize();

    x_ = -xAxis_ * pos_;
    y_ = -yAxis_ * pos_;
    z_ = -zAxis_ * pos_;

    pitch_ = -math::RadToDeg(acosf(yAxis_.y()));     // Extract the pitch angle.
    yaw_ = -math::RadToDeg(acosf(xAxis_.x()));       // Extract the yaw angle.

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

    if(pitch_ > 90.0f)
        pitch_ = 90.0f;

    else if(pitch_ < -90.0f)
        pitch_ = -90.0f;

    return pitch_;
}

float Camera::RestricYaw(float _yaw)
{
    yaw_ += _yaw;

    if(yaw_ > 360.0f)
        yaw_ -= 360.0f;

    else if(yaw_ < -360.0f)
        yaw_ += 360.0f;

    return yaw_;
}

void Camera::UpdateView()
{
#if _CRUS_TEMP_DISABLED
    RECT ws;
    GetWindowRect(Window::inst().hWnd(), &ws);

    POINT pt;
    GetCursorPos(&pt);

    float const x = ws.left + (ws.right - ws.left) / 2.0f - pt.x;
    float const y = ws.top + (ws.bottom - ws.top) / 2.0f - pt.y;

    SetCursorPos(ws.left + ((ws.right - ws.left) >> 1), ws.top + ((ws.bottom - ws.top) >> 1));

    RestricYaw(x * 0.05f);
    RestricPitch(y * 0.05f);

    //RestricYaw(controller.x() * 0.05f);
    //RestricPitch(controller.y() * 0.05f);
#endif

    rot_.FromAxisAngle(kWORLD_AXIS_Y.v(), yaw_);
    rot_ ^= math::Quaternion::GetFromAxisAngle(kWORLD_AXIS_X.v(), pitch_);

    view_.FromQuaternion(rot_.q());

    x_ = -xAxis_.Normalize() * pos_;
    y_ = -yAxis_.Normalize() * pos_;
    z_ = -zAxis_.Normalize() * pos_;
}

void Camera::Update()
{
    UpdateView();
}
};