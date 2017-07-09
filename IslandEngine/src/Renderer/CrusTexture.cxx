/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: texture routines implementation file.
****
********************************************************************************************************************************/
#include <thread>
#include <future>
#include <mutex>
#include <vector>
#include <array>

#include "System\CrusSystem.h"
#include "Renderer\CrusRender.h"

#include "Renderer\CrusTexture.h"
#include "Manager\CrusTARGA.h"

#include <type_traits>



//#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
//#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

namespace isle {
Texture::Texture(eTEXTURE_TYPE _type, std::string &&_path) : type_(_type)
{
    path_ = _path;

    name_ = _path.substr(0, _path.find_last_of('.'));
}

bool Texture::Init()
{
    if (type_ == eTEXTURE_TYPE::n2D) {
        if (!LoadTARGA(&image, path_))
            return false;

        Render::inst().CreateTBO(GL_TEXTURE_2D, id_);

        glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR_MIPMAP_LINEAR
        glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureStorage2D(id_, 1, image.bpp_, image.width_, image.height_);
        glTextureSubImage2D(id_, 0, 0, 0, image.width_, image.height_, image.format_, image.type_, image.data_.data());

        glGenerateTextureMipmap(id_);

        /*auto maxAnisotrophy{0.0f};
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotrophy);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotrophy);*/
    }

    else if (type_ == eTEXTURE_TYPE::nCUBE) {
        std::array<std::tuple<int32, std::string, Image>, 6> tuples = {{
            {0, "_px", {}},
            {1, "_nx", {}},
            {2, "_py", {}},
            {3, "_ny", {}},
            {4, "_pz", {}},
            {5, "_nz", {}}
        }};

        std::vector<std::future<bool>> futures;

        /*for (auto &[index, name, image] : tuples)
            ;*/

        for (auto &tuple : tuples)
            futures.emplace_back(std::async(/*std::launch::async, */LoadTARGA, &std::get<2>(tuple), path_ + std::get<1>(tuple)));

        Render::inst().CreateTBO(GL_TEXTURE_CUBE_MAP, id_);

        glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(id_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        for (auto &future : futures)
            if (!future.get())
                return false;

        glTextureStorage2D(id_, 1, std::get<2>(tuples[0]).bpp_, std::get<2>(tuples[0]).width_, std::get<2>(tuples[0]).height_);
        
        for (auto const &tuple : tuples)
            glTextureSubImage3D(id_, 0, 0, 0, std::get<0>(tuple),
                                std::get<2>(tuple).width_, std::get<2>(tuple).height_, 1,
                                std::get<2>(tuple).format_, std::get<2>(tuple).type_, std::get<2>(tuple).data_.data());

        glGenerateTextureMipmap(id_);
    }

    w_ = static_cast<uint16>(image.width_);
    h_ = static_cast<uint16>(image.height_);

    return true;
}

void Texture::Bind() const
{
    glBindTextureUnit(unit_, id_);
}

void Texture::ToStream(std::ostream &_stream) const
{
    _stream << name_;
}
};