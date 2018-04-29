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

    void Update(float side = 15.0f, float step = 1.0f, u16 subdivs = 5);
    void Draw();

private:
    Program shader_;
    u32 vao_;

    u16 count_;

    auto Build(float side, float step, u16 subdivs);
};
};

#endif // CRUS_GRID_H