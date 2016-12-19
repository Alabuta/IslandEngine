#include "Game\CrusRect.h"

namespace isle {

Rect::Rect(float _x, float _y, float _width, float _height) :
    x_(_x), y_(_y), width_(_width), height_(_height)
{
    center_.x = width_  * 0.5f + x_;
    center_.y = height_ * 0.5f + y_;

    xmin_ = x_;
    ymin_ = y_;

    xmax_ = x_ + width_;
    ymax_ = y_ + height_;
}

Rect::Rect(math::Point const &_postion, math::Point const &_size)
{
    position_.x = _postion.x;
    position_.y = _postion.y;

    size_.x = _size.x;
    size_.y = _size.y;

    center_.x = width_  * 0.5f + x_;
    center_.y = height_ * 0.5f + y_;

    xmin_ = x_;
    ymin_ = y_;

    xmax_ = x_ + width_;
    ymax_ = y_ + height_;
}

/*static*/ Rect Rect::CreateFromMinMax(float xmin, float ymin, float xmax, float ymax)
{
    return Rect(xmin, ymin, xmax - xmin, ymax - ymin);
}

bool Rect::Contains(math::Point const &point) const
{
    return (point.x >= min().x) && (point.x <= max().x)
        && (point.y >= min().y) && (point.y <= max().y);
}

bool Rect::Overlaps(Rect const &rect) const
{
    if ((xmin_ > rect.xmax()) || (rect.xmin() > xmax_))
        return false;

    if ((ymin_ > rect.ymax()) || (rect.ymin() > ymax_))
        return false;

    return true;
}

math::Point Rect::NormalizedToPoint(math::Point const &normalizedCoords) const
{
    math::Point normalized{math::clamp(normalizedCoords.x, 0, 1), math::clamp(normalizedCoords.y, 0, 1)};

    return math::Point{xmin_ + width_ * normalized.x, ymin_ + height_ * normalized.y};
}

math::Point Rect::PointToNormalized(math::Point const &point) const
{
    return math::Point{math::clamp((point.x - xmin_) / width_, 0, 1), math::clamp((point.y - ymin_) / height_, 0, 1)};
}
};