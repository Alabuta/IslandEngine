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

namespace isle
{
struct alignas(sizeof(u32)) RGBA
{
    union {
        u8 channels[4];
        u32 value;
    };
};

//std::istream &operator >> (std::istream &in, RGBA &per)
//{
//    in >> per.value;
//    return in;
//}

struct Image {
    alignas(sizeof(__m128i)) std::vector<RGBA> data_;

    i32 bpp_{0};
    i32 width_{0}, height_{0};
    u32 type_{0}, format_{0};

    u8 BytesPerPixel() const;
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

    i8 anisotrophyLevel{0};
#endif

    Image image;

    Texture(eTEXTURE_TYPE type, std::string_view path, u8 samplesCount = 1);

    bool Init();

    void Bind() const;

    auto id() const { return id_; }

    auto w() const { return w_; }
    auto h() const { return h_; }

    friend std::ostream &operator<< (std::ostream &stream, Texture const &texture);

private:
    u32 id_{0}, unit_{0};
    u16 w_{0}, h_{0};
    u64 handle_;

    u8 samplesCount_{1};

    eTEXTURE_TYPE type_{eTEXTURE_TYPE::nUNKNOWN};

    std::string path_{""};

    Texture() = default;
};
};

#endif // CRUS_TEXTURE_H