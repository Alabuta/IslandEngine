/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****    Description: vertex attributes declaration.
****
********************************************************************************************************************************/

namespace isle {

inline Position::Position(Position const &_v) : Position::Position(_v.x_, _v.y_, _v.z_) {}

inline Position::Position(float const _v[]) : x_(_v[0]), y_(_v[1]), z_(_v[2]) {}
inline Position::Position(float _x, float _y, float _z) : x_(_x), y_(_y), z_(_z) {}

inline Position::Position(Position &&_v)
{
    x_ = std::move(_v.x_);
    y_ = std::move(_v.y_);
    z_ = std::move(_v.z_);
}

inline float Position::x() const
{
    return x_;
}

inline float Position::y() const
{
    return y_;
}

inline float Position::z() const
{
    return z_;
}

inline float const *const Position::v() const
{
    return (v_);
};

inline float Position::x(float _x)
{
    return x_ = _x;
}

inline float Position::y(float _y)
{
    return y_ = _y;
}

inline float Position::z(float _z)
{
    return z_ = _z;
}

inline Position const &Position::operator= (Position const &_v)
{
    if (this != &_v) {
        x_ = _v.x_;  y_ = _v.y_;  z_ = _v.z_;
    }

    return *this;
}

inline Position const &Position::operator= (float _s)
{
    x_ = _s;  y_ = _s;  z_ = _s;

    return *this;
}

inline bool Position::operator== (Position const &_v) const
{
    return math::CloseEnough(x_, _v.x_)
        && math::CloseEnough(y_, _v.y_)
        && math::CloseEnough(z_, _v.z_);
}

inline bool Position::operator!= (Position const &_v) const
{
    return !(*this == _v);
}

inline Position const &Position::operator= (Position &&_v)
{
    x_ = std::move(_v.x_);
    y_ = std::move(_v.y_);
    z_ = std::move(_v.z_);

    return *this;
}
};