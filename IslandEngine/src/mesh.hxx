#pragma once

#include <vector>



#include "System/CrusIsland.h"
#include "System/CrusTypes.h"
#include "Renderer/CrusOpenGLExts.h"

#include "vertex_format.hxx"


namespace isle
{

enum class PRIMITIVE_TOPOLOGY {
    POINTS = 0,
    LINES, LINE_LOOP, LINE_STRIP,
    TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN
};

namespace staging
{
struct submesh_t {
    PRIMITIVE_TOPOLOGY topology;

    vertices_t  vertices;
    indices_t_ indices;
};

struct mesh_t {
    std::vector<submesh_t> submeshes;
};

struct scene_t {
    std::vector<mesh_t> meshes;

    std::vector<std::byte> buffer;
};
}
}