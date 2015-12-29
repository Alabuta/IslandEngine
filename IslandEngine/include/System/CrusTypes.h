/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    20th April 2012 / 8th July 2009.
****    Description: engine types - all of user types must be here.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_TYPES_H                // Include guard "CrusTypes.h"
#define CRUS_TYPES_H

#include <cfloat>
#include <cmath>

// Engine types.
typedef char const *astr;
typedef char const *const acstr;
typedef wchar_t const *wstr;
typedef wchar_t const *const wcstr;

typedef unsigned char byte;

typedef __int8  int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

union real {
    float f_;
    int32 i_;
};

// Values sizes of application usertypes.
namespace crus
{
inline bool IsInf(real _f)
{
    return (_f.i_ & 0x7FFFFFFF) == 0x7F800000;
}

inline bool IsNan(real _f)
{
    return ((_f.i_ & 0x7F800000) == 0x7F800000 && (_f.i_ & 0x7F800000) != 0);
}

bool IsEqualBasedULP(float a, float b, float max_diff, int32 ulp_diff);
};

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

    if(fabsf(a.f_ - b.f_) <= _max_diff * ((b.f_ > a.f_) ? b.f_ : a.f_))
        return true;

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

#endif // CRUS_TYPES_H