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

    /*constexpr Vector(Vector const &vector) = default;
    constexpr Vector(Vector &&vector) = default;*/

    template<class T, typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::array<float, 3>>>...>
    constexpr Vector(T &&xyz) : xyz(std::forward<T>(xyz)) { };
    constexpr Vector(float x, float y, float z) : xyz({ x, y, z }) { };

    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    Vector operator+ (T &&v) const;
    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    Vector operator- (T &&v) const;
    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    Vector operator^ (T &&v) const;       // Cross product.

    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    float operator* (T &&v) const;        // Dot product.

    Vector operator+ (float s) const;
    Vector operator- (float s) const;
    Vector operator* (float s) const;
    Vector operator/ (float s) const;

    Vector const &operator= (float s);

    /*Vector &operator= (Vector const &v);
    Vector &operator= (Vector &&v);*/

    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    bool operator== (T &&v) const;
    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    bool operator!= (T &&v) const;

    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    Vector const &operator+= (T &&v);
    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    Vector const &operator-= (T &&v);
    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    Vector const &operator^= (T &&v);

    Vector const &operator+= (float s);
    Vector const &operator-= (float s);
    Vector const &operator*= (float s);
    Vector const &operator/= (float s);

    Vector &operator+ ();
    Vector operator- () const;

    Vector &operator++ ();
    Vector &operator-- ();

    float GetNorm() const;
    float GetLenght() const;

    Vector const &Normalize();

    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    Vector &LerpStable(T &&v, float alpha);

    /*Vector &LerpStable();
    Vector LerpStable() const;

    static Vector LerpStable();*/

    template<class T, typename std::enable_if_t<is_vector_t<T>>...>
    static Vector GetNormalized(T &&v);

    static Vector GetNormalized(float x, float y, float z);

    constexpr static Vector One();

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