/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: a simple mesh loader header file.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_MESH_LOADER_H          // Include guard "CrusMeshLoader.h"
#define CRUS_MESH_LOADER_H

#include <string>
#include <vector>

#ifdef _MSC_VER
#include <filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif

#include "System\CrusSystem.h"

#include "Math\CrusVector.h"
#include "Renderer\CrusPosition.h"
#include "Renderer\CrusUV.h"

namespace isle {

bool LoadOBJ(fs::path const &path, std::vector<Position> &positions, std::vector<math::Vector> &normals, std::vector<UV> &uvs, std::vector<std::vector<std::size_t>> &faces);


template<typename T>
bool SaveBinaryModel(fs::path path, std::vector<T> const &vertex_buffer)
{
    using namespace std::string_literals;

    std::ofstream file(path += fs::path{".bin"s}, std::ios::out | std::ios::trunc | std::ios::binary);

    if (file.bad() || file.fail()) {
        log::Error() << "can't open file: "s << path;
        return false;
    }

    auto count = static_cast<std::size_t>(std::size(vertex_buffer));
    file.write(reinterpret_cast<char const *>(&count), sizeof(count));

    file.write(reinterpret_cast<char const *>(std::data(vertex_buffer)), std::size(vertex_buffer) * sizeof(std::decay_t<T>));

    return true;
}

template<typename T>
bool LoadBinaryModel(fs::path path, std::vector<T> &vertex_buffer)
{
    using namespace std::string_literals;

    std::ifstream file(path += fs::path{".bin"s}, std::ios::in | std::ios::binary);

    if (file.bad() || file.fail()) {
        log::Error() << "can't open file: "s << path;
        return false;
    }

    std::size_t count = 18348;
    file.read(reinterpret_cast<char *>(&count), sizeof(count));

    if (count < 1 || count % 3 != 0)
        return false;

    vertex_buffer.resize(count);
    file.read(reinterpret_cast<char *>(std::data(vertex_buffer)), std::size(vertex_buffer) * sizeof(std::decay_t<T>));

    return true;
}

template<typename T>
bool LoadModel(std::string_view _name, u32 &count, std::vector<T> &vertex_buffer)
{
    using namespace std::string_literals;

    std::vector<decltype(glTF::Vertex::pos)> positions;
    std::vector<decltype(glTF::Vertex::normal)> normals;
    std::vector<decltype(glTF::Vertex::uv)> uvs;

    std::vector<std::vector<std::size_t>> faces;

    auto current_path = fs::current_path();

    fs::path directory{"../contents/meshes/"s};
    fs::path name{std::data(_name)};

    auto const path = directory / name;

    if (!LoadBinaryModel(path, vertex_buffer)) {
        /*if (LoadOBJ(path, positions, normals, uvs, faces)) {
            for (auto &&face : faces) {
                std::transform(face.begin(), face.end(), face.begin(), [] (auto &&a) { return a - 1; });

                for (auto it_index = face.cbegin(); it_index < face.cend(); std::advance(it_index, 1)) {
                    auto position = positions.at(*it_index);
                    auto uv = uvs.at(*++it_index);
                    auto normal = normals.at(*++it_index);

                    vertex_buffer.emplace_back(std::move(position), std::move(normal), std::move(uv));
                }
            }

            SaveBinaryModel(path, vertex_buffer);
        }

        else*/ return false;
    }

    count = static_cast<std::decay_t<decltype(count)>>(std::size(vertex_buffer) / 3);

    return true;
}

};

#endif // CRUS_MESH_LOADER_H