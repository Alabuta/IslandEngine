#pragma once

#ifndef CRUS_TRANSFORM_H            // Include guard "CrusTransform.h"
#define CRUS_TRANSFORM_H

#include "Math\CrusVector.h"
#include "Math\CrusMatrix.h"
#include "Math\CrusQuaternion.h"

#include "Game\CrusComponent.h"

namespace isle {
class Transform final : Component {
public:

    Transform() = default;
    ~Transform() = default;

    void LookAt(math::Vector target, math::Vector up);
    void RotateAround(math::Vector axis);
    void Translate(math::Vector direction, float distance);

    eCOMPONENT_TYPES type() const override { return eCOMPONENT_TYPES::nVIEWPORT; }

    void ToStream(std::ostream &stream) const;

private:
};
};

#endif // CRUS_TRANSFORM_H