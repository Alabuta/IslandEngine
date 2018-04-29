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

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
inline Vector Vector::operator+ (T &&_v) const
{
    return Vector(x + _v.x, y + _v.y, z + _v.z);
}

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
inline Vector Vector::operator- (T &&_v) const
{
    return Vector(x - _v.x, y - _v.y, z - _v.z);
}

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
inline Vector Vector::operator^ (T &&_v) const
{
    return Vector
    (
        y * _v.z - z * _v.y,
        z * _v.x - x * _v.z,
        x * _v.y - y * _v.x
    );
}

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
inline float Vector::operator* (T &&_v) const
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

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
inline bool Vector::operator== (T &&_v) const
{
    return CloseEnough(x, _v.x)
        && CloseEnough(y, _v.y)
        && CloseEnough(z, _v.z);
}

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
inline bool Vector::operator!= (T &&_v) const
{
    return !(*this == _v);
}

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
inline Vector const &Vector::operator+= (T &&_v)
{
    x += _v.x;    y += _v.y;    z += _v.z;
    return *this;
}

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
inline Vector const &Vector::operator-= (T &&_v)
{
    x -= _v.x;    y -= _v.y;    z -= _v.z;
    return *this;
}

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
inline Vector const &Vector::operator^= (T &&_v)
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

inline Vector &Vector::operator+ ()
{
    return *this;
}

inline Vector Vector::operator- () const
{
    return Vector(-x, -y, -z);
}

inline Vector &Vector::operator++ ()
{
    ++x;    ++y;    ++z;
    return *this;
}

inline Vector &Vector::operator-- ()
{
    --x;    --y;    --z;
    return *this;
}

inline Vector operator++ (Vector &_v, i32)
{
    Vector ret(_v);
    ++_v;
    return ret;
}

inline Vector operator-- (Vector &_v, i32)
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
    return std::sqrt(GetNorm());
}

inline Vector const &Vector::Normalize()
{
    return *this /= GetLenght();
}

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
/*static*/ inline Vector isle::math::Vector::GetNormalized(T &&v)
{
    if constexpr (std::is_rvalue_reference_v<T>)
        return std::move(v.Normalize());

    else return Vector(v).Normalize();
}

/*static*/ inline Vector Vector::GetNormalized(float _x, float _y, float _z)
{
    return Vector(_x, _y, _z).Normalize();
}

template<class T, typename std::enable_if_t<is_vector_t<T>>...>
inline Vector &isle::math::Vector::LerpStable(T &&v, float alpha)
{
    return *this = *this * (1.f - alpha) + v * alpha;
}

/*static*/ inline constexpr Vector Vector::One()
{
    return Vector(1, 1, 1);
}

inline std::ostream &operator<< (std::ostream &_stream, Vector const &_v)
{
    return _stream << _v.x << "; " << _v.y << "; " << _v.z;
}
};