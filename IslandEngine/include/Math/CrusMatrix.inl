/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: Matrix4x4 inlined functions implementation.
****
********************************************************************************************************************************/

namespace isle::math {

inline Matrix Matrix::operator/ (Matrix const &_m) const
{
    return *this * Inverse(_m);
}

inline Matrix Matrix::operator/ (float _s) const
{
    auto const s = 1.0f / _s;
    return *this * s;
}

inline Matrix &Matrix::operator= (Matrix &&_m)
{
    m = std::move(_m.m);

    return *this;
}

inline Matrix &Matrix::operator= (Matrix const &_m)
{
    if (this != &_m)
        m = _m.m;

    return *this;
}

inline bool Matrix::operator!= (Matrix const &_m) const
{
    return !(*this == _m);
}

inline Matrix &Matrix::operator/= (Matrix const &_m)
{
    return *this *= Inverse(_m);
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

// :TODO: maybe it was better if deleted.
/*static inline Matrix Matrix::GetFromQuaternion(float const *const _q)
{
    Matrix m;
    return m.FromQuaternion(_q);
}*/
};