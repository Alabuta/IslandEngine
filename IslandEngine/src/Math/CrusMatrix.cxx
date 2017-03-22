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
    _00_ = _m00;  _01_ = _m01;  _02_ = _m02; _03_ = _m03;
    _04_ = _m04;  _05_ = _m05;  _06_ = _m06; _07_ = _m07;
    _08_ = _m08;  _09_ = _m09;  _10_ = _m10; _11_ = _m11;
    _12_ = _m12;  _13_ = _m13;  _14_ = _m14; _15_ = _m15;
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
        _00_ + _m._00_, _01_ + _m._01_, _02_ + _m._02_, _03_ + _m._03_,
        _04_ + _m._04_, _05_ + _m._05_, _06_ + _m._06_, _07_ + _m._07_,
        _08_ + _m._08_, _09_ + _m._09_, _10_ + _m._10_, _11_ + _m._11_,
        _12_ + _m._12_, _13_ + _m._13_, _14_ + _m._14_, _15_ + _m._15_
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
        _00_ - _m._00_, _01_ - _m._01_, _02_ - _m._02_, _03_ - _m._03_,
        _04_ - _m._04_, _05_ - _m._05_, _06_ - _m._06_, _07_ - _m._07_,
        _08_ - _m._08_, _09_ - _m._09_, _10_ - _m._10_, _11_ - _m._11_,
        _12_ - _m._12_, _13_ - _m._13_, _14_ - _m._14_, _15_ - _m._15_
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
        _00_ * _m._00_ + _01_ * _m._04_ + _02_ * _m._08_ + _03_ * _m._12_,
        _00_ * _m._01_ + _01_ * _m._05_ + _02_ * _m._09_ + _03_ * _m._13_,
        _00_ * _m._02_ + _01_ * _m._06_ + _02_ * _m._10_ + _03_ * _m._14_,
        _00_ * _m._03_ + _01_ * _m._07_ + _02_ * _m._11_ + _03_ * _m._15_,

        _04_ * _m._00_ + _05_ * _m._04_ + _06_ * _m._08_ + _07_ * _m._12_,
        _04_ * _m._01_ + _05_ * _m._05_ + _06_ * _m._09_ + _07_ * _m._13_,
        _04_ * _m._02_ + _05_ * _m._06_ + _06_ * _m._10_ + _07_ * _m._14_,
        _04_ * _m._03_ + _05_ * _m._07_ + _06_ * _m._11_ + _07_ * _m._15_,

        _08_ * _m._00_ + _09_ * _m._04_ + _10_ * _m._08_ + _11_ * _m._12_,
        _08_ * _m._01_ + _09_ * _m._05_ + _10_ * _m._09_ + _11_ * _m._13_,
        _08_ * _m._02_ + _09_ * _m._06_ + _10_ * _m._10_ + _11_ * _m._14_,
        _08_ * _m._03_ + _09_ * _m._07_ + _10_ * _m._11_ + _11_ * _m._15_,

        _12_ * _m._00_ + _13_ * _m._04_ + _14_ * _m._08_ + _15_ * _m._12_,
        _12_ * _m._01_ + _13_ * _m._05_ + _14_ * _m._09_ + _15_ * _m._13_,
        _12_ * _m._02_ + _13_ * _m._06_ + _14_ * _m._10_ + _15_ * _m._14_,
        _12_ * _m._03_ + _13_ * _m._07_ + _14_ * _m._11_ + _15_ * _m._15_
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
        _00_ + _s, _01_ + _s, _02_ + _s, _03_ + _s,
        _04_ + _s, _05_ + _s, _06_ + _s, _07_ + _s,
        _08_ + _s, _09_ + _s, _10_ + _s, _11_ + _s,
        _12_ + _s, _13_ + _s, _14_ + _s, _15_ + _s
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
        _00_ - _s, _01_ - _s, _02_ - _s, _03_ - _s,
        _04_ - _s, _05_ - _s, _06_ - _s, _07_ - _s,
        _08_ - _s, _09_ - _s, _10_ - _s, _11_ - _s,
        _12_ - _s, _13_ - _s, _14_ - _s, _15_ - _s
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
        _00_ * _s, _01_ * _s, _02_ * _s, _03_ * _s,
        _04_ * _s, _05_ * _s, _06_ * _s, _07_ * _s,
        _08_ * _s, _09_ * _s, _10_ * _s, _11_ * _s,
        _12_ * _s, _13_ * _s, _14_ * _s, _15_ * _s
    );
#endif
}

bool Matrix::operator== (Matrix const &_m) const
{
    return CloseEnough(_00_, _m._00_) && CloseEnough(_01_, _m._01_)
        && CloseEnough(_02_, _m._02_) && CloseEnough(_03_, _m._03_)
        && CloseEnough(_04_, _m._04_) && CloseEnough(_05_, _m._05_)
        && CloseEnough(_06_, _m._06_) && CloseEnough(_07_, _m._07_)
        && CloseEnough(_08_, _m._08_) && CloseEnough(_09_, _m._09_)
        && CloseEnough(_10_, _m._10_) && CloseEnough(_11_, _m._11_)
        && CloseEnough(_12_, _m._12_) && CloseEnough(_13_, _m._13_)
        && CloseEnough(_14_, _m._14_) && CloseEnough(_15_, _m._15_);
}

Matrix &Matrix::operator+= (Matrix const &_m)
{
#ifdef CRUS_USE_SSE_MATH
    row_[0] = _mm_add_ps(row_[0], _m.row_[0]);
    row_[1] = _mm_add_ps(row_[1], _m.row_[1]);
    row_[2] = _mm_add_ps(row_[2], _m.row_[2]);
    row_[3] = _mm_add_ps(row_[3], _m.row_[3]);
#else
    _00_ += _m._00_;  _01_ += _m._01_;  _02_ += _m._02_; _03_ += _m._03_;
    _04_ += _m._04_;  _05_ += _m._05_;  _06_ += _m._06_; _07_ += _m._07_;
    _08_ += _m._08_;  _09_ += _m._09_;  _10_ += _m._10_; _11_ += _m._11_;
    _12_ += _m._12_;  _13_ += _m._13_;  _14_ += _m._14_; _15_ += _m._15_;
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
    _00_ -= _m._00_;  _01_ -= _m._01_;  _02_ -= _m._02_; _03_ -= _m._03_;
    _04_ -= _m._04_;  _05_ -= _m._05_;  _06_ -= _m._06_; _07_ -= _m._07_;
    _08_ -= _m._08_;  _09_ -= _m._09_;  _10_ -= _m._10_; _11_ -= _m._11_;
    _12_ -= _m._12_;  _13_ -= _m._13_;  _14_ -= _m._14_; _15_ -= _m._15_;
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

    _00_ = temp._00_ * _m._00_ + temp._01_ * _m._04_ + temp._02_ * _m._08_ + temp._03_ * _m._12_;
    _01_ = temp._00_ * _m._01_ + temp._01_ * _m._05_ + temp._02_ * _m._09_ + temp._03_ * _m._13_;
    _02_ = temp._00_ * _m._02_ + temp._01_ * _m._06_ + temp._02_ * _m._10_ + temp._03_ * _m._14_;
    _03_ = temp._00_ * _m._03_ + temp._01_ * _m._07_ + temp._02_ * _m._11_ + temp._03_ * _m._15_;

    _04_ = temp._04_ * _m._00_ + temp._05_ * _m._04_ + temp._06_ * _m._08_ + temp._07_ * _m._12_;
    _05_ = temp._04_ * _m._01_ + temp._05_ * _m._05_ + temp._06_ * _m._09_ + temp._07_ * _m._13_;
    _06_ = temp._04_ * _m._02_ + temp._05_ * _m._06_ + temp._06_ * _m._10_ + temp._07_ * _m._14_;
    _07_ = temp._04_ * _m._03_ + temp._05_ * _m._07_ + temp._06_ * _m._11_ + temp._07_ * _m._15_;

    _08_ = temp._08_ * _m._00_ + temp._09_ * _m._04_ + temp._10_ * _m._08_ + temp._11_ * _m._12_;
    _09_ = temp._08_ * _m._01_ + temp._09_ * _m._05_ + temp._10_ * _m._09_ + temp._11_ * _m._13_;
    _10_ = temp._08_ * _m._02_ + temp._09_ * _m._06_ + temp._10_ * _m._10_ + temp._11_ * _m._14_;
    _11_ = temp._08_ * _m._03_ + temp._09_ * _m._07_ + temp._10_ * _m._11_ + temp._11_ * _m._15_;

    _12_ = temp._12_ * _m._00_ + temp._13_ * _m._04_ + temp._14_ * _m._08_ + temp._15_ * _m._12_;
    _13_ = temp._12_ * _m._01_ + temp._13_ * _m._05_ + temp._14_ * _m._09_ + temp._15_ * _m._13_;
    _14_ = temp._12_ * _m._02_ + temp._13_ * _m._06_ + temp._14_ * _m._10_ + temp._15_ * _m._14_;
    _15_ = temp._12_ * _m._03_ + temp._13_ * _m._07_ + temp._14_ * _m._11_ + temp._15_ * _m._15_;
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
    _00_ += _s; _01_ += _s; _02_ += _s; _03_ += _s;
    _04_ += _s; _05_ += _s; _06_ += _s; _07_ += _s;
    _08_ += _s; _09_ += _s; _10_ += _s; _11_ += _s;
    _12_ += _s; _13_ += _s; _14_ += _s; _15_ += _s;
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
    _00_ -= _s; _01_ -= _s; _02_ -= _s; _03_ -= _s;
    _04_ -= _s; _05_ -= _s; _06_ -= _s; _07_ -= _s;
    _08_ -= _s; _09_ -= _s; _10_ -= _s; _11_ -= _s;
    _12_ -= _s; _13_ -= _s; _14_ -= _s; _15_ -= _s;
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
    _00_ *= _s; _01_ *= _s; _02_ *= _s; _03_ *= _s;
    _04_ *= _s; _05_ *= _s; _06_ *= _s; _07_ *= _s;
    _08_ *= _s; _09_ *= _s; _10_ *= _s; _11_ *= _s;
    _12_ *= _s; _13_ *= _s; _14_ *= _s; _15_ *= _s;
#endif

    return *this;
}

Matrix const &Matrix::RotateOnOX(float _degree)
{
    //  1  0  0  0
    //  0  c -s  0
    //  0  s  c  0
    //  0  0  0  1

    auto const s = std::sin(DegToRad(_degree));
    auto const c = std::cos(DegToRad(_degree));

    _00_ = 1.f; _01_ = 0.f;  _02_ = 0.f;
    _04_ = 0.f; _05_ = c;    _06_ = -s;
    _08_ = 0.f; _09_ = s;    _10_ = +c;

    return *this;
}

Matrix const &Matrix::RotateOnOY(float _degree)
{
    //  c  0 -s  0
    //  0  1  0  0
    //  s  0  c  0
    //  0  0  0  1

    auto const s = std::sin(DegToRad(_degree));
    auto const c = std::cos(DegToRad(_degree));

    _00_ = c;   _01_ = 0.f; _02_ = -s;
    _04_ = 0.f; _05_ = 1.f; _06_ = 0.f;
    _08_ = s;   _09_ = 0.f; _10_ = +c;

    return *this;
}

Matrix const &Matrix::RotateOnOZ(float _degree)
{
    //  c -s  0  0
    //  s  c  0  0
    //  0  0  1  0
    //  0  0  0  1

    auto const s = std::sin(DegToRad(_degree));
    auto const c = std::cos(DegToRad(_degree));

    _00_ = c;    _01_ = -s;   _02_ = 0.f;
    _04_ = s;    _05_ = +c;   _06_ = 0.f;
    _08_ = 0.f;  _09_ = 0.f;  _10_ = 1.f;

    return *this;
}

Matrix const &Matrix::Rotate(Vector const &_axis, float _degree)
{
    auto const s = std::sin(DegToRad(_degree));
    auto const c = std::cos(DegToRad(_degree));

    _00_ = _axis.x() * _axis.x() * (1.f - c) + c;
    _01_ = _axis.x() * _axis.y() * (1.f - c) + _axis.z() * s;
    _02_ = _axis.x() * _axis.z() * (1.f - c) - _axis.y() * s;
    _03_ = 0.f;

    _04_ = _axis.y() * _axis.x() * (1.f - c) - _axis.z() * s;
    _05_ = _axis.y() * _axis.y() * (1.f - c) + c;
    _06_ = _axis.y() * _axis.z() * (1.f - c) + _axis.x() * s;
    _07_ = 0.f;

    _08_ = _axis.z() * _axis.x() * (1.f - c) + _axis.y() * s;
    _09_ = _axis.z() * _axis.y() * (1.f - c) - _axis.x() * s;
    _10_ = _axis.z() * _axis.z() * (1.f - c) + c;
    _11_ = 0.f;

    _12_ = 0.f;
    _13_ = 0.f;
    _14_ = 0.f;
    _15_ = 1.f;

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

    _00_ = _x;
    _05_ = _y;
    _10_ = _z;

    return *this;
}

Matrix const &Matrix::Translate(float _x, float _y, float _z)
{
    // 1  0  0  x
    // 0  1  0  y
    // 0  0  1  z
    // 0  0  0  1

    x_ = _x;
    y_ = _y;
    z_ = _z;

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
    return _00_ * Minor(1, 2, 3, 1, 2, 3) - _01_ * Minor(1, 2, 3, 0, 2, 3) +
           _02_ * Minor(1, 2, 3, 0, 1, 3) - _03_ * Minor(1, 2, 3, 0, 1, 2);
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
        _m._00_, _m._04_, _m._08_, _m._12_,
        _m._01_, _m._05_, _m._09_, _m._13_,
        _m._02_, _m._06_, _m._10_, _m._14_,
        _m._03_, _m._07_, _m._11_, _m._15_
    );
#endif
}

Matrix const &Matrix::Transpose()
{
#ifdef CRUS_USE_SSE_MATH
    _MM_TRANSPOSE4_PS(row_[0], row_[1], row_[2], row_[3]);
#else
    vec_ = {
        _00_, _04_, _08_, _12_,
        _01_, _05_, _09_, _13_,
        _02_, _06_, _10_, _14_,
        _03_, _07_, _11_, _15_
    };
#endif

    return *this;
}

Matrix const &Matrix::MakeIdentity()
{
    vec_.fill(0.f);

    _00_ = 1.f;
    _05_ = 1.f;
    _10_ = 1.f;
    _15_ = 1.f;

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

    _p.v()[0] = np[0] * _00_ + np[1] * _01_ + np[2] * _02_ + 1.f * _03_;
    _p.v()[1] = np[0] * _04_ + np[1] * _05_ + np[2] * _06_ + 1.f * _07_;
    _p.v()[2] = np[0] * _08_ + np[1] * _09_ + np[2] * _10_ + 1.f * _11_;
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

    _v.v()[0] = nv[0] * _00_ + nv[1] * _01_ + nv[2] * _02_;
    _v.v()[1] = nv[0] * _04_ + nv[1] * _05_ + nv[2] * _06_;
    _v.v()[2] = nv[0] * _08_ + nv[1] * _09_ + nv[2] * _10_;
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

    _00_ = 1.0f - c[9] - c[11]; _01_ = c[7] + c[5];         _02_ = c[8] - c[4];
    _04_ = c[7] - c[5];         _05_ = 1.0f - c[6] - c[11]; _06_ = c[10] + c[3];
    _08_ = c[8] + c[4];         _09_ = c[10] - c[3];        _10_ = 1.0f - c[6] - c[9];

    _03_ = _07_ = _11_ = 0.0f;
    _15_ = 1.0f;

    return *this;
}
};