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

extern "C"
{
#include <mmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
#include <wmmintrin.h>
#include <tmmintrin.h>
#include <immintrin.h>   // (Meta-header)
}

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

//std::istream &operator >> (std::istream &in, RGBA &per)
//{
//    in >> per.value;
//    return in;
//}

struct Image {
    alignas(sizeof(__m128i)) std::vector<RGBA> data_;

    int32 bpp_{0};
    int32 width_{0}, height_{0};
    uint32 type_{0}, format_{0};

    uint8 BytesPerPixel() const;
};

class Texture final : public CrusObject {
public:
    enum class eTEXTURE_TYPE {
        nUNKNOWN = 0, nNONE, n2D, n3D, nCUBE, n2D_ARRAY, nCUBE_ARRAY
    };

#if _CRUS_NOT_YET_IMPLEMENTED
    enum class eFILTERING_MODE {
        nPOINT = 0, nBILINEAR, nTRILINEAR
    } filteringMode{eFILTERING_MODE::nPOINT};

    enum class eWRAP_MODE {
        nCLAMP = 0, nREPEAT
    } wrapMode{eWRAP_MODE::nREPEAT};

    int8 anisotrophyLevel{0};
#endif

    Image image;

    Texture(eTEXTURE_TYPE type, std::string &&path);

    bool Init();

    void Bind() const;

    auto id() const { return id_; }

    auto w() const { return w_; }
    auto h() const { return h_; }

    std::ostream &ToStream(std::ostream &stream) const;

private:
    uint32 id_{0}, unit_{0};
    uint16 w_{0}, h_{0};

    eTEXTURE_TYPE type_{eTEXTURE_TYPE::nUNKNOWN};

    std::string path_{""};

    Texture() = default;
};
};

#endif // CRUS_TEXTURE_H