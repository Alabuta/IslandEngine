#include <algorithm>
#include <numeric>
#include <vector>
#include <array>
#include <string>
#include <type_traits>

#include "System\CrusSystem.h"
#include "Manager\CrusTARGA.h"

#include "Game\CrusSprite.h"

#include "System\CrusTime.h"

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

#include <intrin.h>

isle::Rect GetCroppedSpriteRectSSE(isle::Image const &image, isle::Rect const &rect)
{
    auto const texels = reinterpret_cast<isle::RGBA const *const>(image.data_.data());

    // In on cycle pass the algorithm reads the 16 texels.
    // So, make sure that left and right bounds are multiples of 16.
    auto const left = (static_cast<uint32>(rect.x()) >> 4) << 4;
    auto const top = static_cast<uint32>(image.height_ - rect.ymax());

    auto const right = (((static_cast<uint32>(rect.xmax()) >> 4) + 1) << 4) - left;
    auto const bottom = static_cast<uint32>(rect.height());

    auto col_offset = 4, row_offset = 0;

    auto const one = _mm_set1_epi32(0xFFFFFFFF);
    auto const zero = _mm_setzero_si128();

    auto const alpha = _mm_set1_epi32(0xFF000000);

    auto texels1 = reinterpret_cast<__m128i const *>(&texels[left + (0 * sizeof(__m128i) / sizeof(uint32)) + image.width_ * top]);
    auto texels2 = reinterpret_cast<__m128i const *>(&texels[left + (1 * sizeof(__m128i) / sizeof(uint32)) + image.width_ * top]);
    auto texels3 = reinterpret_cast<__m128i const *>(&texels[left + (2 * sizeof(__m128i) / sizeof(uint32)) + image.width_ * top]);
    auto texels4 = reinterpret_cast<__m128i const *>(&texels[left + (3 * sizeof(__m128i) / sizeof(uint32)) + image.width_ * top]);

    __m128i r1, r2, r3, r4;
    __m128i pk;
    __m128i zeros, ones;

    __m128i rightLowMask, rightHighMask;
    __m128i leftLowMask, leftHighMask;

    __m128i mask;

    int32 rightShift, leftShift;
    int32 rightLowShift, rightHighShift;
    int32 leftHighShift, leftLowShift;

    // Used for BSF and BSR functions.
    auto firstBit = 0ul, lastBit = 0ul;
    auto isThereBit = false;

    // The required variables. By default, sprite boundaries is invalid.
    auto leftmost = static_cast<uint32>(image.width_), rightmost = 0u;
    auto topmost = static_cast<uint32>(image.height_), bottommost = 0u;

    for (auto row = 0u; row < bottom; ++row) {
        for (auto col = 0u; col < right >> 2; col += 4) {
            if (col + 4 >= right >> 2) {
                col_offset = -4;
                row_offset = 1;
            }

            else {
                col_offset = 4;
                row_offset = 0;
            }

            // Prefetching upcoming texels horizontall line.
            _mm_prefetch(reinterpret_cast<char const *>(&texels1[col + col_offset + ((image.width_ * (row + row_offset)) >> 2)]), _MM_HINT_NTA);
            _mm_prefetch(reinterpret_cast<char const *>(&texels2[col + col_offset + ((image.width_ * (row + row_offset)) >> 2)]), _MM_HINT_NTA);
            _mm_prefetch(reinterpret_cast<char const *>(&texels3[col + col_offset + ((image.width_ * (row + row_offset)) >> 2)]), _MM_HINT_NTA);
            _mm_prefetch(reinterpret_cast<char const *>(&texels4[col + col_offset + ((image.width_ * (row + row_offset)) >> 2)]), _MM_HINT_NTA);

            // Keeping only alpha values.
            r1 = _mm_and_si128(texels1[col + ((image.width_ * row) >> 2)], alpha);
            r2 = _mm_and_si128(texels2[col + ((image.width_ * row) >> 2)], alpha);
            r3 = _mm_and_si128(texels3[col + ((image.width_ * row) >> 2)], alpha);
            r4 = _mm_and_si128(texels4[col + ((image.width_ * row) >> 2)], alpha);

            // Packing all 16 texels' alphas to one 128bit register.
            pk = _mm_packs_epi16(_mm_packs_epi32(r1, r2), _mm_packs_epi32(r3, r4));

            // Comparing packed alphas to zero and inverting result.
            zeros = _mm_cmpeq_epi8(pk, zero);
            ones = _mm_xor_si128(zeros, one);

            // In on cycle pass the algorithm reads the 16 texels. If the sprite boundaries is not a multiple of 16,
            // create a clipping mask for the texels are not included in this sprite.
            rightShift = std::max<int32>((col << 2) + left + 16 - static_cast<int32>(rect.xmax()), 0);   // Texels amount that will be clipped at line's end.
            leftShift = std::max<int32>(static_cast<int32>(rect.x()) - ((col << 2) + left), 0);          // Texels amount that will be clipped at line's begin.

            rightLowShift = std::max<int32>(rightShift - 8, 0);
            rightHighShift = rightShift - rightLowShift;

            leftHighShift = std::max<int32>(leftShift - 8, 0);
            leftLowShift = leftShift - leftHighShift;

            rightLowMask = _mm_srli_epi64(one, rightLowShift << 3);
            rightHighMask = _mm_srli_epi64(one, rightHighShift << 3);

            leftLowMask = _mm_slli_epi64(one, leftLowShift << 3);
            leftHighMask = _mm_slli_epi64(one, leftHighShift << 3);

            // Merging line's begin and end masks in one register of 16 byte values.
            mask = _mm_and_si128(_mm_unpackhi_epi64(leftLowMask, leftHighMask), _mm_unpackhi_epi64(rightLowMask, rightHighMask));

            // Applying clipping mask.
            ones = _mm_and_si128(ones, mask);

            isThereBit = _BitScanForward64(&firstBit, ones.m128i_u64[0]) != 0;

            if (!isThereBit) {
                isThereBit = _BitScanForward64(&firstBit, ones.m128i_u64[1]) != 0;

                if (isThereBit)
                    firstBit += 0x40;    // High register qword offset.
            }

            if (isThereBit) {
                firstBit >>= 3;

                if ((left + firstBit + (col << 2)) < leftmost)
                    leftmost = left + firstBit + (col << 2);

                if (row < topmost)
                    topmost = row;

                if (row > bottommost)
                    bottommost = row;
            }

            isThereBit = _BitScanReverse64(&lastBit, ones.m128i_u64[1]) != 0;

            if (isThereBit)
                lastBit += 0x40;    // High register qword offset.

            else isThereBit = _BitScanReverse64(&lastBit, ones.m128i_u64[0]) != 0;

            if (isThereBit) {
                lastBit >>= 3;

                if ((left + lastBit + (col << 2)) > rightmost)
                    rightmost = left + lastBit + (col << 2);

                if (row < topmost)
                    topmost = row;

                if (row > bottommost)
                    bottommost = row;
            }
        }
    }

    auto x = static_cast<float>(leftmost == static_cast<decltype(leftmost)>(image.width_) ? 0 : leftmost);
    auto w = static_cast<float>(rightmost == 0 ? 0 : rightmost - x + 1);

    auto y = static_cast<float>(topmost == static_cast<decltype(topmost)>(image.height_) ? 0 : topmost);
    auto h = static_cast<float>(bottommost == -1 ? 0 : bottommost - y + 1);

    y = rect.height() - y - h + rect.y();

    return isle::Rect(x, y, w, h);
}

isle::Rect GetCroppedSpriteRect(isle::Image const &image, isle::Rect const &rect)
{
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

/*static*/ Sprite Sprite::Create(std::shared_ptr<Texture> const &_texture, uint32 _number, Rect const &_rect, Point const &_pivot, float _pixelsPerUnit)
{
    Sprite sprite;

    sprite.textureSheet_ = _texture;
    sprite.rect_ = _rect;
    sprite.pivot_ = _pivot;
    sprite.pixelsPerUnit_ = _pixelsPerUnit;
    sprite.textureRect_ = sprite.rect_;

    Image image;

    if (!sprite.textureSheet_.expired())
        image = sprite.textureSheet_.lock()->image;

    else return{ };

#if _CRUS_TEMP_DISABLED
    if (image.BytesPerPixel() == 4)
        sprite.textureRect_ = std::move(GetCroppedSpriteRect(image, sprite.rect_));
#endif

    if (image.BytesPerPixel() == 4)
        sprite.textureRect_ = GetCroppedSpriteRectSSE(image, sprite.rect_);

    if (sprite.textureRect_.GetSquare() < 1.0f)
        return{ };

    if (!sprite.BuildGeometry())
        return{ };

    if (!sprite.textureSheet_.expired())
        sprite.name_ = sprite.textureSheet_.lock()->name() + "_" + std::to_string(_number);

    sprite.MakeValid();

    return sprite;
}

bool Sprite::BuildGeometry()
{
    // Convert from pixels to world units.
    auto left = (textureRect_.x() - rect_.x() - pivot_.x) / pixelsPerUnit_;
    auto top = -(textureRect_.y() - rect_.y() - pivot_.y) / pixelsPerUnit_;

    auto right = (textureRect_.xmax() - rect_.x() - pivot_.x) / pixelsPerUnit_;
    auto bottom = -(textureRect_.ymax() - rect_.y() - pivot_.y) / pixelsPerUnit_;

    // :TODO: it's just a simple rectangle that bounds a cropped sprite texture rather than convex hull.
    // Lower left vertex -> top left vertex -> lower right vertex -> top right vertex (GL_TRIANGLE_STRIP).
    vertices_.emplace_back(std::move(Position(left, bottom)));
    vertices_.emplace_back(std::move(Position(left, top)));
    vertices_.emplace_back(std::move(Position(right, bottom)));
    vertices_.emplace_back(std::move(Position(right, top)));
    vertices_.shrink_to_fit();

    Rect textureSheetRect;

    if (!textureSheet_.expired())
        textureSheetRect = Rect(0, 0, textureSheet_.lock()->w(), textureSheet_.lock()->h());

    else {
        log::Error() << "texture doesn't exist.";
        return false;
    }

    // Getting normalized coordinates for uv rectangle.
    auto normalizedSpriteMins = textureSheetRect.PointToNormalized(Point{textureRect_.x(), textureRect_.y()});
    auto normalizedSpriteMaxs = textureSheetRect.PointToNormalized(Point{textureRect_.xmax(), textureRect_.ymax()});

    uvs_.emplace_back(std::move(UV(normalizedSpriteMins.x, 1 - normalizedSpriteMaxs.y)));
    uvs_.emplace_back(std::move(UV(normalizedSpriteMins.x, 1 - normalizedSpriteMins.y)));
    uvs_.emplace_back(std::move(UV(normalizedSpriteMaxs.x, 1 - normalizedSpriteMaxs.y)));
    uvs_.emplace_back(std::move(UV(normalizedSpriteMaxs.x, 1 - normalizedSpriteMins.y)));
    uvs_.shrink_to_fit();

    // A simple index buffer for quad.
    indices_.resize(4);
    std::iota(indices_.begin(), indices_.end(), 0);
    indices_.shrink_to_fit();

    return true;
}

void Sprite::ToStream(std::ostream &_stream) const
{
    _stream << name_;
}
};