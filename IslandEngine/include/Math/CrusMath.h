/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    27th November 2013 / 10th March 2010.
****    Description: the math library and anything else.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_MATH_H                 // Include guard "CrusMath.h"
#define CRUS_MATH_H

#include <cmath>
#include <memory.h>

#include "System\CrusIsland.h"

// Disable any definitions min() and max() macroses.
#undef max
#undef min

namespace isle
{
namespace math
{
UNIT_TEST_HERITABLE_CLASS

float const kPI              = 3.14159265358979323846f;
float const kPI_DIV_180      = 0.01745329251994329576f;
float const kPI_DIV_180_IN   = 57.2957795130823208767f;

float const kEPSILON         = FLT_EPSILON;
float const kINF             = FLT_MAX;

class Matrix;
class Quaternion;
class Vector;

inline float DegToRad(float degree)
{
    return degree * kPI_DIV_180;
}

inline float RadToDeg(float radian)
{
    return radian * kPI_DIV_180_IN;
}

/*long double operator"" _deg(long double deg)
{
    return deg*3.141592 / 180;
}*/

inline bool CloseEnough(float a, float b)
{
    return crus::IsEqualBasedULP(a, b, kEPSILON, 10);
    //fabsf((a - b) / (b != 0.0f ? b : 1.0f)) < kEPSILON;
}

inline bool IsTooSmall(float x)
{
    return fabsf(x) < kEPSILON;
    //return CloseEnough(0.0f, x);
}

inline float min(float a, float b)
{
    return a < b ? a : b;
}

inline float max(float a, float b)
{
    return a > b ? a : b;
}

inline float clamp(float x, float min, float max)
{
    return math::max(math::min(x, max), min);
}
};
};

#endif // CRUS_MATH_H