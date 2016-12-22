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

inline UV::UV(UV const &_v) : UV::UV(_v.x_, _v.y_) {}

inline UV::UV(float const _v[]) : x_(_v[0]), y_(_v[1]) {}
inline UV::UV(float _x, float _y) : x_(_x), y_(_y) {}

inline UV::UV(UV &&_v)
{
    x_ = std::move(_v.x_);
    y_ = std::move(_v.y_);
}

inline float UV::x() const
{
    return x_;
}

inline float UV::y() const
{
    return y_;
}

inline float const *const UV::v() const
{
    return (v_);
};

inline float UV::x(float _x)
{
    return x_ = _x;
}

inline float UV::y(float _y)
{
    return y_ = _y;
}

inline UV const &UV::operator= (UV const &_v)
{
    if (this != &_v) {
        x_ = _v.x_;  y_ = _v.y_;
    }

    return *this;
}

inline UV const &UV::operator= (float _s)
{
    x_ = _s;  y_ = _s;
    return *this;
}

inline bool UV::operator== (UV const &_v) const
{
    return math::CloseEnough(x_, _v.x_)
        && math::CloseEnough(y_, _v.y_);
}

inline bool UV::operator!= (UV const &_v) const
{
    return !(*this == _v);
}

inline UV const &UV::operator= (UV &&_v)
{
    x_ = std::move(_v.x_);
    y_ = std::move(_v.y_);

    return *this;
}
};