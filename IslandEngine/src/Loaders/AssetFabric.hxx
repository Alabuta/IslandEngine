#pragma once

#include <future>
#include <optional>

#include <string>
#include <string_view>

using namespace std::string_literals;
using namespace std::string_view_literals;



#include "System/CrusIsland.h"
#include "System/CrusTypes.h"
#include "System/CrusBook.h"

#include "vertex_format.hxx"
#include "mesh.hxx"


namespace isle
{
struct Asset final {
    struct Submesh {
        PRIMITIVE_TOPOLOGY topology;

        vertices_t  vertices;
        std::optional<indices_t_> indices;
    };

    struct Mesh {
        std::vector<Submesh> submeshes;
    };

    std::vector<Mesh> meshes;
};


class AssetFabric final {
public:

    struct buffer_t {
        std::vector<std::byte> vertex;
        std::vector<std::byte> index;
    } buffer;

    std::future<std::optional<Asset>> load(std::string_view name)
    {
        return std::async(std::launch::async, &AssetFabric::load_gltf, *this, name);
    }

private:

    std::optional<Asset> load_gltf(std::string_view name);
};
}
