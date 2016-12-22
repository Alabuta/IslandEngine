/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 1st May 2012.
****    Description: viewport routines.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_VIEWPORT_H             // Include guard "CrusViewport.h"
#define CRUS_VIEWPORT_H

#include "Camera\CrusCamera.h"

namespace isle {
namespace intf {
class Viewport {
public:
    enum class eVIEWPORT_TYPE {
        nORTHOGRAPHIC = 1, nPERSPECTIVE, nSTEREO
    };

    Viewport() : x_(-1), y_(-1), w_(-1), h_(-1), cam_{nullptr} {};
    ~Viewport() {};

    void Create(Viewport::eVIEWPORT_TYPE type);

    void SetViewport(int16 x, int16 y, int16 w, int16 h);
    void SetCamera(Camera *const cam);

    void MakeCurrent() const;

    Camera &cam();

    math::Matrix const &proj() const;
    math::Matrix projView() const;

    int16 x() const;
    int16 y() const;
    int16 w() const;
    int16 h() const;

private:
    int16 x_, y_, w_, h_;

    math::Matrix proj_;
    Camera *cam_;

    struct {
        bool IsActive_ : 1;
        bool IsMain_ : 1;         // The viewport occupies the entire space.
        bool _reserved_ : 6;
    };

    struct {
        uint8 fill_mode_ : 2;     // Point, wireframe or solid.
        uint8 cull_mode_ : 2;     // Off, back, front and both faces.
        bool _reserved_ : 4;
    };
};

inline math::Matrix const &Viewport::proj() const
{
    return proj_;
}

inline math::Matrix Viewport::projView() const
{
    return proj_ * cam_->view();
}

inline Camera &Viewport::cam()
{
    return *cam_;
}

inline int16 Viewport::x() const
{
    return x_;
}

inline int16 Viewport::y() const
{
    return y_;
}

inline int16 Viewport::w() const
{
    return w_;
}

inline int16 Viewport::h() const
{
    return h_;
}
};
};

#endif // CRUS_VIEWPORT_H