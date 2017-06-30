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

    explicit Matrix() = default;

    Matrix(Matrix &&m);
    Matrix(Matrix const &m);

    Matrix(std::array<float, 16> &&vec);
    Matrix(std::array<float, 16> const &vec);

    //explicit Matrix(Vector const &position, Vector const &rotation, Vector const &sizing);

    explicit Matrix(float m00, float m01, float m02, float m03,
                    float m04, float m05, float m06, float m07,
                    float m08, float m09, float m10, float m11,
                    float m12, float m13, float m14, float m15);

    std::array<float, 16> const &m() const;
    std::array<float, 16> &m();

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

    Matrix const &RotateOnOX(float angle);
    Matrix const &RotateOnOY(float angle);
    Matrix const &RotateOnOZ(float angle);

    Matrix const &Rotate(Vector const &axis, float angle);
    Matrix const &Rotate(float x, float y, float z, float angle);

    Matrix const &Scale(float x, float y, float z);
    Matrix const &Translate(float x, float y, float z);

    float Minor(uint8, uint8, uint8, uint8, uint8, uint8) const;
    float Det() const;

    Matrix Inverse() const;

    static Matrix Transpose(Matrix const &m);
    Matrix const &Transpose();

    static Matrix Identity();
    Matrix const &MakeIdentity();

    Vector TransformPosition(Vector const &p) const;
    Vector TransformPosition(Vector &&p) const;
    void TransformPosition(Vector &p) const;

    Vector TransformVector(Vector const &v) const;
    Vector TransformVector(Vector &&v) const;
    void TransformVector(Vector &v) const;

    Matrix const &FromQuaternion(float const q[]);
    // :TODO: maybe it was better if deleted.
    //static Matrix GetFromQuaternion(float const *const q);

    Vector &xAxis();
    Vector &yAxis();
    Vector &zAxis();

    float &xOrigin();
    float &yOrigin();
    float &zOrigin();

    Vector origin() const;

private:
    union alignas(sizeof(__m128))
    {
        // A little tricky, but it necessary. ;)
        struct {
            Vector xAxis_; float x_;
            Vector yAxis_; float y_;
            Vector zAxis_; float z_;
            float div_[4];
        };

        std::array<float, 16> vec_;

#if CRUS_USE_SSE_MATH
        __m128 row_[4];
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