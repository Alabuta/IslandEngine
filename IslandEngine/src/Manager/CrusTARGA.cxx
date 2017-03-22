/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: TARGA loader implementation file.
****
********************************************************************************************************************************/
#include <ostream>
#include <sstream>
#include <fstream>
#include <iterator>

#include "System\CrusSystem.h"
#include "Renderer\CrusRender.h"

#include "Renderer\CrusTexture.h"
#include "Manager\CrusTARGA.h"

namespace {
auto constexpr kTEXTURES_PATH = R"(../contents/textures/)";
};

namespace isle {
uint8 Image::BytesPerPixel() const
{
    switch (bpp_) {
        case GL_RED:
            return 1;

        case GL_RG:
            return 2;

        case GL_RGB:
        default:
            return 3;

        case GL_RGBA:
            return 4;
    }
}

bool LoadUncompressedTARGA(Image &_image, std::ifstream &_file)
{
    size_t const count = _image.bpp_ * _image.width_ * _image.height_;

    switch (_image.bpp_) {
        /*case 1:
            _texture.bpp = GL_RED;
            break;

        case 2:
            _texture.bpp = GL_RG;
            break;

        case 3:
            _image.bpp_ = GL_RGB;
            _image.type_ = GL_BGR;
            break;*/

        case 4:
            _image.bpp_ = GL_RGBA;
            _image.type_ = GL_BGRA;
            break;

        default:
            _file.close();
            return false;
    }

    _image.data_.resize(count);
    _file.read(reinterpret_cast<char *>(_image.data_.data()), count);
    //_image.data_.assign(std::istreambuf_iterator<char>(_file), std::istreambuf_iterator<char>());
    _image.data_.shrink_to_fit();

    _file.close();
    return true;
}

bool LoadCompressedTARGA(Image &_image, std::ifstream &_file)
{
    size_t const size = _image.bpp_ * _image.width_ * _image.height_;
    size_t const pixelsCount = _image.width_ * _image.height_;

    switch (_image.bpp_) {
        /*case 1:
        _texture.bpp = GL_RED;
        break;

        case 2:
        _texture.bpp = GL_RG;
        break;

        case 3:
            _image.bpp_ = GL_RGB;
            _image.type_ = GL_BGR;
            break;*/

        case 4:
            _image.bpp_ = GL_RGBA;
            _image.type_ = GL_BGRA;
            break;

        default:
            _file.close();
            return false;
    }

    _image.data_.reserve(pixelsCount);

    RGBA texel{0};
    byte chunkheader = 0;

    size_t currentPixel = 0;

    while (currentPixel < pixelsCount) {
        _file.read(reinterpret_cast<char *>(&chunkheader), sizeof(chunkheader));

        if (chunkheader < 128) {
            ++chunkheader;

            for (auto i = 0; i < chunkheader; ++i) {
                _file.read(reinterpret_cast<char *>(texel.channels), sizeof(texel));
                _image.data_.push_back(std::move(texel));
            }
        }

        else {
            chunkheader -= 127;

            _file.read(reinterpret_cast<char *>(texel.channels), sizeof(texel));
            _image.data_.insert(_image.data_.end(), chunkheader, texel);
        }

        currentPixel += chunkheader;
    }

    _image.data_.shrink_to_fit();

    _file.close();
    return true;
}

bool LoadTARGA(Image *const _image, std::string const &_name)
{
    std::string path(kTEXTURES_PATH + _name);
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        log::Error() << "can't open texture file: " << _name.data();
        return false;
    }

    file.seekg(sizeof(byte) * 2, std::ios::beg);

    char headerTARGA = 0;
    file.read(&headerTARGA, sizeof(headerTARGA));

    file.seekg(sizeof(byte) * 9, std::ios::cur);

    char header[6]{0};

    file.read(header, sizeof(header));

    _image->width_  = header[1] * 256 + header[0];
    _image->height_ = header[3] * 256 + header[2];
    _image->bpp_    = header[4] / 8;

    if (_image->width_ * _image->height_ * _image->bpp_ < 0) {
        file.close();
        return false;
    }

    switch (headerTARGA) {
        case 2:
            log::Debug() << "Uncompressed: " << measure<>::execution([&_image, &file]
            {
                LoadUncompressedTARGA(*_image, file);
            });
            break;

        case 10:
            log::Debug() << "Compressed: " << measure<>::execution([&_image, &file]
            {
                LoadCompressedTARGA(*_image, file);
            });
            break;

        default:
            break;
    }

    _image->format_ = GL_UNSIGNED_BYTE;

    file.close();
    return true;
}
};