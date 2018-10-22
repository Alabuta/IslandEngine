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
#include <iomanip>

extern isle::Input::Controller controller;

namespace isle {

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
        aim = -view_.zAxis;
    }

    Create(_behavior);

    pitch_ = -math::RadToDeg(std::acos(view_.yAxis.y));     // Extract the pitch angle.
    yaw_ = -math::RadToDeg(std::acos(view_.xAxis.x));       // Extract the yaw angle.
}


void isle::Camera::LookAt(math::Vector const &eye, math::Vector const &target)
{
    aim_ = target;

    auto z = eye - target;

    if (math::IsTooSmall(z.GetLenght())) z.z = 1.f;

    z.Normalize();

    auto x = kWORLD_AXIS_Y ^ z;

    if (math::IsTooSmall(x.GetLenght())) {
        if (std::abs(kWORLD_AXIS_Y.z) == 1.f) z.x += math::kEPSILON;
        else z.z += math::kEPSILON;

        z.Normalize();
        x = kWORLD_AXIS_Y ^ z;
    }

    x.Normalize();

    auto y = z ^ x;
    y.Normalize();

    pitch_ = -math::RadToDeg(std::acos(y.y));           // Extract the pitch angle.
    yaw_ = -math::RadToDeg(std::atan2(x.x, z.z));       // Extract the yaw angle.

    view_.xAxis = x;
    view_.yAxis = y;
    view_.zAxis = z;
    //view_.pos = eye;
}

void Camera::LookAt(math::Vector const &_aim)
{
    aim_ = _aim;
    view_.zAxis = pos_ - aim_;

    if (math::IsTooSmall(view_.zAxis.x) && math::IsTooSmall(view_.zAxis.z))
        view_.zAxis.z += math::kEPSILON;

    view_.zAxis.Normalize();

    view_.xAxis = (kWORLD_AXIS_Y ^ view_.zAxis).Normalize();
    view_.yAxis = (view_.zAxis ^ view_.xAxis).Normalize();

    view_.x = -view_.xAxis * pos_;
    view_.y = -view_.yAxis * pos_;
    view_.z = -view_.zAxis * pos_;

    pitch_ = -math::RadToDeg(std::acos(view_.yAxis.y));     // Extract the pitch angle.
    yaw_ = -math::RadToDeg(std::atan2(view_.xAxis.x, view_.zAxis.z));       // Extract the yaw angle.

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

    float const pt_x = ws.left + (ws.right - ws.left) / 2.0f - pt.x;
    float const pt_y = ws.top + (ws.bottom - ws.top) / 2.0f - pt.y;

    SetCursorPos(ws.left + ((ws.right - ws.left) >> 1), ws.top + ((ws.bottom - ws.top) >> 1));

    RestricYaw(pt_x * 0.05f);
    RestricPitch(pt_y * 0.05f);

    //RestricYaw(controller.u * 0.05f);
    //RestricPitch(controller.v * 0.05f);

    rot_.FromAxisAngle(kWORLD_AXIS_Y.xyz.data(), yaw_);
    rot_ ^= math::Quaternion::GetFromAxisAngle(kWORLD_AXIS_X.xyz.data(), pitch_);

    view_.FromQuaternion(rot_.q());

    /*auto m = view_.Rotate(kWORLD_AXIS_Y, yaw_);
    view_ = view_.Rotate(kWORLD_AXIS_X, pitch_) * m;*/

    auto const keyA = GetKeyState('A');
    auto const keyD = GetKeyState('D');

    float x = (((keyD >> 0xF) ? 1.f : 0) - ((keyA >> 0xF) ? 1.f : 0));

    auto const keyE = GetKeyState('E');
    auto const keyQ = GetKeyState('Q');

    float y = (((keyE >> 0xF) ? 1.f : 0) - ((keyQ >> 0xF) ? 1.f : 0));

    auto const keyW = GetKeyState('W');
    auto const keyS = GetKeyState('S');

    float z = (((keyS >> 0xF) ? 1.f : 0) - ((keyW >> 0xF) ? 1.f : 0));

    auto const xAxis = math::Vector::GetNormalized(view_.xAxis);
    auto const yAxis = math::Vector::GetNormalized(view_.yAxis);
    auto const zAxis = math::Vector::GetNormalized(view_.zAxis);

    auto const direction = math::Vector{x, y, z} * 8.f;
    auto pos = pos_;

    pos.x += math::Vector{xAxis.x, yAxis.x, zAxis.x} * direction;
    pos.y += math::Vector{xAxis.y, yAxis.y, zAxis.y} * direction;
    pos.z += math::Vector{xAxis.z, yAxis.z, zAxis.z} * direction;

    pos_.LerpStable(pos, System::time.delta());

    view_.x = -xAxis * pos_;
    view_.y = -yAxis * pos_;
    view_.z = -zAxis * pos_;
}

void Camera::Update()
{
    UpdateView();
}
};