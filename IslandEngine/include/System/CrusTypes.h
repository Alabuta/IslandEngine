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

using i8  = __int8;
using i16 = __int16;
using i32 = __int32;
using i64 = __int64;

using u8  = unsigned __int8;
using u16 = unsigned __int16;
using u32 = unsigned __int32;
using u64 = unsigned __int64;

union real {
    float f;
    i32 i;
};

// Values sizes of application usertypes.
namespace crus
{
bool IsEqualBasedULP(float a, float b, float max_diff, i32 ulp_diff);
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

#if _CRUS_TEMP_DISABLED
template<class C, std::enable_if_t<!is_printable_t<std::decay_t<C>> && is_iterable_t<std::decay_t<C>>>...>
std::ostream &operator<< (std::ostream &stream, C &&container)
{
    using T = std::decay_t<C>;

    stream << "[ ";
    std::copy(std::cbegin(container), std::cend(container), std::ostream_iterator<typename T::value_type>(stream, " "));
    return stream << "]";
}
#endif


template<class... Ts>
constexpr std::array<std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>, sizeof...(Ts)> make_array(Ts &&...t)
{
    return {{std::forward<Ts>(t)...}};
}
};

#endif // CRUS_TYPES_H