/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: texture routines header file.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_TEXTURE_H              // Include guard "CrusTexture.h"
#define CRUS_TEXTURE_H

#include <vector>
#include "Game\CrusObject.h"

namespace isle {
struct alignas(sizeof(uint32)) RGBA
{
    union {
        byte channels[4];
        uint32 value;
    };
};

struct Image {
    int32 bpp_{0};
    int32 width_{0}, height_{0};
    uint32 format_{0}, type_{0};

    alignas(sizeof(__m128i)) std::vector<byte> data_;

    uint8 BytesPerPixel() const;
};

class Texture final : public CrusObject {
public:
#if _CRUS_NOT_YET_IMPLEMENTED
    enum class eTEXTURE_TYPE {
        nUNKNOWN = 0, nNONE, n2D, n3D, nCUBE, n2D_ARRAY, nCUBE_ARRAY
    } type{eTEXTURE_TYPE::nUNKNOWN};

    enum class eFILTERING_MODE {
        nPOINT = 0, nBILINEAR, nTRILINEAR
    } filteringMode{eFILTERING_MODE::nPOINT};

    enum class eWRAP_MODE {
        nCLAMP = 0, nREPEAT
    } wrapMode{eWRAP_MODE::nREPEAT};

    int8 anisotrophyLevel{0};
#endif

    Image image;

    Texture(std::string &&path);

    bool Init();

    void Bind() const;

    auto id() const { return id_; }

    auto w() const { return w_; }
    auto h() const { return h_; }

    void ToStream(std::ostream &stream) const;

private:
    uint32 id_{0};
    uint16 w_{0}, h_{0};

    std::string path_{""};

    Texture() = default;
};
};

#endif // CRUS_TEXTURE_H