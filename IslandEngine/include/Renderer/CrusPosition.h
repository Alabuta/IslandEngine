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

#include "Math\CrusMath.h"

namespace isle {
class Position {
public:

    Position(Position const &v);

    explicit Position(float const v[]);
    explicit Position(float x, float y, float z = 0);

    Position(Position &&v);

    float x() const;
    float y() const;
    float z() const;

    float const *const v() const;

    float x(float x);
    float y(float y);
    float z(float z);

    Position const &operator= (Position const &v);
    Position const &operator= (float s);

    Position const &operator= (Position &&v);

    bool operator== (Position const &v) const;
    bool operator!= (Position const &v) const;

    std::string ToString() const;
    void ToStream(std::ostream &stream) const;

    operator std::string() const;

private:
    union {
        struct {
            float x_, y_, z_;
        };

        struct {
            float v_[3];
        };
    };
};
};

#include "Renderer\CrusPosition.inl"

#endif // CRUS_POSITION_H