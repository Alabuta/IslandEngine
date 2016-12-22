/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 11th April 2010.
****	Description: texture routines implementation file.
****
********************************************************************************************************************************/
#include "System\CrusSystem.h"
#include "Renderer\CrusRender.h"

#include "Renderer\CrusTexture.h"
#include "Manager\CrusTARGA.h"

namespace isle {
Texture::Texture(std::string &&_name) : name_(std::move(_name))
{}

bool Texture::Init()
{
    Image image;

    if (!LoadTARGA(image, name_))
        return false;

    isle::Render::inst().CreateTBO(GL_TEXTURE_2D, id_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR_MIPMAP_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, image.bpp_, image.width_, image.height_, 0, image.type_, image.format_, image.data_.data());
    //glGenerateMipmap(GL_TEXTURE_2D);

    //auto maxAnisotrophy{0.0f};
    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotrophy);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotrophy);

    if (glGetError() != GL_NO_ERROR) {
        Book::AddEvent(eNOTE::nERROR, "%s (%d)", __FUNCTION__, __LINE__);
        return false;
    }

    w_ = static_cast<uint16>(image.width_);
    h_ = static_cast<uint16>(image.height_);

    return true;
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, id_);
}
};