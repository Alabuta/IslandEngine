/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: viewport windows routines.
****
********************************************************************************************************************************/
#include "System\CrusSystem.h"
#include "System\CrusWindow.h"
#include "Renderer\CrusRender.h"

#include "Interface\CrusViewport.h"

isle::intf::Viewport viewport;

namespace isle::intf {
void Viewport::Create(Viewport::eVIEWPORT_TYPE _type)
{
    _type;
}

void Viewport::SetViewport(i16 _x, i16 _y, i16 _w, i16 _h)
{
    x_ = _x;    y_ = _y;
    w_ = _w;    h_ = _h;

    auto const kFOV = 72.f, zNear = 0.01f, zFar = 100.f;
    auto const f = 1.f / std::tan(kFOV * math::kPI_DIV_180 * 0.5f);

    auto const aspect = static_cast<float>(w_) / static_cast<float>(h_);

    // Default OpenGL perspective projection matrix.
    auto kA = -(zFar + zNear) / (zFar - zNear);
    auto kB = -2.f * zFar * zNear / (zFar - zNear);

    // Depth value mapped from 0 to 1. See 'D3DXMatrixPerspectiveFovRH'.
    if constexpr (Render::kDEPTH_CLIPPED_FROM_ZERO_TO_ONE) {
        if constexpr (Render::kREVERSED_DEPTH) {
            if constexpr (Render::kINFINITE_FAR_PLANE) {
                kA = 0;
                kB = zNear;
            }

            else {
                kA = zNear / (zNear - zFar);
                kB = -zNear * zFar / (zNear - zFar);
            }
        }

        else {
            kA = zFar / (zNear - zFar);
            kB = zNear * zFar / (zNear - zFar);
        }
    }

    proj_ = math::Matrix
    (
        f / aspect, 0.f, 0.f, 0.f,
        0.f, f, 0.f, 0.f,
        0.f, 0.f, kA, kB,
        0.f, 0.f, -1.f, 0.f
    );

    invProj_ = proj_;
    invProj_.Inverse();

    glViewport(x_, y_, w_, h_);

    auto const w2 = w_ * 0.5f;
    auto const h2 = h_ * 0.5f;

    viewport_ = math::Matrix
    (
        w2, 0.f, 0.f, 0.f,
        0.f, h2, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        w2 + x_, h2 + y_, 0.f, 1.f
    );
}

void Viewport::SetCamera(Camera2 *const _cam)
{
    cam_ = _cam;

    SetViewport(x_, y_, w_, h_);
}

void Viewport::MakeCurrent() const
{
    glViewport(x_, y_, w_, h_);
}
};