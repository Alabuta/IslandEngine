#pragma once

#ifndef CRUS_SPRITE_RENDERER_H       // Include guard "CrusSpriteRenderer.h"
#define CRUS_SPRITE_RENDERER_H

#include "Renderer\CrusColors.h"

#include "Game\CrusRenderer.h"

namespace isle {
class SpriteRenderer : Renderer {
public:
    Color color{colors::kWHITE};

private:
};
};

#endif // CRUS_SPRITE_RENDERER_H