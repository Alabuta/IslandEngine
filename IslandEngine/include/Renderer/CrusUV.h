/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****    Description: vertex attributes declaration.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_UV_H                   // Include guard "CrusUV.h"
#define CRUS_UV_H

#include "Math\CrusMath.h"

namespace isle {
class UV {
public:

    UV(UV const &v);

    explicit UV(float const v[]);
    explicit UV(float x, float y);

    UV(UV &&v);

    float x() const;
    float y() const;

    float const *const v() const;

    float x(float x);
    float y(float y);

    UV const &operator= (UV const &v);
    UV const &operator= (float s);

    UV const &operator= (UV &&v);

    bool operator== (UV const &v) const;
    bool operator!= (UV const &v) const;

    acstr str();

private:
    union {
        struct {
            float x_, y_;
        };

        struct {
            float v_[2];
        };
    };
};
};

#include "Renderer\CrusUV.inl"

#endif // CRUS_UV_H