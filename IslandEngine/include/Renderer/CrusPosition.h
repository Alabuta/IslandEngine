/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: vertex attributes declaration.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_POSITION_H              // Include guard "CrusPosition.h"
#define CRUS_POSITION_H
#include <string>
#include <sstream>

#include "Math\CrusMath.h"

namespace isle {
struct Position {

    Position() = default;

    template<class T, typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::array<float, 3>>>...>
    constexpr Position(T &&xyz) : xyz(std::forward(xyz)) { };
    constexpr Position(float x, float y, float z = 0) : xyz({ x, y, z }) { };

    bool operator== (Position const &position)  const;
    bool operator!= (Position const &position) const;

    explicit operator std::string() const;

    friend std::ostream &operator<< (std::ostream &stream, Position const &position);

    union {
        struct {
            float x, y, z;
        };

        std::array<float, 3> xyz;
    };
};
};

#include "Renderer\CrusPosition.inl"

#endif // CRUS_POSITION_H