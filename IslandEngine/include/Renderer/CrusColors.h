/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: indices of color - all of used colors must be here.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_COLORS_H               // Include guard "CrusColors.h"
#define CRUS_COLORS_H

#include <array>

namespace isle {
class Color {
public:
    explicit Color() =default;

    explicit Color(float r, float g, float b, float a = 1.f)
        : r_(r), g_(g), b_(b), a_(a)
    { };

    explicit Color(std::array<float, 4> const &vec)
        : vec_(vec)
    { };

    explicit Color(std::array<float, 4> &&vec)
        : vec_(std::move(vec))
    { };

    float r() const { return r_; };
    float g() const { return g_; };
    float b() const { return b_; };
    float a() const { return a_; };

    std::array<float, 4> const &c() const { return vec_; };

private:
    union {
        struct {
            float r_, g_, b_, a_;
        };

        std::array<float, 4> vec_;
    };
};

namespace colors {
Color const kCLEARGRAY(0.631372f, 0.631372f, 0.631372f);
Color const kBLACK(0.0f, 0.0f, 0.0f);
Color const kDARKGRAY(0.250980f, 0.250980f, 0.250980f);
Color const kGRAY(0.501960f, 0.501960f, 0.501960f);
Color const kCARMINE(0.607843f, 0.0f, 0.156862f);
Color const kNAVYBLUE(0.0f, 0.015686f, 0.376470f);
Color const kBLUE(0.0f, 0.0f, 1.0f);
Color const kDARKGREEN(0.039063f, 0.429688f, 0.039063f); // !
Color const kINDIGO(0.149019f, 0.0f, 0.262745f);
Color const kVIOLET(0.784313f, 0.0f, 0.784313f);
Color const kUMBER(0.541176f, 0.282352f, 0.196078f);
Color const kDARKBROWN(0.247058f, 0.137254f, 0.121568f);
Color const kMAROON(0.6f, 0.149019f, 0.0f);      // !
Color const kRED(1.0f, 0.0f, 0.0f);
Color const kGREEN(0.0f, 1.0f, 0.0f);
Color const kPOWDERBLUE(0.0f, 0.254901f, 0.6f);      // !
Color const kWHITE(1.0f, 1.0f, 1.0f);
Color const kYELLOW(1.0f, 1.0f, 0.0f);
Color const kAQUA(0.392156f, 0.862745f, 1.0f);
Color const kSPRINGGREEN(0.262745f, 1.0f, 0.639215f); // !
Color const kPINK(1.0f, 0.690196f, 0.690196f);
Color const kSANDYBROWN(0.894117f, 0.674509f, 0.474509f);
Color const kGOLD(1.0f, 1.0f, 0.388235f);
Color const kJADE(0.0f, 0.6f, 0.329411f); // !
}
};

#endif // CRUS_COLORS_H