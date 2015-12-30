/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 14th August 2009.
****    Description: Matrix4x4 definition.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_MATRIX_H               // Include guard "CrusMatrix.h"
#define CRUS_MATRIX_H

#include "Math\CrusMath.h"

namespace isle
{
namespace math
{
class Matrix {
public:
    UNIT_TEST_HERITABLE_CLASS

    Matrix();
    ~Matrix();

    Matrix(Matrix const &m);

    explicit Matrix(Vector const &position, Vector const &rotation, Vector const &sizing);
    explicit Matrix(float const m[]);
    explicit Matrix(float m0,   float m1,   float m2,   float m3,
                    float m4,   float m5,   float m6,   float m7,
                    float m8,   float m9,   float m10,  float m11,
                    float m12,  float m13,  float m14,  float m15);

    float const *const m() const;
    float *m();

    Matrix operator+ (Matrix const &m) const;
    Matrix operator- (Matrix const &m) const;
    Matrix operator* (Matrix const &m) const;
    Matrix operator/ (Matrix const &m) const;

    Matrix operator+ (float s) const;
    Matrix operator- (float s) const;
    Matrix operator* (float s) const;
    Matrix operator/ (float s) const;

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
    Matrix Transpose() const;

    static Matrix GetIdentity();
    Matrix const &MakeIdentity();

    void TransformPoint(float *const p) const;
    void TransformVector(float *const v) const;

    Matrix const &FromQuaternion(float const q[]);
    // :TODO: maybe it was better if deleted.
    //static Matrix GetFromQuaternion(float const *const q);

private:
    union {
        struct {
            float m0_,   m1_,   m2_,   m3_;
            float m4_,   m5_,   m6_,   m7_;
            float m8_,   m9_,   m10_,  m11_;
            float m12_,  m13_,  m14_,  m15_;
        };

        struct {
            float m_[16];
        };
    };
};
};
};

#include "Math\CrusMatrix.inl"

#endif // CRUS_MATRIX_H