/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 14th March 2010.
****    Description: camera frame routines.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_CAMERA_H               // Include guard "CrusCamera.h"
#define CRUS_CAMERA_H

#include <memory>

#include "Math\CrusVector.h"
#include "Math\CrusMatrix.h"
#include "Math\CrusQuaternion.h"

namespace __hidden {
class ICameraBehaviour;
}

namespace isle {
/**
@brief Camera2 class for camera creation and control
@detailed »спользуетс€ дл€ создани€ нескольких видов камер
*/
class Camera2 {
public:
    // The basis world unit vectors.
    static math::Vector constexpr kWORLD_AXIS_X{1.0f, 0.0f, 0.0f};
    static math::Vector constexpr kWORLD_AXIS_Y{0.0f, 1.0f, 0.0f};
    static math::Vector constexpr kWORLD_AXIS_Z{0.0f, 0.0f, 1.0f};

    enum class eCAM_BEHAVIOR {
        nNIL = 0x00, nFIRST, nFREE, nTHIRD, nSDK
    };

    Camera2();
    ~Camera2();

    void Create(Camera2::eCAM_BEHAVIOR behavior);
    void SetBehavior(Camera2::eCAM_BEHAVIOR behavior);

    void Update();

    void LookAt(math::Vector const &aim);
    void LookAt(float x, float y, float z);

    void LookAt(math::Vector const &eye, math::Vector const &target);


    math::Matrix const &view() const;

    math::Vector const &aim() const;
    math::Vector const &SetAim(math::Vector const &pos);
    math::Vector const &SetAim(float x, float y, float z);

    math::Vector const &pos() const;
    math::Vector const &SetPos(math::Vector const &pos);
    math::Vector const &SetPos(float x, float y, float z);

    // :TODO: temporary solution - must be deleted.
    static Camera2 &inst();

private:
    math::Matrix view_;

    math::Quaternion rot_;

    // For data aligment reason.
    struct {
        math::Vector aim_; float pitch_;
        math::Vector pos_; float yaw_;
    };

    eCAM_BEHAVIOR behavior_{eCAM_BEHAVIOR::nNIL};

    Camera2(Camera2 const &cam);
    //Camera2 const &operator= (Camera2 const &cam);

    float RestricPitch(float pitch);
    float RestricYaw(float yaw);

    void UpdateView();
};

inline math::Vector const &Camera2::aim() const
{
    return aim_;
}

inline math::Vector const &Camera2::SetAim(math::Vector const &_aim)
{
    return aim_ = _aim;
}

inline math::Vector const &Camera2::SetAim(float _x, float _y, float _z)
{
    return SetAim(math::Vector(_x, _y, _z));
}

inline math::Vector const &Camera2::pos() const
{
    return pos_;
}

inline math::Vector const &Camera2::SetPos(math::Vector const &_pos)
{
    return pos_ = _pos;
}

inline math::Vector const &Camera2::SetPos(float _x, float _y, float _z)
{
    return SetPos(math::Vector(_x, _y, _z));
}

inline void Camera2::LookAt(float _x, float _y, float _z)
{
    LookAt(math::Vector(_x, _y, _z));
}

inline math::Matrix const &Camera2::view() const
{
    return view_;
}
};

#endif // CRUS_SYSTEM_H