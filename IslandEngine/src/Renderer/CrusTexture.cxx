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


using namespace std::string_literals;
using namespace std::string_view_literals;


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
        if (!LoadTARGA(&image, path_ + ".tga"s))
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
        std::array<std::tuple<i32, std::string, Image>, 6> tuples{{
            { 0, "_px"s, {} },
            { 1, "_nx"s, {} },
            { 2, "_py"s, {} },
            { 3, "_ny"s, {} },
            { 4, "_pz"s, {} },
            { 5, "_nz"s, {} }
        }};

        std::vector<std::future<bool>> futures;

        for (auto &[index, name, img] : tuples) {
            UNREFERENCED_PARAMETER(index);
            futures.emplace_back(std::async(/*std::launch::async, */LoadTARGA, &img, path_ + name + ".tga"s));
        }

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

        for (auto &[index, name, image] : tuples) {
            UNREFERENCED_PARAMETER(name);
            glTextureSubImage3D(id_, 0, 0, 0, index, image.width_, image.height_, 1, image.format_, image.type_, image.data_.data());
        }

        glGenerateTextureMipmap(id_);
    }

    w_ = static_cast<u16>(image.width_);
    h_ = static_cast<u16>(image.height_);

    handle_ = glGetTextureHandleARB(id_);
    glMakeTextureHandleResidentARB(handle_);

    return true;
}

void Texture::Bind() const
{
    glBindTextureUnit(unit_, id_);
}


inline std::ostream &operator<< (std::ostream &_stream, Texture const &_texture)
{
    return _stream << _texture.name_;
}
};