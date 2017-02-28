/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****	17th December 2015 / 1th April 2009.
****	Description: font render header file.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_TEXT_OUT_H             // Include guard "CrusTextOut.h"
#define CRUS_TEXT_OUT_H

#if _CRUS_OBSOLETE

#include <memory>
#include <vector>

#include "Renderer\CrusProgram.h"

namespace isle {
class Textout {
public:
    Textout();
    ~Textout();

    bool Init(Texture const *const texture, float zoom, uint16 _x, uint16 _y, uint16 _w, uint16 _h);

    void SetText(std::string const &text);
    void SetTexture(Texture const *const texture);

    void Render();

private:
    Program program_;
    uint32 vao_{0};

    struct {
        float w_{0}, h_{0};
    } rectSymbol_;

    union {
        struct {
            float x_, y_, w_, h_;
        };

        struct {
            float rect_[4];
        };
    } texDispCoord_;

    std::string text_{""};
    Texture const *texture_{nullptr};
};
};

#endif

#endif // CRUS_TEXT_OUT_H