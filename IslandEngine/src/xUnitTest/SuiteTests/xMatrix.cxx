/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: Matrix class unit-testing.
****
********************************************************************************************************************************/
#include <numeric>
#include "..\src\Math\CrusMatrix.cxx"

UNIT_SUITE_CLASS(isle::math::Matrix)
{
    using xUnit::rand;

    // The class constructor testing...
    {
        std::array<float, 16> vec;
        for (auto &a : vec)
            a = rand<float>();

        Matrix const m1(vec[0], vec[1], vec[02], vec[03], vec[04], vec[05], vec[06], vec[07],
                        vec[8], vec[9], vec[10], vec[11], vec[12], vec[13], vec[14], vec[15]);

        for (auto i = 0; i < 16; ++i)
            CHECK_EQUAL(vec[i], m1.vec_[i], "Matrix(float, float, ...)");

        Matrix const m2(vec);
        Matrix const m3(std::array<float, 16>{vec[0], vec[1], vec[02], vec[03], vec[04], vec[05], vec[06], vec[07],
                         vec[8], vec[9], vec[10], vec[11], vec[12], vec[13], vec[14], vec[15]});

        for (auto i = 0; i < 16; ++i)
            CHECK_EQUAL(vec[i], m2.vec_[i], "Matrix(std::array<float, 16> const &)");

        for (auto i = 0; i < 16; ++i)
            CHECK_EQUAL(vec[i], m3.vec_[i], "Matrix(std::array<float, 16> &&)");

        Matrix const m4(m2);
        Matrix const m5(std::move(Matrix(vec)));

        for (auto i = 0; i < 16; ++i)
            CHECK_EQUAL(vec[i], m4.vec_[i], "Matrix(Matrix const &)");

        for (auto i = 0; i < 16; ++i)
            CHECK_EQUAL(vec[i], m5.vec_[i], "Matrix(Matrix &&)");
    }

    // Getters...
    {
        std::array<float, 16> vec;
        for (auto &a : vec)
            a = rand<float>();

        Matrix const m(vec);

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m.vec_[i], m.m()[i], "m() const")
    }

    // Setters...
    {
        std::array<float, 16> vec;
        for (auto &a : vec)
            a = rand<float>();

        Matrix m1;

        for (auto i = 0; i < 16; ++i)
            CHECK_EQUAL(vec[i], m1.m()[i] = vec[i], "m()");
    }

    // The sum and sub operators...
    {
        std::array<float, 16> vecs[2];

        for (auto &vec : vecs)
            for (auto &a : vec)
                a = rand<float>();

        Matrix const m1(vecs[0]), m2(vecs[1]);

        for (auto i = 0; i < 16; ++i)
            CHECK_EQUAL(vecs[0][i] + vecs[1][i], (m1 + m2).m()[i], "operator+ (Matrix const &) const");

        for (auto i = 0; i < 16; ++i)
            CHECK_EQUAL(vecs[0][i] - vecs[1][i], (m1 - m2).m()[i], "operator- (Matrix const &) const");
    }

    // The sum, sub, mult and div with scalar operators...
    {
        std::array<float, 16> vec;
        for (auto &a : vec)
            a = rand<float>();

        Matrix const m(vec);

        float const s = rand<float>();

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m.m()[i] + s, (m + s).m()[i], "operator+ (float) const")

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m.m()[i] - s, (m - s).m()[i], "operator- (float) const")

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m.m()[i] * s, (m * s).m()[i], "operator* (float) const")

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m.m()[i] / s, (m / s).m()[i], "operator/ (float) const")
    }

    // The assignment operators...
    {
        std::array<float, 16> vec;
        for (auto &a : vec)
            a = rand<float>();

        Matrix const m0(vec);

        Matrix m1;

        for (auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m0.m()[i], (m1 = m0).m()[i], "operator= (Matrix const &)");

        m1 = m1;

        for (auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m0.m()[i], (m1 = m0).m()[i], "operator= (Matrix const &)");
    }

    // The bool operators...
    {
        std::array<float, 16> vec;
        for (auto &a : vec)
            a = rand<float>();

        Matrix const m1(vec), m2(vec);

        CHECK(m1 == m2, "operator== (Matrix const &) const")
        CHECK(!(m1 != m2), "operator!= (Matrix const &) const")
    }

    // The sum and sub operators...
    {
        std::array<float, 16> vecs[2];

        for (auto &vec : vecs)
            for (auto &a : vec)
                a = rand<float>();

        Matrix const m1(vecs[0]), m2(vecs[1]);
        Matrix m3;

        m3 = m1;
        m3 += m2;

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(vecs[0][i] + vecs[1][i], m3.m()[i], "operator+= (Matrix const &)")

        m3 = m1;
        m3 -= m2;

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(vecs[0][i] - vecs[1][i], m3.m()[i], "operator-= (Matrix const &)")
    }

    // The sum, sub, mult and div with scalar operators...
    {
        std::array<float, 16> vec;
        for (auto &a : vec)
            a = rand<float>();

        Matrix const m1(vec);

        float const s = rand<float>();

        Matrix m2;

        m2 = m1;
        m2 += s;

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m1.m()[i] + s, m2.m()[i], "operator/= (float)")

        m2 = m1;
        m2 -= s;

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m1.m()[i] - s, m2.m()[i], "operator/= (float)")

        m2 = m1;
        m2 *= s;

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m1.m()[i] * s, m2.m()[i], "operator/= (float)")

        m2 = m1;
        m2 /= s;

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m1.m()[i] / s, m2.m()[i], "operator/= (float)")
    }

    // The matrix determinant routines...
    {
        Matrix m(
            +1.3f, +5.0f, +9.0f, +3.1f,
            -2.4f, +6.0f, -2.3f, +4.9f,
            +3.0f, +7.7f, -1.1f, -5.0f,
            +4.0f, -8.1f, +2.0f, +6.0f
        );

        CHECK_EQUAL(-5322.2631f, m.Det(), "Det() const")
    }

    // Identity matrix creation routines...
    {
        Matrix const identity(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        std::array<float, 16> vec;
        for (auto &a : vec)
            a = rand<float>();

        Matrix m(vec);

        CHECK(identity == Matrix::Identity(), "Identity()");
        CHECK(identity == m.MakeIdentity(), "MakeIdentity()");
    }

    // The matrix transpose routines...
    {
        Matrix m(
            1.f, -2.f, -3.f, -4.f,
            5.f, -6.f, -7.f, -8.f,
            9.f, -10.f, 11.f, 12.f,
            13.f, 14.f, 15.f, 16.f
        );

        Matrix const transposed(
            +1.f, +5.f, +9.f, +13.f,
            -2.f, -6.f, -10.f, 14.f,
            -3.f, -7.f, +11.f, 15.f,
            -4.f, -8.f, +12.f, 16.f
        );

        CHECK(transposed == Matrix::Transpose(m), "Transpose(Matrix const &)");
        CHECK(transposed == m.Transpose(), "Transpose()");
    }

    // The matrix multiplication operators...
    {
        std::array<float, 16> vec;
        for(auto &a : vec)
            a = rand<float>();

        Matrix const m(vec);

        CHECK(Matrix::Identity() == Matrix::Identity() * Matrix::Identity(), "Matrix operator* (Matrix const &) const");

        CHECK(m == Matrix::Identity() * m, "Matrix operator* (Matrix const &) const");
        CHECK(m == m * Matrix::Identity(), "Matrix operator* (Matrix const &) const");

        Matrix const m0(
            +10.f, 20.f, 30.f, 40.f,
            -6.f,  20.f, 46.f, 72.f,
            -22.f, 20.f, 62.f, 104.f,
            -38.f, 20.f, 78.f, 136.f
        );

        Matrix m1;
        std::iota(m1.vec_.begin(), m1.vec_.end(), 1.f);

        Matrix m2;
        std::iota(m2.vec_.begin(), m2.vec_.end(), -7.f);

        CHECK(m0 == m1 * m2, "Matrix operator* (Matrix const &) const");
        CHECK(m0 != m2 * m1, "Matrix operator* (Matrix const &) const");

        auto m3 = m1;

        CHECK(m0 == (m1 *= m2), "operator*= (Matrix const &)");
        CHECK(m0 != (m2 *= m3), "operator*= (Matrix const &)");
    }

    // The matrix translation routines...
    /*{

        CHECK(saved == vec, "TransformPosition(Vector &) const");
    }*/

    // The transform position by matrix routines...
    {
        std::array<float, 3> vec{rand<float>(), rand<float>(), rand<float>()};

        Vector v(vec);
        auto const saved = v;

        auto m = Matrix::Identity();
        m.TransformPosition(v);

        CHECK(saved == v, "TransformPosition(Vector &) const");

        m.Translate(-vec[0], -vec[1], -vec[2]);
        m.TransformPosition(v);

        CHECK(Vector(0, 0, 0) == v, "TransformPosition(Vector &) const");
        CHECK(Vector(0, 0, 0) == m.TransformPosition(saved), "TransformPosition(Vector &) const");
        CHECK(Vector(0, 0, 0) == m.TransformPosition(Vector(vec)), "TransformPosition(Vector &&) const");

        m = Matrix::Identity();
        m.RotateOnOZ(90.f);

        //CHECK(Vector(-1, 0, 0) == m.TransformPosition(Vector(0, 1, 0)), "TransformPosition(Vector &&) const");
    }

    // The matrix inverse routines...
    /*{
    std::array<float, 16> vec;
    for (auto &a : vec)
    a = rand<float>();

    Matrix const m(vec);

    auto transposed = Matrix::Transpose(m);

    CHECK(Matrix::Identity() == transposed * m, "Matrix operator* (Matrix const &) const");
    }*/
}