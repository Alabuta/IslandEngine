#pragma once

#ifndef CRUS_RAY_H                  // Include guard "CrusRay.h"
#define CRUS_RAY_H

#include "Math\CrusVector.h"

namespace isle {
class Ray final {
public:

    math::Vector origin;
    math::Vector direction;

    /*math::Vector invDirection;
    int sign[3];*/

    Ray() = default;
    Ray(math::Vector &&origin, math::Vector &&direction);

    math::Vector GetPoint(float distance) const;
};
};

#endif // CRUS_RAY_H