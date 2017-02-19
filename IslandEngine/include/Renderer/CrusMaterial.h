#pragma once

#ifndef CRUS_MATERIAL_H             // Include guard "CrusMaterial.h"
#define CRUS_MATERIAL_H

#include "Renderer\CrusColors.h"
#include "Renderer\CrusTexture.h"
#include "Renderer\CrusProgram.h"

#include "Game\CrusObject.h"

namespace isle {
class Material final : CrusObject {
public:

    Color color{colors::kWHITE};
    Texture texture;
    Program program;

private:
};
};

#endif // CRUS_MATERIAL_H