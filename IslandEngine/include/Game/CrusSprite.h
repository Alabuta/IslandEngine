#pragma once

#ifndef CRUS_SPRITE_H               // Include guard "CrusSprite.h"
#define CRUS_SPRITE_H

#include <memory>
#include <vector>

#include "Math\CrusVector.h"
#include "Renderer\CrusTexture.h"

#include "Game\CrusObject.h"
#include "Game\CrusBounds.h"
#include "Game\CrusRect.h"
#include "Renderer\CrusPosition.h"
#include "Renderer\CrusUV.h"

namespace isle {
class Sprite final : CrusObject {
public:

    static Sprite Create(std::shared_ptr<Texture> const &texture, Rect const &rect, math::Point const &pivot, float pixelsPerUnit = 100);

    Texture const &textureSheet() const;

    Bounds const &bounds() const;
    Rect const &rect() const;
    math::Point const &pivot() const;

    Rect const &textureRect() const;
    Rect const &textureRectOffset() const;

    std::vector<uint16> const &indices() const;
    std::vector<Position> const &vertices() const;
    std::vector<UV> const &uvs() const;

private:
    std::weak_ptr<Texture> textureSheet_;

    Bounds bounds_;
    Rect rect_;
    math::Point pivot_;

    Rect textureRect_, textureRectOffset_;

    std::vector<uint16> indices_;
    std::vector<Position> vertices_;
    std::vector<UV> uvs_;

    float pixelsPerUnit_;

    void BuildGeometry();
};

__forceinline Texture const &Sprite::Sprite::textureSheet() const
{
    if (!textureSheet_.expired())
        return *textureSheet_.lock();

    return Texture("");
}

__forceinline Bounds const &Sprite::bounds() const
{
    return bounds_;
}

__forceinline Rect const &Sprite::rect() const
{
    return rect_;
}

__forceinline math::Point const &Sprite::pivot() const
{
    return pivot_;
}

__forceinline Rect const &Sprite::textureRect() const
{
    return textureRect_;
}

__forceinline Rect const &Sprite::textureRectOffset() const
{
    return textureRectOffset_;
}

__forceinline std::vector<uint16> const &Sprite::indices() const
{
    return indices_;
}

__forceinline std::vector<Position> const &Sprite::vertices() const
{
    return vertices_;
}

__forceinline std::vector<UV> const &Sprite::uvs() const
{
    return uvs_;
}
};

#endif // CRUS_SPRITE_H