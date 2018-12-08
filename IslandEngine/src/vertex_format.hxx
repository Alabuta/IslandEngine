#pragma once

#include <variant>
#include <tuple>

#include "System/CrusIsland.h"
#include "System/CrusTypes.h"

namespace isle
{
namespace semantic
{
    enum class eSEMANTIC_INDEX : std::uint32_t {
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
        static auto constexpr index{static_cast<std::uint32_t>(SI)};

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


template<std::uint32_t N, class T>
struct vec {
    using type = T;
    static auto constexpr number{N};

    std::array<T, N> array;
};

using attribute_t = std::variant<
    vec<1, std::int8_t>,
    vec<2, std::int8_t>,
    vec<3, std::int8_t>,
    vec<4, std::int8_t>,

    vec<1, std::uint8_t>,
    vec<2, std::uint8_t>,
    vec<3, std::uint8_t>,
    vec<4, std::uint8_t>,

    vec<1, std::int16_t>,
    vec<2, std::int16_t>,
    vec<3, std::int16_t>,
    vec<4, std::int16_t>,

    vec<1, std::uint16_t>,
    vec<2, std::uint16_t>,
    vec<3, std::uint16_t>,
    vec<4, std::uint16_t>,

    vec<1, std::int32_t>,
    vec<2, std::int32_t>,
    vec<3, std::int32_t>,
    vec<4, std::int32_t>,

    vec<1, std::uint32_t>,
    vec<2, std::uint32_t>,
    vec<3, std::uint32_t>,
    vec<4, std::uint32_t>,

    vec<1, std::float_t>,
    vec<2, std::float_t>,
    vec<3, std::float_t>,
    vec<4, std::float_t>
>;


struct attribute_description_t {
    std::size_t offset;
    semantics_t semantic;
    attribute_t attribute;
    bool normalized;

    attribute_description_t(std::size_t offset, semantics_t semantic, attribute_t &&attribute, bool normalized)
        : offset{ offset }, semantic{ semantic }, attribute{ attribute }, normalized{ normalized } { }
};

using vertex_layout_t = std::vector<attribute_description_t>;

struct vertex_buffer_t {

    vertex_layout_t layout;

    std::vector<std::byte> buffer;
};


using indices_t = std::variant<std::uint8_t, std::uint16_t, std::uint32_t>;
using index_buffer_t = wrap_variant_by_vector<indices_t>::type;

struct indices_t_ {
    std::size_t begin, end;

    std::variant<std::uint8_t, std::uint16_t, std::uint32_t> type;
};

struct vertices_t {
    std::size_t begin, end;

    vertex_layout_t layout;
};
}
