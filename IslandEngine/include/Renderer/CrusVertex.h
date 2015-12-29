/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    26th April 2012 / 12th March 2010.
****    Description: mesh attributes declaration.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_VERTEX_H               // Include guard "CrusVertex.h"
#define CRUS_VERTEX_H

#include "Math\CrusMath.h"

namespace isle
{
class Vertex {
public:

    Vertex();
    ~Vertex();

    explicit Vertex(float const v[]);
    explicit Vertex(float x, float y, float z);

    float x() const;
    float y() const;
    float z() const;

    float const *const v() const;

    float x(float x);
    float y(float y);
    float z(float z);

    Vertex const &operator= (Vertex const &v);
    Vertex const &operator= (float s);

    bool operator== (Vertex const &v) const;
    bool operator!= (Vertex const &v) const;

    acstr str();

private:
    float x_, y_, z_;
};
};

#include "Renderer\CrusVertex.inl"

#endif // CRUS_VERTEX_H