/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 10th March 2010.
****    Description: Matrix4x4 inlined functions implementation.
****
********************************************************************************************************************************/

namespace isle::math {
inline Matrix::Matrix() { }
inline Matrix::~Matrix() { }

inline Matrix::Matrix(Matrix const &_m)
{
    memcpy(m_, _m.m_, sizeof(m_));
}

inline Matrix::Matrix(float const _m[])
{
    memcpy(m_, _m, sizeof(m_));
}

inline float const *const Matrix::m() const
{
    return m_;
};

inline float *Matrix::m()
{
    return m_;
};

inline Matrix Matrix::operator/ (Matrix const &_m) const
{
    return *this * _m.Inverse();
}

inline Matrix Matrix::operator/ (float _s) const
{
    float const s = 1.0f / _s;
    return *this * s;
}

inline Matrix const &Matrix::operator= (Matrix const &_m)
{
    if (this != &_m)
        memcpy(m_, _m.m_, sizeof(m_));

    return *this;
}

inline bool Matrix::operator!= (Matrix const &_m) const
{
    return !(*this == _m);
}

inline Matrix const &Matrix::operator/= (Matrix const &_m)
{
    return *this *= _m.Inverse();
}

inline Matrix const &Matrix::operator/= (float _s)
{
    float const s = 1.0f / _s;
    return *this *= s;
}

// :TODO: maybe it was better if deleted.
/*static inline Matrix Matrix::GetFromQuaternion(float const *const _q)
{
    Matrix m;
    return m.FromQuaternion(_q);
}*/
};