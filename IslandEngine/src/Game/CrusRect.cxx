#include <algorithm>

#include "System\CrusBook.h"
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

Rect::Rect(Point const &_postion, Point const &_size)
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

/*static*/ Rect Rect::CreateFromMinMax(float _xmin, float _ymin, float _xmax, float _ymax)
{
    return Rect(_xmin, _ymin, _xmax - _xmin, _ymax - _ymin);
}

bool Rect::Contains(Point const &_point) const
{
    return (_point.x >= min().x) && (_point.x <= max().x)
        && (_point.y >= min().y) && (_point.y <= max().y);
}

bool Rect::Overlaps(Rect const &_rect) const
{
    if ((xmin_ > _rect.xmax()) || (_rect.xmin() > xmax_))
        return false;

    if ((ymin_ > _rect.ymax()) || (_rect.ymin() > ymax_))
        return false;

    return true;
}

Point Rect::NormalizedToPoint(Point const &_normalizedCoords) const
{
    Point normalized{math::clamp(_normalizedCoords.x, 0.f, 1.f), math::clamp(_normalizedCoords.y, 0.f, 1.f)};

    return Point{xmin_ + width_ * normalized.x, ymin_ + height_ * normalized.y};
}

Point Rect::NormalizedToPoint(Point &&_normalizedCoords) const
{
    _normalizedCoords.x = xmin_ + width_ * math::clamp(_normalizedCoords.x, 0.f, 1.f);
    _normalizedCoords.y = ymin_ + height_ * math::clamp(_normalizedCoords.y, 0.f, 1.f);

    return std::move(_normalizedCoords);
}

Point Rect::PointToNormalized(Point const &_point) const
{
    return Point{math::clamp((_point.x - xmin_) / width_, 0, 1), math::clamp((_point.y - ymin_) / height_, 0.f, 1.f)};
}

Point Rect::PointToNormalized(Point &&_point) const
{
    _point.x = math::clamp((_point.x - xmin_) / width_, 0.f, 1.f);
    _point.y = math::clamp((_point.y - ymin_) / height_, 0.f, 1.f);

    return std::move(_point);
}

std::ostream &Rect::ToStream(std::ostream &_stream) const
{
    return _stream << x_ << "; " << y_ << "; " << width_ << "; " << height_ << ".";
}
};