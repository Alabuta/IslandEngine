/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2014 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****	Description: engine's time routines file.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_TIME_H                 // Include guard "CrusTime.h"
#define CRUS_TIME_H

#include <cmath>
#include "System\CrusTypes.h"
#include <System\CrusWindow.h>

namespace isle
{
class Time {
public:
    Time();

    void Restart();
    void Update();

    float delta() const {return delta_;};
    float elapsed() const {return elapsed_;};

private:
    size_t const kBufferSize{50};
    float buffer[50];

    LARGE_INTEGER freq_{0}, time_[3]{{0}, {0}, {0}};
    float delta_{0.0f}, elapsed_{0.0f};
    uint8 frames_{1}, counter_{1};
};
}

#endif // CRUS_TIME_H