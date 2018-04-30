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

template<class C>
constexpr bool is_iterable_v = is_iterable<C>::value;

template<class C, class = void>
struct is_container : std::false_type { };

template<class C>
struct is_container<C, std::void_t<decltype(std::size(std::declval<C>()), std::data(std::declval<C>()))>> : std::true_type { };

template<class C>
constexpr bool is_container_v = is_container<C>::value;

template<class T> struct always_false : std::false_type { };

template<class T, class... Ts>
struct are_same_types {
    static auto constexpr value_type = std::conjunction_v<std::is_same<T, std::decay_t<Ts>>...>;
};

template<class T, class... Ts>
inline auto constexpr are_same_types_v = are_same_types<T, Ts...>::value_type;

//template<class T, typename std::enable_if_t<std::is_integral_v<std::decay_t<T>>>...>
constexpr std::uint16_t operator"" _ui16(unsigned long long value)
{
    return static_cast<std::uint16_t>(value);
}

template<std::size_t i = 0, typename T, typename V>
constexpr void set_tuple(T &&tuple, V value)
{
    std::get<i>(tuple) = value;

    if constexpr (i + 1 < std::tuple_size_v<std::decay_t<T>>)
        set_tuple<i + 1>(std::forward<T>(tuple), value);
}


template<class... Ts>
constexpr std::array<std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>, sizeof...(Ts)> make_array(Ts &&...t)
{
    return {{std::forward<Ts>(t)...}};
}

namespace detail {
template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(T(&a)[N], std::index_sequence<I...>)
{
    return {{a[I]...}};
}
}

template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T(&a)[N])
{
    return detail::to_array_impl(a, std::make_index_sequence<N>{});
}

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

};

#endif // CRUS_TYPES_H