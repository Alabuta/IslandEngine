#include <algorithm>
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

                if (i < topmosts[i])
                    topmosts[i] = i;

                if (i > bottommosts[i])
                    bottommosts[i] = i;
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

    sprite.texture_ = _texture;
    sprite.rect_ = _rect;
    sprite.pivot_ = _pivot;
    sprite.pixelsPerUnit_ = _pixelsPerUnit;

    Image image;

    if (!sprite.texture_.expired())
        if (!LoadTARGA(image, sprite.texture_.lock()->name()))
            return sprite;


    sprite.textureRect_ = std::move(GetCroppedTextureRect(image, sprite.rect_));
    Book::AddEvent(eNOTE::nDEBUG, "%f|%f|%f", sprite.textureRect_.x(), sprite.textureRect_.width(), sprite.textureRect_.xmax());
    Book::AddEvent(eNOTE::nDEBUG, "%f|%f|%f", sprite.textureRect_.y(), sprite.textureRect_.height(), sprite.textureRect_.ymax());

    sprite.BuildGeometry();

    return sprite;
}

void Sprite::BuildGeometry()
{
    auto x = textureRect_.x() - rect_.x();
    auto y = textureRect_.y() - rect_.y();

    auto w = textureRect_.width();
    auto h = textureRect_.height();

    Book::AddEvent(eNOTE::nDEBUG, "%f|%f|%f|%f", x, y, w, h);
}
};