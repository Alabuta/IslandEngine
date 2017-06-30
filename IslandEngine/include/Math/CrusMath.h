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

#if CRUS_USE_SSE_MATH
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
    return crus::IsEqualBasedULP(a, b, kEPSILON, std::numeric_limits<float>::digits10);
    //fabsf((a - b) / (b != 0.0f ? b : 1.0f)) < kEPSILON;
}

inline bool IsTooSmall(float x)
{
    return std::abs(x) < kEPSILON;
    //return CloseEnough(0.0f, val);
}

inline float clamp(float x, float min, float max)
{
    return std::max(std::min(x, max), min);
}

inline float float_next(float val)
{
    if (std::isnan(val) || std::isinf(val)) {
        if(val < 0)
            return std::numeric_limits<float>::lowest();

        else throw std::invalid_argument("argument must be finite or real number.");
    }

    if(val >= std::numeric_limits<float>::max())
        throw std::overflow_error("argument must be less than largest finite value.");

    if (val == 0)
        return std::numeric_limits<float>::min();

    int exp;

    if (std::frexp(val, &exp) == -0.5f)
        --exp;

    auto difference = std::ldexp(1.f, exp - std::numeric_limits<float>::digits);

    if (difference == 0.f)
        difference = std::numeric_limits<float>::min();

    return val + difference;
}

inline float float_prior(float val)
{
    if (std::isnan(val) || std::isinf(val)) {
        if (val > 0)
            return std::numeric_limits<float>::max();

        else throw std::invalid_argument("argument must be finite or real number.");
    }

    if (val <= -std::numeric_limits<float>::max())
        throw std::overflow_error("argument must be less than largest finite value.");

    if (val == 0)
        return -std::numeric_limits<float>::min();

    int exp;

    if (std::frexp(val, &exp) == 0.5f)
        --exp;

    auto difference = std::ldexp(1.f, exp - std::numeric_limits<float>::digits);

    if (difference == 0.f)
        difference = std::numeric_limits<float>::min();

    return val - difference;
}

#if _CRUS_NOT_YET_IMPLEMENTED
inline float float_distance(float a, float b)
{
    if (!std::isfinite(a))
        throw std::invalid_argument("argument 'a' must be finite.");

    if (!std::isfinite(b))
        throw std::invalid_argument("argument 'b' must be finite.");

    if (a > b)
        return -float_distance(a, b);

    if (a == b)
        return 0;

    if (a == 0)
        return 1 + std::abs(float_distance(b < 0 ? -std::numeric_limits<float>::min() : std::numeric_limits<float>::min(), b));

    if (b == 0)
        return 1 + std::abs(float_distance(a < 0 ? -std::numeric_limits<float>::min() : std::numeric_limits<float>::min(), a));

    if (std::signbit(a) != std::signbit(b))
        return 2 + std::abs(float_distance(b < 0 ? -std::numeric_limits<float>::min() : std::numeric_limits<float>::min(), b))
                 + std::abs(float_distance(a < 0 ? -std::numeric_limits<float>::min() : std::numeric_limits<float>::min(), a));

    if (a < 0)
        return float_distance(-b, -a);

    int exp;

    std::frexp(val, &exp);
}
#endif
};
};

inline bool crus::IsInf(real _f)
{
    return (_f.i_ & 0x7FFFFFFF) == 0x7F800000;
}

inline bool crus::IsNan(real _f)
{
    return ((_f.i_ & 0x7F800000) == 0x7F800000 && (_f.i_ & 0x7F800000) != 0);
}

// :TODO: move to other place.
inline bool crus::IsEqualBasedULP(float _a, float _b, float _max_diff, int32 _ulp_diff)
{
    real a, b;

    a.f_ = _a;
    b.f_ = _b;

#if _CRUS_REAL_CHECK_ON_INF
    if(IsInf(a) || IsInf(b))
        return a.f_ == _b.f_;
#endif

#if _CRUS_REAL_CHECK_ON_NAN
    if(IsNan(a) || IsNan(b))
        return false;
#endif

    if(fabsf(a.f_ - b.f_) <= _max_diff * (b.f_ > a.f_ ? b.f_ : a.f_))
        return true;

    /*if (std::abs(a.f_ - b.f_) <= _max_diff * std::max(b.f_ > a.f_ ? b.f_ : a.f_, std::numeric_limits<float>::epsilon()))
        return true;*/

    if((a.i_ >> 31) != (b.i_ >> 31))
        return false;

    if(abs(a.i_ - b.i_) <= _ulp_diff)
        return true;

    return false;
}

/*
    The UPL steps and precisions

     Float     Length       Float     Length               Precision 
     Value      Value   Precision  Precision                    Size
    —————————————————————————————————————————————————————————————————
     10^0     1   m      1.19e-07    119  nm    virus
     10^1     10  m      9.54E-07    .954 mm    e. coli bacteria
     10^2     100 m      7.63E-06    7.63 mm    red blood cell
     10^3     1   km     6.10E-05    61.0 mm    human hair width
     10^4     10  km     0.000977    .976 mm    toenail thickness
     10^5     100 km     0.00781     7.81 mm    size of an ant
     10^6   .16x e. r.   0.0625      62.5 mm    credit card width
     10^7   1.6x e. r.   1           1    m     uh… a meter
     10^8   .14x s. r.   8           8    m     4 Chewbaccas
     10^9   1.4x s. r.   64          64   m     half a football field
*/

#endif // CRUS_MATH_H