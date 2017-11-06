/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: math lib routines unit-testing.
****
********************************************************************************************************************************/
#include "Math\CrusMath.h"

UNIT_SUITE_CLASS(isle::math)
{
    using xUnit::rand;

    CHECK_EQUAL(kPI, DegToRad(180.f), "DegToRad(float)");
    CHECK_EQUAL(180.f, RadToDeg(kPI), "RadToDeg(float)");

    // :TODO: not good solution for floats compare...
    CHECK(IsTooSmall(kMIN * 0.9999f), "IsTooSmall(float)");
    CHECK(!IsTooSmall(kMIN * 1.0001f), "IsTooSmall(float)");

    CHECK_EQUAL(2.9999f, clamp(2.9999f, 1.999999f, 2.999999f), "clamp()");

    {
        auto const x = rand<float>();

        CHECK(float_next(x) > x, "float_next(float)");
        CHECK(float_prior(x) < x, "float_prior(float)");

        CHECK(float_next(float_prior(x)) == x, "float_prior(float)");
    }

    {
        auto const x = 1.000000119209290f, y = 1.000000000000000f, z = 1.000000014901161f;

        CHECK(CloseEnough(x, y), "CloseEnough(float, float)");
        CHECK(CloseEnough(y, z), "CloseEnough(float, float)");
        CHECK(CloseEnough(z, x), "CloseEnough(float, float)");

        auto const f = 0.1f;
        auto w = 0.f;

        for (auto i = 0; i < 10; ++i)
            w += f;

        CHECK(CloseEnough(w, 1.f), "CloseEnough(float, float)");
        CHECK(CloseEnough(f * 10.f, 1.f), "CloseEnough(float, float)");

        CHECK(CloseEnough((1.f / 10.f) * 10.f, 1.f), "CloseEnough(float, float)");


        CHECK(CloseEnough(0.999999762f, 1.f), "CloseEnough(float, float)");
    }
}