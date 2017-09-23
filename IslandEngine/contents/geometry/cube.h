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

using isle::Position;

constexpr std::array<Position, 8> positions = {{
    {-0.5f, -0.5f, +0.5f},
    {+0.5f, -0.5f, +0.5f},
    {-0.5f, +0.5f, +0.5f},
    {+0.5f, +0.5f, +0.5f},
    {-0.5f, +0.5f, -0.5f},
    {+0.5f, +0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    {+0.5f, -0.5f, -0.5f}
}};

using isle::math::Vector;

constexpr std::array<Vector, 4 * 6> normals = {{
    {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}
}};

/*constexpr std::array<std::uint8_t, 6 * 2 * 3> indices = {
    0, 1, 2
};*/
};

#endif // CRUS_CUBE_H