#pragma once

#ifndef CRUS_BOUNDS_H               // Include guard "CrusBounds.h"
#define CRUS_BOUNDS_H

#include "Math\CrusVector.h"
#include "Game\CrusRay.h"

namespace isle {
class Bounds final {
public:

    math::Vector center;
    math::Vector extents;

    math::Vector max;
    math::Vector min;

    Bounds() = default;
    Bounds(math::Vector &&center, math::Vector &&size);
    Bounds(Bounds &&bounds);

    bool IsInside(math::Vector const &point) const;
    bool IsInsideOrOn(math::Vector const &point) const;

    math::Vector ClosestPoint(math::Vector const &point) const;

    bool Intersects(Bounds const &bounds) const;

    bool IntersectRay(Ray const &ray) const;
    bool IntersectRay(Ray const &ray, float &distance) const;

    //bool IntersectSphere(Sphere const &sphere) const;

    Bounds const &operator= (Bounds &&bounds);

private:
};
};

#endif // CRUS_BOUNDS_H