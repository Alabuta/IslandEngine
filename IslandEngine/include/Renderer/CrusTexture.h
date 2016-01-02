/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 11th April 2010.
****	Description: texture routines header file.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_TEXTURE_H              // Include guard "CrusTexture.h"
#define CRUS_TEXTURE_H

namespace isle
{
class Texture {
public:
    Texture();
    ~Texture();

    bool Init(std::string const &name);

    void Bind() const;

    uint32 id() const {return id_;}

    uint16 w() const {return w_;}
    uint16 h() const {return h_;}

private:
    uint32 id_{0};
    uint16 w_{0}, h_{0};
};
};

#endif // CRUS_TEXTURE_H