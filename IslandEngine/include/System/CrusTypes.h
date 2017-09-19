/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 8th July 2009.
****    Description: engine types - all of user types must be here.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_TYPES_H                // Include guard "CrusTypes.h"
#define CRUS_TYPES_H

#include <cfloat>
#include <cmath>
#include <iterator>
#include <type_traits>

// Engine types.
using astr  = char const *;
using acstr = char const *const;
using wstr  = wchar_t const *;
using wcstr = wchar_t const *const;

using byte = unsigned __int8;

using int8  = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;

using uint8  = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

union real {
    float f_;
    int32 i_;
};

// Values sizes of application usertypes.
namespace crus
{
bool IsInf(real f);
bool IsNan(real f);

bool IsEqualBasedULP(float a, float b, float max_diff, int32 ulp_diff);
};



namespace isle {
template<class C, class = void>
struct is_printable : std::false_type { };

template<class C>
struct is_printable<C, std::void_t<decltype(std::cout << std::declval<C>())>> : std::true_type { };

/*template<class T>
constexpr bool is_printable_v = is_printable<T>::value;*/

template<class T>
constexpr bool is_printable_t = std::is_same_v<is_printable<T>, std::true_type>;

template<class C, class = void>
struct is_iterable : std::false_type { };

template<class C>
struct is_iterable<C, std::void_t<decltype(std::cbegin(std::declval<C>()), std::cend(std::declval<C>()))>> : std::true_type { };

template<class T>
constexpr bool is_iterable_v = is_iterable<T>::value;
};

#endif // CRUS_TYPES_H