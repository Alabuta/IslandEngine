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

namespace isle {
class Sprite final : CrusObject {
public:

    static Sprite Create(std::shared_ptr<Texture> const &texture, Rect const &rect, math::Point const &pivot, float pixelsPerUnit = 100);

    Texture const &texture() const;

    Bounds const &bounds() const;
    Rect const &rect() const;
    math::Point const &pivot() const;

    Rect const &textureRect() const;
    Rect const &textureRectOffset() const;

    uint16 const *const triangles() const;
    math::Point const *const vertices() const;
    math::Point const *const uvs() const;

private:
    std::weak_ptr<Texture> texture_;

    Bounds bounds_;
    Rect rect_;
    math::Point pivot_;

    Rect textureRect_, textureRectOffset_;

    std::vector<uint16> triangles_;
    std::vector<math::Point> vertices_, uvs_;

    float pixelsPerUnit_;

    void BuildGeometry();
};

__forceinline Texture const &Sprite::Sprite::texture() const
{
    if (!texture_.expired())
        return *texture_.lock();

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

__forceinline uint16 const *const Sprite::triangles() const
{
    return triangles_.data();
}

__forceinline math::Point const *const Sprite::vertices() const
{
    return vertices_.data();
}

__forceinline math::Point const *const Sprite::uvs() const
{
    return uvs_.data();
}
};

#endif // CRUS_SPRITE_H