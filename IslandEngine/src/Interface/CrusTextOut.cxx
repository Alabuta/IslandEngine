/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 1th April 2010.
****	Description: font render routines implementation file.
****
********************************************************************************************************************************/
#include <string>

#include "System\CrusSystem.h"
#include "Renderer\CrusRender.h"

#include "Renderer\CrusTexture.h"
#include "Interface\CrusTextOut.h"

#if _CRUS_OBSOLETE
namespace isle {
math::Matrix mNumbers(
    1, 0, 0, 0,
    0, 1, 0, 0.72f,
    0, 0, 1, 0,
    0, 0, 0, 1
);

Textout::Textout()
{
    for (auto &a : texDispCoord_.rect_)
        a = 0;
}

Textout::~Textout() { };

bool Textout::Init(Texture const *const _texture, float _zoom, uint16 _x, uint16 _y, uint16 _w, uint16 _h)
{
    SetTexture(_texture);

    rectSymbol_.w_ = static_cast<float>(_w);
    rectSymbol_.h_ = static_cast<float>(_h);

    float const ratio = rectSymbol_.h_ / rectSymbol_.w_;

    texDispCoord_.x_ = static_cast<float>(_x) / texture_->w();
    texDispCoord_.y_ = static_cast<float>(_y) / texture_->h();

    float const quad_plane[] = {
        -_zoom,  _zoom * ratio, 0.0f, 0.0f,                             rectSymbol_.h_ / texture_->h(),
        -_zoom, -_zoom * ratio, 0.0f, 0.0f,                             0.0f,
         _zoom,  _zoom * ratio, 0.0f, rectSymbol_.w_ / texture_->w(),   rectSymbol_.h_ / texture_->h(),
         _zoom, -_zoom * ratio, 0.0f, rectSymbol_.w_ / texture_->w(),   0.0f
    };

    program_.AssignNew({"Interface/textout.glsl"});

    isle::Render::inst().CreateVAO(vao_);

    {
        uint32 vbo = 0;
        isle::Render::inst().CreateBO(GL_ARRAY_BUFFER, vbo);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_plane), quad_plane, GL_STATIC_DRAW);

    glVertexAttribPointer(isle::Render::nPOSITION, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
    glEnableVertexAttribArray(isle::Render::nPOSITION);

    glVertexAttribPointer(isle::Render::nTEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void const *>(sizeof(float) * 3));
    glEnableVertexAttribArray(isle::Render::nTEX_COORD);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void Textout::SetText(std::string const &_text)
{
    text_ = _text;
}

void Textout::SetTexture(Texture const *const _texture)
{
    if (_texture == nullptr)
        log::Error() << "texture pointer is invalid.";

    else texture_ = _texture;
}

void Textout::Render()
{
    program_.UseThis();
    texture_->Bind();

    float const width = 0.64f * 0.002511f * Render::inst().vp_.w() * 28 / 288.0f;
    float x = 0;

    if (text_.size() > 1)
        x = -width * 1.0f;

    for (auto &symbol : text_) {
        texDispCoord_.w_ = ((symbol - 48) - floorf((symbol - 48) / 5.0f) * 5) * rectSymbol_.w_ / texture_->w();
        texDispCoord_.h_ = (1 - floorf((symbol - 48) / 5.0f)) * rectSymbol_.h_ / texture_->h();

        auto uniformLocation = glGetUniformLocation(program_.program(), "inDispCoord");
        glProgramUniform4fv(program_.program(), uniformLocation, 1, texDispCoord_.rect_);

        Render::inst().UpdateViewport(0, 1, &mNumbers.Translate(x + Camera::inst().pos().x(), mNumbers.m()[7], 0));

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        x += width * 2.0f;
    }
}
}
#endif