/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****    Description: mesh attributes declaration.
****
********************************************************************************************************************************/

namespace isle
{
inline Vertex::Vertex() {}
inline Vertex::~Vertex() {}

inline Vertex::Vertex(float const _v[]) : x_(_v[0]), y_(_v[1]), z_(_v[2]) {}
inline Vertex::Vertex(float _x, float _y, float _z) : x_(_x), y_(_y), z_(_z) {}

inline float Vertex::x() const
{
    return x_;
}

inline float Vertex::y() const
{
    return y_;
}

inline float Vertex::z() const
{
    return z_;
}

inline float const *const Vertex::v() const
{
    return (&x_);
};

inline float Vertex::x(float _x)
{
    return x_ = _x;
}

inline float Vertex::y(float _y)
{
    return y_ = _y;
}

inline float Vertex::z(float _z)
{
    return z_ = _z;
}

inline Vertex const &Vertex::operator= (Vertex const &_v)
{
    if(this != &_v){
        x_ = _v.x_;  y_ = _v.y_;  z_ = _v.z_;
    }

    return *this;
}

inline Vertex const &Vertex::operator= (float _s)
{
    x_ = _s;  y_ = _s;  z_ = _s;
    return *this;
}

inline bool Vertex::operator== (Vertex const &_v) const
{
    return math::CloseEnough(x_, _v.x_)
        && math::CloseEnough(y_, _v.y_)
        && math::CloseEnough(z_, _v.z_);
}

inline bool Vertex::operator!= (Vertex const &_v) const
{
    return !(*this == _v);
}
};