/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: texture routines implementation file.
****
********************************************************************************************************************************/
#include "System\CrusSystem.h"
#include "Renderer\CrusRender.h"

#include "Renderer\CrusTexture.h"
#include "Manager\CrusTARGA.h"

//#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
//#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

namespace isle {
Texture::Texture(std::string &&_path)
{
    path_ = _path;

    name_ = _path.substr(0, _path.find_last_of('.'));
}

bool Texture::Init()
{
    if (!LoadTARGA(&image, path_))
        return false;

    isle::Render::inst().CreateTBO(GL_TEXTURE_2D, id_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, image.bpp_, image.width_, image.height_, 0, image.type_, image.format_, image.data_.data());
    //glGenerateMipmap(GL_TEXTURE_2D);

    /*auto maxAnisotrophy{0.0f};
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotrophy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotrophy);*/

    w_ = static_cast<uint16>(image.width_);
    h_ = static_cast<uint16>(image.height_);

    return true;
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::ToStream(std::ostream &_stream) const
{
    _stream << name_;
}
};