/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: the math library and anything else.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_MATH_H                 // Include guard "CrusMath.h"
#define CRUS_MATH_H

#include <cmath>
#include <memory>
#include <algorithm>
#include <array>
#include <limits>

#include "System\CrusIsland.h"
#include "System\CrusTypes.h"

#ifdef CRUS_USE_SSE_MATH
extern "C"
{
#include <mmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
#include <wmmintrin.h>
#include <tmmintrin.h>
#include <immintrin.h>   // (Meta-header)
}

#include <intrin.h>
#endif

// Disable any definitions min() and max() macroses.
#undef max
#undef min

namespace isle {

struct Point {
    union {
        struct {
            float x, y;
        };

        struct {
            float pt[2];
        };
    };
};

namespace math {
UNIT_TEST_HERITABLE_CLASS

auto constexpr kPI = 3.14159265358979323846f;
auto constexpr kPI_DIV_180 = 0.01745329251994329576f;
auto constexpr kPI_DIV_180_INV = 57.2957795130823208767f;

auto constexpr kEPSILON = std::numeric_limits<float>::epsilon();
auto constexpr kINF = std::numeric_limits<float>::infinity();

class Matrix;
class Quaternion;
class Vector;

inline float DegToRad(float degree)
{
    return degree * kPI_DIV_180;
}

inline float RadToDeg(float radian)
{
    return radian * kPI_DIV_180_INV;
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

inline float clamp(float x, float min, float max)
{
    return std::max(std::min(x, max), min);
}
};
};

#endif // CRUS_MATH_H