#pragma once

#include <vector>



#include "System/CrusIsland.h"
#include "System/CrusTypes.h"
#include "Renderer\CrusOpenGLExts.h"

#include "vertex_format.hxx"


namespace isle
{
enum class PRIMITIVE_TYPE {
    NONE,
    POINTS = GL_POINTS,
    LINES, LINE_LOOP, LINE_STRIP,
    TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN
};

namespace staging
{
struct submesh {
    std::size_t begin, end;
    std::size_t material;
};

struct mesh {
    PRIMITIVE_TYPE primitive_type{PRIMITIVE_TYPE::NONE};

    std::size_t primitives_count{0};

    isle::vertex_buffer_t vertices;
    isle::index_buffer_t indices;

    std::vector<submesh> submeshes;
};

struct scene_t {
    std::vector<mesh> meshes;
};
}
}