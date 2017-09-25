#pragma once

#ifndef CRUS_CUBE_H                 // Include guard "cube.h"
#define CRUS_CUBE_H

#include <array>
#include "engine.h"

namespace cube {
// 8 Verticies
// 6 Normals
// 14 Texture Coordinates
// 12 Triangles

constexpr std::array<isle::Position, 8> positions = {{
    {-0.5f, -0.5f, +0.5f},
    {+0.5f, -0.5f, +0.5f},
    {-0.5f, +0.5f, +0.5f},
    {+0.5f, +0.5f, +0.5f},
    {-0.5f, +0.5f, -0.5f},
    {+0.5f, +0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    {+0.5f, -0.5f, -0.5f}
}};

constexpr std::array<isle::math::Vector, 6> normals = {{
    {+0.0f, +0.0f, +1.0f},
    {+0.0f, +1.0f, +0.0f},
    {+0.0f, +0.0f, -1.0f},
    {+0.0f, -1.0f, +0.0f},
    {+1.0f, +0.0f, +0.0f},
    {-1.0f, +0.0f, +0.0f}
}};

constexpr std::array<isle::UV, 14> uvs = {{
    {0.375f, 0.00f}, {0.625f, 0.25f},
    {0.375f, 0.25f}, {0.625f, 0.00f},
    {0.625f, 0.50f}, {0.375f, 0.50f},
    {0.625f, 0.75f}, {0.375f, 0.75f},
    {0.625f, 1.00f}, {0.375f, 1.00f},
    {0.875f, 0.25f}, {0.875f, 0.00f},
    {0.125f, 0.00f}, {0.125f, 0.25f}
}};

constexpr std::array<std::array<std::size_t, 9>, 12> faces = {{
    {{0,3,2, 0,0,0, 0,1,2}},   {{0,1,3, 0,0,0, 0,3,1}},   {{2,5,4, 1,1,1, 2,4,5}},
    {{2,3,5, 1,1,1, 2,1,4}},   {{4,7,6, 2,2,2, 5,6,7}},   {{4,5,7, 2,2,2, 5,4,6}},
    {{6,1,0, 3,3,3, 7,8,9}},   {{6,7,1, 3,3,3, 7,6,8}},   {{1,5,3, 4,4,4, 3,10,1}},
    {{1,7,5, 4,4,4, 3,11,10}}, {{6,2,4, 5,5,5, 12,2,13}}, {{6,0,2, 5,5,5, 12,0,2}}
}};
};

#endif // CRUS_CUBE_H