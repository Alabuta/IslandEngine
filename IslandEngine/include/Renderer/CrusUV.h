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
struct UV {

    UV() = default;

    template<class T, typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::array<float, 2>>>...>
    constexpr UV(T &&uv) : uv(std::forward(uv)) { };
    constexpr UV(float u, float v) : uv({u, v}) { };

    bool operator== (UV const &v) const;
    bool operator!= (UV const &v) const;

    explicit operator std::string() const;

    friend std::ostream &operator<< (std::ostream &stream, UV const &uv);

    union {
        struct {
            float u, v;
        };

        std::array<float, 2> uv;
    };
};
};

#include "Renderer\CrusUV.inl"

#endif // CRUS_UV_H