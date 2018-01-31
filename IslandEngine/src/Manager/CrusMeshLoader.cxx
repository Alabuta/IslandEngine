/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: a simple mesh loader header file.
****
********************************************************************************************************************************/
#include "Manager\CrusMeshLoader.h"

namespace isle {


bool LoadOBJ(std::string const &path, std::vector<Position> &positions, std::vector<math::Vector> &normals, std::vector<UV> &uvs, std::vector<std::vector<std::size_t>> &faces)
{
    if (path.empty()) {
        log::Error() << "file name is invalid.";
        return false;
    }

    std::ifstream file(path, std::ios::in);

    if (!file.is_open()) {
        log::Error() << "can't open file: " << path;
        return false;
    }

    std::vector<std::size_t> face;

    std::array<char, 256> line;
    std::string attribute;

    float x, y, z;
    std::size_t inx;

    while (file.getline(line.data(), line.size())) {
        std::istringstream stream(line.data());

        stream >> attribute;

        if (attribute == "v") {
            stream >> x >> y >> z;
            positions.emplace_back(x, y, z);
        }

        else if (attribute == "vn") {
            stream >> x >> y >> z;
            normals.emplace_back(x, y, z);
        }

        else if (attribute == "vt") {
            stream >> x >> y;
            uvs.emplace_back(x, y);
        }

        else if (attribute == "f") {
            face.clear();

            std::istringstream in(&line[1]);

            bool b = true;

            while (b) {
                in >> inx;

                if (in.eof() || in.bad())
                    b = false;

                else if (in.fail()) {
                    in.clear();
                    in.ignore(std::numeric_limits<std::streamsize>::max(), '/');
                    continue;
                }

                face.emplace_back(inx);
            }

            faces.push_back(face);
        }
    }

    file.close();

    return true;
}
};