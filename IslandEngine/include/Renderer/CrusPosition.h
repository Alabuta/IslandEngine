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

    template<class T, typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::array<float, 3>>>...>
    constexpr Position(T &&xyz) : xyz(std::forward(xyz)) { };
    constexpr Position(float x, float y, float z = 0) : xyz({x, y, z}) { };

    constexpr float x(float x) const { return xyz[0]; };
    constexpr float y(float y) const { return xyz[1]; };
    constexpr float z(float z) const { return xyz[2]; };

    bool operator== (Position const &p)  const
    {
        return isle::math::CloseEnough(xyz[0], p.xyz[0])
            && isle::math::CloseEnough(xyz[1], p.xyz[1])
            && isle::math::CloseEnough(xyz[2], p.xyz[2]);
    }

    bool operator!= (Position const &p) const { return !(*this == p); }

    std::ostream &ToStream(std::ostream &_stream) const
    {
        return _stream << xyz[0] << "; " << xyz[1] << "; " << xyz[2];
    }

    explicit operator std::string() const
    {
        static std::ostringstream ss;
        ss.str("");

        ss << xyz[0] << "; " << xyz[1] << "; " << xyz[2];

        return ss.good() ? ss.str() : "[undefined]";
    }

    std::array<float, 3> xyz;
};


std::ostream &operator<< (std::ostream &stream, Position const &p)
{
    return p.ToStream(stream);
}
};

//#include "Renderer\CrusPosition.inl"

#endif // CRUS_POSITION_H