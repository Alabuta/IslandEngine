/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: three-component vector definition.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_VECTOR_H               // Include guard "CrusVector.h"
#define CRUS_VECTOR_H

#include "Math\CrusMath.h"

namespace isle::math {
class Vector {
public:
    UNIT_TEST_HERITABLE_CLASS;

    explicit Vector() = default;

    Vector(Vector const &v);

    Vector(std::array<float, 3> &&vec);
    Vector(std::array<float, 3> const &vec);

    explicit Vector(float x, float y, float z);

    Vector(Vector &&v);

    float x() const;
    float y() const;
    float z() const;

    std::array<float, 3> const &v() const;
    std::array<float, 3> &v();

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

    Vector const &operator= (Vector &&v);

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

    static Vector GetNormalized(Vector const &v);
    static Vector GetNormalized(float x, float y, float z);

    static Vector One();

    void ToStream(std::ostream &stream) const;

private:
    union {
        struct {
            float x_, y_, z_;
        };

        std::array<float, 3> vec_;
    };
};
};

#include "Math\CrusVector.inl"

#endif // CRUS_VECTOR_H