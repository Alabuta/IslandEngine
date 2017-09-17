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
    Rect(Point const &postion, Point const &size);

    static Rect CreateFromMinMax(float xmin, float ymin, float xmax, float ymax);

    bool Contains(Point const &point) const;
    bool Overlaps(Rect const &rect) const;

    float GetSquare() const;

    Point NormalizedToPoint(Point const &normalizedCoords) const;
    Point NormalizedToPoint(Point &&normalizedCoords) const;

    Point PointToNormalized(Point const &point) const;
    Point PointToNormalized(Point &&point) const;

    float x() const;
    float y() const;

    Point const &position() const;

    float width() const;
    float height() const;

    Point const &size() const;

    Point const &center() const;

    float xmin() const;
    float ymin() const;

    Point const &min() const;

    float xmax() const;
    float ymax() const;

    Point const &max() const;

    std::ostream &ToStream(std::ostream &stream) const;

private:

    union {
        struct {
            float x_, y_;
        };

        struct {
            Point position_;
        };
    };

    union {
        struct {
            float width_, height_;
        };

        struct {
            Point size_;
        };
    };

    Point center_;

    union {
        struct {
            float xmin_, ymin_;
        };

        struct {
            Point min_;
        };
    };

    union {
        struct {
            float xmax_, ymax_;
        };

        struct {
            Point max_;
        };
    };
};

__forceinline float Rect::GetSquare() const
{
    return width_ * height_;
}

__forceinline float Rect::x() const
{
    return x_;
}

__forceinline float Rect::y() const
{
    return y_;
}

__forceinline Point const &Rect::position() const
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

__forceinline Point const &Rect::size() const
{
    return size_;
}

__forceinline Point const &Rect::center() const
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

__forceinline Point const &Rect::min() const
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

__forceinline Point const &Rect::max() const
{
    return max_;
}
};

#endif // CRUS_RECT_H