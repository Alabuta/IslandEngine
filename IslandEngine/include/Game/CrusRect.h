#pragma once

#ifndef CRUS_RECT_H                 // Include guard "CrusRect.h"
#define CRUS_RECT_H

#include "Math\CrusMath.h"
#include "Math\CrusVector.h"

namespace isle {
class Rect final {
public:

    Rect() = default;
    Rect(float x, float y, float width, float height);
    Rect(math::Point const &postion, math::Point const &size);

    static Rect CreateFromMinMax(float xmin, float ymin, float xmax, float ymax);

    bool Contains(math::Point const &point) const;
    bool Overlaps(Rect const &rect) const;

    math::Point NormalizedToPoint(math::Point const &normalizedCoords) const;
    math::Point PointToNormalized(math::Point const &point) const;

    float x() const;
    float y() const;

    math::Point const &position() const;

    float width() const;
    float height() const;

    math::Point const &size() const;

    math::Point const &center() const;

    float xmin() const;
    float ymin() const;

    math::Point const &min() const;

    float xmax() const;
    float ymax() const;

    math::Point const &max() const;

private:

    union {
        struct {
            float x_, y_;
        };

        struct {
            math::Point position_;
        };
    };

    union {
        struct {
            float width_, height_;
        };

        struct {
            math::Point size_;
        };
    };

    math::Point center_;

    union {
        struct {
            float xmin_, ymin_;
        };

        struct {
            math::Point min_;
        };
    };

    union {
        struct {
            float xmax_, ymax_;
        };

        struct {
            math::Point max_;
        };
    };
};

__forceinline float Rect::x() const
{
    return x_;
}

__forceinline float Rect::y() const
{
    return y_;
}

__forceinline math::Point const &Rect::position() const
{
    return position_;
}

__forceinline float Rect::width() const
{
    return width_;
}

__forceinline float Rect::height() const
{
    return height_;
}

__forceinline math::Point const &Rect::size() const
{
    return size_;
}

__forceinline math::Point const &Rect::center() const
{
    return center_;
}

__forceinline float Rect::xmin() const
{
    return xmin_;
}

__forceinline float Rect::ymin() const
{
    return ymin_;
}

__forceinline math::Point const &Rect::min() const
{
    return min_;
}

__forceinline float Rect::xmax() const
{
    return xmax_;
}

__forceinline float Rect::ymax() const
{
    return ymax_;
}

__forceinline math::Point const &Rect::max() const
{
    return max_;
}
};

#endif // CRUS_RECT_H