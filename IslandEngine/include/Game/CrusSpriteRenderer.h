#pragma once

#ifndef CRUS_SPRITE_RENDERER_H       // Include guard "CrusSpriteRenderer.h"
#define CRUS_SPRITE_RENDERER_H

#include "Renderer\CrusColors.h"

#include "Game\CrusRenderer.h"
#include "Game\CrusSprite.h"

namespace isle {
class SpriteRenderer : Renderer {
public:
    Color color{colors::kWHITE};
    bool flipX{false}, flipY{false};
    Sprite sprite;

private:
};
};

#endif // CRUS_SPRITE_RENDERER_H