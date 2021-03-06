#pragma once

#include <iterator>
#include <type_traits>
#include <variant>
#include <vector>
#include <array>
#include <tuple>

// using i8  = std::int8_t;
// using i16 = std::int16_t;
// using i32 = std::int32_t;
// using i64 = std::int64_t;

// using u8  = std::uint8_t;
// using u16 = std::uint16_t;
// using u32 = std::uint32_t;
// using u64 = std::uint64_t;


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
        using type = std::disjunction<std::is_same<T, Ts>...>;
        static auto constexpr value = type::value;
    };

    template<class T, class... Ts>
    auto constexpr is_one_of_v = is_one_of<T, Ts...>::value;

    //template<class T, typename std::enable_if_t<std::is_integral_v<std::decay_t<T>>>...>
    constexpr std::uint16_t operator"" _ui16(unsigned long long value)
    {
        return static_cast<std::uint16_t>(value);
    }

    constexpr std::uint32_t operator"" _ui32(unsigned long long value)
    {
        return static_cast<std::uint32_t>(value);
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
        if constexpr (std::is_void_v<T>)
            return {{ std::forward<Ts>(t)... }};

        else return {{ static_cast<T>(t)... }};
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

    template<class V>
    struct wrap_variant_by_vector;

    template<class... Ts>
    struct wrap_variant_by_vector<std::variant<Ts...>> {
        using type = std::variant<std::vector<Ts>...>;
    };

    template<class S, class T>
    struct is_variant_has_type;

    template<class S, class... Ts>
    struct is_variant_has_type<S, std::variant<Ts...>> {
        static auto constexpr value = isle::is_one_of_v<S, Ts...>;
    };
}
