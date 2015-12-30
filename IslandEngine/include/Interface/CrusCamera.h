/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 14th March 2010.
****    Description: camera frame routines.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_CAMERA_H               // Include guard "CrusCamera.h"
#define CRUS_CAMERA_H

#include "Math\CrusVector.h"
#include "Math\CrusMatrix.h"
#include "Math\CrusQuaternion.h"

namespace isle
{
/**
@brief Camera class for camera creation and control
@detailed »спользуетс€ дл€ создани€ нескольких видов камер
*/
class Camera {
public:
    // The natural basis world unit vectors.
    static math::Vector const kWORLD_AXIS_X;
    static math::Vector const kWORLD_AXIS_Y;
    static math::Vector const kWORLD_AXIS_Z;

    enum class eCAM_BEHAVIOR {
        nNIL = 0x00, nFIRST, nFREE, nTHIRD, nSDK
    };

    Camera();
    ~Camera();

    void Create(Camera::eCAM_BEHAVIOR behavior);
    void SetBehavior(Camera::eCAM_BEHAVIOR behavior);

    void Update();

    void LookAt(math::Vector const &aim);
    void LookAt(float x, float y, float z);

    math::Matrix const &view() const;

    math::Vector const &aim() const;
    math::Vector const &SetAim(math::Vector const &pos);
    math::Vector const &SetAim(float x, float y, float z);

    math::Vector const &pos() const;
    math::Vector const &SetPos(math::Vector const &pos);
    math::Vector const &SetPos(float x, float y, float z);

    // :TODO: temporary solution - must be deleted.
    static Camera &inst();

private:

    // A little tricky, but it necessary. ;)
    union {
        struct {
            math::Matrix view_;
        };

        struct {
            math::Vector xAxis_; float x_;
            math::Vector yAxis_; float y_;
            math::Vector zAxis_; float z_;
        };
    };

    math::Quaternion rot_;

    // For data aligment reason.
    struct {
        math::Vector aim_; float pitch_;
        math::Vector pos_; float yaw_;
    };

    eCAM_BEHAVIOR behavior_{eCAM_BEHAVIOR::nNIL};

    Camera(Camera const &cam);
    Camera const &operator= (Camera const &cam);

    float RestricPitch(float pitch);
    float RestricYaw(float yaw);

    void UpdateView();
};

inline math::Vector const &Camera::aim() const
{
    return aim_;
}

inline math::Vector const &Camera::SetAim(math::Vector const &_aim)
{
    return aim_ = _aim;
}

inline math::Vector const &Camera::SetAim(float _x, float _y, float _z)
{
    return SetAim(math::Vector(_x, _y, _z));
}

inline math::Vector const &Camera::pos() const
{
    return pos_;
}

inline math::Vector const &Camera::SetPos(math::Vector const &_pos)
{
    return pos_ = _pos;
}

inline math::Vector const &Camera::SetPos(float _x, float _y, float _z)
{
    return SetPos(math::Vector(_x, _y, _z));
}

inline void Camera::LookAt(float _x, float _y, float _z)
{
    LookAt(math::Vector(_x, _y, _z));
}

inline math::Matrix const &Camera::view() const
{
    return view_;
}
};

#endif // CRUS_SYSTEM_H