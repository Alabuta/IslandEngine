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

namespace isle::math {
class Matrix {
public:
    UNIT_TEST_HERITABLE_CLASS;

    explicit Matrix() = default;

    Matrix(Matrix &&m);
    Matrix(Matrix const &m);

    explicit Matrix(std::array<float, 16> const &vec);

    //explicit Matrix(Vector const &position, Vector const &rotation, Vector const &sizing);

    explicit Matrix(float m00, float m01, float m02, float m03,
                    float m04, float m05, float m06, float m07,
                    float m08, float m09, float m10, float m11,
                    float m12, float m13, float m14, float m15);

    std::array<float, 16> const &m() const;
    std::array<float, 16> &m();

    Matrix const &Mult(Matrix const &m);

    Matrix operator+ (Matrix const &m) const;
    Matrix operator- (Matrix const &m) const;
    Matrix operator* (Matrix const &m) const;
    Matrix operator/ (Matrix const &m) const;

    Matrix operator+ (float s) const;
    Matrix operator- (float s) const;
    Matrix operator* (float s) const;
    Matrix operator/ (float s) const;

    Matrix const &operator= (Matrix &&m);
    Matrix const &operator= (Matrix const &m);

    bool operator== (Matrix const &m) const;
    bool operator!= (Matrix const &m) const;

    Matrix const &operator+= (Matrix const &m);
    Matrix const &operator-= (Matrix const &m);
    Matrix const &operator*= (Matrix const &m);
    Matrix const &operator/= (Matrix const &m);

    Matrix const &operator+= (float s);
    Matrix const &operator-= (float s);
    Matrix const &operator*= (float s);
    Matrix const &operator/= (float s);

    Matrix const &RotateOnOX(float angle);
    Matrix const &RotateOnOY(float angle);
    Matrix const &RotateOnOZ(float angle);

    Matrix const &Rotate(float const *const axis, float angle);
    Matrix const &Rotate(float x, float y, float z, float angle);

    Matrix const &Scale(float x, float y, float z);
    Matrix const &Translate(float x, float y, float z);

    float Minor(uint8, uint8, uint8, uint8, uint8, uint8) const;
    float Det() const;

    Matrix Inverse() const;

    static Matrix Transpose(Matrix const &m);
    Matrix const &Transpose();

    static Matrix GetIdentity();
    Matrix const &MakeIdentity();

    void TransformPoint(float *const p) const;
    void TransformVector(float *const v) const;

    Matrix const &FromQuaternion(float const q[]);
    // :TODO: maybe it was better if deleted.
    //static Matrix GetFromQuaternion(float const *const q);

private:
    union alignas(sizeof(__m128))
    {
        struct {
            float _00_, _01_, _02_, _03_;
            float _04_, _05_, _06_, _07_;
            float _08_, _09_, _10_, _11_;
            float _12_, _13_, _14_, _15_;
        } m_;

        std::array<float, 16> vec_;
#ifdef CRUS_USE_SSE_MATH
        __m128 row_[4];
#endif
    };
};
};

#include "Math\CrusMatrix.inl"

#endif // CRUS_MATRIX_H