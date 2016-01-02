/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 11th April 2010.
****	Description: TARGA loader implementation file.
****
********************************************************************************************************************************/
#include <fstream>

#include "System\CrusSystem.h"
#include "Renderer\CrusRenderer.h"

#include "Manager\CrusTARGA.h"

namespace isle
{
bool LoadUncompressedTARGA(Image &_image, std::ifstream &_file)
{
    char header[6]{0};

    _file.read(header, sizeof(header));

    _image.w_     = header[1] * 256 + header[0];
    _image.h_     = header[3] * 256 + header[2];
    _image.bpp_   = header[4] / 8;

    if(_image.w_ * _image.h_ < 0){
        _file.close();
        return false;
    }

    uint32 const count = _image.bpp_ * _image.w_ * _image.h_;

    switch (_image.bpp_) {
        /*case 1:
            _texture.bpp = GL_RED;
            break;

        case 2:
            _texture.bpp = GL_RG;
            break;*/

        case 3:
            _image.bpp_ = GL_RGB;
            break;

        case 4:
            _image.bpp_ = GL_RGBA;
            break;

        default:
            _file.close();
            return false;
    }

    switch (_image.bpp_) {
        case GL_RGB:
            _image.type_ = GL_BGR;
            break;

        case GL_RGBA:
            _image.type_ = GL_BGRA;
            break;

        default:
            _file.close();
            return false;
    }

    _image.data_.reserve(count);
    _file.read(reinterpret_cast<char *>(_image.data_.data()), count);
    //_image.data_.shrink_to_fit();

    _file.close();
    return true;
}

bool LoadTARGA(Image &_image, std::string const &_name)
{
    std::string path("../textures/" + _name);
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        Book::AddEvent(eNOTE::nERROR, "can't open texture file: \"%s\".", _name.data());
        return false;
    }

    file.seekg(sizeof(byte) * 2, std::ios::beg);

    char headerTARGA = 0;
    file.read(&headerTARGA, sizeof(headerTARGA));

    file.seekg(sizeof(byte) * 9, std::ios::cur);

    switch (headerTARGA) {
        case 2:
            LoadUncompressedTARGA(_image, file);
            break;

        case 10:
            break;

        default:
            break;
    }

    _image.format_ = GL_UNSIGNED_BYTE;

    file.close();
    return true;
}
};