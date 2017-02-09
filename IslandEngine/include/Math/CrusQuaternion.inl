/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 10th March 2010.
****    Description: quaternion.
****
********************************************************************************************************************************/
#include <utility>

namespace isle {
namespace math {
inline Quaternion::Quaternion() { }
inline Quaternion::~Quaternion() { }

inline Quaternion::Quaternion(Quaternion const &_q)
    : w_(_q.w_), x_(_q.x_), y_(_q.y_), z_(_q.z_)
{ }

inline Quaternion::Quaternion(float const _q[])
    : w_(_q[0]), x_(_q[1]), y_(_q[2]), z_(_q[3])
{ }

inline Quaternion::Quaternion(float _w, float _x, float _y, float _z)
    : w_(_w), x_(_x), y_(_y), z_(_z)
{ }

inline Quaternion::Quaternion(Quaternion &&_q)
{
    x_ = std::move(_q.x_);
    y_ = std::move(_q.y_);
    z_ = std::move(_q.z_);
    w_ = std::move(_q.w_);
}

inline float Quaternion::w() const
{
    return w_;
}

inline float Quaternion::x() const
{
    return x_;
}

inline float Quaternion::y() const
{
    return y_;
}

inline float Quaternion::z() const
{
    return z_;
}

inline float const *const Quaternion::q() const
{
    return (&w_);
};

inline float Quaternion::w(float _w)
{
    return w_ = _w;
}

inline float Quaternion::x(float _x)
{
    return x_ = _x;
}

inline float Quaternion::y(float _y)
{
    return y_ = _y;
}

inline float Quaternion::z(float _z)
{
    return z_ = _z;
}

inline float &Quaternion::w()
{
    return w_;
}

inline float &Quaternion::x()
{
    return x_;
}

inline float &Quaternion::y()
{
    return y_;
}

inline float &Quaternion::z()
{
    return z_;
}

inline Quaternion Quaternion::operator+ (Quaternion const &_q) const
{
    return Quaternion(w_ + _q.w_, x_ + _q.x_, y_ + _q.y_, z_ + _q.z_);
}

inline Quaternion Quaternion::operator- (Quaternion const &_q) const
{
    return Quaternion(w_ - _q.w_, x_ - _q.x_, y_ - _q.y_, z_ - _q.z_);
}

inline Quaternion Quaternion::operator^ (Quaternion const &_q) const
{
    float const c[8] = {(w_ + x_) * (_q.w_ + _q.x_),
                        (z_ - y_) * (_q.y_ - _q.z_),
                        (x_ - w_) * (_q.y_ + _q.z_),
                        (y_ + z_) * (_q.x_ - _q.w_),
                        (x_ + z_) * (_q.x_ + _q.y_),
                        (x_ - z_) * (_q.x_ - _q.y_),
                        (w_ + y_) * (_q.w_ - _q.z_),
                        (w_ - y_) * (_q.w_ + _q.z_)};

    return Quaternion
    (
        c[1] + (-c[4] - c[5] + c[6] + c[7]) * 0.5f,
        c[0] - (c[4] + c[5] + c[6] + c[7]) * 0.5f,
        -c[2] + (c[4] - c[5] + c[6] - c[7]) * 0.5f,
        -c[3] + (c[4] - c[5] - c[6] + c[7]) * 0.5f
    );
}

inline float Quaternion::operator* (Quaternion const &_q) const
{
    return w_ * _q.w_ + x_ * _q.x_ + y_ * _q.y_ + z_ * _q.z_;
}

inline Quaternion Quaternion::operator+ (float _s) const
{
    return Quaternion(w_ + _s, x_ + _s, y_ + _s, z_ + _s);
}

inline Quaternion Quaternion::operator- (float _s) const
{
    return Quaternion(w_ - _s, x_ - _s, y_ - _s, z_ - _s);
}

inline Quaternion Quaternion::operator* (float _s) const
{
    return Quaternion(w_ * _s, x_ * _s, y_ * _s, z_ * _s);
}

inline Quaternion Quaternion::operator/ (float _s) const
{
    float const s = 1.0f / _s;
    return Quaternion(w_ * s, x_ * s, y_ * s, z_ * s);
}

inline Quaternion const &Quaternion::operator= (Quaternion const &_q)
{
    if (this != &_q) {
        w_ = _q.w_;  x_ = _q.x_;  y_ = _q.y_;  z_ = _q.z_;
    }

    return *this;
}

inline Quaternion const &Quaternion::operator= (float _s)
{
    w_ = _s;  x_ = _s;  y_ = _s;  z_ = _s;
    return *this;
}
inline Quaternion const &Quaternion::operator= (Quaternion &&_q)
{
    x_ = std::move(_q.x_);
    y_ = std::move(_q.y_);
    z_ = std::move(_q.z_);
    w_ = std::move(_q.w_);

    return *this;
}

inline bool Quaternion::operator== (Quaternion const &_q) const
{
    return CloseEnough(w_, _q.w_) && CloseEnough(x_, _q.x_)
        && CloseEnough(y_, _q.y_) && CloseEnough(z_, _q.z_);
}

inline bool Quaternion::operator!= (Quaternion const &_q) const
{
    return !(*this == _q);
}

inline Quaternion const &Quaternion::operator+= (Quaternion const &_q)
{
    w_ += _q.w_;    x_ += _q.x_;    y_ += _q.y_;    z_ += _q.z_;
    return *this;
}

inline Quaternion const &Quaternion::operator-= (Quaternion const &_q)
{
    w_ -= _q.w_;    x_ -= _q.x_;    y_ -= _q.y_;    z_ -= _q.z_;
    return *this;
}

inline Quaternion const &Quaternion::operator^= (Quaternion const &_q)
{
    float const c[8] = {(w_ + x_) * (_q.w_ + _q.x_),
                        (z_ - y_) * (_q.y_ - _q.z_),
                        (x_ - w_) * (_q.y_ + _q.z_),
                        (y_ + z_) * (_q.x_ - _q.w_),
                        (x_ + z_) * (_q.x_ + _q.y_),
                        (x_ - z_) * (_q.x_ - _q.y_),
                        (w_ + y_) * (_q.w_ - _q.z_),
                        (w_ - y_) * (_q.w_ + _q.z_)};

    /*float c[8];


        _declspec(align(16)) float adds11_1[8] = {c[0], c[3], c[1], c[2]};
        _declspec(align(16)) float adds11_2[8] = {c[1], c[1], c[0], c[0]};
{
        _declspec(align(16)) float adds2_1[8] = {c[1], -c[2], -c[0], c[3]};
        _declspec(align(16)) float adds2_2[8] = {c[3], -c[3],  c[2], -c[2]};

        __asm {
            movaps xmm0, adds11_1;
            movaps xmm1, adds2_1;

            addps xmm1, xmm0;
            movaps adds11_1, xmm1;

            movaps xmm2, adds11_2;
            movaps xmm3, adds2_2;

            addps xmm3, xmm2;
            movaps adds11_2, xmm3;
        };

        memcpy(coeff1, adds11_1, ES::f32::QUARTA);
        memcpy(coeff1 + 4, adds11_2, ES::f32::QUARTA);
    }

    float coeff2[8];


        _declspec(align(16)) float adds21_1[8] = {_q.c[0], _q.c[2], _q.c[2], _q.c[1]};
        _declspec(align(16)) float adds21_2[8] = {_q.c[1], _q.c[1], _q.c[0], _q.c[0]};
{
        _declspec(align(16)) float adds2_1[8] = {_q.c[1], -_q.c[3], _q.c[3], -_q.c[0]};
        _declspec(align(16)) float adds2_2[8] = {_q.c[2], -_q.c[2], -_q.c[3], _q.c[3]};

        __asm {
            movaps xmm0, adds21_1;
            movaps xmm1, adds2_1;

            addps xmm1, xmm0;
            movaps adds21_1, xmm1;

            movaps xmm2, adds21_2;
            movaps xmm3, adds2_2;

            addps xmm3, xmm2;
            movaps adds21_2, xmm3;
        };

        memcpy(coeff2, adds21_1, ES::f32::QUARTA);
        memcpy(coeff2 + 4, adds21_2, ES::f32::QUARTA);
    }

    float c[8] = {coeff1[0] * coeff2[0],
                     coeff1[1] * coeff2[1],
                     coeff1[2] * coeff2[2],
                     coeff1[3] * coeff2[3],
                     coeff1[4] * coeff2[4],
                     coeff1[5] * coeff2[5],
                     coeff1[6] * coeff2[6],
                     coeff1[7] * coeff2[7]};

    __asm {
        movaps xmm4, adds11_1;
        movaps xmm5, adds21_1;

        addps xmm5, xmm4;
        movaps adds11_1, xmm5;

        movaps xmm6, adds11_2;
        movaps xmm7, adds21_2;

        addps xmm7, xmm6;
        movaps adds11_2, xmm7;
    };

    memcpy(coeff1 + 0, adds11_1, ES::f32::QUARTA);
    memcpy(coeff1 + 4, adds11_2, ES::f32::QUARTA);*/

    w_ = c[1] + (-c[4] - c[5] + c[6] + c[7]) * 0.5f;
    x_ = c[0] - (c[4] + c[5] + c[6] + c[7]) * 0.5f;
    y_ = -c[2] + (c[4] - c[5] + c[6] - c[7]) * 0.5f;
    z_ = -c[3] + (c[4] - c[5] - c[6] + c[7]) * 0.5f;

    return *this;
}

inline Quaternion const &Quaternion::operator+= (float _s)
{
    w_ += _s;    x_ += _s;    y_ += _s;    z_ += _s;
    return *this;
}

inline Quaternion const &Quaternion::operator-= (float _s)
{
    w_ -= _s;    x_ -= _s;    y_ -= _s;    z_ -= _s;
    return *this;
}

inline Quaternion const &Quaternion::operator*= (float _s)
{
    w_ *= _s;    x_ *= _s;    y_ *= _s;    z_ *= _s;
    return *this;
}

inline Quaternion const &Quaternion::operator/= (float _s)
{
    float const s = 1.0f / _s;

    w_ *= s;    x_ *= s;    y_ *= s;    z_ *= s;
    return *this;
}

inline Quaternion const &Quaternion::operator+ () const
{
    return *this;
}

inline Quaternion Quaternion::operator- () const
{
    return Quaternion(-w_, -x_, -y_, -z_);
}

inline Quaternion const &Quaternion::Conjugate()
{
    w_ = -w_;
    return *this;
}

inline Quaternion const &Quaternion::Inverse()
{
    Conjugate();
    return *this /= GetNorm();
}

inline float Quaternion::GetNorm() const
{
    return w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_;
}

inline float Quaternion::GetLenght() const
{
    return sqrtf(GetNorm());
}

inline Quaternion const &Quaternion::Normalize()
{
    return *this /= GetLenght();
}

/*static*/ inline Quaternion Quaternion::GetNormalized(float const *const _q)
{
    return Quaternion(_q).Normalize();
}

/*static*/ inline Quaternion
Quaternion::GetNormalized(float _w, float _x, float _y, float _z)
{
    return Quaternion(_w, _x, _y, _z).Normalize();
}

inline Quaternion const &
Quaternion::FromAxisAngle(float const *const _axis, float _angle)
{
    const float sinAngle = sinf(DegToRad(_angle) * 0.5f);
    const float cosAngle = cosf(DegToRad(_angle) * 0.5f);

    w_ = cosAngle;
    x_ = _axis[0] * sinAngle;
    y_ = _axis[1] * sinAngle;
    z_ = _axis[2] * sinAngle;

    return *this;
}

inline Quaternion const &
Quaternion::FromMatrix4x4(float const *const _m)
{
    float S = _m[0] + _m[5] + _m[10];

    if (S > 0.0f) {
        S = sqrtf(S + 1.0f);

        w_ = S * 0.5f;
        S = 0.5f / S;

        x_ = (_m[6] - _m[9]) * S;
        y_ = (_m[8] - _m[2]) * S;
        z_ = (_m[1] - _m[4]) * S;
    }

    else {
        size_t i = 0;

        if (_m[5] > _m[0])
            i = 1;

        if (_m[10] > _m[(i << 2) + i])
            i = 2;

        size_t const nxt[3] = {1, 2, 0}, j = nxt[i], k = nxt[j];
        S = sqrtf((_m[(i << 2) + i] - (_m[(j << 2) + j] + _m[(k << 2) + k])) + 1.0f);

        *((&w_) + i + 1) = S * 0.5f;
        S = 0.5f / S;

        w_ = (_m[(j << 2) + k] - _m[(k << 2) + j]) * S;
        *((&w_) + j + 1) = (_m[(i << 2) + j] + _m[(j << 2) + i]) * S;
        *((&w_) + k + 1) = (_m[(i << 2) + k] + _m[(k << 2) + i]) * S;
    }

    return *this;
}

/*static*/ inline Quaternion
Quaternion::GetFromAxisAngle(float const *const _axis, float _angle)
{
    Quaternion q;
    return q.FromAxisAngle(_axis, _angle);
}

/*static*/ inline Quaternion Quaternion::GetFromMatrix4x4(float const *const _m)
{
    Quaternion q;
    return q.FromMatrix4x4(_m);
}

inline void Quaternion::ToAxisAngle(float *const _axis, float &_angle)
{
    Normalize();

    float sinHalfAngle = 1.0f - w_ * w_;

    if (sinHalfAngle <= 0.0f) {
        _angle = 0.0f;

        _axis[0] = 1.0f;
        _axis[1] = 0.0f;
        _axis[2] = 0.0f;
    }

    else {
        sinHalfAngle = 1.0f / sqrtf(sinHalfAngle);
        _angle = RadToDeg(2.0f * acosf(w_));

        _axis[0] = x_ * sinHalfAngle;
        _axis[1] = y_ * sinHalfAngle;
        _axis[2] = z_ * sinHalfAngle;
    }
}
};
};