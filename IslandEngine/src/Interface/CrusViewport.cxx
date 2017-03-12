/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 1st May 2012.
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
    float const kFOV = 72.0f, zNear = 0.01f, zFar = 1000.0f;//4194304.0f;//4194304.0f
    float const cotang = 1.0f / tanf(kFOV * math::kPI_DIV_180 * 0.5f);

    float const aspect = static_cast<float>(w_) / static_cast<float>(h_);

#if _CRUS_OBSOLETE
    float const kA = (zNear + zFar) / (zNear - zFar);
    float const kB = 2.0f * zNear * zFar / (zNear - zFar);
#endif

    float const kA = zNear / (zNear - zFar);
    float const kB = zNear * zFar / (zNear - zFar);

    proj_ = math::Matrix
    (
        cotang / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, cotang, 0.0f, 0.0f,
        0.0f, 0.0f, kA, kB,
        0.0f, 0.0f, -1.0f, 0.0f
    );

    glViewport(x_, y_, w_, h_);
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