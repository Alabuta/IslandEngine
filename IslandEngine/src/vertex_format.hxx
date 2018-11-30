#pragma once

#include <variant>
#include <tuple>

#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_GTX_quaternion
#define GLM_GTX_transform
#include <glm/glm.hpp>

#include "System/CrusIsland.h"
#include "System/CrusTypes.h"

namespace isle
{
namespace semantic
{
    enum class eSEMANTIC_INDEX : std::size_t {
        nPOSITION = 0,
        nNORMAL,
        nTEXCOORD_0,
        nTEXCOORD_1,
        nTANGENT,
        nCOLOR_0,
        nJOINTS_0,
        nWEIGHTS_0
    };

    template<eSEMANTIC_INDEX SI>
    struct attribute {
        template<eSEMANTIC_INDEX si>
        auto constexpr operator< (attribute<si>) const noexcept
        {
            return SI < si;
        }
    };

    struct position : attribute<eSEMANTIC_INDEX::nPOSITION> { };
    struct normal : attribute<eSEMANTIC_INDEX::nNORMAL> { };
    struct tex_coord_0 : attribute<eSEMANTIC_INDEX::nTEXCOORD_0> { };
    struct tex_coord_1 : attribute<eSEMANTIC_INDEX::nTEXCOORD_1> { };
    struct tangent : attribute<eSEMANTIC_INDEX::nTANGENT> { };
    struct color_0 : attribute<eSEMANTIC_INDEX::nCOLOR_0> { };
    struct joints_0 : attribute<eSEMANTIC_INDEX::nJOINTS_0> { };
    struct weights_0 : attribute<eSEMANTIC_INDEX::nWEIGHTS_0> { };
}

using semantics_t = std::variant<
    semantic::position,
    semantic::normal,
    semantic::tex_coord_0,
    semantic::tex_coord_1,
    semantic::tangent,
    semantic::color_0,
    semantic::joints_0,
    semantic::weights_0
>;



using vertex_format_t = std::variant<
    std::pair<
        std::tuple<semantic::position>,
        std::tuple<glm::vec<3, std::float_t>>
    >,
    std::pair<
        std::tuple<semantic::position, semantic::normal>,
        std::tuple<glm::vec<3, std::float_t>, glm::vec<3, std::float_t>>
    >,
    std::pair<
        std::tuple<semantic::position, semantic::tex_coord_0>,
        std::tuple<glm::vec<3, std::float_t>, glm::vec<2, std::float_t>>
    >,
    std::pair<
        std::tuple<semantic::position, semantic::normal, semantic::tex_coord_0>,
        std::tuple<glm::vec<3, std::float_t>, glm::vec<3, std::float_t>, glm::vec<2, std::float_t>>
    >,
    std::pair<
        std::tuple<semantic::position, semantic::normal, semantic::tangent>,
        std::tuple<glm::vec<3, std::float_t>, glm::vec<3, std::float_t>, glm::vec<4, std::float_t>>
    >,
    std::pair<
        std::tuple<semantic::position, semantic::normal, semantic::tex_coord_0, semantic::tangent>,
        std::tuple<glm::vec<3, std::float_t>, glm::vec<3, std::float_t>, glm::vec<2, std::float_t>, glm::vec<4, std::float_t>>
    >
>;


template<class V>
struct to_vertex_format_buffer;

template<class... Ts>
struct to_vertex_format_buffer<std::variant<Ts...>> {
    using type = std::variant<
        std::pair<
            std::tuple_element_t<0, Ts>,
            std::vector<std::tuple_element_t<1, Ts>>
        >...
    >;
};

using vertex_buffer_t = to_vertex_format_buffer<vertex_format_t>::type;


template<class T, class V>
struct is_vertex_format;

template<class T, class... Ts>
struct is_vertex_format<T, std::variant<Ts...>> {
    static auto constexpr value = is_one_of_v<T, Ts...>;
};

template<class T>
constexpr bool is_vertex_format_v = is_vertex_format<T, vertex_format_t>::value;

using indices_t = std::variant<
    glm::vec<1, std::int8_t>,
    glm::vec<1, std::uint8_t>,
    glm::vec<1, std::int16_t>,
    glm::vec<1, std::uint16_t>,
    glm::vec<1, std::int32_t>,
    glm::vec<1, std::uint32_t>
>;

using index_buffer_t = wrap_variant_by_vector<indices_t>::type;
}
