/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 14th August 2009.
****    Description: three-component vector definition.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_VECTOR_H               // Include guard "CrusVector.h"
#define CRUS_VECTOR_H

#include "Math\CrusMath.h"

namespace isle
{
namespace math
{
class Vector {
public:
    UNIT_TEST_HERITABLE_CLASS

    Vector();
    ~Vector();

    Vector(Vector const &v);

    explicit Vector(float const v[]);
    explicit Vector(float x, float y, float z);

    float x() const;
    float y() const;
    float z() const;

    float const *const v() const;
    float *v();

    float x(float x);
    float y(float y);
    float z(float z);

    float &x();
    float &y();
    float &z();

    Vector operator+ (Vector const &v) const;
    Vector operator- (Vector const &v) const;
    Vector operator^ (Vector const &v) const;       // Cross product.

    float operator* (Vector const &v) const;        // Dot product.

    Vector operator+ (float s) const;
    Vector operator- (float s) const;
    Vector operator* (float s) const;
    Vector operator/ (float s) const;

    Vector const &operator= (Vector const &v);
    Vector const &operator= (float s);

    bool operator== (Vector const &v) const;
    bool operator!= (Vector const &v) const;

    Vector const &operator+= (Vector const &v);
    Vector const &operator-= (Vector const &v);
    Vector const &operator^= (Vector const &v);

    Vector const &operator+= (float s);
    Vector const &operator-= (float s);
    Vector const &operator*= (float s);
    Vector const &operator/= (float s);

    Vector const &operator+ () const;
    Vector operator- () const;

    Vector const &operator++ ();
    Vector const &operator-- ();

    float GetNorm() const;
    float GetLenght() const;

    Vector const &Normalize();

    static Vector GetNormalized(float const *const v);
    static Vector GetNormalized(float x, float y, float z);

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
};

#include "Math\CrusVector.inl"

#endif // CRUS_VECTOR_H