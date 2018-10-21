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
struct are_same {
    static auto constexpr value_type = std::conjunction_v<std::is_same<T, std::decay_t<Ts>>...>;
};

template<class T, class... Ts>
inline auto constexpr are_same_v = are_same<T, Ts...>::value_type;

template<class T, class... Ts>
struct is_one_of {
    static auto constexpr value = std::disjunction_v<std::is_same<T, Ts>...>;
};

template<class T, class... Ts>
auto constexpr is_one_of_v = is_one_of<T, Ts...>::value;

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

namespace detail
{
template<class T, class... Ts> struct return_type_helper { using type = T; };
template<class... Ts> struct return_type_helper<void, Ts...> : std::common_type<Ts...> { };

template<class T, class... Ts>
using return_type_helper_t = typename return_type_helper<T, Ts...>::type;
}

template<class T = void, class... Ts>
constexpr std::array<detail::return_type_helper_t<T, Ts...>, sizeof...(Ts)> make_array(Ts &&...t)
{
    return {{ std::forward<Ts>(t)... }};
}

namespace detail
{
template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(T(&a)[N], std::index_sequence<I...>)
{
    return {{ a[I]... }};
}
}

template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T(&a)[N])
{
    return detail::to_array_impl(a, std::make_index_sequence<N>{});
}

template<class T, std::size_t N = 1>
struct instances_number {
    using type = T;
    static auto constexpr number = N;
};

template<class T, class Tuple, std::size_t I = 0>
constexpr std::size_t get_type_instances_number()
{
    using E = std::tuple_element_t<I, Tuple>;

    if constexpr (std::is_same_v<T, typename E::type>)
        return E::number;

    else if constexpr (I + 1 < std::tuple_size_v<Tuple>)
        return get_type_instances_number<T, Tuple, I + 1>();

    else return 0;
}

#if _CRUS_TEMP_DISABLED
template<class V>
struct wrap_variant_by_vector;

template<class... Ts>
struct wrap_variant_by_vector<std::variant<Ts...>> {
    using type = std::variant<std::vector<Ts>...>;
};
#endif

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