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


#define N 8000000

//alignas(sizeof(__m128)) std::vector<float> xx;

//class alignas(sizeof(__m128)) sse_t final : public std::vector<float>{
//};
//
//static_assert(alignof(/*decltype(*/sse_t) == sizeof(__m128), "Vector isn't aligned by 128 bits!");

struct alignas(sizeof(__int32)) Texel
{
    union {
        byte channels[4];
        uint32 value;
    };
};

auto const kDIM = 32;

struct alignas(sizeof(__m128i)) ImageRow
{
    Texel pixels[kDIM];
};

// <обычна€> функци€
float inner2(float *x, float *y, int n)
{
    float s;
    int i;
    s = 0;
    for (i = 0; i < n; i++)
        s += x[i] * y[i];
    return s;
}

// функци€ с использованием SSE intrinsics
float inner0(float *x, float *y, int n)
{
    __declspec(align(sizeof(__m128))) struct {
        float value[4];
    } sum;

    auto xx = reinterpret_cast<__m128 const *>(x);
    auto yy = reinterpret_cast<__m128 const *>(y);

    auto s = _mm_setzero_ps();

    __m128 p;

    auto constexpr dim = sizeof(__m128) / sizeof(float);

    for (auto i = 0; i < n / dim; ++i) {
        _mm_prefetch(reinterpret_cast<char const *>(&xx[i + dim]), _MM_HINT_NTA);
        _mm_prefetch(reinterpret_cast<char const *>(&yy[i + dim]), _MM_HINT_NTA);

        p = _mm_mul_ps(xx[i], yy[i]);
        s = _mm_add_ps(s, p);
    }

    _mm_store_ps(sum.value, s);

    return sum.value[0] + sum.value[1] + sum.value[2] + sum.value[3];
}

// функци€ с использованием SSE intrinsics
float inner1(float *x, float *y, int n)
{
    float sum;

    auto xx = reinterpret_cast<__m128 const *>(x);
    auto yy = reinterpret_cast<__m128 const *>(y);

    auto s = _mm_setzero_ps();

    __m128 p;

    auto constexpr dim = sizeof(__m128) / sizeof(float);

    for (auto i = 0; i < n / dim; ++i) {
        // предвыборка данных в кэш (на несколько итераций вперед)
        _mm_prefetch(reinterpret_cast<char const *>(&xx[i + dim]), _MM_HINT_NTA);
        _mm_prefetch(reinterpret_cast<char const *>(&yy[i + dim]), _MM_HINT_NTA);

        p = _mm_mul_ps(xx[i], yy[i]); // векторное умножение четырех чисел
        s = _mm_add_ps(s, p);          // векторное сложение четырех чисел
    }

    //p = _mm_movehl_ps(p, s); // перемещение двух старших значений s в младшие p
    //s = _mm_add_ps(s, p);    // векторное сложение
    //p = _mm_shuffle_ps(s, s, 1); //перемещение второго значени€ в s в младшую позицию в p
    //s = _mm_add_ss(s, p);    // скал€рное сложение

    s = _mm_hadd_ps(s, s);
    s = _mm_hadd_ps(s, s);

    _mm_store_ss(&sum, s); // запись младшего значени€ в пам€ть

    return sum;
}

void TryIt()
{
    float *x, *y, s;
    int i;

    // выделение пам€ти с выравниванием
    x = (float *)_mm_malloc(N * sizeof(float), sizeof(__m128));
    y = (float *)_mm_malloc(N * sizeof(float), sizeof(__m128));


    for (i = 0; i < N; i++) {
        x[i] = (float)10 * i / N;
        y[i] = (float)10 * (N - i - 1) / N;
    }

    {
        auto elapsed = isle::measure<>::execution([&] ()
        {
            s = inner0(x, y, N);
        });

        isle::log::Debug() << '\n';
        isle::log::Debug() << "inner0(): " << elapsed << "; Result: " << s;
    }

    {
        auto elapsed = isle::measure<>::execution([&] ()
        {
            s = inner1(x, y, N);
        });

        isle::log::Debug() << "inner1(): " << elapsed << "; Result: " << s;
    }

    {
        auto elapsed = isle::measure<>::execution([&] ()
        {
            s = inner2(x, y, N);
        });

        isle::log::Debug() << "inner2(): " << elapsed << "; Result: " << s;
    }

    _mm_free(x); _mm_free(y);
}

isle::Rect CompareSSE(Texel *texels, uint32 width, uint32 height, isle::Rect const &rect)
{
    // Make sure that left and right bounds are multiples of sixteen - 'cause in one cycle of the algorithm reads sixteen pixels.
    auto const left = (static_cast<uint32>(rect.x()) >> 4) << 4;
    auto const top = static_cast<uint32>(height - rect.ymax());;

    auto const right = (((static_cast<uint32>(rect.xmax()) >> 4) + 1) << 4) - left;
    auto const bottom = static_cast<uint32>(rect.height());;

    auto const one = _mm_set1_epi32(0xFFFFFFFF);
    auto const zero = _mm_setzero_si128();

    auto const alpha = _mm_set1_epi32(0xFF000000);

    auto texels1 = reinterpret_cast<__m128i const *>(&texels[left + (0 * sizeof(__m128i) / sizeof(uint32)) + width * top]);
    auto texels2 = reinterpret_cast<__m128i const *>(&texels[left + (1 * sizeof(__m128i) / sizeof(uint32)) + width * top]);
    auto texels3 = reinterpret_cast<__m128i const *>(&texels[left + (2 * sizeof(__m128i) / sizeof(uint32)) + width * top]);
    auto texels4 = reinterpret_cast<__m128i const *>(&texels[left + (3 * sizeof(__m128i) / sizeof(uint32)) + width * top]);

    __m128i r1, r2, r3, r4;
    __m128i pk1, pk2, pk3;
    __m128i zeros, ones;

    __m128i right_low_mask, right_high_mask;
    __m128i left_low_mask, left_high_mask;

    __m128i mask;

    int32 right_shift, left_shift;
    int32 right_low_shift, right_high_shift;
    int32 left_high_shift, left_low_shift;

    auto col_offset = 4, row_offset = 0;

    uint32 leftmost = width, rightmost = 0;
    uint32 topmost = height, bottommost = 0;

    auto first = 0ul, last = 0ul;

    auto isThereBit = false;

    for (auto row = 0ui32; row < bottom; ++row) {
        for (auto col = 0ui32; col < right >> 2; col += 4) {
            if (col + 4 >= right >> 2) {
                col_offset = -4;
                row_offset = 1;
            }

            else {
                col_offset = 4;
                row_offset = 0;
            }

            _mm_prefetch(reinterpret_cast<char const *>(&texels1[col + col_offset + ((width * (row + row_offset)) >> 2)]), _MM_HINT_NTA);
            _mm_prefetch(reinterpret_cast<char const *>(&texels2[col + col_offset + ((width * (row + row_offset)) >> 2)]), _MM_HINT_NTA);
            _mm_prefetch(reinterpret_cast<char const *>(&texels3[col + col_offset + ((width * (row + row_offset)) >> 2)]), _MM_HINT_NTA);
            _mm_prefetch(reinterpret_cast<char const *>(&texels4[col + col_offset + ((width * (row + row_offset)) >> 2)]), _MM_HINT_NTA);

            r1 = _mm_and_si128(texels1[col + ((width * row) >> 2)], alpha);
            r2 = _mm_and_si128(texels2[col + ((width * row) >> 2)], alpha);
            r3 = _mm_and_si128(texels3[col + ((width * row) >> 2)], alpha);
            r4 = _mm_and_si128(texels4[col + ((width * row) >> 2)], alpha);

            pk1 = _mm_packs_epi32(r1, r2);
            pk2 = _mm_packs_epi32(r3, r4);

            pk3 = _mm_packs_epi16(pk1, pk2);

            zeros = _mm_cmpeq_epi8(pk3, zero);
            ones = _mm_xor_si128(zeros, one);

            right_shift = std::max<int32>((col << 2) + left + 16 - static_cast<int32>(rect.xmax()), 0);
            left_shift = std::max<int32>(static_cast<int32>(rect.x()) - ((col << 2) + left), 0);

            right_low_shift = std::max<int32>(right_shift - 8, 0);
            right_high_shift = right_shift - right_low_shift;

            left_high_shift = std::max<int32>(left_shift - 8, 0);
            left_low_shift = left_shift - left_high_shift;

            right_low_mask = _mm_srli_epi64(one, right_low_shift << 3);
            right_high_mask = _mm_srli_epi64(one, right_high_shift << 3);

            left_low_mask = _mm_slli_epi64(one, left_low_shift << 3);
            left_high_mask = _mm_slli_epi64(one, left_high_shift << 3);

            mask = _mm_and_si128(_mm_unpackhi_epi64(left_low_mask, left_high_mask), _mm_unpackhi_epi64(right_low_mask, right_high_mask));

            ones = _mm_and_si128(ones, mask);

            isThereBit = _BitScanForward64(&first, ones.m128i_u64[0]) != 0;

            if (!isThereBit) {
                isThereBit = _BitScanForward64(&first, ones.m128i_u64[1]) != 0;

                if (isThereBit)
                    first += 0x40;
            }

            if (isThereBit) {
                first >>= 3;

                if ((left + first + (col << 2)) < leftmost)
                    leftmost = left + first + (col << 2);

                if (row < topmost)
                    topmost = row;

                if (row > bottommost)
                    bottommost = row;
            }

            isThereBit = _BitScanReverse64(&last, ones.m128i_u64[1]) != 0;

            if (isThereBit)
                last += 0x40;

            else isThereBit = _BitScanReverse64(&last, ones.m128i_u64[0]) != 0;

            if (isThereBit) {
                last >>= 3;

                if ((left + last + (col << 2)) > rightmost)
                    rightmost = left + last + (col << 2);

                if (row < topmost)
                    topmost = row;

                if (row > bottommost)
                    bottommost = row;
            }
        }
    }

    auto x = static_cast<float>(leftmost == width ? 0 : leftmost);
    auto w = static_cast<float>(rightmost == 0 ? 0 : rightmost - x + 1);

    auto y = static_cast<float>(topmost == height ? 0 : topmost);
    auto h = static_cast<float>(bottommost == -1 ? 0 : bottommost - y + 1);

    y = rect.height() - y - h + rect.y();

    return isle::Rect(x, y, w, h);
}

isle::Rect CompareNaive(Texel *x, size_t n)
{
    size_t left = n, right = 0;

    for (auto i = 0; i < n; ++i) {
        for (auto j = 0; j < n; ++j) {
            if (x[i * kDIM + j].channels[3] > 0) {
                if (j < left)
                    left = i;

                if (j > right)
                    right = i;
            }
        }
    }

    return isle::Rect((float)left, 0, (float)right, 0);
}

void Compare()
{
    /*size_t const n = 4000;

    auto x = (int32 *)_mm_malloc(n * sizeof(int32), sizeof(__m128i));
    auto y = (int32 *)_mm_malloc(n * sizeof(int32), sizeof(__m128i));

    memset(x, 0, n * sizeof(int32));
    memset(y, 0, n * sizeof(int32));

    for (auto i = 5; i < n - 5; ++i) {
        x[i] = 1;
        y[i] = 1;
    }*/

    //alignas(sizeof(__m128)) Pixel pixels[kDIM];
    //static_assert(alignof(pixels) == sizeof(__m128), "Array isn't aligned by 128 bits!");
    auto pixels = (Texel *)_mm_malloc(kDIM * kDIM * sizeof(Texel), sizeof(__m128i));

    if (pixels == nullptr)
        return;

    memset(pixels, 0, kDIM * kDIM * sizeof(Texel));

    for (auto i = 4; i < kDIM - 4; ++i) {
        for (auto j = 4; j < kDIM - 4; ++j) {
            pixels[i * kDIM + j].channels[3] = 0xFF;
        }
    }

    isle::Rect result;

    {
        auto elapsed = isle::measure<>::execution([&] ()
        {
            result = CompareNaive(pixels, kDIM);
        });

        //isle::log::Debug() << "CompareNaive(): " << elapsed << "; Result: " << result;
    }

    /*{
        auto elapsed = isle::measure<>::execution([&] ()
        {
            result = CompareSSE(pixels, kDIM);
        });

        isle::log::Debug() << "CompareSSE(): " << elapsed << "; Result: " << result;
    }*/

    _mm_free(pixels);

    /*_mm_free(x);
    _mm_free(y);*/
}

isle::Rect SIMD(isle::Image const &image, isle::Rect const &rect)
{
    // :TODO: should be better return original texture bound rect?
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
        image = sprite.textureSheet_.lock()->image;

    else return{ };
    //TryIt();

    //Compare();

    if (true) {
        auto elapsed = measure<std::chrono::microseconds>::execution([&sprite, &image] ()
        {
            sprite.textureRect_ = std::move(GetCroppedTextureRect(image, sprite.rect_));
        });

        log::Debug() << sprite.textureRect_ << "; " << elapsed;
    }

    if (true) {
        auto data = (Texel *)_aligned_malloc(image.data_.size() * sizeof(byte), sizeof(__m128i));

        if (data != nullptr) {
            //alignas(sizeof(__m128i)) std::vector<Texel> data;
            //static_assert(alignof(/*decltype(*/data) == sizeof(__m128), "Vector isn't aligned by 128 bits!");

            std::memcpy(data, image.data_.data(), image.data_.size() * sizeof(byte));
            //std::copy(image.data_.begin(), image.data_.end(), d.begin());

            auto elapsed = measure<std::chrono::microseconds>::execution([&sprite, &image, &data] ()
            {
                if (image.BytesPerPixel() == 4)
                    sprite.textureRect_ = CompareSSE(data, image.width_, image.height_, sprite.rect_);
            });

            _aligned_free(data);

            log::Debug() << sprite.textureRect_ << "; " << elapsed;
        }
    }

    if (sprite.textureRect_.GetSquare() < 1.0f)
        return{ };

    if (!sprite.BuildGeometry())
        return{ };

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

    // :TODO: it's just a simple rectangle that bounds a cropped sprite texture.
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
        log::Error() << "texture doesn't exist.";
        return false;
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

    return true;
}
};