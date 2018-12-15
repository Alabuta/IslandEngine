#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <set>
#include <execution>

using namespace std::string_literals;
using namespace std::string_view_literals;

#ifdef _MSC_VER
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif

#include <chrono>

#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_SWIZZLE
#define GLM_GTX_polar_coordinates
#define GLM_GTX_quaternion
#define GLM_GTX_transform

//#pragma warning(push, 3)
//#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/polar_coordinates.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
//#pragma warning(pop)

#ifdef _MSC_VER
#pragma warning(push, 3)
#pragma warning(disable: 4127)
#include "nlohmann/json.hpp"
#pragma warning(pop)
#else
#include "nlohmann/json.hpp"
#endif

#include "Loaders/AssetFabric.hxx"
#include "Loaders/glTF.hxx"

#ifdef max
#undef max
#endif

namespace
{
enum class GL {
    BYTE = 0x1400, UNSIGNED_BYTE,
    SHORT, UNSIGNED_SHORT,
    INT, UNSIGNED_INT,
    FLOAT,

    ARRAY_BUFFER = 0x8892,
    ELEMENT_ARRAY_BUFFER
};


enum class PRIMITIVE_MODE {
    POINTS = 0,
    LINES, LINE_LOOP, LINE_STRIP,
    TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN
};
}


namespace
{
using accessor_t = std::pair<isle::semantics_t, std::size_t>;
using accessors_set_t = std::set<accessor_t>;

std::optional<isle::semantics_t> get_semantic(std::string_view name)
{
    using namespace isle;

    if (name == "POSITION"sv)
        return semantic::position{ };

    else if (name == "NORMAL"sv)
        return semantic::normal{ };

    else if (name == "TEXCOORD_0"sv)
        return semantic::tex_coord_0{ };

    else if (name == "TEXCOORD_1"sv)
        return semantic::tex_coord_1{ };

    else if (name == "TANGENT"sv)
        return semantic::tangent{ };

    else if (name == "COLOR_0"sv)
        return semantic::color_0{ };

    else if (name == "JOINTS_0"sv)
        return semantic::joints_0{ };

    else if (name == "WEIGHTS_0"sv)
        return semantic::weights_0{ };

    return { };
}
}


namespace isle::glTF
{
std::optional<PRIMITIVE_TOPOLOGY> constexpr get_primitive_topology(PRIMITIVE_MODE mode)
{
    switch (mode) {
        case PRIMITIVE_MODE::POINTS:
            return PRIMITIVE_TOPOLOGY::POINTS;

        case PRIMITIVE_MODE::LINES:
            return PRIMITIVE_TOPOLOGY::LINES;

        case PRIMITIVE_MODE::LINE_LOOP:
            return PRIMITIVE_TOPOLOGY::LINE_LOOP;

        case PRIMITIVE_MODE::LINE_STRIP:
            return PRIMITIVE_TOPOLOGY::LINE_STRIP;

        case PRIMITIVE_MODE::TRIANGLES:
            return PRIMITIVE_TOPOLOGY::TRIANGLES;

        case PRIMITIVE_MODE::TRIANGLE_STRIP:
            return PRIMITIVE_TOPOLOGY::TRIANGLE_STRIP;

        case PRIMITIVE_MODE::TRIANGLE_FAN:
            return PRIMITIVE_TOPOLOGY::TRIANGLE_FAN;

        default:
            return { };
    }
}

template<std::size_t N>
std::size_t constexpr attribute_size(GL componentType)
{
    switch (componentType) {
        case GL::BYTE:
            return N * sizeof(std::int8_t);

        case GL::UNSIGNED_BYTE:
            return N * sizeof(std::uint8_t);

        case GL::SHORT:
            return N * sizeof(std::int16_t);

        case GL::UNSIGNED_SHORT:
            return N * sizeof(std::uint16_t);

        case GL::INT:
            return N * sizeof(std::int32_t);

        case GL::UNSIGNED_INT:
            return N * sizeof(std::uint32_t);

        case GL::FLOAT:
            return N * sizeof(std::float_t);

        default:
            return 0;
    }
}

std::size_t attribute_size(std::string_view type, GL componentType)
{
    if (type == "SCALAR"sv)
        return attribute_size<1>(componentType);

    else if (type == "VEC2"sv)
        return attribute_size<2>(componentType);

    else if (type == "VEC3"sv)
        return attribute_size<3>(componentType);

    else if (type == "VEC4"sv)
        return attribute_size<4>(componentType);

    return 0;
}

template<std::size_t N>
std::optional<attribute_t> constexpr instantiate_attribute(GL componentType)
{
    switch (componentType) {
        case GL::BYTE:
            return vec<N, std::int8_t>{};

        case GL::UNSIGNED_BYTE:
            return vec<N, std::uint8_t>{};

        case GL::SHORT:
            return vec<N, std::int16_t>{};

        case GL::UNSIGNED_SHORT:
            return vec<N, std::uint16_t>{};

        case GL::INT:
            return vec<N, std::int32_t>{};

        case GL::UNSIGNED_INT:
            return vec<N, std::uint32_t>{};

        case GL::FLOAT:
            return vec<N, std::float_t>{};

        default:
            return { };
    }
}

std::optional<attribute_t> instantiate_attribute(std::string_view type, GL componentType)
{
    if (type == "SCALAR"sv)
        return glTF::instantiate_attribute<1>(componentType);

    else if (type == "VEC2"sv)
        return glTF::instantiate_attribute<2>(componentType);

    else if (type == "VEC3"sv)
        return glTF::instantiate_attribute<3>(componentType);

    else if (type == "VEC4"sv)
        return glTF::instantiate_attribute<4>(componentType);

    return { };
}

std::optional<index_buffer_t> instantiate_index_buffer(GL componentType)
{
    switch (componentType) {
        case GL::UNSIGNED_BYTE:
            return std::vector<std::uint8_t>{};

        case GL::UNSIGNED_SHORT:
            return std::vector<std::uint16_t>{};

        case GL::UNSIGNED_INT:
            return std::vector<std::uint32_t>{};

        default:
            return { };
    }
}

std::optional<indices_t> instantiate_index(GL componentType)
{
    switch (componentType) {
        case GL::UNSIGNED_BYTE:
            return std::uint8_t{};

        case GL::UNSIGNED_SHORT:
            return std::uint16_t{};

        case GL::UNSIGNED_INT:
            return std::uint32_t{};

        default:
            return { };
    }
}
}

namespace isle::glTF
{
struct scene_t {
    std::string name;
    std::vector<std::size_t> nodes;
};

struct node_t {
    std::string name;

    std::variant<glm::mat4, std::tuple<glm::vec3, glm::quat, glm::vec3>> transform;

    std::optional<std::size_t> mesh;
    std::optional<std::size_t> camera;

    std::vector<std::size_t> children;
};

struct buffer_t {
    std::size_t byteLength;
    std::string uri;
};

struct image_t {
    struct view_t {
        std::size_t bufferView;
        std::string mimeType;
    };

    std::variant<std::string, view_t> data;
};

struct mesh_t {
    struct primitive_t {
        std::optional<std::size_t> material;
        std::optional<std::size_t> indices;

        std::vector<accessor_t> attributes;

        accessors_set_t attribute_accessors;
#if NOT_YET_IMPLEMENTED
        saa_t attribute_accessors2;
#endif
        PRIMITIVE_MODE mode{PRIMITIVE_MODE::TRIANGLES};
    };

    std::vector<primitive_t> primitives;
};

struct material_t {
    struct pbr_t {
        struct texture_t {
            std::size_t index;
            std::size_t texCoord{0};
        };

        std::optional<texture_t> baseColorTexture;
        std::optional<texture_t> metallicRoughnessTexture;

        std::array<float, 4> baseColorFactor{{1.f, 1.f, 1.f, 1.f}};

        float metallicFactor, roughnessFactor;
    } pbr;

    struct normal_texture_t {
        std::size_t index;
        std::size_t texCoord{0};
        float scale;
    };

    std::optional<normal_texture_t> normalTexture;

    struct occlusion_texture_t {
        std::size_t index;
        std::size_t texCoord{0};
        float strength;
    };

    std::optional<occlusion_texture_t> occlusionTexture;

    struct emissive_texture_t {
        std::size_t index;
        std::size_t texCoord{0};
    };

    std::optional<emissive_texture_t> emissiveTexture;

    std::array<float, 3> emissiveFactor{{1.f, 1.f, 1.f}};

    std::string name;
    bool doubleSided{false};
};

struct camera_t {
    std::string type;

    struct perspective_t {
        float aspectRatio, yfov;
        float znear, zfar;
    };

    struct orthographic_t {
        float xmag, ymag;
        float znear, zfar;
    };

    std::variant<perspective_t, orthographic_t> instance;
};

struct texture_t {
    std::size_t source;
    std::size_t sampler;
};

struct sampler_t {
    std::uint32_t minFilter, magFilter;
    std::uint32_t wrapS, wrapT;
};

struct buffer_view_t {
    std::size_t buffer;
    std::size_t byteOffset{0};
    std::size_t byteLength;
    std::optional<std::size_t> byteStride;
    std::uint32_t target;
};

struct accessor_t {
    std::size_t bufferView{0};
    std::size_t byteOffset{0};
    std::size_t count;

    struct sparse_t {
        std::size_t count;
        std::size_t valuesBufferView;

        std::size_t indicesBufferView;
        std::uint32_t indicesComponentType;
    };

    std::optional<sparse_t> sparse;

    std::vector<float> min, max;

    GL componentType;

    std::string type;

    bool normalized{false};
};
}

namespace isle::glTF
{
void from_json(nlohmann::json const &j, buffer_t &buffer)
{
    buffer.byteLength = j.at("byteLength"s).get<decltype(buffer_t::byteLength)>();
    buffer.uri = j.at("uri"s).get<decltype(buffer_t::uri)>();
}

void from_json(nlohmann::json const &j, image_t &image)
{
    if (j.count("uri"s))
        image.data = j.at("uri"s).get<std::string>();

    else image.data = image_t::view_t{
        j.at("bufferView"s).get<std::size_t>(),
        j.at("mimeType"s).get<std::string>()
    };
}

void from_json(nlohmann::json const &j, scene_t &scene)
{
    if (j.count("name"s))
        scene.name = j.at("name"s).get<decltype(scene_t::name)>();

    else scene.name = ""s;

    scene.nodes = j.at("nodes"s).get<decltype(scene_t::nodes)>();
}

void from_json(nlohmann::json const &j, node_t &node)
{
    if (j.count("name"s))
        node.name = j.at("name"s).get<decltype(node_t::name)>();

    else node.name = ""s;

    if (j.count("matrix"s)) {
        std::array<float, 16> matrix;
        matrix = j.at("matrix"s).get<std::decay_t<decltype(matrix)>>();

        // :TODO:
        // If the determinant of the transform is a negative value,
        // the winding order of the mesh triangle faces should be reversed.
        // This supports negative scales for mirroring geometry.
        node.transform = glm::make_mat4(std::data(matrix));
    }

    else {
        std::array<float, 3> translation{{0.f, 0.f, 0.f}};
        std::array<float, 4> rotation{{0.f, 1.f, 0.f, 0.f}};
        std::array<float, 3> scale{{1.f, 1.f, 1.f}};

        if (j.count("translation"s))
            translation = j.at("translation"s).get<std::decay_t<decltype(translation)>>();

        if (j.count("rotation"s))
            rotation = j.at("rotation"s).get<std::decay_t<decltype(rotation)>>();

        if (j.count("scale"s))
            scale = j.at("scale"s).get<std::decay_t<decltype(scale)>>();

        node.transform = std::make_tuple(
            glm::make_vec3(std::data(translation)), glm::make_quat(std::data(rotation)), glm::make_vec3(std::data(scale))
        );
    }

    if (j.count("children"s))
        node.children = j.at("children"s).get<std::decay_t<decltype(node.children)>>();

    if (j.count("mesh"s))
        node.mesh = j.at("mesh"s).get<std::decay_t<decltype(node.mesh)::value_type>>();

    if (j.count("camera"s))
        node.camera = j.at("camera"s).get<std::decay_t<decltype(node.camera)::value_type>>();
}

void from_json(nlohmann::json const &j, mesh_t &mesh)
{
    auto const json = j.at("primitives"s);

    std::transform(std::cbegin(json), std::cend(json), std::back_inserter(mesh.primitives), [] (nlohmann::json const &json_primitive)
    {
        mesh_t::primitive_t primitive;

        if (json_primitive.count("material"s))
            primitive.material = json_primitive.at("material"s).get<decltype(mesh_t::primitive_t::material)::value_type>();

        if (json_primitive.count("indices"s))
            primitive.indices = json_primitive.at("indices"s).get<decltype(mesh_t::primitive_t::indices)::value_type>();

        auto const json_attributes = json_primitive.at("attributes"s);

        for (auto it = std::begin(json_attributes); it != std::end(json_attributes); ++it) {
            if (auto semantic = get_semantic(it.key()); semantic) {
                auto index = it->get<std::size_t>();

                primitive.attribute_accessors.emplace(semantic.value(), index);
            }

            if (auto semantic = get_semantic(it.key()); semantic) {
                auto index = it->get<std::size_t>();

                primitive.attributes.emplace_back(semantic.value(), index);
            }

            /*std::visit([&primitive, index = it->get<std::size_t>()] (auto semantic)
            {
                using s_t = decltype(semantic);

                if constexpr (!std::is_same_v<s_t, std::monostate>)
                    primitive.attribute_accessors2[s_t::I] = std::make_pair(std::make_pair(semantic, index), std::monostate{});

            }, get_semantic2(it.key()));*/
        }

        if (json_primitive.count("mode"s))
            primitive.mode = json_primitive.at("mode"s).get<decltype(mesh_t::primitive_t::mode)>();

        return primitive;
    });
}

void from_json(nlohmann::json const &j, material_t &material)
{
    auto &&json_pbrMetallicRoughness = j.at("pbrMetallicRoughness"s);

    if (json_pbrMetallicRoughness.count("baseColorTexture"s)) {
        auto const json_baseColorTexture = json_pbrMetallicRoughness.at("baseColorTexture"s);

        material.pbr.baseColorTexture = {
            json_baseColorTexture.at("index"s).get<decltype(material_t::pbr_t::texture_t::index)>(),
            json_baseColorTexture.count("texCoord"s) ? json_baseColorTexture.at("texCoord"s).get<decltype(material_t::pbr_t::texture_t::texCoord)>() : 0
        };
    }

    material.pbr.baseColorFactor = json_pbrMetallicRoughness.at("baseColorFactor"s).get<decltype(material_t::pbr_t::baseColorFactor)>();

    if (json_pbrMetallicRoughness.count("metallicRoughnessTexture"s)) {
        auto const json_metallicRoughnessTexture = json_pbrMetallicRoughness.at("metallicRoughnessTexture"s);

        material.pbr.metallicRoughnessTexture = {
            json_metallicRoughnessTexture.at("index"s).get<decltype(material_t::pbr_t::texture_t::index)>(),
            json_metallicRoughnessTexture.count("texCoord"s) ? json_metallicRoughnessTexture.at("texCoord"s).get<decltype(material_t::pbr_t::texture_t::texCoord)>() : 0
        };
    }

    material.pbr.metallicFactor = json_pbrMetallicRoughness.at("metallicFactor"s).get<decltype(material_t::pbr_t::metallicFactor)>();
    material.pbr.roughnessFactor = json_pbrMetallicRoughness.at("roughnessFactor"s).get<decltype(material_t::pbr_t::roughnessFactor)>();

    if (j.count("normalTexture"s)) {
        auto const json_normalTexture = j.at("normalTexture"s);

        material.normalTexture = {
            json_normalTexture.at("index"s).get<decltype(material_t::normal_texture_t::index)>(),
            json_normalTexture.count("texCoord"s) ? json_normalTexture.at("texCoord"s).get<decltype(material_t::normal_texture_t::texCoord)>() : 0,
            json_normalTexture.at("scale"s).get<decltype(material_t::normal_texture_t::scale)>()
        };
    }

    if (j.count("occlusionTexture"s)) {
        auto const json_occlusionTexture = j.at("occlusionTexture"s);

        material.occlusionTexture = {
            json_occlusionTexture.at("index"s).get<decltype(material_t::occlusion_texture_t::index)>(),
            json_occlusionTexture.count("texCoord"s) ? json_occlusionTexture.at("texCoord"s).get<decltype(material_t::occlusion_texture_t::texCoord)>() : 0,
            json_occlusionTexture.at("strength"s).get<decltype(material_t::occlusion_texture_t::strength)>()
        };
    }

    if (j.count("emissiveTexture"s)) {
        auto const json_emissiveTexture = j.at("emissiveTexture"s);

        material.emissiveTexture = {
            json_emissiveTexture.at("index"s).get<decltype(material_t::emissive_texture_t::index)>(),
            json_emissiveTexture.count("texCoord"s) ? json_emissiveTexture.at("texCoord"s).get<decltype(material_t::emissive_texture_t::texCoord)>() : 0
        };

        material.emissiveFactor = j.at("emissiveFactor"s).get<decltype(material_t::emissiveFactor)>();
    }

    material.name = j.at("name"s).get<decltype(material_t::name)>();
    material.doubleSided = j.at("doubleSided"s).get<decltype(material_t::doubleSided)>();
}

void from_json(nlohmann::json const &j, camera_t &camera)
{
    camera.type = j.at("type"s).get<decltype(camera_t::type)>();

    auto &&json_camera = j.at(camera.type);

    if (camera.type == "perspective"s) {
        camera_t::perspective_t instance;

        instance.aspectRatio = json_camera.get<decltype(camera_t::perspective_t::aspectRatio)>();
        instance.yfov = json_camera.get<decltype(camera_t::perspective_t::yfov)>();
        instance.znear = json_camera.get<decltype(camera_t::perspective_t::znear)>();

        if (json_camera.count("zfar"s))
            instance.zfar = json_camera.get<decltype(camera_t::perspective_t::zfar)>();

        else instance.zfar = std::numeric_limits<float>::infinity();

        camera.instance = instance;
    }

    else {
        camera.instance = camera_t::orthographic_t{
            json_camera.get<decltype(camera_t::orthographic_t::xmag)>(),
            json_camera.get<decltype(camera_t::orthographic_t::ymag)>(),
            json_camera.get<decltype(camera_t::orthographic_t::znear)>(),
            json_camera.get<decltype(camera_t::orthographic_t::zfar)>()
        };
    }
}

void from_json(nlohmann::json const &j, texture_t &texture)
{
    texture.source = j.at("source"s).get<decltype(texture_t::source)>();
    texture.sampler = j.at("sampler"s).get<decltype(texture_t::sampler)>();
}

void from_json(nlohmann::json const &j, sampler_t &sampler)
{
    sampler.minFilter = j.at("minFilter"s).get<decltype(sampler_t::minFilter)>();
    sampler.magFilter = j.at("magFilter"s).get<decltype(sampler_t::magFilter)>();

    sampler.wrapS = j.at("wrapS"s).get<decltype(sampler_t::wrapS)>();
    sampler.wrapT = j.at("wrapT"s).get<decltype(sampler_t::wrapT)>();
}

void from_json(nlohmann::json const &j, buffer_view_t &bufferView)
{
    bufferView.buffer = j.at("buffer"s).get<decltype(buffer_view_t::buffer)>();

    if (j.count("byteOffset"s))
        bufferView.byteOffset = j.at("byteOffset"s).get<decltype(buffer_view_t::byteOffset)>();

    else bufferView.byteOffset = 0;

    bufferView.byteLength = j.at("byteLength"s).get<decltype(buffer_view_t::byteLength)>();

    if (j.count("byteStride"s))
        bufferView.byteStride = j.at("byteStride"s).get<decltype(buffer_view_t::byteStride)::value_type>();

    if (j.count("target"s))
        bufferView.target = j.at("target"s).get<decltype(buffer_view_t::target)>();
}

void from_json(nlohmann::json const &j, accessor_t &accessor)
{
    accessor.bufferView = j.at("bufferView"s).get<decltype(accessor_t::bufferView)>();

    if (j.count("byteOffset"s))
        accessor.byteOffset = j.at("byteOffset"s).get<decltype(accessor_t::byteOffset)>();

    else accessor.byteOffset = 0;

    accessor.count = j.at("count"s).get<decltype(accessor_t::count)>();

    if (j.count("sparse"s)) {
        auto const json_sparse = j.at("sparse"s);

        accessor_t::sparse_t sparse;

        sparse.count = json_sparse.at("count"s).get<decltype(accessor_t::sparse_t::count)>();

        sparse.valuesBufferView = json_sparse.at("values"s).at("bufferView"s).get<decltype(accessor_t::sparse_t::valuesBufferView)>();

        sparse.indicesBufferView = json_sparse.at("indices"s).at("bufferView"s).get<decltype(accessor_t::sparse_t::indicesBufferView)>();
        sparse.indicesComponentType = json_sparse.at("indices"s).at("componentType"s).get<decltype(accessor_t::sparse_t::indicesComponentType)>();

        accessor.sparse = sparse;
    }

    if (j.count("min"s)) accessor.min = j.at("min"s).get<decltype(accessor_t::min)>();
    if (j.count("max"s)) accessor.max = j.at("max"s).get<decltype(accessor_t::max)>();

    accessor.type = j.at("type"s).get<decltype(accessor_t::type)>();
    accessor.componentType = j.at("componentType"s).get<decltype(accessor_t::componentType)>();
}
}

namespace isle
{
std::optional<Asset> AssetFabric::load_gltf(std::string_view name)
{
    fs::path contents{"contents/scenes"s};

    if (!fs::exists(fs::current_path() / contents))
        contents = fs::current_path() / fs::path{"../"s} / contents;

    auto folder = contents / name;

    nlohmann::json json;

    {
        auto path = folder / fs::path{"scene.gltf"s};

        std::ifstream file{path.native(), std::ios::in};

        if (file.bad() || file.fail()) {
            log::Error() << "failed to open file: "s << path;
            return { };
        }

        file >> json;
    }

#if TEMPORARILY_DISABLED
    auto scenes = json.at("scenes"s).get<std::vector<glTF::scene_t>>();
    auto nodes = json.at("nodes"s).get<std::vector<glTF::node_t>>();
   
    auto images = ([&json] {
        if (json.count("images"s))
            return json.at("images"s).get<std::vector<glTF::image_t>>();

        return std::vector<glTF::image_t>{};
    })();

    auto textures = ([&json] {
        if (json.count("textures"s))
            return json.at("textures"s).get<std::vector<glTF::texture_t>>();

        return std::vector<glTF::texture_t>{};
    })();

    auto samplers = ([&json] {
        if (json.count("samplers"s))
            return json.at("samplers"s).get<std::vector<glTF::sampler_t>>();

        return std::vector<glTF::sampler_t>{};
    })();

    auto materials = json.at("materials"s).get<std::vector<glTF::material_t>>();

    auto cameras = ([&json] {
        if (json.count("cameras"s))
            return json.at("cameras"s).get<std::vector<glTF::camera_t>>();

        return std::vector<glTF::camera_t>{};
    })();
#endif

    auto meshes = json.at("meshes"s).get<std::vector<glTF::mesh_t>>();

    auto buffers = json.at("buffers"s).get<std::vector<glTF::buffer_t>>();
    auto bufferViews = json.at("bufferViews"s).get<std::vector<glTF::buffer_view_t>>();
    auto accessors = json.at("accessors"s).get<std::vector<glTF::accessor_t>>();

    std::vector<std::vector<std::byte>> binBuffers;
    binBuffers.reserve(std::size(buffers));

    for (auto &&buffer : buffers) {
        auto path = folder / fs::path{buffer.uri};

        std::ifstream file{path.native(), std::ios::in | std::ios::binary};

        if (file.bad() || file.fail()) {
            log::Debug() << "failed to open file: "s << path;
            return { };
        }

        std::vector<std::byte> byteCode(buffer.byteLength);

        if (!byteCode.empty())
            file.read(reinterpret_cast<char *>(std::data(byteCode)), std::size(byteCode));

        binBuffers.push_back(std::move(byteCode));
    }

    Asset asset;

    std::size_t vertexBufferOffset = 0;
    std::size_t indexBufferOffset = 0;

    for (auto &&mesh : meshes) {
        Asset::Mesh xmesh;

        for (auto &&primitive : mesh.primitives) {
            Asset::Submesh submesh;

            if (auto mode = glTF::get_primitive_topology(primitive.mode); mode)
                submesh.topology = *mode;

            if (primitive.indices) {
                auto &&accessor = accessors.at(primitive.indices.value());

                if (auto instance = glTF::instantiate_index(accessor.componentType); instance) {
                    indices_t_ indices;

                    auto indexTypeSize = std::visit([&indices] (auto &&instance)
                    {
                        using T = std::decay_t<decltype(instance)>;

                        indices.type = std::move(instance);

                        return sizeof(T);

                    }, std::move(*instance));

                    auto &&bufferView = bufferViews.at(accessor.bufferView);
                    auto &&binBuffer = binBuffers.at(bufferView.buffer);

                    auto const length = accessor.count * indexTypeSize;

                    auto const begin = accessor.byteOffset + bufferView.byteOffset;
                    auto const end = begin + bufferView.byteLength;

                    buffer.index.resize(indexBufferOffset + length);

                    indices.begin = indexBufferOffset;
                    indices.count = accessor.count;

                    if (bufferView.byteStride) {
                        std::size_t srcIndex = begin, dstIndex = 0;

                        for (; srcIndex < end; srcIndex += *bufferView.byteStride, ++dstIndex)
                            memcpy(&buffer.index.at(dstIndex), &binBuffer.at(srcIndex), indexTypeSize);
                    }

                    else memcpy(std::data(buffer.index), &binBuffer.at(begin), end - begin);

                    indexBufferOffset += length;

                    submesh.indices.emplace(std::move(indices));
                }
            }

            std::size_t vertexSize = 0;
            std::size_t verticesNumber = 0;

            for (auto &&attribute : primitive.attribute_accessors) {
                auto [semantic, accessorIndex] = attribute;

                auto &&accessor = accessors.at(accessorIndex);

                if (auto instance = glTF::instantiate_attribute(accessor.type, accessor.componentType); instance) {
                    verticesNumber = std::max(verticesNumber, accessor.count);

                    auto offset = vertexBufferOffset + vertexSize;
                    auto normalized = accessor.normalized;

                    auto &&layout = submesh.vertices.layout;

                    auto attributeSize = std::visit([offset, semantic, normalized, &layout] (auto &&instance)
                    {
                        using T = std::decay_t<decltype(instance)>;

                        layout.emplace_back(offset, semantic, std::move(instance), normalized);

                        return sizeof(T);

                    }, std::move(*instance));

                    vertexSize += attributeSize;
                }
            }

            buffer.vertex.resize(vertexBufferOffset + verticesNumber * vertexSize);

            std::size_t attributeOffset = 0;

            for (auto &&attribute : primitive.attribute_accessors) {
                auto [semantic, accessorIndex] = attribute;

                auto &&accessor = accessors.at(accessorIndex);

                if (auto instance = glTF::instantiate_attribute(accessor.type, accessor.componentType); instance) {
                    auto attributeSize = std::visit([] (auto &&instance)
                    {
                        return sizeof(std::decay_t<decltype(instance)>);

                    }, std::move(*instance));

                    auto dstOffset = vertexBufferOffset + attributeOffset;

                    auto &&bufferView = bufferViews.at(accessor.bufferView);
                    auto &&binBuffer = binBuffers.at(bufferView.buffer);

                    auto const length = accessor.count * attributeSize;

                    auto const begin = accessor.byteOffset + bufferView.byteOffset;
                    auto const end = begin + bufferView.byteLength;

                    auto const step = bufferView.byteStride ? *bufferView.byteStride : attributeSize;

                    auto srcIndex = begin, dstIndex = dstOffset;

                    for (; srcIndex < end; srcIndex += step, dstIndex += vertexSize)
                        std::uninitialized_copy_n(&binBuffer.at(srcIndex), attributeSize,
                                                  reinterpret_cast<std::byte *>(&buffer.vertex.at(dstIndex)));

                    attributeOffset += attributeSize;

#if 1
                    std::vector<float> xxxxx((std::size(buffer.vertex) - 2 * 3) / 4);
                    std::uninitialized_copy(std::next(std::begin(buffer.vertex), 2 * 3), std::end(buffer.vertex), reinterpret_cast<std::byte *>(std::data(xxxxx)));
#else
                    std::vector<float> xxxxx((std::size(buffer)) / 4);
                    std::uninitialized_copy(std::next(std::begin(buffer), 0), std::end(buffer), reinterpret_cast<std::byte *>(std::data(xxxxx)));
#endif
                }
            }

            submesh.vertices.begin = vertexBufferOffset;
            submesh.vertices.count = verticesNumber;

            xmesh.submeshes.push_back(std::move(submesh));

            vertexBufferOffset += verticesNumber * vertexSize;
        }

        asset.meshes.push_back(std::move(xmesh));
    }

    return asset;
}
}


namespace isle::glTF
{
bool load(std::string_view name, vertex_buffer_t &vertices, index_buffer_t &indices)
{
    fs::path contents{"contents/scenes"s};

    if (!fs::exists(fs::current_path() / contents))
        contents = fs::current_path() / fs::path{"../"s} / contents;

    auto folder = contents / name;

    nlohmann::json json;

    {
        auto path = folder / fs::path{"scene.gltf"s};

        std::ifstream file{path.native(), std::ios::in};

        if (file.bad() || file.fail()) {
            log::Error() << "failed to open file: "s << path;
            return false;
        }

        file >> json;
    }

    auto scenes = json.at("scenes"s).get<std::vector<glTF::scene_t>>();
    auto nodes = json.at("nodes"s).get<std::vector<glTF::node_t>>();

    ;

    auto meshes = json.at("meshes"s).get<std::vector<glTF::mesh_t>>();

    auto buffers = json.at("buffers"s).get<std::vector<glTF::buffer_t>>();
    auto bufferViews = json.at("bufferViews"s).get<std::vector<glTF::buffer_view_t>>();
    auto accessors = json.at("accessors"s).get<std::vector<glTF::accessor_t>>();

#if TEMPORARILY_DISABLED
    auto images = ([&json] {
        if (json.count("images"s))
            return json.at("images"s).get<std::vector<glTF::image_t>>();

        return std::vector<glTF::image_t>{};
    })();

    auto textures = ([&json] {
        if (json.count("textures"s))
            return json.at("textures"s).get<std::vector<glTF::texture_t>>();

        return std::vector<glTF::texture_t>{};
    })();

    auto samplers = ([&json] {
        if (json.count("samplers"s))
            return json.at("samplers"s).get<std::vector<glTF::sampler_t>>();

        return std::vector<glTF::sampler_t>{};
    })();

    auto materials = json.at("materials"s).get<std::vector<glTF::material_t>>();

    auto cameras = ([&json] {
        if (json.count("cameras"s))
            return json.at("cameras"s).get<std::vector<glTF::camera_t>>();

        return std::vector<glTF::camera_t>{};
    })();
#endif

    std::vector<std::vector<std::byte>> binBuffers;
    binBuffers.reserve(std::size(buffers));

    for (auto &&buffer : buffers) {
        auto path = folder / fs::path{buffer.uri};

        std::ifstream file{path.native(), std::ios::in | std::ios::binary};

        if (file.bad() || file.fail()) {
            log::Debug() << "failed to open file: "s << path;
            return false;
        }

        std::vector<std::byte> byte_code(buffer.byteLength);

        if (!byte_code.empty())
            file.read(reinterpret_cast<char *>(std::data(byte_code)), std::size(byte_code));

        binBuffers.emplace_back(std::move(byte_code));
    }

#if OBSOLETE
    std::vector<attribute_buffer_t> attribute_buffers;
    attribute_buffers.reserve(std::size(accessors));

    for (auto &&accessor : accessors) {
        if (auto buffer = instantiate_attribute_buffer(accessor.type, accessor.componentType); buffer) {
            auto &&buffer_view = bufferViews.at(accessor.bufferView);
            auto &&bin_buffer = bin_buffers.at(buffer_view.buffer);

            std::visit([&accessor, &buffer_view, &bin_buffer, &attribute_buffers] (auto &&buffer) {
                auto const size = sizeof(typename std::decay_t<decltype(buffer)>::value_type);

                std::size_t const begin = accessor.byteOffset + buffer_view.byteOffset;
                std::size_t const end = begin + accessor.count * size;

                buffer.resize(accessor.count);

                if (buffer_view.byteStride != 0) {
                    std::size_t src_index = begin, dst_index = 0u;

                    for (; src_index < end; src_index += buffer_view.byteStride, ++dst_index)
                        memmove(&buffer.at(dst_index), &bin_buffer.at(src_index), size);
                }

                else memmove(std::data(buffer), &bin_buffer.at(begin), end - begin);

                attribute_buffers.emplace_back(std::move(buffer));

            }, std::move(buffer.value()));
        }
    }
#endif

    for (auto &&mesh : meshes) {
        for (auto &&primitive : mesh.primitives) {
            if (primitive.indices) {
                auto &&accessor = accessors.at(primitive.indices.value());

                if (auto index_buffer = instantiate_index_buffer(accessor.componentType); index_buffer) {
                    auto &&bufferView = bufferViews.at(accessor.bufferView);
                    auto &&binBuffer = binBuffers.at(bufferView.buffer);

                    indices = std::move(index_buffer.value());

                    std::size_t const begin = accessor.byteOffset + bufferView.byteOffset;

                    std::visit([begin, count = accessor.count, byteStride = bufferView.byteStride, &binBuffer] (auto &&indices)
                    {
                        using T = std::decay_t<decltype(indices)>::value_type;

                        auto size = sizeof(T);
                        std::size_t const end = begin + count * size;

                        indices.resize(count);

                        if (byteStride) {
                            std::size_t src_index = begin, dst_index = 0u;

                            for (; src_index < end; src_index += *byteStride, ++dst_index)
                                memcpy(&indices.at(dst_index), &binBuffer.at(src_index), size);
                        }

                        else memcpy(std::data(indices), &binBuffer.at(begin), end - begin);

                    }, indices);
                }
            }

            std::size_t vertexSize = 0;
            std::size_t verticesNumber = 0;
            std::size_t attributeOffset = 0;

            for (auto &&attribute : primitive.attribute_accessors) {
                auto [semantic, accessorIndex] = attribute;

                auto &&accessor = accessors.at(accessorIndex);

                auto attributeSize = glTF::attribute_size(accessor.type, accessor.componentType);

                if (attributeSize == 0) {
                    log::Warning() << "unsupported attribute type"s;
                    continue;
                }

                vertexSize += attributeSize;
                verticesNumber = std::max(verticesNumber, accessor.count);
            }

            auto &&buffer = vertices.buffer;
            buffer.resize(verticesNumber * vertexSize);

            std::size_t dstOffset = 0;

            for (auto &&attribute : primitive.attribute_accessors) {
                auto [semantic, accessor_index] = attribute;

                auto &&accessor = accessors.at(accessor_index);
                auto &&bufferView = bufferViews.at(accessor.bufferView);
                auto &&binBuffer = binBuffers.at(bufferView.buffer);

                auto normalized = accessor.normalized;

                if (auto attribute = glTF::instantiate_attribute(accessor.type, accessor.componentType); attribute) {
                    auto attributeSize = std::visit([dstOffset, semantic, normalized, &vertices] (auto &&attribute)
                    {
                        using A = std::decay_t<decltype(attribute)>;
                        auto size = sizeof(A);

                        vertices.layout.emplace_back(dstOffset, semantic, std::move(attribute), normalized);

                        return size;

                    }, std::move(attribute.value()));

                    std::size_t const begin = accessor.byteOffset + bufferView.byteOffset;
                    std::size_t const end = begin + accessor.count * attributeSize;
                    std::size_t const step = bufferView.byteStride ? *bufferView.byteStride : attributeSize;

                    auto srcIndex = begin, dstIndex = dstOffset;

                    auto start = std::chrono::system_clock::now();

                    for (; srcIndex < end; srcIndex += step, dstIndex += vertexSize)
                        std::uninitialized_copy_n(&binBuffer.at(srcIndex), attributeSize,
                                                  reinterpret_cast<std::byte *>(&buffer.at(dstIndex)));
                        //memcpy(&buffer.at(dstIndex), &binBuffer.at(srcIndex), attributeSize);

                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
                    log::Debug() << duration.count();

                    dstOffset += attributeSize;
                }
            }

            log::Debug() << vertexSize << ' ' << verticesNumber;

            /*std::vector<float> xxxxx(std::size(vertices.buffer) / 4);
            std::uninitialized_copy(std::begin(vertices.buffer), std::end(vertices.buffer), reinterpret_cast<std::byte *>(std::data(xxxxx)));*/
        }
    }

    return true;
}
;
}