/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 26th July 2012.
****    Description: ...
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_JOYSTICK_H             // Include guard "CrusJoystick.h"
#define CRUS_JOYSTICK_H

#include "System\CrusInput.h"

namespace isle
{
namespace Input
{
class Joystick : Device/*<Joystick>*/ {
private:
    int16 x_, y_, z_;
    int16 Rx_, Ry_, Rz_;

    int16 slider1_, slider2_;
    uint8 button_;

public:

    Joystick();
    ~Joystick();
};
};
};

#endif // CRUS_JOYSTICK_H