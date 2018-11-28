#pragma once




#include <glm/glm.hpp>


#include "System/CrusIsland.h"
#include "System/CrusTypes.h"
#include "System/CrusBook.h"

namespace isle::glTF
{
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;


    Vertex() = default;

    template<class P, class N, class UV, typename std::enable_if_t<are_same_v<glm::vec3, std::decay_t<P>, std::decay_t<N>> && std::is_same_v<glm::vec2, std::decay_t<UV>>>...>
    constexpr Vertex(P &&_position, N &&_normal, UV &&_uv)
    {
        pos = std::forward<P>(_position);
        normal = std::forward<N>(_normal);
        uv = std::forward<UV>(_uv);
    }

    template<class P, class N, class UV,
        typename std::enable_if_t<are_same_v<std::array<float, 3>, std::decay_t<P>, std::decay_t<N>> && std::is_same_v<std::array<float, 2>, std::decay_t<UV>>>...>
        constexpr Vertex(P &&_position, N &&_normal, UV &&_uv)
    {
        pos = vec3{std::forward<P>(_position)};
        normal = vec3{std::forward<N>(_normal)};
        uv = vec2{std::forward<UV>(_uv)};
    }

    template<class T, typename std::enable_if_t<std::is_same_v<Vertex, std::decay_t<T>>>...>
    constexpr bool operator== (T &&rhs) const
    {
        return pos == rhs.pos && normal == rhs.normal && uv == rhs.uv;
    }
};

//std::ostream &operator<< (std::ostream &_stream, Vertex const &v)
//{
//    return _stream << v.pos << " " << v.normal << " " << v.uv;
//}

bool load(std::string_view name, std::vector<Vertex> &vertices, std::vector<std::uint32_t> &indices);
}