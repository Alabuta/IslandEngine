/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: grid, just grid.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_GRID_H                 // Include guard "CrusGrid.h"
#define CRUS_GRID_H

#include "Renderer\CrusPosition.h"

namespace isle::intf {
class Grid {
public:

    void Update(float law = 15.0f, float grle = 1.0f, uint16 subdivs = 5);
    void Draw();

private:
    Program shader_;
    uint32 vao_;
    uint16 count_[2];

    Position *Build(float law, float grle, uint16 subdivs);
};
};

#endif // CRUS_GRID_H