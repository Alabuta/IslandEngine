/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 14th August 2009.
****    Description: three-component vector implementation.
****
********************************************************************************************************************************/
#include <utility>

namespace isle::math {

inline Vector Vector::operator+ (Vector const &_v) const
{
    return Vector(x + _v.x, y + _v.y, z + _v.z);
}

inline Vector Vector::operator- (Vector const &_v) const
{
    return Vector(x - _v.x, y - _v.y, z - _v.z);
}

inline Vector Vector::operator^ (Vector const &_v) const
{
    return Vector
    (
        y * _v.z - z * _v.y,
        z * _v.x - x * _v.z,
        x * _v.y - y * _v.x
    );
}

inline float Vector::operator* (Vector const &_v) const
{
    return x * _v.x + y * _v.y + z * _v.z;
}

inline Vector Vector::operator+ (float _s) const
{
    return Vector(x + _s, y + _s, z + _s);
}

inline Vector Vector::operator- (float _s) const
{
    return Vector(x - _s, y - _s, z - _s);
}

inline Vector Vector::operator* (float _s) const
{
    return Vector(x * _s, y * _s, z * _s);
}

inline Vector Vector::operator/ (float _s) const
{
    float const s = 1.0f / _s;
    return Vector(x * s, y * s, z * s);
}

inline Vector const &Vector::operator= (float _s)
{
    x = _s;  y = _s;  z = _s;
    return *this;
}

inline Vector const &Vector::operator= (Vector const &_v)
{
    if (this != &_v)
        vec = _v.vec;

    return *this;
}

inline Vector const &Vector::operator= (Vector &&_v)
{
    vec = std::move(_v.vec);
    return *this;
}

inline bool Vector::operator== (Vector const &_v) const
{
    return CloseEnough(x, _v.x)
        && CloseEnough(y, _v.y)
        && CloseEnough(z, _v.z);
}

inline bool Vector::operator!= (Vector const &_v) const
{
    return !(*this == _v);
}

inline Vector const &Vector::operator+= (Vector const &_v)
{
    x += _v.x;    y += _v.y;    z += _v.z;
    return *this;
}

inline Vector const &Vector::operator-= (Vector const &_v)
{
    x -= _v.x;    y -= _v.y;    z -= _v.z;
    return *this;
}

inline Vector const &Vector::operator^= (Vector const &_v)
{
    return *this = Vector(y * _v.z - z * _v.y,
                          z * _v.x - x * _v.z,
                          x * _v.y - y * _v.x);
}

inline Vector const &Vector::operator+= (float _s)
{
    x += _s;    y += _s;    z += _s;
    return *this;
}

inline Vector const &Vector::operator-= (float _s)
{
    x -= _s;    y -= _s;    z -= _s;
    return *this;
}

inline Vector const &Vector::operator*= (float _s)
{
    x *= _s;    y *= _s;    z *= _s;
    return *this;
}

inline Vector const &Vector::operator/= (float _s)
{
    float const s = 1.0f / _s;

    x *= s;    y *= s;    z *= s;
    return *this;
}

inline Vector const &Vector::operator+ () const
{
    return *this;
}

inline Vector Vector::operator- () const
{
    return Vector(-x, -y, -z);
}

inline Vector const &Vector::operator++ ()
{
    ++x;    ++y;    ++z;
    return *this;
}

inline Vector const &Vector::operator-- ()
{
    --x;    --y;    --z;
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
    return x * x + y * y + z * z;
}

inline float Vector::GetLenght() const
{
    return sqrtf(GetNorm());
}

inline Vector const &Vector::Normalize()
{
    return *this /= GetLenght();
}

/*static*/ inline Vector Vector::GetNormalized(float _x, float _y, float _z)
{
    return Vector(_x, _y, _z).Normalize();
}

/*static*/ inline Vector Vector::One()
{
    return Vector(1, 1, 1);
}

inline std::ostream &operator<< (std::ostream &_stream, Vector const &_v)
{
    return _stream << _v.x << "; " << _v.y << "; " << _v.z;
}
};