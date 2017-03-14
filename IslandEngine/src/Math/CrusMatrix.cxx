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
#include "Math\CrusVector.h"

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
#ifdef CRUS_USE_SSE_MATH
    Matrix temp;

    temp.row_[0] = _mm_add_ps(row_[0], _m.row_[0]);
    temp.row_[1] = _mm_add_ps(row_[1], _m.row_[1]);
    temp.row_[2] = _mm_add_ps(row_[2], _m.row_[2]);
    temp.row_[3] = _mm_add_ps(row_[3], _m.row_[3]);

    return temp;
#else
    return Matrix
    (
        m_._00_ + _m.m_._00_, m_._01_ + _m.m_._01_, m_._02_ + _m.m_._02_, m_._03_ + _m.m_._03_,
        m_._04_ + _m.m_._04_, m_._05_ + _m.m_._05_, m_._06_ + _m.m_._06_, m_._07_ + _m.m_._07_,
        m_._08_ + _m.m_._08_, m_._09_ + _m.m_._09_, m_._10_ + _m.m_._10_, m_._11_ + _m.m_._11_,
        m_._12_ + _m.m_._12_, m_._13_ + _m.m_._13_, m_._14_ + _m.m_._14_, m_._15_ + _m.m_._15_
    );
#endif
}

Matrix Matrix::operator- (Matrix const &_m) const
{
#ifdef CRUS_USE_SSE_MATH
    Matrix temp;

    temp.row_[0] = _mm_sub_ps(row_[0], _m.row_[0]);
    temp.row_[1] = _mm_sub_ps(row_[1], _m.row_[1]);
    temp.row_[2] = _mm_sub_ps(row_[2], _m.row_[2]);
    temp.row_[3] = _mm_sub_ps(row_[3], _m.row_[3]);

    return temp;
#else
    return Matrix
    (
        m_._00_ - _m.m_._00_, m_._01_ - _m.m_._01_, m_._02_ - _m.m_._02_, m_._03_ - _m.m_._03_,
        m_._04_ - _m.m_._04_, m_._05_ - _m.m_._05_, m_._06_ - _m.m_._06_, m_._07_ - _m.m_._07_,
        m_._08_ - _m.m_._08_, m_._09_ - _m.m_._09_, m_._10_ - _m.m_._10_, m_._11_ - _m.m_._11_,
        m_._12_ - _m.m_._12_, m_._13_ - _m.m_._13_, m_._14_ - _m.m_._14_, m_._15_ - _m.m_._15_
    );
#endif
}

Matrix Matrix::operator* (Matrix const &_m) const
{
#ifdef CRUS_USE_SSE_MATH
    Matrix temp;

    __m128 row[4] = {_m.row_[0], _m.row_[1], _m.row_[2], _m.row_[3]};
    _MM_TRANSPOSE4_PS(row[0], row[1], row[2], row[3]);

    {
        auto const sum1 = _mm_hadd_ps(_mm_mul_ps(row_[0], row[0]), _mm_mul_ps(row_[0], row[1]));
        auto const sum2 = _mm_hadd_ps(_mm_mul_ps(row_[0], row[2]), _mm_mul_ps(row_[0], row[3]));

        temp.row_[0] = _mm_hadd_ps(sum1, sum2);
    }

    {
        auto const sum1 = _mm_hadd_ps(_mm_mul_ps(row_[1], row[0]), _mm_mul_ps(row_[1], row[1]));
        auto const sum2 = _mm_hadd_ps(_mm_mul_ps(row_[1], row[2]), _mm_mul_ps(row_[1], row[3]));

        temp.row_[1] = _mm_hadd_ps(sum1, sum2);
    }

    {
        auto const sum1 = _mm_hadd_ps(_mm_mul_ps(row_[2], row[0]), _mm_mul_ps(row_[2], row[1]));
        auto const sum2 = _mm_hadd_ps(_mm_mul_ps(row_[2], row[2]), _mm_mul_ps(row_[2], row[3]));

        temp.row_[2] = _mm_hadd_ps(sum1, sum2);
    }

    {
        auto const sum1 = _mm_hadd_ps(_mm_mul_ps(row_[3], row[0]), _mm_mul_ps(row_[3], row[1]));
        auto const sum2 = _mm_hadd_ps(_mm_mul_ps(row_[3], row[2]), _mm_mul_ps(row_[3], row[3]));

        temp.row_[3] = _mm_hadd_ps(sum1, sum2);
    }

    return temp;
#else
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
#endif
}

Matrix Matrix::operator+ (float _s) const
{
#ifdef CRUS_USE_SSE_MATH
    Matrix temp;

    auto const s = _mm_set_ps1(_s);

    temp.row_[0] = _mm_add_ps(row_[0], s);
    temp.row_[1] = _mm_add_ps(row_[1], s);
    temp.row_[2] = _mm_add_ps(row_[2], s);
    temp.row_[3] = _mm_add_ps(row_[3], s);

    return temp;
#else
    return Matrix
    (
        m_._00_ + _s, m_._01_ + _s, m_._02_ + _s, m_._03_ + _s,
        m_._04_ + _s, m_._05_ + _s, m_._06_ + _s, m_._07_ + _s,
        m_._08_ + _s, m_._09_ + _s, m_._10_ + _s, m_._11_ + _s,
        m_._12_ + _s, m_._13_ + _s, m_._14_ + _s, m_._15_ + _s
    );
#endif
}

Matrix Matrix::operator- (float _s) const
{
#ifdef CRUS_USE_SSE_MATH
    Matrix temp;

    auto const s = _mm_set_ps1(_s);

    temp.row_[0] = _mm_sub_ps(row_[0], s);
    temp.row_[1] = _mm_sub_ps(row_[1], s);
    temp.row_[2] = _mm_sub_ps(row_[2], s);
    temp.row_[3] = _mm_sub_ps(row_[3], s);

    return temp;
#else
    return Matrix
    (
        m_._00_ - _s, m_._01_ - _s, m_._02_ - _s, m_._03_ - _s,
        m_._04_ - _s, m_._05_ - _s, m_._06_ - _s, m_._07_ - _s,
        m_._08_ - _s, m_._09_ - _s, m_._10_ - _s, m_._11_ - _s,
        m_._12_ - _s, m_._13_ - _s, m_._14_ - _s, m_._15_ - _s
    );
#endif
}

Matrix Matrix::operator* (float _s) const
{
#ifdef CRUS_USE_SSE_MATH
    Matrix temp;

    auto const s = _mm_set_ps1(_s);

    temp.row_[0] = _mm_mul_ps(row_[0], s);
    temp.row_[1] = _mm_mul_ps(row_[1], s);
    temp.row_[2] = _mm_mul_ps(row_[2], s);
    temp.row_[3] = _mm_mul_ps(row_[3], s);

    return temp;
#else
    return Matrix
    (
        m_._00_ * _s, m_._01_ * _s, m_._02_ * _s, m_._03_ * _s,
        m_._04_ * _s, m_._05_ * _s, m_._06_ * _s, m_._07_ * _s,
        m_._08_ * _s, m_._09_ * _s, m_._10_ * _s, m_._11_ * _s,
        m_._12_ * _s, m_._13_ * _s, m_._14_ * _s, m_._15_ * _s
    );
#endif
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

Matrix &Matrix::operator+= (Matrix const &_m)
{
#ifdef CRUS_USE_SSE_MATH
    row_[0] = _mm_add_ps(row_[0], _m.row_[0]);
    row_[1] = _mm_add_ps(row_[1], _m.row_[1]);
    row_[2] = _mm_add_ps(row_[2], _m.row_[2]);
    row_[3] = _mm_add_ps(row_[3], _m.row_[3]);
#else
    m_._00_ += _m.m_._00_;  m_._01_ += _m.m_._01_;  m_._02_ += _m.m_._02_; m_._03_ += _m.m_._03_;
    m_._04_ += _m.m_._04_;  m_._05_ += _m.m_._05_;  m_._06_ += _m.m_._06_; m_._07_ += _m.m_._07_;
    m_._08_ += _m.m_._08_;  m_._09_ += _m.m_._09_;  m_._10_ += _m.m_._10_; m_._11_ += _m.m_._11_;
    m_._12_ += _m.m_._12_;  m_._13_ += _m.m_._13_;  m_._14_ += _m.m_._14_; m_._15_ += _m.m_._15_;
#endif

    return *this;
}

Matrix &Matrix::operator-= (Matrix const &_m)
{
#ifdef CRUS_USE_SSE_MATH
    row_[0] = _mm_sub_ps(row_[0], _m.row_[0]);
    row_[1] = _mm_sub_ps(row_[1], _m.row_[1]);
    row_[2] = _mm_sub_ps(row_[2], _m.row_[2]);
    row_[3] = _mm_sub_ps(row_[3], _m.row_[3]);
#else
    m_._00_ -= _m.m_._00_;  m_._01_ -= _m.m_._01_;  m_._02_ -= _m.m_._02_; m_._03_ -= _m.m_._03_;
    m_._04_ -= _m.m_._04_;  m_._05_ -= _m.m_._05_;  m_._06_ -= _m.m_._06_; m_._07_ -= _m.m_._07_;
    m_._08_ -= _m.m_._08_;  m_._09_ -= _m.m_._09_;  m_._10_ -= _m.m_._10_; m_._11_ -= _m.m_._11_;
    m_._12_ -= _m.m_._12_;  m_._13_ -= _m.m_._13_;  m_._14_ -= _m.m_._14_; m_._15_ -= _m.m_._15_;
#endif

    return *this;
}

Matrix &Matrix::operator*= (Matrix const &_m)
{
#ifdef CRUS_USE_SSE_MATH
    // 16 mul; 12 sum
    __m128 row[4] = {_m.row_[0], _m.row_[1], _m.row_[2], _m.row_[3]};
    _MM_TRANSPOSE4_PS(row[0], row[1], row[2], row[3]);

    {
        auto const sum1 = _mm_hadd_ps(_mm_mul_ps(row_[0], row[0]), _mm_mul_ps(row_[0], row[1]));
        auto const sum2 = _mm_hadd_ps(_mm_mul_ps(row_[0], row[2]), _mm_mul_ps(row_[0], row[3]));

        row_[0] = _mm_hadd_ps(sum1, sum2);
    }

    {
        auto const sum1 = _mm_hadd_ps(_mm_mul_ps(row_[1], row[0]), _mm_mul_ps(row_[1], row[1]));
        auto const sum2 = _mm_hadd_ps(_mm_mul_ps(row_[1], row[2]), _mm_mul_ps(row_[1], row[3]));

        row_[1] = _mm_hadd_ps(sum1, sum2);
    }

    {
        auto const sum1 = _mm_hadd_ps(_mm_mul_ps(row_[2], row[0]), _mm_mul_ps(row_[2], row[1]));
        auto const sum2 = _mm_hadd_ps(_mm_mul_ps(row_[2], row[2]), _mm_mul_ps(row_[2], row[3]));

        row_[2] = _mm_hadd_ps(sum1, sum2);
    }

    {
        auto const sum1 = _mm_hadd_ps(_mm_mul_ps(row_[3], row[0]), _mm_mul_ps(row_[3], row[1]));
        auto const sum2 = _mm_hadd_ps(_mm_mul_ps(row_[3], row[2]), _mm_mul_ps(row_[3], row[3]));

        row_[3] = _mm_hadd_ps(sum1, sum2);
    }
#else
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
#endif

    return *this;
}

Matrix &Matrix::operator+= (float _s)
{
#ifdef CRUS_USE_SSE_MATH
    auto const s = _mm_set_ps1(_s);

    row_[0] = _mm_add_ps(row_[0], s);
    row_[1] = _mm_add_ps(row_[1], s);
    row_[2] = _mm_add_ps(row_[2], s);
    row_[3] = _mm_add_ps(row_[3], s);
#else
    m_._00_ += _s; m_._01_ += _s; m_._02_ += _s; m_._03_ += _s;
    m_._04_ += _s; m_._05_ += _s; m_._06_ += _s; m_._07_ += _s;
    m_._08_ += _s; m_._09_ += _s; m_._10_ += _s; m_._11_ += _s;
    m_._12_ += _s; m_._13_ += _s; m_._14_ += _s; m_._15_ += _s;
#endif

    return *this;
}

Matrix &Matrix::operator-= (float _s)
{
#ifdef CRUS_USE_SSE_MATH
    auto const s = _mm_set_ps1(_s);

    row_[0] = _mm_sub_ps(row_[0], s);
    row_[1] = _mm_sub_ps(row_[1], s);
    row_[2] = _mm_sub_ps(row_[2], s);
    row_[3] = _mm_sub_ps(row_[3], s);
#else
    m_._00_ -= _s; m_._01_ -= _s; m_._02_ -= _s; m_._03_ -= _s;
    m_._04_ -= _s; m_._05_ -= _s; m_._06_ -= _s; m_._07_ -= _s;
    m_._08_ -= _s; m_._09_ -= _s; m_._10_ -= _s; m_._11_ -= _s;
    m_._12_ -= _s; m_._13_ -= _s; m_._14_ -= _s; m_._15_ -= _s;
#endif

    return *this;
}

Matrix &Matrix::operator*= (float _s)
{
#ifdef CRUS_USE_SSE_MATH
    auto const s = _mm_set_ps1(_s);

    row_[0] = _mm_mul_ps(row_[0], s);
    row_[1] = _mm_mul_ps(row_[1], s);
    row_[2] = _mm_mul_ps(row_[2], s);
    row_[3] = _mm_mul_ps(row_[3], s);
#else
    m_._00_ *= _s; m_._01_ *= _s; m_._02_ *= _s; m_._03_ *= _s;
    m_._04_ *= _s; m_._05_ *= _s; m_._06_ *= _s; m_._07_ *= _s;
    m_._08_ *= _s; m_._09_ *= _s; m_._10_ *= _s; m_._11_ *= _s;
    m_._12_ *= _s; m_._13_ *= _s; m_._14_ *= _s; m_._15_ *= _s;
#endif

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

    m_._00_ = 1.f; m_._01_ = 0.f;  m_._02_ = 0.f;
    m_._04_ = 0.f; m_._05_ = c;    m_._06_ = -s;
    m_._08_ = 0.f; m_._09_ = s;    m_._10_ = c;

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

    m_._00_ = +c;   m_._01_ = 0.f; m_._02_ = s;
    m_._04_ = 0.f;  m_._05_ = 1.f; m_._06_ = 0.f;
    m_._08_ = -s;   m_._09_ = 0.f; m_._10_ = c;

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

    m_._00_ = c;    m_._01_ = -s;   m_._02_ = 0.f;
    m_._04_ = s;    m_._05_ = c;    m_._06_ = 0.f;
    m_._08_ = 0.f;  m_._09_ = 0.f;  m_._10_ = 1.f;

    return *this;
}

Matrix const &Matrix::Rotate(Vector const &_axis, float _angle)
{
    auto const s = std::sin(DegToRad(_angle));
    auto const c = std::cos(DegToRad(_angle));

    m_._00_ = _axis.x() * _axis.x() * (1.f - c) + c;
    m_._01_ = _axis.x() * _axis.y() * (1.f - c) + _axis.z() * s;
    m_._02_ = _axis.x() * _axis.z() * (1.f - c) - _axis.y() * s;
    m_._03_ = 0.f;

    m_._04_ = _axis.y() * _axis.x() * (1.f - c) - _axis.z() * s;
    m_._05_ = _axis.y() * _axis.y() * (1.f - c) + c;
    m_._06_ = _axis.y() * _axis.z() * (1.f - c) + _axis.x() * s;
    m_._07_ = 0.f;

    m_._08_ = _axis.z() * _axis.x() * (1.f - c) + _axis.y() * s;
    m_._09_ = _axis.z() * _axis.y() * (1.f - c) - _axis.x() * s;
    m_._10_ = _axis.z() * _axis.z() * (1.f - c) + c;
    m_._11_ = 0.f;

    m_._12_ = 0.f;
    m_._13_ = 0.f;
    m_._14_ = 0.f;
    m_._15_ = 1.f;

    return *this;
}

Matrix const &Matrix::Rotate(float _x, float _y, float _z, float _angle)
{
    Vector const axis(_x, _y, _z);
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
#ifdef CRUS_USE_SSE_MATH
    auto temp(_m);

    _MM_TRANSPOSE4_PS(temp.row_[0], temp.row_[1], temp.row_[2], temp.row_[3]);

    return temp;
#else
    return Matrix
    (
        _m.m_._00_, _m.m_._04_, _m.m_._08_, _m.m_._12_,
        _m.m_._01_, _m.m_._05_, _m.m_._09_, _m.m_._13_,
        _m.m_._02_, _m.m_._06_, _m.m_._10_, _m.m_._14_,
        _m.m_._03_, _m.m_._07_, _m.m_._11_, _m.m_._15_
    );
#endif
}

Matrix const &Matrix::Transpose()
{
#ifdef CRUS_USE_SSE_MATH
    _MM_TRANSPOSE4_PS(row_[0], row_[1], row_[2], row_[3]);
#else
    vec_ = {
        m_._00_, m_._04_, m_._08_, m_._12_,
        m_._01_, m_._05_, m_._09_, m_._13_,
        m_._02_, m_._06_, m_._10_, m_._14_,
        m_._03_, m_._07_, m_._11_, m_._15_
    };
#endif

    return *this;
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

Vector Matrix::TransformPosition(Vector const &_p) const
{
    auto np = _p;

    TransformPosition(np);

    return np;
}

Vector Matrix::TransformPosition(Vector &&_p) const
{
    auto np = std::move(_p);

    TransformPosition(np);

    return np;
}

void Matrix::TransformPosition(Vector &_p) const
{
    auto const np = _p.v();

    _p.v()[0] = np[0] * m_._00_ + np[1] * m_._01_ + np[2] * m_._02_ + 1.f * m_._03_;
    _p.v()[1] = np[0] * m_._04_ + np[1] * m_._05_ + np[2] * m_._06_ + 1.f * m_._07_;
    _p.v()[2] = np[0] * m_._08_ + np[1] * m_._09_ + np[2] * m_._10_ + 1.f * m_._11_;
}

Vector Matrix::TransformVector(Vector const &_v) const
{
    auto nv = _v;

    TransformVector(nv);

    return nv;
}

Vector Matrix::TransformVector(Vector &&_v) const
{
    auto nv = std::move(_v);

    TransformVector(nv);

    return nv;
}

void Matrix::TransformVector(Vector &_v) const
{
    auto const nv = _v.v();

    _v.v()[0] = nv[0] * m_._00_ + nv[1] * m_._01_ + nv[2] * m_._02_;
    _v.v()[1] = nv[0] * m_._04_ + nv[1] * m_._05_ + nv[2] * m_._06_;
    _v.v()[2] = nv[0] * m_._08_ + nv[1] * m_._09_ + nv[2] * m_._10_;
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