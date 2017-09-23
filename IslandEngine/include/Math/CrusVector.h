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

#include <array>

#include "Math\CrusMath.h"

namespace isle::math {
class Vector {
public:
    UNIT_TEST_HERITABLE_CLASS;

    Vector() = default;

    constexpr Vector(Vector const &vector) = default;
    constexpr Vector(Vector &&vector) = default;

    /*template<class T, typename std::enable_if_t<std::is_same_v<std::decay_t<T>, Vector>>...>
    constexpr Vector(T &&vector) : uv(std::forward<typename T::uv>(vector.uv)) { };*/

    template<class T, typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::array<float, 3>>>...>
    constexpr Vector(T &&array) : xyz(std::forward<T>(array)) { };

    constexpr Vector(float x, float y, float z) : xyz({x, y, z}) { };

    Vector operator+ (Vector const &v) const;
    Vector operator- (Vector const &v) const;
    Vector operator^ (Vector const &v) const;       // Cross product.

    float operator* (Vector const &v) const;        // Dot product.

    Vector operator+ (float s) const;
    Vector operator- (float s) const;
    Vector operator* (float s) const;
    Vector operator/ (float s) const;

    Vector const &operator= (float s);

    Vector const &operator= (Vector const &v);
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

    template<class T, typename std::enable_if_t<std::is_same_v<std::decay_t<T>, Vector>>...>
    static Vector GetNormalized(T &&v)
    {
        if constexpr (std::is_rvalue_reference_v<T>)
            return std::move(v.Normalize());

        else return Vector(v).Normalize();
    }

    static Vector GetNormalized(float x, float y, float z);

    static Vector One();

    friend std::ostream &operator<< (std::ostream &stream, Vector const &vector);

    union {
        struct {
            float x, y, z;
        };

        std::array<float, 3> xyz;
    };
};
};

#include "Math\CrusVector.inl"

#endif // CRUS_VECTOR_H