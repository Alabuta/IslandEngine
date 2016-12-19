#pragma once

#ifndef CRUS_RENDERER_H             // Include guard "CrusRenderer.h"
#define CRUS_RENDERER_H

#include "Game\CrusComponent.h"
#include "Game\CrusBounds.h"

namespace isle {
class Renderer : Component {
public:
    bool enabled{true};

    Renderer() = default;
    ~Renderer() = default;

    Bounds const &bounds() const;

private:
    Bounds bounds_;

};
};

#endif // CRUS_RENDERER_H