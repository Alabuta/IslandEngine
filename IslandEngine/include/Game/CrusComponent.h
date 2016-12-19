#pragma once

#ifndef CRUS_COMPONENT_H            // Include guard "CrusComponent.h"
#define CRUS_COMPONENT_H

#include <memory>
#include "Game\CrusObject.h"

class Transform;

enum class eCOMPONENT_TYPES {
    nTRANSFORM = 1,
    nMESH_RENDERER
};

namespace isle {
class Component : CrusObject {
public:

    virtual ~Component() = default;

    virtual eCOMPONENT_TYPES type() const = 0;

protected:
    Component() = default;

private:
    Component(Component const &) = delete;
};
};

#endif // CRUS_COMPONENT_H