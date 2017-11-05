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

void Viewport::SetViewport(int16 _x, int16 _y, int16 _w, int16 _h)
{
    x_ = _x;    y_ = _y;
    w_ = _w;    h_ = _h;

    // :TODO: find the correct near and far plane values.
    auto const kFOV = 72.0f, zNear = 0.01f, zFar = 100.0f;//4194304.0f;//4194304.0f
    auto const f = 1.0f / std::tan(kFOV * math::kPI_DIV_180 * 0.5f);

    auto const aspect = static_cast<float>(w_) / static_cast<float>(h_);

#if _CRUS_OBSOLETE
    auto const kA = zNear / (zNear - zFar);
    auto const kB = zNear * zFar / (zNear - zFar);
#endif

    auto const kA = (zNear + zFar) / (zNear - zFar);
    auto const kB = 2.0f * zNear * zFar / (zNear - zFar);

    proj_ = math::Matrix
    (
        f / aspect, 0.f, 0.f, 0.f,
        0.f, f, 0.f, 0.f,
        0.f, 0.f, kA, kB,
        0.f, 0.f, -1.f, 0.f
    );

    glViewport(x_, y_, w_, h_);

    float const w2 = w_ * 0.5f;
    float const h2 = h_ * 0.5f;

    viewport_ = math::Matrix
    (
        w2, 0.f, 0.f, 0.f,
        0.f, h2, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        w2 + x_, h2 + y_, 0.0f, 1.0f
    );
}

void Viewport::SetCamera(Camera *const _cam)
{
    cam_ = _cam;

    SetViewport(x_, y_, w_, h_);
}

void Viewport::MakeCurrent() const
{
    glViewport(x_, y_, w_, h_);
}
};