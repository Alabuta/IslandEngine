/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 22th June 2013.
****    Description: supporting a gamepad xbox 360 controller.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_GAMEPAD_H              // Include guard "CrusGamepad.h"
#define CRUS_GAMEPAD_H

#include <XInput.h>

namespace isle
{
namespace Input
{
class Gamepad : Device {
private:
    XINPUT_STATE state_;
    u32 num_;

    /*struct {
        u16 bDPAD_UP_:1;
        u16 bDPAD_DOWN_:1;
        u16 bDPAD_LEFT_:1;
        u16 bDPAD_RIGHT_:1;
        u16 bSTART_:1;
        u16 bBACK_:1;
        u16 bLEFT_THUMB_:1;
        u16 bRIGHT_THUMB_:1;
        u16 bLEFT_SHOULDER_:1;
        u16 bRIGHT_SHOULDER_:1;
        u16 bA_:1;
        u16 bB_:1;
        u16 bX_:1;
        u16 bY_:1;
    };*/

public:

    Gamepad();
    ~Gamepad();
};
};
};

#endif // CRUS_GAMEPAD_H