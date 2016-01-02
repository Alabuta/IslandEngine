/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 6th April 2012.
****    Description: Matrix class unit-testing.
****
********************************************************************************************************************************/
#include "..\src\Math\CrusMatrix.cxx"

UNIT_SUITE_CLASS(isle::math::Matrix)
{
    using xUnit::rand;

    // The class constructor testing...
    {
        float const m[16] = {
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>()
        };

        Matrix const m1(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7],
                        m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);

        Matrix const m2(m);

        Matrix const m3(m2);

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m[i], m1.m_[i], "Matrix(float, float, ...)")

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m[i], m2.m_[i], "Matrix(float const [])")

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m2.m_[i], m3.m_[i], "Matrix(Matrix const &)")
    }

    // Getters...
    {
        Matrix const m(
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>()
        );

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m.m_[i], m.m()[i], "m() const")
    }

    // Setters...
    {
        float const m[16] = {
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>()
        };

        Matrix m1;

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m[i], m1.m()[i] = m[i], "m()")
    }

    // The sum and sub operators...
    {
        float const m[2][16] = {
            {
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>()
            },
            {
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>()
            }
        };

        Matrix const m1(m[0]), m2(m[1]);

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m[0][i] + m[1][i], (m1 + m2).m()[i], 
                        "operator+ (Matrix const &) const")

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m[0][i] - m[1][i], (m1 - m2).m()[i],
                        "operator- (Matrix const &) const")
    }

    // The sum, sub, mult and div with scalar operators...
    {
        Matrix const m(
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>()
        );

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
        Matrix const m0(
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>()
        );

        Matrix m1;

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m0.m()[i], (m1 = m0).m()[i], "operator= (Matrix const &)")
    }

    // The bool operators...
    {
        float const m[16] = {
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>()
        };

        Matrix const m1(m), m2(m);

        CHECK(m1 == m2, "operator== (Matrix const &) const")
        CHECK(!(m1 != m2), "operator!= (Matrix const &) const")
    }

    // The sum and sub operators...
    {
        float const m[2][16] = {
            {
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>()
            },
            {
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>(),
                rand<float>(), rand<float>(), rand<float>(), rand<float>()
            }
        };

        Matrix const m1(m[0]), m2(m[1]);
        Matrix m3;

        m3 = m1;
        m3 += m2;

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m[0][i] + m[1][i], m3.m()[i], "operator+= (Matrix const &)")

        m3 = m1;
        m3 -= m2;

        for(auto i = 0; i < 16; ++i)
            CHECK_EQUAL(m[0][i] - m[1][i], m3.m()[i], "operator-= (Matrix const &)")
    }

    // The sum, sub, mult and div with scalar operators...
    {
        Matrix const m1(
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>(),
            rand<float>(), rand<float>(), rand<float>(), rand<float>()
        );

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

    // The matrix routines...
    {
        Matrix const identity(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        Matrix m(
            1.3f,  5.0f,  9.0f,  3.1f,
           -2.4f,  6.0f, -2.3f,  4.9f,
            3.0f,  7.7f, -1.1f, -5.0f,
            4.0f, -8.1f,  2.0f,  6.0f
        );

        Matrix transposed(
            1.3f, -2.4f,  3.0f,  4.0f,
            5.0f,  6.0f, -2.3f,  4.9f,
            9.0f,  7.7f, -1.1f, -5.0f,
            3.1f, -8.1f,  2.0f,  6.0f
        );

        CHECK_EQUAL(-5322.2631f, m.Det(), "Det() const")

        CHECK(identity == m.MakeIdentity(), "MakeIdentity()")
    }
}