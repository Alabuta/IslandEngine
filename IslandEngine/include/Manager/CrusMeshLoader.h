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

#include "System\CrusSystem.h"

#include "Math\CrusVector.h"
#include "Renderer\CrusPosition.h"
#include "Renderer\CrusUV.h"

namespace isle {

bool LoadOBJ(std::string const &path, std::vector<Position> &positions, std::vector<math::Vector> &normals, std::vector<UV> &uvs, std::vector<std::vector<std::size_t>> &faces);


template<typename T>
bool SaveBinaryModel(std::string const &path, std::vector<T> const &vertex_buffer)
{
    if (path.empty()) {
        log::Error() << "file name is invalid.";
        return false;
    }

    std::ofstream file(path + ".bin", std::ios::out | std::ios::trunc | std::ios::binary);

    if (!file.is_open()) {
        log::Error() << "can't open file: " << path;
        return false;
    }

    auto count = static_cast<std::size_t>(vertex_buffer.size());
    file.write(reinterpret_cast<char const *>(&count), sizeof(count));

    file.write(reinterpret_cast<char const *>(vertex_buffer.data()), vertex_buffer.size() * sizeof(std::decay_t<T>));

    return true;
}

template<typename T>
bool LoadBinaryModel(std::string const &path, std::vector<T> &vertex_buffer)
{
    if (path.empty()) {
        log::Error() << "file name is invalid.";
        return false;
    }

    std::ifstream file(path + ".bin", std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        log::Error() << "can't open file: " << path;
        return false;
    }

    std::size_t count = 18348;
    file.read(reinterpret_cast<char *>(&count), sizeof(count));

    if (count < 1 || count % 3 != 0)
        return false;

    vertex_buffer.resize(count);
    file.read(reinterpret_cast<char *>(vertex_buffer.data()), vertex_buffer.size() * sizeof(T));

    return true;
}

template<typename T>
bool LoadModel(std::string const &name, uint32 &count, std::vector<T> &vertex_buffer)
{
    std::vector<Position> positions;
    std::vector<math::Vector> normals;
    std::vector<UV> uvs;

    std::vector<std::vector<std::size_t>> faces;

    auto path = "../contents/meshes/" + name;

    if (!LoadBinaryModel(path, vertex_buffer)) {
        if (LoadOBJ(path, positions, normals, uvs, faces)) {
            for (auto &face : faces)
                std::transform(face.begin(), face.end(), face.begin(), [] (auto &&a) { return a - 1; });

            for (auto const &face : faces) {
                for (auto it_index = face.cbegin(); it_index < face.cend(); std::advance(it_index, 2))
                    vertex_buffer.emplace_back(T{
                    positions.at(*it_index),
                    normals.at(*std::next(it_index, 2)),
                    uvs.at(*++it_index)
                                               });
            }

            SaveBinaryModel(path, vertex_buffer);
        }

        else return false;
    }

    count = static_cast<std::decay_t<decltype(count)>>(vertex_buffer.size() / 3);

    return true;
}

};

#endif // CRUS_MESH_LOADER_H