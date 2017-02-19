#include "Game\CrusRay.h"


namespace isle {

Ray::Ray(math::Vector &&_origin, math::Vector &&_direction) :
    origin(std::move(_origin)), direction(std::move(_direction))
{
    direction.Normalize();

    //invDirection = std::move(math::Vector(1.0f / direction.x(), 1.0f / direction.y(), 1.0f / direction.z()));
}

math::Vector Ray::GetPoint(float _distance) const
{
    return origin + direction * _distance;
}
};