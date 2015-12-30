/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****    Description: grid, just grid.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_GRID_H                 // Include guard "CrusGrid.h"
#define CRUS_GRID_H

namespace isle
{
namespace intf
{
class Grid {
public:
    Grid(){};
    ~Grid(){};

    void Update(float law = 15.0f, float grle = 1.0f, uint16 subdivs = 5);
    void Draw();

private:
    Program shader_;
    uint32 vao_;
    uint16 count_[2];

    Vertex *Build(float law, float grle, uint16 subdivs);
};
};
};

#endif // CRUS_GRID_H