/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 14th August 2009.
****    Description: three-component vector implementation.
****
********************************************************************************************************************************/
#include <utility>

namespace isle
{
namespace math
{
inline Vector::Vector() {}
inline Vector::~Vector() {}

inline Vector::Vector(Vector const &_v) : Vector::Vector(_v.x_, _v.y_, _v.z_) {}

inline Vector::Vector(float const _v[]) : Vector::Vector(_v[0],_v[1], _v[2]) {}
inline Vector::Vector(float _x, float _y, float _z) : x_(_x), y_(_y), z_(_z) {}

inline Vector::Vector(Vector &&_v)
{
    x_ = std::move(_v.x_);
    y_ = std::move(_v.y_);
    z_ = std::move(_v.z_);
}

inline float Vector::x() const
{
    return x_;
}

inline float Vector::y() const
{
    return y_;
}

inline float Vector::z() const
{
    return z_;
}

inline float const *const Vector::v() const
{
    return (&x_);
}

inline float *Vector::v()
{
    return (&x_);
}

inline float Vector::x(float _x)
{
    return x_ = _x;
}

inline float Vector::y(float _y)
{
    return y_ = _y;
}

inline float Vector::z(float _z)
{
    return z_ = _z;
}

inline float &Vector::x()
{
    return x_;
}

inline float &Vector::y()
{
    return y_;
}

inline float &Vector::z()
{
    return z_;
}

inline Vector Vector::operator+ (Vector const &_v) const
{
    return Vector(x_ + _v.x_, y_ + _v.y_, z_ + _v.z_);
}

inline Vector Vector::operator- (Vector const &_v) const
{
    return Vector(x_ - _v.x_, y_ - _v.y_, z_ - _v.z_);
}

inline Vector Vector::operator^ (Vector const &_v) const
{
    return Vector
    (
        y_ * _v.z_ - z_ * _v.y_,
        z_ * _v.x_ - x_ * _v.z_,
        x_ * _v.y_ - y_ * _v.x_
    );
}

inline float Vector::operator* (Vector const &_v) const
{
    return x_ * _v.x_ + y_ * _v.y_ + z_ * _v.z_;
}

inline Vector Vector::operator+ (float _s) const
{
    return Vector(x_ + _s, y_ + _s, z_ + _s);
}

inline Vector Vector::operator- (float _s) const
{
    return Vector(x_ - _s, y_ - _s, z_ - _s);
}

inline Vector Vector::operator* (float _s) const
{
    return Vector(x_ * _s, y_ * _s, z_ * _s);
}

inline Vector Vector::operator/ (float _s) const
{
    float const s = 1.0f / _s;
    return Vector(x_ * s, y_ * s, z_ * s);
}

inline Vector const &Vector::operator= (Vector const &_v)
{
    if(this != &_v){
        x_ = _v.x_;  y_ = _v.y_;  z_ = _v.z_;
    }

    return *this;
}

inline Vector const &Vector::operator= (float _s)
{
    x_ = _s;  y_ = _s;  z_ = _s;
    return *this;
}

inline Vector const &Vector::operator= (Vector &&_v)
{
    x_ = std::move(_v.x_);
    y_ = std::move(_v.y_);
    z_ = std::move(_v.z_);
    return *this;
}

inline bool Vector::operator== (Vector const &_v) const
{
    return CloseEnough(x_, _v.x_)
        && CloseEnough(y_, _v.y_)
        && CloseEnough(z_, _v.z_);
}

inline bool Vector::operator!= (Vector const &_v) const
{
    return !(*this == _v);
}

inline Vector const &Vector::operator+= (Vector const &_v)
{
    x_ += _v.x_;    y_ += _v.y_;    z_ += _v.z_;
    return *this;
}

inline Vector const &Vector::operator-= (Vector const &_v)
{
    x_ -= _v.x_;    y_ -= _v.y_;    z_ -= _v.z_;
    return *this;
}

inline Vector const &Vector::operator^= (Vector const &_v)
{
    return *this = Vector(y_ * _v.z_ - z_ * _v.y_,
                          z_ * _v.x_ - x_ * _v.z_,
                          x_ * _v.y_ - y_ * _v.x_);
}

inline Vector const &Vector::operator+= (float _s)
{
    x_ += _s;    y_ += _s;    z_ += _s;
    return *this;
}

inline Vector const &Vector::operator-= (float _s)
{
    x_ -= _s;    y_ -= _s;    z_ -= _s;
    return *this;
}

inline Vector const &Vector::operator*= (float _s)
{
    x_ *= _s;    y_ *= _s;    z_ *= _s;
    return *this;
}

inline Vector const &Vector::operator/= (float _s)
{
    float const s = 1.0f / _s;

    x_ *= s;    y_ *= s;    z_ *= s;
    return *this;
}

inline Vector const &Vector::operator+ () const
{
    return *this;
}

inline Vector Vector::operator- () const
{
    return Vector(-x_, -y_, -z_);
}

inline Vector const &Vector::operator++ ()
{
    ++x_;    ++y_;    ++z_;
    return *this;
}

inline Vector const &Vector::operator-- ()
{
    --x_;    --y_;    --z_;
    return *this;
}

inline Vector operator++ (Vector &_v, int32)
{
    Vector ret(_v);
    ++_v;
    return ret;
}

inline Vector operator-- (Vector &_v, int32)
{
    Vector ret(_v);
    --_v;
    return ret;
}

inline float Vector::GetNorm() const
{
    return x_ * x_ + y_ * y_ + z_ * z_;
}

inline float Vector::GetLenght() const
{
    return sqrtf(GetNorm());
}

inline Vector const &Vector::Normalize()
{
    return *this /= GetLenght();
}

/*static*/ inline Vector Vector::GetNormalized(float const *const _v)
{
    return Vector(_v).Normalize();
}

/*static*/ inline Vector Vector::GetNormalized(float _x, float _y, float _z)
{
    return Vector(_x, _y, _z).Normalize();
}
};
};