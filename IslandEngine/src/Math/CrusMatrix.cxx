/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: Matrix4x4 implementation.
****
********************************************************************************************************************************/
#include "System\CrusTypes.h"
#include "Math\CrusMatrix.h"

namespace isle::math {
Matrix::Matrix(float _m00, float _m01, float _m02, float _m03,
               float _m04, float _m05, float _m06, float _m07,
               float _m08, float _m09, float _m10, float _m11,
               float _m12, float _m13, float _m14, float _m15)
{
    m_._00_ = _m00;  m_._01_ = _m01;  m_._02_ = _m02; m_._03_ = _m03;
    m_._04_ = _m04;  m_._05_ = _m05;  m_._06_ = _m06; m_._07_ = _m07;
    m_._08_ = _m08;  m_._09_ = _m09;  m_._10_ = _m10; m_._11_ = _m11;
    m_._12_ = _m12;  m_._13_ = _m13;  m_._14_ = _m14; m_._15_ = _m15;
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
        m_._00_ + _m.m_._00_, m_._01_ + _m.m_._01_, m_._02_ + _m.m_._02_, m_._03_ + _m.m_._03_,
        m_._04_ + _m.m_._04_, m_._05_ + _m.m_._05_, m_._06_ + _m.m_._06_, m_._07_ + _m.m_._07_,
        m_._08_ + _m.m_._08_, m_._09_ + _m.m_._09_, m_._10_ + _m.m_._10_, m_._11_ + _m.m_._11_,
        m_._12_ + _m.m_._12_, m_._13_ + _m.m_._13_, m_._14_ + _m.m_._14_, m_._15_ + _m.m_._15_
    );
}

Matrix Matrix::operator- (Matrix const &_m) const
{
    return Matrix
    (
        m_._00_ - _m.m_._00_, m_._01_ - _m.m_._01_, m_._02_ - _m.m_._02_, m_._03_ - _m.m_._03_,
        m_._04_ - _m.m_._04_, m_._05_ - _m.m_._05_, m_._06_ - _m.m_._06_, m_._07_ - _m.m_._07_,
        m_._08_ - _m.m_._08_, m_._09_ - _m.m_._09_, m_._10_ - _m.m_._10_, m_._11_ - _m.m_._11_,
        m_._12_ - _m.m_._12_, m_._13_ - _m.m_._13_, m_._14_ - _m.m_._14_, m_._15_ - _m.m_._15_
    );
}

Matrix Matrix::operator* (Matrix const &_m) const
{
    return Matrix
    (
        m_._00_ * _m.m_._00_ + m_._01_ * _m.m_._04_ + m_._02_ * _m.m_._08_ + m_._03_ * _m.m_._12_,
        m_._00_ * _m.m_._01_ + m_._01_ * _m.m_._05_ + m_._02_ * _m.m_._09_ + m_._03_ * _m.m_._13_,
        m_._00_ * _m.m_._02_ + m_._01_ * _m.m_._06_ + m_._02_ * _m.m_._10_ + m_._03_ * _m.m_._14_,
        m_._00_ * _m.m_._03_ + m_._01_ * _m.m_._07_ + m_._02_ * _m.m_._11_ + m_._03_ * _m.m_._15_,

        m_._04_ * _m.m_._00_ + m_._05_ * _m.m_._04_ + m_._06_ * _m.m_._08_ + m_._07_ * _m.m_._12_,
        m_._04_ * _m.m_._01_ + m_._05_ * _m.m_._05_ + m_._06_ * _m.m_._09_ + m_._07_ * _m.m_._13_,
        m_._04_ * _m.m_._02_ + m_._05_ * _m.m_._06_ + m_._06_ * _m.m_._10_ + m_._07_ * _m.m_._14_,
        m_._04_ * _m.m_._03_ + m_._05_ * _m.m_._07_ + m_._06_ * _m.m_._11_ + m_._07_ * _m.m_._15_,

        m_._08_ * _m.m_._00_ + m_._09_ * _m.m_._04_ + m_._10_ * _m.m_._08_ + m_._11_ * _m.m_._12_,
        m_._08_ * _m.m_._01_ + m_._09_ * _m.m_._05_ + m_._10_ * _m.m_._09_ + m_._11_ * _m.m_._13_,
        m_._08_ * _m.m_._02_ + m_._09_ * _m.m_._06_ + m_._10_ * _m.m_._10_ + m_._11_ * _m.m_._14_,
        m_._08_ * _m.m_._03_ + m_._09_ * _m.m_._07_ + m_._10_ * _m.m_._11_ + m_._11_ * _m.m_._15_,

        m_._12_ * _m.m_._00_ + m_._13_ * _m.m_._04_ + m_._14_ * _m.m_._08_ + m_._15_ * _m.m_._12_,
        m_._12_ * _m.m_._01_ + m_._13_ * _m.m_._05_ + m_._14_ * _m.m_._09_ + m_._15_ * _m.m_._13_,
        m_._12_ * _m.m_._02_ + m_._13_ * _m.m_._06_ + m_._14_ * _m.m_._10_ + m_._15_ * _m.m_._14_,
        m_._12_ * _m.m_._03_ + m_._13_ * _m.m_._07_ + m_._14_ * _m.m_._11_ + m_._15_ * _m.m_._15_
    );
}

Matrix Matrix::operator+ (float _s) const
{
    return Matrix
    (
        m_._00_ + _s, m_._01_ + _s, m_._02_ + _s, m_._03_ + _s,
        m_._04_ + _s, m_._05_ + _s, m_._06_ + _s, m_._07_ + _s,
        m_._08_ + _s, m_._09_ + _s, m_._10_ + _s, m_._11_ + _s,
        m_._12_ + _s, m_._13_ + _s, m_._14_ + _s, m_._15_ + _s
    );
}

Matrix Matrix::operator- (float _s) const
{
    return Matrix
    (
        m_._00_ - _s, m_._01_ - _s, m_._02_ - _s, m_._03_ - _s,
        m_._04_ - _s, m_._05_ - _s, m_._06_ - _s, m_._07_ - _s,
        m_._08_ - _s, m_._09_ - _s, m_._10_ - _s, m_._11_ - _s,
        m_._12_ - _s, m_._13_ - _s, m_._14_ - _s, m_._15_ - _s
    );
}

Matrix Matrix::operator* (float _s) const
{
    return Matrix
    (
        m_._00_ * _s, m_._01_ * _s, m_._02_ * _s, m_._03_ * _s,
        m_._04_ * _s, m_._05_ * _s, m_._06_ * _s, m_._07_ * _s,
        m_._08_ * _s, m_._09_ * _s, m_._10_ * _s, m_._11_ * _s,
        m_._12_ * _s, m_._13_ * _s, m_._14_ * _s, m_._15_ * _s
    );
}

bool Matrix::operator== (Matrix const &_m) const
{
    return CloseEnough(m_._00_, _m.m_._00_) && CloseEnough(m_._01_, _m.m_._01_)
        && CloseEnough(m_._02_, _m.m_._02_) && CloseEnough(m_._03_, _m.m_._03_)
        && CloseEnough(m_._04_, _m.m_._04_) && CloseEnough(m_._05_, _m.m_._05_)
        && CloseEnough(m_._06_, _m.m_._06_) && CloseEnough(m_._07_, _m.m_._07_)
        && CloseEnough(m_._08_, _m.m_._08_) && CloseEnough(m_._09_, _m.m_._09_)
        && CloseEnough(m_._10_, _m.m_._10_) && CloseEnough(m_._11_, _m.m_._11_)
        && CloseEnough(m_._12_, _m.m_._12_) && CloseEnough(m_._13_, _m.m_._13_)
        && CloseEnough(m_._14_, _m.m_._14_) && CloseEnough(m_._15_, _m.m_._15_);
}

Matrix const &Matrix::operator+= (Matrix const &_m)
{
    m_._00_ += _m.m_._00_;  m_._01_ += _m.m_._01_;  m_._02_ += _m.m_._02_; m_._03_ += _m.m_._03_;
    m_._04_ += _m.m_._04_;  m_._05_ += _m.m_._05_;  m_._06_ += _m.m_._06_; m_._07_ += _m.m_._07_;
    m_._08_ += _m.m_._08_;  m_._09_ += _m.m_._09_;  m_._10_ += _m.m_._10_; m_._11_ += _m.m_._11_;
    m_._12_ += _m.m_._12_;  m_._13_ += _m.m_._13_;  m_._14_ += _m.m_._14_; m_._15_ += _m.m_._15_;

    return *this;
}

Matrix const &Matrix::operator-= (Matrix const &_m)
{
    m_._00_ -= _m.m_._00_;  m_._01_ -= _m.m_._01_;  m_._02_ -= _m.m_._02_; m_._03_ -= _m.m_._03_;
    m_._04_ -= _m.m_._04_;  m_._05_ -= _m.m_._05_;  m_._06_ -= _m.m_._06_; m_._07_ -= _m.m_._07_;
    m_._08_ -= _m.m_._08_;  m_._09_ -= _m.m_._09_;  m_._10_ -= _m.m_._10_; m_._11_ -= _m.m_._11_;
    m_._12_ -= _m.m_._12_;  m_._13_ -= _m.m_._13_;  m_._14_ -= _m.m_._14_; m_._15_ -= _m.m_._15_;

    return *this;
}

Matrix const &Matrix::operator*= (Matrix const &_m)
{
    // 64 mul; 48 sum.
    Matrix temp(std::move(*this));

    m_._00_ = temp.m_._00_ * _m.m_._00_ + temp.m_._01_ * _m.m_._04_ + temp.m_._02_ * _m.m_._08_ + temp.m_._03_ * _m.m_._12_;
    m_._01_ = temp.m_._00_ * _m.m_._01_ + temp.m_._01_ * _m.m_._05_ + temp.m_._02_ * _m.m_._09_ + temp.m_._03_ * _m.m_._13_;
    m_._02_ = temp.m_._00_ * _m.m_._02_ + temp.m_._01_ * _m.m_._06_ + temp.m_._02_ * _m.m_._10_ + temp.m_._03_ * _m.m_._14_;
    m_._03_ = temp.m_._00_ * _m.m_._03_ + temp.m_._01_ * _m.m_._07_ + temp.m_._02_ * _m.m_._11_ + temp.m_._03_ * _m.m_._15_;
                                                                                                                 
    m_._04_ = temp.m_._04_ * _m.m_._00_ + temp.m_._05_ * _m.m_._04_ + temp.m_._06_ * _m.m_._08_ + temp.m_._07_ * _m.m_._12_;
    m_._05_ = temp.m_._04_ * _m.m_._01_ + temp.m_._05_ * _m.m_._05_ + temp.m_._06_ * _m.m_._09_ + temp.m_._07_ * _m.m_._13_;
    m_._06_ = temp.m_._04_ * _m.m_._02_ + temp.m_._05_ * _m.m_._06_ + temp.m_._06_ * _m.m_._10_ + temp.m_._07_ * _m.m_._14_;
    m_._07_ = temp.m_._04_ * _m.m_._03_ + temp.m_._05_ * _m.m_._07_ + temp.m_._06_ * _m.m_._11_ + temp.m_._07_ * _m.m_._15_;
                                                                                                                 
    m_._08_ = temp.m_._08_ * _m.m_._00_ + temp.m_._09_ * _m.m_._04_ + temp.m_._10_ * _m.m_._08_ + temp.m_._11_ * _m.m_._12_;
    m_._09_ = temp.m_._08_ * _m.m_._01_ + temp.m_._09_ * _m.m_._05_ + temp.m_._10_ * _m.m_._09_ + temp.m_._11_ * _m.m_._13_;
    m_._10_ = temp.m_._08_ * _m.m_._02_ + temp.m_._09_ * _m.m_._06_ + temp.m_._10_ * _m.m_._10_ + temp.m_._11_ * _m.m_._14_;
    m_._11_ = temp.m_._08_ * _m.m_._03_ + temp.m_._09_ * _m.m_._07_ + temp.m_._10_ * _m.m_._11_ + temp.m_._11_ * _m.m_._15_;
                                                                                                                 
    m_._12_ = temp.m_._12_ * _m.m_._00_ + temp.m_._13_ * _m.m_._04_ + temp.m_._14_ * _m.m_._08_ + temp.m_._15_ * _m.m_._12_;
    m_._13_ = temp.m_._12_ * _m.m_._01_ + temp.m_._13_ * _m.m_._05_ + temp.m_._14_ * _m.m_._09_ + temp.m_._15_ * _m.m_._13_;
    m_._14_ = temp.m_._12_ * _m.m_._02_ + temp.m_._13_ * _m.m_._06_ + temp.m_._14_ * _m.m_._10_ + temp.m_._15_ * _m.m_._14_;
    m_._15_ = temp.m_._12_ * _m.m_._03_ + temp.m_._13_ * _m.m_._07_ + temp.m_._14_ * _m.m_._11_ + temp.m_._15_ * _m.m_._15_;

    return *this;
}

Matrix const &Matrix::operator+= (float _s)
{
    m_._00_ += _s; m_._01_ += _s; m_._02_ += _s; m_._03_ += _s;
    m_._04_ += _s; m_._05_ += _s; m_._06_ += _s; m_._07_ += _s;
    m_._08_ += _s; m_._09_ += _s; m_._10_ += _s; m_._11_ += _s;
    m_._12_ += _s; m_._13_ += _s; m_._14_ += _s; m_._15_ += _s;

    return *this;
}

Matrix const &Matrix::operator-= (float _s)
{
    m_._00_ -= _s; m_._01_ -= _s; m_._02_ -= _s; m_._03_ -= _s;
    m_._04_ -= _s; m_._05_ -= _s; m_._06_ -= _s; m_._07_ -= _s;
    m_._08_ -= _s; m_._09_ -= _s; m_._10_ -= _s; m_._11_ -= _s;
    m_._12_ -= _s; m_._13_ -= _s; m_._14_ -= _s; m_._15_ -= _s;

    return *this;
}

Matrix const &Matrix::operator*= (float _s)
{
    m_._00_ *= _s; m_._01_ *= _s; m_._02_ *= _s; m_._03_ *= _s;
    m_._04_ *= _s; m_._05_ *= _s; m_._06_ *= _s; m_._07_ *= _s;
    m_._08_ *= _s; m_._09_ *= _s; m_._10_ *= _s; m_._11_ *= _s;
    m_._12_ *= _s; m_._13_ *= _s; m_._14_ *= _s; m_._15_ *= _s;

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

    m_._00_ = 1.0f; m_._01_ = 0.0f; m_._02_ = 0.0f;
    m_._04_ = 0.0f; m_._05_ = c;    m_._06_ = -s;
    m_._08_ = 0.0f; m_._09_ = s;    m_._10_ = c;

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

    m_._00_ = c;   m_._01_ = 0.0f; m_._02_ = s;
    m_._04_ = 0.0f; m_._05_ = 1.0f; m_._06_ = 0.0f;
    m_._08_ = -s;   m_._09_ = 0.0f; m_._10_ = c;

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

    m_._00_ = c;    m_._01_ = -s;   m_._02_ = 0.0f; //m_._03_  = 0.0f;
    m_._04_ = s;    m_._05_ = c;    m_._06_ = 0.0f; //m_._07_  = 0.0f;
    m_._08_ = 0.0f; m_._09_ = 0.0f; m_._10_ = 1.0f; //m_._11_ = 0.0f;
    //m_._12_ = 0.0f; m_._13_ = 0.0f; m_._14_ = 0.0f; m_._15_ = 1.0f;

    return *this;
}

Matrix const &Matrix::Rotate(float const *const _axis, float _angle)
{
    float const s = sinf(DegToRad(_angle));
    float const c = cosf(DegToRad(_angle));

    m_._00_ = (_axis[0] * _axis[0]) * (1.0f - c) + c;
    m_._01_ = (_axis[0] * _axis[1]) * (1.0f - c) + _axis[2] * s;
    m_._02_ = (_axis[0] * _axis[2]) * (1.0f - c) - _axis[1] * s;
    m_._03_ = 0.0f;

    m_._04_ = (_axis[1] * _axis[0]) * (1.0f - c) - _axis[2] * s;
    m_._05_ = (_axis[1] * _axis[1]) * (1.0f - c) + c;
    m_._06_ = (_axis[1] * _axis[2]) * (1.0f - c) + _axis[0] * s;
    m_._07_ = 0.0f;

    m_._08_ = (_axis[2] * _axis[0]) * (1.0f - c) + _axis[1] * s;
    m_._09_ = (_axis[2] * _axis[1]) * (1.0f - c) - _axis[0] * s;
    m_._10_ = (_axis[2] * _axis[2]) * (1.0f - c) + c;
    m_._11_ = 0.0f;

    m_._12_ = 0.0f;
    m_._13_ = 0.0f;
    m_._14_ = 0.0f;
    m_._15_ = 1.0f;

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

    m_._00_ = _x;
    m_._05_ = _y;
    m_._10_ = _z;

    /*m_._00_  = _x;   m_._01_  = 0.0f; m_._02_  = 0.0f; m_._03_  = 0.0f;
    m_._04_  = 0.0f; m_._05_  = _y;   m_._06_  = 0.0f; m_._07_  = 0.0f;
    m_._08_  = 0.0f; m_._09_  = 0.0f; m_._10_ = _z;   m_._11_ = 0.0f;
    m_._12_ = 0.0f; m_._13_ = 0.0f; m_._14_ = 0.0f; m_._15_ = 1.0f;*/

    return *this;
}

Matrix const &Matrix::Translate(float _x, float _y, float _z)
{
    // 1  0  0  tx
    // 0  1  0  ty
    // 0  0  1  tz
    // 0  0  0  1

    m_._03_ = _x;
    m_._07_ = _y;
    m_._11_ = _z;

    return *this;
}

float Matrix::Minor(uint8 _a, uint8 _b, uint8 _c, uint8 _d, uint8 _e, uint8 _f) const
{
    _a <<= 2;    _b <<= 2;    _c <<= 2;

    return vec_[_a + _d] * (vec_[_b + _e] * vec_[_c + _f] - vec_[_c + _e] * vec_[_b + _f]) -
           vec_[_a + _e] * (vec_[_b + _d] * vec_[_c + _f] - vec_[_c + _d] * vec_[_b + _f]) +
           vec_[_a + _f] * (vec_[_b + _d] * vec_[_c + _e] - vec_[_c + _d] * vec_[_b + _e]);
}

float Matrix::Det() const
{
    return m_._00_ * Minor(1, 2, 3, 1, 2, 3) - m_._01_ * Minor(1, 2, 3, 0, 2, 3) +
           m_._02_ * Minor(1, 2, 3, 0, 1, 3) - m_._03_ * Minor(1, 2, 3, 0, 1, 2);
}

Matrix Matrix::Inverse() const
{
    float det = Det();

    if (det == 0.0f)
        return *this;

    det = 1.0f / det;

    return Matrix
    (
        +Minor(1, 2, 3, 1, 2, 3) * det,
        -Minor(0, 2, 3, 1, 2, 3) * det,
        +Minor(0, 1, 3, 1, 2, 3) * det,
        -Minor(0, 1, 2, 1, 2, 3) * det,

        -Minor(1, 2, 3, 0, 2, 3) * det,
        +Minor(0, 2, 3, 0, 2, 3) * det,
        -Minor(0, 1, 3, 0, 2, 3) * det,
        +Minor(0, 1, 2, 0, 2, 3) * det,

        +Minor(1, 2, 3, 0, 1, 3) * det,
        -Minor(0, 2, 3, 0, 1, 3) * det,
        +Minor(0, 1, 3, 0, 1, 3) * det,
        -Minor(0, 1, 2, 0, 1, 3) * det,

        -Minor(1, 2, 3, 0, 1, 2) * det,
        +Minor(0, 2, 3, 0, 1, 2) * det,
        -Minor(0, 1, 3, 0, 1, 2) * det,
        +Minor(0, 1, 2, 0, 1, 2) * det
    );
}

Matrix Matrix::Transpose(Matrix const &_m)
{
    return Matrix
    (
        _m.m_._00_, _m.m_._04_, _m.m_._08_, _m.m_._12_,
        _m.m_._01_, _m.m_._05_, _m.m_._09_, _m.m_._13_,
        _m.m_._02_, _m.m_._06_, _m.m_._10_, _m.m_._14_,
        _m.m_._03_, _m.m_._07_, _m.m_._11_, _m.m_._15_
    );
}

Matrix const &Matrix::Transpose()
{
    vec_ = {
        m_._00_, m_._04_, m_._08_, m_._12_,
        m_._01_, m_._05_, m_._09_, m_._13_,
        m_._02_, m_._06_, m_._10_, m_._14_,
        m_._03_, m_._07_, m_._11_, m_._15_
    };

    return *this;
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
    vec_.fill(0.f);

    m_._00_ = 1.f;
    m_._05_ = 1.f;
    m_._10_ = 1.f;
    m_._15_ = 1.f;

    return *this;
}

void Matrix::TransformPoint(float *const _p) const
{
    float const np[3] = {_p[0], _p[1], _p[2]};

    _p[0] = np[0] * m_._00_ + np[1] * m_._04_ + np[2] * m_._08_ + m_._12_;
    _p[1] = np[0] * m_._01_ + np[1] * m_._05_ + np[2] * m_._09_ + m_._13_;
    _p[2] = np[0] * m_._02_ + np[1] * m_._06_ + np[2] * m_._10_ + m_._14_;
}

void Matrix::TransformVector(float *const _v) const
{
    float const nv[3] = {_v[0], _v[1], _v[2]};

    _v[0] = nv[0] * m_._00_ + nv[1] * m_._04_ + nv[2] * m_._08_;
    _v[1] = nv[0] * m_._01_ + nv[1] * m_._05_ + nv[2] * m_._09_;
    _v[2] = nv[0] * m_._02_ + nv[1] * m_._06_ + nv[2] * m_._10_;
}

Matrix const &Matrix::FromQuaternion(float const _q[])
{
    float const c[12] = {
        _q[1] + _q[1],  _q[2] + _q[2],  _q[3] + _q[3],

        (_q[1] + _q[1]) * _q[0],   (_q[2] + _q[2]) * _q[0],   (_q[3] + _q[3]) * _q[0],
        (_q[1] + _q[1]) * _q[1],   (_q[2] + _q[2]) * _q[1],   (_q[3] + _q[3]) * _q[1],
        (_q[2] + _q[2]) * _q[2],   (_q[3] + _q[3]) * _q[2],   (_q[3] + _q[3]) * _q[3]
    };

    //  1 - 2 * (yy + zz)                2 * (xy + zw)              2 * (xz - yw)
    //      2 * (xy - zw)            1 - 2 * (xx + zz)              2 * (yz + xw)
    //      2 * (xz + yw)                2 * (yz - xw)          1 - 2 * (xx + yy)

    m_._00_ = 1.0f - c[9] - c[11]; m_._01_ = c[7] + c[5];         m_._02_ = c[8] - c[4];
    m_._04_ = c[7] - c[5];         m_._05_ = 1.0f - c[6] - c[11]; m_._06_ = c[10] + c[3];
    m_._08_ = c[8] + c[4];         m_._09_ = c[10] - c[3];        m_._10_ = 1.0f - c[6] - c[9];

    m_._03_ = m_._07_ = m_._11_ = 0.0f;
    m_._15_ = 1.0f;

    return *this;
}
};