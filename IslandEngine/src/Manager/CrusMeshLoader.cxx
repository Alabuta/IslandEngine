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


bool LoadOBJ(fs::path const &path, std::vector<Position> &positions, std::vector<math::Vector> &normals, std::vector<UV> &uvs, std::vector<std::vector<std::size_t>> &faces)
{
    using namespace std::string_literals;

    std::ifstream file(path, std::ios::in);

    if (file.bad() || file.fail()) {
        log::Error() << "can't open file: "s << path;
        return false;
    }

    std::vector<std::size_t> face;

    std::array<char, 256> line;
    std::string attribute;

    float x, y, z;
    std::size_t inx;

    while (file.getline(std::data(line), std::size(line))) {
        std::istringstream stream(std::data(line));

        stream >> attribute;

        if (attribute == "v"s) {
            stream >> x >> y >> z;
            positions.emplace_back(x, y, z);
        }

        else if (attribute == "vn"s) {
            stream >> x >> y >> z;
            normals.emplace_back(x, y, z);
        }

        else if (attribute == "vt"s) {
            stream >> x >> y;
            uvs.emplace_back(x, y);
        }

        else if (attribute == "f"s) {
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

            faces.emplace_back(face);
        }
    }

    file.close();

    return true;
}
};