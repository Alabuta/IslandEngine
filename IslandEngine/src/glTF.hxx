#pragma once






#include "System/CrusIsland.h"
#include "System/CrusTypes.h"
#include "System/CrusBook.h"

#include "vertex_format.hxx"
#include "mesh.hxx"

namespace isle::glTF
{

bool load(std::string_view name, vertex_buffer_t &vertices, index_buffer_t &indices);
}