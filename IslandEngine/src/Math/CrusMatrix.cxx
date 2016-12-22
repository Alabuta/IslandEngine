/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 10th March 2010.
****    Description: Matrix4x4 implementation.
****
********************************************************************************************************************************/
#include "System\CrusTypes.h"
#include "Math\CrusMatrix.h"

namespace isle {
namespace math {
Matrix::Matrix(float _m0, float _m1, float _m2, float _m3,
    float _m4, float _m5, float _m6, float _m7,
    float _m8, float _m9, float _m10, float _m11,
    float _m12, float _m13, float _m14, float _m15)
{
    m0_ = _m0;   m1_ = _m1;   m2_ = _m2;  m3_ = _m3;
    m4_ = _m4;   m5_ = _m5;   m6_ = _m6;  m7_ = _m7;
    m8_ = _m8;   m9_ = _m9;   m10_ = _m10; m11_ = _m11;
    m12_ = _m12;  m13_ = _m13;  m14_ = _m14; m15_ = _m15;
}

/*Matrix::Matrix(Vector const &_position, Vector const &_rotation, Vector const &_sizing)
{
    Matrix mTRS;
    mTRS.MakeIdentity();

    mTRS *= 1;
}*/

Matrix Matrix::operator+ (Matrix const &_m) const
{
    return Matrix
        (
        m0_ + _m.m0_, m1_ + _m.m1_, m2_ + _m.m2_, m3_ + _m.m3_,
        m4_ + _m.m4_, m5_ + _m.m5_, m6_ + _m.m6_, m7_ + _m.m7_,
        m8_ + _m.m8_, m9_ + _m.m9_, m10_ + _m.m10_, m11_ + _m.m11_,
        m12_ + _m.m12_, m13_ + _m.m13_, m14_ + _m.m14_, m15_ + _m.m15_
        );
}

Matrix Matrix::operator- (Matrix const &_m) const
{
    return Matrix
        (
        m0_ - _m.m0_, m1_ - _m.m1_, m2_ - _m.m2_, m3_ - _m.m3_,
        m4_ - _m.m4_, m5_ - _m.m5_, m6_ - _m.m6_, m7_ - _m.m7_,
        m8_ - _m.m8_, m9_ - _m.m9_, m10_ - _m.m10_, m11_ - _m.m11_,
        m12_ - _m.m12_, m13_ - _m.m13_, m14_ - _m.m14_, m15_ - _m.m15_
        );
}

// 64 mul; 48 sum.
Matrix Matrix::operator* (Matrix const &_m) const
{
    return Matrix
        (
        m0_  * _m.m0_ + m1_  * _m.m4_ + m2_  * _m.m8_ + m3_  * _m.m12_,
        m0_  * _m.m1_ + m1_  * _m.m5_ + m2_  * _m.m9_ + m3_  * _m.m13_,
        m0_  * _m.m2_ + m1_  * _m.m6_ + m2_  * _m.m10_ + m3_  * _m.m14_,
        m0_  * _m.m3_ + m1_  * _m.m7_ + m2_  * _m.m11_ + m3_  * _m.m15_,

        m4_  * _m.m0_ + m5_  * _m.m4_ + m6_  * _m.m8_ + m7_  * _m.m12_,
        m4_  * _m.m1_ + m5_  * _m.m5_ + m6_  * _m.m9_ + m7_  * _m.m13_,
        m4_  * _m.m2_ + m5_  * _m.m6_ + m6_  * _m.m10_ + m7_  * _m.m14_,
        m4_  * _m.m3_ + m5_  * _m.m7_ + m6_  * _m.m11_ + m7_  * _m.m15_,

        m8_  * _m.m0_ + m9_  * _m.m4_ + m10_ * _m.m8_ + m11_ * _m.m12_,
        m8_  * _m.m1_ + m9_  * _m.m5_ + m10_ * _m.m9_ + m11_ * _m.m13_,
        m8_  * _m.m2_ + m9_  * _m.m6_ + m10_ * _m.m10_ + m11_ * _m.m14_,
        m8_  * _m.m3_ + m9_  * _m.m7_ + m10_ * _m.m11_ + m11_ * _m.m15_,

        m12_ * _m.m0_ + m13_ * _m.m4_ + m14_ * _m.m8_ + m15_ * _m.m12_,
        m12_ * _m.m1_ + m13_ * _m.m5_ + m14_ * _m.m9_ + m15_ * _m.m13_,
        m12_ * _m.m2_ + m13_ * _m.m6_ + m14_ * _m.m10_ + m15_ * _m.m14_,
        m12_ * _m.m3_ + m13_ * _m.m7_ + m14_ * _m.m11_ + m15_ * _m.m15_
        );
}

Matrix Matrix::operator+ (float _s) const
{
    return Matrix
        (
        m0_ + _s, m1_ + _s, m2_ + _s, m3_ + _s,
        m4_ + _s, m5_ + _s, m6_ + _s, m7_ + _s,
        m8_ + _s, m9_ + _s, m10_ + _s, m11_ + _s,
        m12_ + _s, m13_ + _s, m14_ + _s, m15_ + _s
        );
}

Matrix Matrix::operator- (float _s) const
{
    return Matrix
        (
        m0_ - _s, m1_ - _s, m2_ - _s, m3_ - _s,
        m4_ - _s, m5_ - _s, m6_ - _s, m7_ - _s,
        m8_ - _s, m9_ - _s, m10_ - _s, m11_ - _s,
        m12_ - _s, m13_ - _s, m14_ - _s, m15_ - _s
        );
}

Matrix Matrix::operator* (float _s) const
{
    return Matrix
        (
        m0_  * _s, m1_  * _s, m2_  * _s, m3_  * _s,
        m4_  * _s, m5_  * _s, m6_  * _s, m7_  * _s,
        m8_  * _s, m9_  * _s, m10_ * _s, m11_ * _s,
        m12_ * _s, m13_ * _s, m14_ * _s, m15_ * _s
        );
}

bool Matrix::operator== (Matrix const &_m) const
{
    return CloseEnough(m0_, _m.m0_) && CloseEnough(m1_, _m.m1_)
        && CloseEnough(m2_, _m.m2_) && CloseEnough(m3_, _m.m3_)
        && CloseEnough(m4_, _m.m4_) && CloseEnough(m5_, _m.m5_)
        && CloseEnough(m6_, _m.m6_) && CloseEnough(m7_, _m.m7_)
        && CloseEnough(m8_, _m.m8_) && CloseEnough(m9_, _m.m9_)
        && CloseEnough(m10_, _m.m10_) && CloseEnough(m11_, _m.m11_)
        && CloseEnough(m12_, _m.m12_) && CloseEnough(m13_, _m.m13_)
        && CloseEnough(m14_, _m.m14_) && CloseEnough(m15_, _m.m15_);
}

Matrix const &Matrix::operator+= (Matrix const &_m)
{
    m0_ += _m.m0_;   m1_ += _m.m1_;   m2_ += _m.m2_;  m3_ += _m.m3_;
    m4_ += _m.m4_;   m5_ += _m.m5_;   m6_ += _m.m6_;  m7_ += _m.m7_;
    m8_ += _m.m8_;   m9_ += _m.m9_;   m10_ += _m.m10_; m11_ += _m.m11_;
    m12_ += _m.m12_;  m13_ += _m.m13_;  m14_ += _m.m14_; m15_ += _m.m15_;

    return *this;
}

Matrix const &Matrix::operator-= (Matrix const &_m)
{
    m0_ -= _m.m0_;   m1_ -= _m.m1_;   m2_ -= _m.m2_;  m3_ -= _m.m3_;
    m4_ -= _m.m4_;   m5_ -= _m.m5_;   m6_ -= _m.m6_;  m7_ -= _m.m7_;
    m8_ -= _m.m8_;   m9_ -= _m.m9_;   m10_ -= _m.m10_; m11_ -= _m.m11_;
    m12_ -= _m.m12_;  m13_ -= _m.m13_;  m14_ -= _m.m14_; m15_ -= _m.m15_;

    return *this;
}

Matrix const &Matrix::operator*= (Matrix const &_m)
{
    m0_ = m0_  * _m.m0_ + m1_  * _m.m4_ + m2_  * _m.m8_ + m3_  * _m.m12_;
    m1_ = m0_  * _m.m1_ + m1_  * _m.m5_ + m2_  * _m.m9_ + m3_  * _m.m13_;
    m2_ = m0_  * _m.m2_ + m1_  * _m.m6_ + m2_  * _m.m10_ + m3_  * _m.m14_;
    m3_ = m0_  * _m.m3_ + m1_  * _m.m7_ + m2_  * _m.m11_ + m3_  * _m.m15_;

    m4_ = m4_  * _m.m0_ + m5_  * _m.m4_ + m6_  * _m.m8_ + m7_  * _m.m12_;
    m5_ = m4_  * _m.m1_ + m5_  * _m.m5_ + m6_  * _m.m9_ + m7_  * _m.m13_;
    m6_ = m4_  * _m.m2_ + m5_  * _m.m6_ + m6_  * _m.m10_ + m7_  * _m.m14_;
    m7_ = m4_  * _m.m3_ + m5_  * _m.m7_ + m6_  * _m.m11_ + m7_  * _m.m15_;

    m8_ = m8_  * _m.m0_ + m9_  * _m.m4_ + m10_ * _m.m8_ + m11_ * _m.m12_;
    m9_ = m8_  * _m.m1_ + m9_  * _m.m5_ + m10_ * _m.m9_ + m11_ * _m.m13_;
    m10_ = m8_  * _m.m2_ + m9_  * _m.m6_ + m10_ * _m.m10_ + m11_ * _m.m14_;
    m11_ = m8_  * _m.m3_ + m9_  * _m.m7_ + m10_ * _m.m11_ + m11_ * _m.m15_;

    m12_ = m12_ * _m.m0_ + m13_ * _m.m4_ + m14_ * _m.m8_ + m15_ * _m.m12_;
    m13_ = m12_ * _m.m1_ + m13_ * _m.m5_ + m14_ * _m.m9_ + m15_ * _m.m13_;
    m14_ = m12_ * _m.m2_ + m13_ * _m.m6_ + m14_ * _m.m10_ + m15_ * _m.m14_;
    m15_ = m12_ * _m.m3_ + m13_ * _m.m7_ + m14_ * _m.m11_ + m15_ * _m.m15_;

    return *this;
}

Matrix const &Matrix::operator+= (float _s)
{
    m0_ += _s; m1_ += _s; m2_ += _s; m3_ += _s;
    m4_ += _s; m5_ += _s; m6_ += _s; m7_ += _s;
    m8_ += _s; m9_ += _s; m10_ += _s; m11_ += _s;
    m12_ += _s; m13_ += _s; m14_ += _s; m15_ += _s;

    return *this;
}

Matrix const &Matrix::operator-= (float _s)
{
    m0_ -= _s; m1_ -= _s; m2_ -= _s; m3_ -= _s;
    m4_ -= _s; m5_ -= _s; m6_ -= _s; m7_ -= _s;
    m8_ -= _s; m9_ -= _s; m10_ -= _s; m11_ -= _s;
    m12_ -= _s; m13_ -= _s; m14_ -= _s; m15_ -= _s;

    return *this;
}

Matrix const &Matrix::operator*= (float _s)
{
    m0_ *= _s; m1_ *= _s; m2_ *= _s; m3_ *= _s;
    m4_ *= _s; m5_ *= _s; m6_ *= _s; m7_ *= _s;
    m8_ *= _s; m9_ *= _s; m10_ *= _s; m11_ *= _s;
    m12_ *= _s; m13_ *= _s; m14_ *= _s; m15_ *= _s;

    return *this;
}

Matrix const &Matrix::RotateOnOX(float _angle)
{
    //  1  0  0  0
    //  0  c -s  0
    //  0  s  c  0
    //  0  0  0  1

    float const s = sinf(DegToRad(_angle));
    float const c = cosf(DegToRad(_angle));

    m0_ = 1.0f; m1_ = 0.0f; m2_ = 0.0f;
    m4_ = 0.0f; m5_ = c;    m6_ = -s;
    m8_ = 0.0f; m9_ = s;    m10_ = c;

    return *this;
}

Matrix const &Matrix::RotateOnOY(float _angle)
{
    //  c  0  s  0
    //  0  1  0  0
    // -s  0  c  0
    //  0  0  0  1

    float const s = sinf(DegToRad(_angle));
    float const c = cosf(DegToRad(_angle));

    m0_ = c;   m1_ = 0.0f; m2_ = s;
    m4_ = 0.0f; m5_ = 1.0f; m6_ = 0.0f;
    m8_ = -s;   m9_ = 0.0f; m10_ = c;

    return *this;
}

Matrix const &Matrix::RotateOnOZ(float _angle)
{
    //  c -s  0  0
    //  s  c  0  0
    //  0  0  1  0
    //  0  0  0  1

    float const s = sinf(DegToRad(_angle));
    float const c = cosf(DegToRad(_angle));

    m0_ = c;    m1_ = -s;   m2_ = 0.0f; //m3_  = 0.0f;
    m4_ = s;    m5_ = c;   m6_ = 0.0f; //m7_  = 0.0f;
    m8_ = 0.0f; m9_ = 0.0f; m10_ = 1.0f; //m11_ = 0.0f;
    //m12_ = 0.0f; m13_ = 0.0f; m14_ = 0.0f; m15_ = 1.0f;

    return *this;
}

Matrix const &Matrix::Rotate(float const *const _axis, float _angle)
{
    float const s = sinf(DegToRad(_angle));
    float const c = cosf(DegToRad(_angle));

    m0_ = (_axis[0] * _axis[0]) * (1.0f - c) + c;
    m1_ = (_axis[0] * _axis[1]) * (1.0f - c) + _axis[2] * s;
    m2_ = (_axis[0] * _axis[2]) * (1.0f - c) - _axis[1] * s;
    m3_ = 0.0f;

    m4_ = (_axis[1] * _axis[0]) * (1.0f - c) - _axis[2] * s;
    m5_ = (_axis[1] * _axis[1]) * (1.0f - c) + c;
    m6_ = (_axis[1] * _axis[2]) * (1.0f - c) + _axis[0] * s;
    m7_ = 0.0f;

    m8_ = (_axis[2] * _axis[0]) * (1.0f - c) + _axis[1] * s;
    m9_ = (_axis[2] * _axis[1]) * (1.0f - c) - _axis[0] * s;
    m10_ = (_axis[2] * _axis[2]) * (1.0f - c) + c;
    m11_ = 0.0f;

    m12_ = 0.0f;
    m13_ = 0.0f;
    m14_ = 0.0f;
    m15_ = 1.0f;

    return *this;
}

Matrix const &Matrix::Rotate(float _x, float _y, float _z, float _angle)
{
    float const axis[3] = {_x, _y, _z};
    Rotate(axis, _angle);

    return *this;
}

Matrix const &Matrix::Scale(float _x, float _y, float _z)
{
    // x  0  0  0
    // 0  y  0  0
    // 0  0  z  0
    // 0  0  0  1

    m0_ = _x;
    m5_ = _y;
    m10_ = _z;

    /*m0_  = _x;   m1_  = 0.0f; m2_  = 0.0f; m3_  = 0.0f;
    m4_  = 0.0f; m5_  = _y;   m6_  = 0.0f; m7_  = 0.0f;
    m8_  = 0.0f; m9_  = 0.0f; m10_ = _z;   m11_ = 0.0f;
    m12_ = 0.0f; m13_ = 0.0f; m14_ = 0.0f; m15_ = 1.0f;*/

    return *this;
}

Matrix const &Matrix::Translate(float _x, float _y, float _z)
{
    // 1  0  0  tx
    // 0  1  0  ty
    // 0  0  1  tz
    // 0  0  0  1

    m3_ = _x;
    m7_ = _y;
    m11_ = _z;

    return *this;
}

float Matrix::Minor(uint8 _a, uint8 _b, uint8 _c, uint8 _d, uint8 _e, uint8 _f) const
{
    _a <<= 2;    _b <<= 2;    _c <<= 2;

    return m_[_a + _d] * (m_[_b + _e] * m_[_c + _f] - m_[_c + _e] * m_[_b + _f]) -
        m_[_a + _e] * (m_[_b + _d] * m_[_c + _f] - m_[_c + _d] * m_[_b + _f]) +
        m_[_a + _f] * (m_[_b + _d] * m_[_c + _e] - m_[_c + _d] * m_[_b + _e]);
}

float Matrix::Det() const
{
    return m0_ * Minor(1, 2, 3, 1, 2, 3) - m1_ * Minor(1, 2, 3, 0, 2, 3) +
        m2_ * Minor(1, 2, 3, 0, 1, 3) - m3_ * Minor(1, 2, 3, 0, 1, 2);
}

Matrix Matrix::Inverse() const
{
    float det = Det();

    if (det == 0.0f)
        return *this;

    det = 1.0f / det;

    return Matrix
        (
        Minor(1, 2, 3, 1, 2, 3) * det,
        -Minor(0, 2, 3, 1, 2, 3) * det,
        Minor(0, 1, 3, 1, 2, 3) * det,
        -Minor(0, 1, 2, 1, 2, 3) * det,

        -Minor(1, 2, 3, 0, 2, 3) * det,
        Minor(0, 2, 3, 0, 2, 3) * det,
        -Minor(0, 1, 3, 0, 2, 3) * det,
        Minor(0, 1, 2, 0, 2, 3) * det,

        Minor(1, 2, 3, 0, 1, 3) * det,
        -Minor(0, 2, 3, 0, 1, 3) * det,
        Minor(0, 1, 3, 0, 1, 3) * det,
        -Minor(0, 1, 2, 0, 1, 3) * det,

        -Minor(1, 2, 3, 0, 1, 2) * det,
        Minor(0, 2, 3, 0, 1, 2) * det,
        -Minor(0, 1, 3, 0, 1, 2) * det,
        Minor(0, 1, 2, 0, 1, 2) * det
        );
}

Matrix Matrix::Transpose() const
{
    return Matrix
        (
        m0_, m4_, m8_, m12_,
        m1_, m5_, m9_, m13_,
        m2_, m6_, m10_, m14_,
        m3_, m7_, m11_, m15_
        );
}

/*static*/ Matrix Matrix::GetIdentity()
{
    return Matrix
        (
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );
}

Matrix const &Matrix::MakeIdentity()
{
    m0_ = 1.0f; m1_ = 0.0f; m2_ = 0.0f; m3_ = 0.0f;
    m4_ = 0.0f; m5_ = 1.0f; m6_ = 0.0f; m7_ = 0.0f;
    m8_ = 0.0f; m9_ = 0.0f; m10_ = 1.0f; m11_ = 0.0f;
    m12_ = 0.0f; m13_ = 0.0f; m14_ = 0.0f; m15_ = 1.0f;

    return *this;
}

void Matrix::TransformPoint(float *const _p) const
{
    float const np[3] = {_p[0], _p[1], _p[2]};

    _p[0] = np[0] * m0_ + np[1] * m4_ + np[2] * m8_ + m12_;
    _p[1] = np[0] * m1_ + np[1] * m5_ + np[2] * m9_ + m13_;
    _p[2] = np[0] * m2_ + np[1] * m6_ + np[2] * m10_ + m14_;
}

void Matrix::TransformVector(float *const _v) const
{
    float const nv[3] = {_v[0], _v[1], _v[2]};

    _v[0] = nv[0] * m0_ + nv[1] * m4_ + nv[2] * m8_;
    _v[1] = nv[0] * m1_ + nv[1] * m5_ + nv[2] * m9_;
    _v[2] = nv[0] * m2_ + nv[1] * m6_ + nv[2] * m10_;
}

Matrix const &Matrix::FromQuaternion(float const _q[])
{
    float const c[12] = {
        _q[1] + _q[1],  _q[2] + _q[2],  _q[3] + _q[3],

        c[0] * _q[0],   c[1] * _q[0],   c[2] * _q[0],
        c[0] * _q[1],   c[1] * _q[1],   c[2] * _q[1],
        c[1] * _q[2],   c[2] * _q[2],   c[2] * _q[3]
    };

    //  1 - 2 * (yy + zz)                2 * (xy + zw)              2 * (xz - yw)
    //      2 * (xy - zw)            1 - 2 * (xx + zz)              2 * (yz + xw)
    //      2 * (xz + yw)                2 * (yz - xw)          1 - 2 * (xx + yy)

    m0_ = 1.0f - c[9] - c[11]; m1_ = c[7] + c[5];         m2_ = c[8] - c[4];
    m4_ = c[7] - c[5];         m5_ = 1.0f - c[6] - c[11]; m6_ = c[10] + c[3];
    m8_ = c[8] + c[4];         m9_ = c[10] - c[3];        m10_ = 1.0f - c[6] - c[9];

    m3_ = m7_ = m11_ = 0.0f;
    m15_ = 1.0f;

    return *this;
}
};
};