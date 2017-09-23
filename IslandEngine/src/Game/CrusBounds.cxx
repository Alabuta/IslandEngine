#include <algorithm>

#include "Game\CrusBounds.h"


namespace isle {

Bounds::Bounds(math::Vector &&_center, math::Vector &&_size) :
    center(std::move(_center)), extents(std::move(_size))
{
    extents *= 0.5f;

    max = center + extents;
    min = center - extents;
}

//Bounds::Bounds(Bounds &&bounds) :
//    center(std::move(bounds.center)), extents(std::move(bounds.extents))
//{}

//__forceinline Bounds const &Bounds::operator= (Bounds &&bounds)
//{
//    center = std::move(bounds.center);
//    extents = std::move(bounds.extents);
//
//    return *this;
//}

__forceinline bool Bounds::IsInside(math::Vector const &point) const
{
    return (point.x > min.x) && (point.x < max.x)
        && (point.y > min.y) && (point.y < max.y)
        && (point.z > min.z) && (point.z < max.z);
}

__forceinline bool Bounds::IsInsideOrOn(math::Vector const &point) const
{
    return (point.x >= min.x) && (point.x <= max.x)
        && (point.y >= min.y) && (point.y <= max.y)
        && (point.z >= min.z) && (point.z <= max.z);
}

math::Vector Bounds::ClosestPoint(math::Vector const &point) const
{
    math::Vector closest = point;

    if (point.x < min.x)
        closest.x = min.x;

    else if (point.x > max.x)
        closest.x = max.x;

    if (point.y < min.y)
        closest.y = min.y;

    else if (point.y > max.y)
        closest.y = max.y;

    if (point.z < min.z)
        closest.z = min.z;

    else if (point.z > max.z)
        closest.z = max.z;

    return closest;
}

bool Bounds::Intersects(Bounds const &bounds) const
{
    if ((min.x > bounds.max.x) || (bounds.min.x > max.x))
        return false;

    if ((min.y > bounds.max.y) || (bounds.min.y > max.y))
        return false;

    if ((min.z > bounds.max.z) || (bounds.min.z > max.z))
        return false;

    return true;
}

__forceinline bool Bounds::IntersectRay(Ray const &ray) const
{
    float distance;

    return IntersectRay(ray, distance);
}

bool Bounds::IntersectRay(Ray const &ray, float &distance) const
{
    auto xmin = (min.x - ray.origin.x) / ray.direction.x;
    auto xmax = (max.x - ray.origin.x) / ray.direction.x;

    if (xmin > xmax)
        std::swap(xmin, xmax);

    auto ymin = (min.y - ray.origin.y) / ray.direction.y;
    auto ymax = (max.y - ray.origin.y) / ray.direction.y;

    if (ymin > ymax)
        std::swap(ymin, ymax);

    if ((xmin > ymax) || (ymin > xmax))
        return false;

    if (ymin > xmin)
        xmin = ymin;

    if (ymax < xmax)
        xmax = ymax;

    auto zmin = (min.z - ray.origin.z) / ray.direction.z;
    auto zmax = (max.z - ray.origin.z) / ray.direction.z;

    if (zmin > zmax)
        std::swap(zmin, zmax);

    if ((xmin > zmax) || (zmin > xmax))
        return false;

    if (zmin > xmin)
        xmin = zmin;

    if (zmax < xmax)
        xmax = zmax;

    distance = xmin;

    /*if (distance < 0) {
        distance = xmax;

        if (distance < 0)
            return false;
    }*/

    return true;
}
};