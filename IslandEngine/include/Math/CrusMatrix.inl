/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: Matrix4x4 inlined functions implementation.
****
********************************************************************************************************************************/

namespace isle::math {
inline Matrix::Matrix(Matrix &&_m)
{
    *this = std::move(_m);
}

inline Matrix::Matrix(Matrix const &_m)
{
    *this = _m;
}

inline Matrix::Matrix(std::array<float, 16> &&_vec)
{
    vec_ = std::move(_vec);
}

inline Matrix::Matrix(std::array<float, 16> const &_vec)
{
    vec_ = _vec;
}

inline std::array<float, 16> const &Matrix::m() const
{
    return vec_;
}

inline std::array<float, 16> &Matrix::m()
{
    return vec_;
}

inline Matrix Matrix::operator/ (Matrix const &_m) const
{
    return *this * _m.Inverse();
}

inline Matrix Matrix::operator/ (float _s) const
{
    auto const s = 1.0f / _s;
    return *this * s;
}

inline Matrix &Matrix::operator= (Matrix &&_m)
{
    vec_ = std::move(_m.vec_);

    return *this;
}

inline Matrix &Matrix::operator= (Matrix const &_m)
{
    if (this != &_m)
        vec_ = _m.vec_;

    return *this;
}

inline bool Matrix::operator!= (Matrix const &_m) const
{
    return !(*this == _m);
}

inline Matrix &Matrix::operator/= (Matrix const &_m)
{
    return *this *= _m.Inverse();
}

inline Matrix &Matrix::operator/= (float _s)
{
    auto const s = 1.f / _s;
    return *this *= s;
}

/*static*/ inline Matrix Matrix::Identity()
{
    return Matrix
    (
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

inline Vector &Matrix::xAxis()
{
    return xAxis_;
}

inline Vector &Matrix::yAxis()
{
    return yAxis_;
}

inline Vector &Matrix::zAxis()
{
    return zAxis_;
}

inline Vector Matrix::origin() const
{
    return Vector(x_, y_, z_);
}

inline float &Matrix::xOrigin()
{
    return x_;
}

inline float &Matrix::yOrigin()
{
    return y_;
}

inline float &Matrix::zOrigin()
{
    return z_;
}

// :TODO: maybe it was better if deleted.
/*static inline Matrix Matrix::GetFromQuaternion(float const *const _q)
{
    Matrix m;
    return m.FromQuaternion(_q);
}*/
};