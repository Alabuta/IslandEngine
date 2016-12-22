#include <algorithm>
#include <numeric>
#include <vector>
#include <string>

#include "System\CrusSystem.h"
#include "Manager\CrusTARGA.h"

#include "Game\CrusSprite.h"

isle::Rect GetCroppedTextureRect(isle::Image const &image, isle::Rect const &rect)
{
    if (image.BytesPerPixel() != 4)
        return isle::Rect(0, 0, 0, 0);

    auto const width = image.width_;
    auto const height = image.height_;

    auto const left = static_cast<int32>(rect.x());
    auto const top = static_cast<int32>(height - rect.y() - rect.height());

    auto const right = static_cast<int32>(rect.xmax());
    auto const bottom = static_cast<int32>(height - rect.y());

    auto const bpp = image.BytesPerPixel();

    int32 leftmost = width;
    int32 rightmost = -1;

    std::vector<int32> topmosts(0), bottommosts(0);

    topmosts.resize(static_cast<size_t>(rect.width()), height);
    bottommosts.resize(static_cast<size_t>(rect.width()), -1);

    byte data = 0;

    for (auto i = top; i < bottom; ++i) {
        for (auto j = left; j < right; ++j) {
            data = image.data_[i * width * bpp + j * bpp + (bpp - 1)];

            if (data > 0) {
                if (j < leftmost)
                    leftmost = j;

                if (j > rightmost)
                    rightmost = j;

                if (i < topmosts[i - top])
                    topmosts[i - top] = i;

                if (i > bottommosts[i - top])
                    bottommosts[i - top] = i;
            }
        }
    }

    float x = static_cast<float>(leftmost == width ? 0 : leftmost);
    float w = static_cast<float>(rightmost == -1 ? 0 : rightmost - x + 1);

    auto ty = *std::min_element(topmosts.begin(), topmosts.end());
    float y = static_cast<float>(ty == height ? 0 : ty);

    auto th = *std::max_element(bottommosts.begin(), bottommosts.end());
    float h = static_cast<float>(th == -1 ? 0 : th - y + 1);

    y = height - y - h;

    return isle::Rect(x, y, w, h);
}

namespace isle {
/*static*/ Sprite Sprite::Create(std::shared_ptr<Texture> const &_texture, Rect const &_rect, math::Point const &_pivot, float _pixelsPerUnit)
{
    Sprite sprite;

    sprite.textureSheet_ = _texture;
    sprite.rect_ = _rect;
    sprite.pivot_ = _pivot;
    sprite.pixelsPerUnit_ = _pixelsPerUnit;

    Image image;

    if (!sprite.textureSheet_.expired())
        if (!LoadTARGA(image, sprite.textureSheet_.lock()->name()))
            return{};


    sprite.textureRect_ = std::move(GetCroppedTextureRect(image, sprite.rect_));

    sprite.BuildGeometry();

    return sprite;
}

void Sprite::BuildGeometry()
{
    // Convert from pixels to world units.
    auto left = (textureRect_.x() - rect_.x() - pivot_.x) / pixelsPerUnit_;
    auto top = -(textureRect_.y() - rect_.y() - pivot_.y) / pixelsPerUnit_;

    auto right = (textureRect_.xmax() - rect_.x() - pivot_.x) / pixelsPerUnit_;
    auto bottom = -(textureRect_.ymax() - rect_.y() - pivot_.y) / pixelsPerUnit_;

    // :TODO: just simple rectangle that bounds a cropped sprite texture.
    // Lower left vertex -> top left vertex -> lower right vertex -> top right vertex (GL_TRIANGLE_STRIP).
    vertices_.emplace_back(Position(left, bottom));
    vertices_.emplace_back(Position(left, top));
    vertices_.emplace_back(Position(right, bottom));
    vertices_.emplace_back(Position(right, top));
    vertices_.shrink_to_fit();

    Rect textureSheetRect;

    if (!textureSheet_.expired())
        textureSheetRect = Rect(0, 0, textureSheet_.lock()->w(), textureSheet_.lock()->h());

    else {
        Book::AddEvent(eNOTE::nERROR, "texture doesn't exist.");
        return;
    }

    // Getting normalized coordinates for uv rectangle.
    auto normalizedSpriteMins = std::move(textureSheetRect.PointToNormalized(math::Point{textureRect_.x(), textureRect_.y()}));
    auto normalizedSpriteMaxs = std::move(textureSheetRect.PointToNormalized(math::Point{textureRect_.xmax(), textureRect_.ymax()}));

    uvs_.emplace_back(UV(normalizedSpriteMins.x, 1 - normalizedSpriteMaxs.y));
    uvs_.emplace_back(UV(normalizedSpriteMins.x, 1 - normalizedSpriteMins.y));
    uvs_.emplace_back(UV(normalizedSpriteMaxs.x, 1 - normalizedSpriteMaxs.y));
    uvs_.emplace_back(UV(normalizedSpriteMaxs.x, 1 - normalizedSpriteMins.y));
    uvs_.shrink_to_fit();

    // A simple index buffer for quad.
    indices_.resize(4);
    std::iota(indices_.begin(), indices_.end(), 0);
    indices_.shrink_to_fit();
}
};