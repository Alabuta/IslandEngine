/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 6th April 2012.
****    Description: mathlib routines unit-testing.
****
********************************************************************************************************************************/
#include "Math\CrusMath.h"

UNIT_SUITE_CLASS(isle::math)
{
    using xUnit::rand;

    float const result = (1.0f / 10.0f) * 10.0f;

    CHECK_EQUAL(kPI, DegToRad(180.0f), "DegToRad(float)");
    CHECK_EQUAL(180.0f, RadToDeg(kPI), "RadToDeg(float)");

    // :TODO: not good solution for floats compare...
    CHECK(CloseEnough(result, 1.0f), "CloseEnough(float, float)");
    CHECK(!IsTooSmall(kEPSILON), "IsTooSmall(float)");

    CHECK_EQUAL(2.9999f, clamp(2.9999f, 1.999999f, 2.999999f), "clamp()");
}