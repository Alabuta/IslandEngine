/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: Matrix4x4 definition.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_MATRIX_H               // Include guard "CrusMatrix.h"
#define CRUS_MATRIX_H

#include "Math\CrusMath.h"
#include "Math\CrusVector.h"

namespace isle::math {
class Matrix {
public:
    UNIT_TEST_HERITABLE_CLASS;

    Matrix() = default;

    constexpr Matrix(Matrix const &m) = default;
    constexpr Matrix(Matrix &&m) = default;

    template<class T, typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::array<float, 16>>>...>
    constexpr Matrix(T &&array) : m(std::forward<T>(array)) { };

    //explicit Matrix(Vector const &position, Vector const &rotation, Vector const &sizing);

    /*constexpr Matrix(float m00, float m01, float m02, float m03,
                     float m04, float m05, float m06, float m07,
                     float m08, float m09, float m10, float m11,
                     float m12, float m13, float m14, float m15);*/

    constexpr Matrix(float _m00, float _m01, float _m02, float _m03, float _m04, float _m05, float _m06, float _m07,
                     float _m08, float _m09, float _m10, float _m11, float _m12, float _m13, float _m14, float _m15) : 
        m({_m00, _m01, _m02, _m03, _m04, _m05, _m06, _m07, _m08, _m09, _m10, _m11, _m12, _m13, _m14, _m15}) { };

    Matrix operator+ (Matrix const &m) const;
    Matrix operator- (Matrix const &m) const;
    Matrix operator* (Matrix const &m) const;
    Matrix operator/ (Matrix const &m) const;

    Matrix operator+ (float s) const;
    Matrix operator- (float s) const;
    Matrix operator* (float s) const;
    Matrix operator/ (float s) const;

    Matrix &operator= (Matrix &&m);
    Matrix &operator= (Matrix const &m);

    bool operator== (Matrix const &m) const;
    bool operator!= (Matrix const &m) const;

    Matrix &operator+= (Matrix const &m);
    Matrix &operator-= (Matrix const &m);
    Matrix &operator*= (Matrix const &m);
    Matrix &operator/= (Matrix const &m);

    Matrix &operator+= (float s);
    Matrix &operator-= (float s);
    Matrix &operator*= (float s);
    Matrix &operator/= (float s);

    Matrix &RotateOnOX(float angle);
    Matrix &RotateOnOY(float angle);
    Matrix &RotateOnOZ(float angle);

    Matrix &Rotate(Vector const &axis, float angle);
    Matrix &Rotate(float x, float y, float z, float angle);

    Matrix &Scale(float x, float y, float z);
    Matrix &Translate(float x, float y, float z);

    float Minor(u8, u8, u8, u8, u8, u8) const;
    float Det() const;

    static Matrix Inverse(Matrix const &m);
    Matrix &Inverse();

    static Matrix Transpose(Matrix const &m);
    Matrix &Transpose();

    static Matrix Identity();
    Matrix &MakeIdentity();

    Vector TransformPosition(Vector const &p) const;
    Vector TransformPosition(Vector &&p) const;
    void TransformPosition(Vector &p) const;

    Vector TransformVector(Vector const &v) const;
    Vector TransformVector(Vector &&v) const;
    void TransformVector(Vector &v) const;

    Matrix &FromQuaternion(float const q[]);
    // :TODO: perhaps it's better to delete.
    //static Matrix GetFromQuaternion(float const *const q);

    static Matrix GetNormalMatrix(Matrix const &ModelViewMatrix);

    friend std::ostream &operator<< (std::ostream &stream, Matrix const &matrix);

#if CRUS_USE_SSE_MATH
    union alignas(sizeof(__m128))
#else
    union
#endif
    {
        // A little tricky, but it necessary. ;)
#if NOT_YET_IMPLEMENTED
        struct {
            Vector xAxis; float _03_;
            Vector yAxis; float _07_;
            Vector zAxis; float _11_;
            Vector pos;   float w;
        };
#endif
        struct {
            Vector xAxis; float x;
            Vector yAxis; float y;
            Vector zAxis; float z;
            float _12_, _13_, _14_, _15_;
        };

        std::array<float, 16> m;

#if CRUS_USE_SSE_MATH
        __m128 rows_[4];
#endif

        struct {
            float _00_, _01_, _02_, _03_;
            float _04_, _05_, _06_, _07_;
            float _08_, _09_, _10_, _11_;
            float _12_, _13_, _14_, _15_;
        };
    };
};
};

#include "Math\CrusMatrix.inl"

#endif // CRUS_MATRIX_H