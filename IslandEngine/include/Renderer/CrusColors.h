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

    template<class T, typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::array<float, 4>>>...>
    constexpr Color(T &&rgb) : rgba(std::forward(rgb)) { };
    constexpr Color(float r, float g, float b, float a = 1.f) : rgba({r, g, b, a}) { };

    friend std::ostream &operator<< (std::ostream &stream, Color const &color);

    union {
        struct {
            float r, g, b, a;
        };

        std::array<float, 4> rgba;
    };
};

inline std::ostream &operator<< (std::ostream &_stream, Color const &_c)
{
    return _stream << _c.r << "; " << _c.g << "; " << _c.b << "; " << _c.a;
}

namespace colors {
Color constexpr kCLEARGRAY(0.631372f, 0.631372f, 0.631372f);
Color constexpr kBLACK(0.0f, 0.0f, 0.0f);
Color constexpr kDARKGRAY(0.250980f, 0.250980f, 0.250980f);
Color constexpr kGRAY(0.501960f, 0.501960f, 0.501960f);
Color constexpr kCARMINE(0.607843f, 0.0f, 0.156862f);
Color constexpr kNAVYBLUE(0.0f, 0.015686f, 0.376470f);
Color constexpr kBLUE(0.0f, 0.0f, 1.0f);
Color constexpr kDARKGREEN(0.039063f, 0.429688f, 0.039063f); // !
Color constexpr kINDIGO(0.149019f, 0.0f, 0.262745f);
Color constexpr kVIOLET(0.784313f, 0.0f, 0.784313f);
Color constexpr kUMBER(0.541176f, 0.282352f, 0.196078f);
Color constexpr kDARKBROWN(0.247058f, 0.137254f, 0.121568f);
Color constexpr kMAROON(0.6f, 0.149019f, 0.0f);      // !
Color constexpr kRED(1.0f, 0.0f, 0.0f);
Color constexpr kGREEN(0.0f, 1.0f, 0.0f);
Color constexpr kPOWDERBLUE(0.0f, 0.254901f, 0.6f);      // !
Color constexpr kWHITE(1.0f, 1.0f, 1.0f);
Color constexpr kYELLOW(1.0f, 1.0f, 0.0f);
Color constexpr kAQUA(0.392156f, 0.862745f, 1.0f);
Color constexpr kSPRINGGREEN(0.262745f, 1.0f, 0.639215f); // !
Color constexpr kPINK(1.0f, 0.690196f, 0.690196f);
Color constexpr kSANDYBROWN(0.894117f, 0.674509f, 0.474509f);
Color constexpr kGOLD(1.0f, 1.0f, 0.388235f);
Color constexpr kJADE(0.0f, 0.6f, 0.329411f); // !
}
};

#endif // CRUS_COLORS_H