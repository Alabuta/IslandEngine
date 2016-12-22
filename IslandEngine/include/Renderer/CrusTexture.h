/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 11th April 2010.
****	Description: texture routines header file.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_TEXTURE_H              // Include guard "CrusTexture.h"
#define CRUS_TEXTURE_H

namespace isle {
class Texture {
public:

    Texture(std::string &&_name);

    bool Init();

    void Bind() const;

    auto id() const { return id_; }

    auto w() const { return w_; }
    auto h() const { return h_; }

    auto name() const { return name_; }

private:
    uint32 id_{0};
    uint16 w_{0}, h_{0};

    std::string name_{""};

    Texture() = default;
};
};

#endif // CRUS_TEXTURE_H