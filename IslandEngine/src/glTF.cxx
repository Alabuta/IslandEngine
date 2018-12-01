#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <set>

using namespace std::string_literals;
using namespace std::string_view_literals;

#ifdef _MSC_VER
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif


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

#include "glTF.hxx"

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

using attribute_t = std::variant<
    glm::vec<1, std::int8_t>,
    glm::vec<2, std::int8_t>,
    glm::vec<3, std::int8_t>,
    glm::vec<4, std::int8_t>,

    glm::vec<1, std::uint8_t>,
    glm::vec<2, std::uint8_t>,
    glm::vec<3, std::uint8_t>,
    glm::vec<4, std::uint8_t>,

    glm::vec<1, std::int16_t>,
    glm::vec<2, std::int16_t>,
    glm::vec<3, std::int16_t>,
    glm::vec<4, std::int16_t>,

    glm::vec<1, std::uint16_t>,
    glm::vec<2, std::uint16_t>,
    glm::vec<3, std::uint16_t>,
    glm::vec<4, std::uint16_t>,

    glm::vec<1, std::int32_t>,
    glm::vec<2, std::int32_t>,
    glm::vec<3, std::int32_t>,
    glm::vec<4, std::int32_t>,

    glm::vec<1, std::uint32_t>,
    glm::vec<2, std::uint32_t>,
    glm::vec<3, std::uint32_t>,
    glm::vec<4, std::uint32_t>,

    glm::vec<1, std::float_t>,
    glm::vec<2, std::float_t>,
    glm::vec<3, std::float_t>,
    glm::vec<4, std::float_t>
>;

using attribute_buffer_t = isle::wrap_variant_by_vector<attribute_t>::type;
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

namespace
{
// semantic_attributes_types<S, VF>
// type = std::variant<std::vector<if semantic == S then std::alternative<I, VF>>...>
template<class... Ts>
using concat_tuples_types = decltype(std::tuple_cat(std::declval<Ts>()...));
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
        std::size_t indices;

        accessors_set_t attribute_accessors;

        std::uint32_t mode{4};
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
    std::size_t byteOffset;
    std::size_t byteLength;
    std::size_t byteStride;
    std::uint32_t target;
};

struct accessor_t {
    std::size_t bufferView;
    std::size_t byteOffset;
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

        primitive.indices = json_primitive.at("indices"s).get<decltype(mesh_t::primitive_t::indices)>();

        auto const json_attributes = json_primitive.at("attributes"s);

        for (auto it = std::begin(json_attributes); it != std::end(json_attributes); ++it) {
            if (auto semantic = get_semantic(it.key()); semantic) {
                auto index = it->get<std::size_t>();

                primitive.attribute_accessors.emplace(semantic.value(), index);
            }
        }

        if (json_primitive.count("mode"s))
            primitive.mode = json_primitive.at("mode"s).get<decltype(mesh_t::primitive_t::mode)>();

        else primitive.mode = 4;

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
        bufferView.byteStride = j.at("byteStride"s).get<decltype(buffer_view_t::byteStride)>();

    else bufferView.byteStride = 0;

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

namespace isle::glTF
{
template<std::size_t N>
std::optional<attribute_buffer_t> instantiate_attribute_buffer(GL componentType)
{
    switch (componentType) {
        case GL::BYTE:
            return std::vector<glm::vec<N, std::int8_t>>{};

        case GL::UNSIGNED_BYTE:
            return std::vector<glm::vec<N, std::uint8_t>>{};

        case GL::SHORT:
            return std::vector<glm::vec<N, std::int16_t>>{};

        case GL::UNSIGNED_SHORT:
            return std::vector<glm::vec<N, std::uint16_t>>{};

        case GL::INT:
            return std::vector<glm::vec<N, std::int32_t>>{};

        case GL::UNSIGNED_INT:
            return std::vector<glm::vec<N, std::uint32_t>>{};

        case GL::FLOAT:
            return std::vector<glm::vec<N, std::float_t>>{};

        default:
            return { };
    }
}

std::optional<attribute_buffer_t> instantiate_attribute_buffer(std::string_view type, GL componentType)
{
    if (type == "SCALAR"sv)
        return glTF::instantiate_attribute_buffer<1>(componentType);

    else if (type == "VEC2"sv)
        return glTF::instantiate_attribute_buffer<2>(componentType);

    else if (type == "VEC3"sv)
        return glTF::instantiate_attribute_buffer<3>(componentType);

    else if (type == "VEC4"sv)
        return glTF::instantiate_attribute_buffer<4>(componentType);

    return std::nullopt;
}

template<std::size_t N>
std::optional<attribute_t> try_to_get_attribute_type(GL componentType)
{
    switch (componentType) {
        case GL::BYTE:
            return glm::vec<N, std::int8_t>{};

        case GL::UNSIGNED_BYTE:
            return glm::vec<N, std::uint8_t>{};

        case GL::SHORT:
            return glm::vec<N, std::int16_t>{};

        case GL::UNSIGNED_SHORT:
            return glm::vec<N, std::uint16_t>{};

        case GL::INT:
            return glm::vec<N, std::int32_t>{};

        case GL::UNSIGNED_INT:
            return glm::vec<N, std::uint32_t>{};

        case GL::FLOAT:
            return glm::vec<N, std::float_t>{};

        default:
            return { };
    }
}

std::optional<attribute_t> try_to_get_attribute_type(std::string_view type, GL componentType)
{
    if (type == "SCALAR"sv)
        return glTF::try_to_get_attribute_type<1>(componentType);

    else if (type == "VEC2"sv)
        return glTF::try_to_get_attribute_type<2>(componentType);

    else if (type == "VEC3"sv)
        return glTF::try_to_get_attribute_type<3>(componentType);

    else if (type == "VEC4"sv)
        return glTF::try_to_get_attribute_type<4>(componentType);

    return std::nullopt;
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
    auto buffer_views = json.at("bufferViews"s).get<std::vector<glTF::buffer_view_t>>();
    auto accessors = json.at("accessors"s).get<std::vector<glTF::accessor_t>>();

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

    std::vector<std::vector<std::byte>> bin_buffers;
    bin_buffers.reserve(std::size(buffers));

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

        bin_buffers.emplace_back(std::move(byte_code));
    }

    std::vector<attribute_buffer_t> attribute_buffers;
    attribute_buffers.reserve(std::size(accessors));

    for (auto &&accessor : accessors) {
        if (auto buffer = instantiate_attribute_buffer(accessor.type, accessor.componentType); buffer) {
            auto &&buffer_view = buffer_views.at(accessor.bufferView);
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

    for (auto &&mesh : meshes) {
        for (auto &&primitive : mesh.primitives) {
            std::visit([&indices] (auto &&source)
            {
                using T = std::decay_t<decltype(source)>;

                if constexpr (is_variant_has_type<T, index_buffer_t>::value)
                    indices = std::forward<T>(source);

            }, attribute_buffers.at(primitive.indices));

            vertex_format_t vertex_format;

            for (auto &&attribute_accessor : primitive.attribute_accessors) {
                auto [semantic, accessor] = attribute_accessor;

                std::visit([&vertex_format, accessor, &attribute_buffers] (auto semantic)
                {
                    // using semantic_attribute_types_t = semantic_attributes_types<T, vertex_format_t>::type
                    // if constexpr (is_variant_has_type<T, semantic_attribute_types_t>::value)
                    //      vertices = 
                    //              std::pair<
                    //                  std::tuple<semantic>,
                    //                  std::tuple<glm::vec<3, std::float_t>>
                    //              >;

                    using S = std::decay_t<decltype(semantic)>;

                    std::visit([&vertex_format] (auto &&attribute)
                    {
                        using A = std::decay_t<decltype(attribute)>::value_type;
                        using P = std::pair<std::tuple<S>, std::tuple<A>>;

                        std::visit([] (auto &&vertex_format)
                        {
                            using VF = std::decay_t<decltype(vertex_format)>;

                            if constexpr (std::is_same_v<VF, std::false_type>)
                                vertex_format = std::false_type{};

                            else if constexpr (std::is_same_v<VF, P>)
                                vertex_format = P{};

                            else {
                                using S0 = std::tuple_element_t<0, VF>;
                                using A0 = std::tuple_element_t<1, VF>;

                                S0 x{};
                                A0 y{};
                                log::Debug() << sizeof(x) << sizeof(y);

                                std::tuple_element_t<0, S0> z{};
                                log::Debug() << sizeof(z);

                                using S1 = concat_tuples_types<S0, std::tuple_element_t<0, P>>;
                                using A1 = concat_tuples_types<A0, std::tuple_element_t<1, P>>;

                                S1 a{};

                                using P1 = std::pair<S1, A1>;

                                P1 p{};

                                if constexpr (is_vertex_format<P1, vertex_format_t>::value)
                                    vertex_format = P1{};

                                /*else vertex_format = std::false_type{};*/
                            }

                        }, vertex_format);

                    }, attribute_buffers.at(accessor));

                }, semantic);
            }

            std::tuple<
                std::pair<
                    std::tuple<semantic::position>,
                    std::vector<std::tuple<glm::vec<3, std::float_t>>>
                >
            > f;

            std::variant<decltype(f)> v = f;

            std::visit([&vertices] (auto &&source) {
                using T = std::decay_t<decltype(source)>;

                if constexpr (is_variant_has_type<T, vertex_buffer_t>::value)
                    vertices = std::forward<T>(source);

            }, v);
        }
    }

    std::vector<glm::vec3> v0(4, {1, 2, 3});
    std::variant<
        std::vector<std::tuple<glm::vec3>>,
        std::vector<std::tuple<glm::vec3, glm::vec2>>
    > v1;

    v1 = std::vector<std::tuple<glm::vec3>>(std::size(v0));

    std::visit([&v0] (auto &&dst)
    {
        using T = std::decay_t<decltype(v0)>::value_type;
        using T0 = std::tuple<T>;
        using T1 = std::decay_t<decltype(dst)>::value_type;

        if constexpr (std::is_same_v<T0, T1>)
            std::uninitialized_copy_n(std::begin(v0), std::size(v0), reinterpret_cast<T *>(std::data(dst)));
            //memmove(std::data(dst), std::data(v0), std::size(v0) * sizeof(T));

    }, v1);

    return false;
}
;
}