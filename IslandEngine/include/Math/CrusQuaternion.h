/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 10th March 2010.
****    Description: quaternion.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_QUATERNION_H           // Include guard "CrusQuaternion.h"
#define CRUS_QUATERNION_H

#include "Math\CrusMath.h"

namespace isle::math {
class Quaternion {
public:
    UNIT_TEST_HERITABLE_CLASS;

    Quaternion();
    ~Quaternion();

    Quaternion(Quaternion const &q);

    explicit Quaternion(float const q[]);
    explicit Quaternion(float w, float x, float y, float z);

    Quaternion(Quaternion &&q);

    float w() const;
    float x() const;
    float y() const;
    float z() const;

    float const *const q() const;

    float w(float w);
    float x(float x);
    float y(float y);
    float z(float z);

    float &w();
    float &x();
    float &y();
    float &z();

    Quaternion operator+ (Quaternion const &q) const;
    Quaternion operator- (Quaternion const &q) const;
    Quaternion operator^ (Quaternion const &q) const;   // Quaternion mult.

    float operator* (Quaternion const &q) const;        // Dot product.

    Quaternion operator+ (float s) const;
    Quaternion operator- (float s) const;
    Quaternion operator* (float s) const;
    Quaternion operator/ (float s) const;

    Quaternion const &operator= (Quaternion const &q);
    Quaternion const &operator= (float s);

    Quaternion const &operator= (Quaternion &&q);

    bool operator== (Quaternion const &q) const;
    bool operator!= (Quaternion const &q) const;

    Quaternion const &operator+= (Quaternion const &q);
    Quaternion const &operator-= (Quaternion const &q);
    Quaternion const &operator^= (Quaternion const &q);

    Quaternion const &operator+= (float s);
    Quaternion const &operator-= (float s);
    Quaternion const &operator*= (float s);
    Quaternion const &operator/= (float s);

    Quaternion const &operator+ () const;
    Quaternion operator- () const;

    float GetNorm() const;
    float GetLenght() const;

    Quaternion const &Conjugate();
    Quaternion const &Inverse();

    Quaternion const &Normalize();
    static Quaternion GetNormalized(float const *const q);
    static Quaternion GetNormalized(float w, float x, float y, float z);

    Quaternion const &FromAxisAngle(float const *const axis, float angle);
    Quaternion const &FromMatrix4x4(float const *const m);

    static Quaternion GetFromAxisAngle(float const *const axis, float angle);
    static Quaternion GetFromMatrix4x4(float const *const m);

    void ToAxisAngle(float *const axis, float &angle);

private:
    union {
        struct {
            float w_, x_, y_, z_;
        };

        struct {
            float q_[4];
        };
    };
};
};

#include "Math\CrusQuaternion.inl"

#endif // CRUS_QUATERNION_H