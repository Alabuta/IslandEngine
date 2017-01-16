#pragma once

#ifndef CRUS_RENDERER_H             // Include guard "CrusRenderer.h"
#define CRUS_RENDERER_H

#include "Game\CrusComponent.h"
#include "Game\CrusBounds.h"
#include "Renderer\CrusMaterial.h"

namespace isle {
class Renderer : Component {
public:
    bool enabled{true};
    bool isVisible{true};
    uint8 sortingLayer{0}, sortingOrder{0};
    Bounds bounds_;

private:

};
};

#endif // CRUS_RENDERER_H