/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    11th July 2013 / 22th June 2013.
****    Description: supporting a gamepad xbox 360 controller.
****
****************************************************************************************/
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
    uint32 num_;

    /*struct {
        uint16 bDPAD_UP_:1;
        uint16 bDPAD_DOWN_:1;
        uint16 bDPAD_LEFT_:1;
        uint16 bDPAD_RIGHT_:1;
        uint16 bSTART_:1;
        uint16 bBACK_:1;
        uint16 bLEFT_THUMB_:1;
        uint16 bRIGHT_THUMB_:1;
        uint16 bLEFT_SHOULDER_:1;
        uint16 bRIGHT_SHOULDER_:1;
        uint16 bA_:1;
        uint16 bB_:1;
        uint16 bX_:1;
        uint16 bY_:1;
    };*/

public:

    Gamepad();
    ~Gamepad();
};
};
};

#endif // CRUS_GAMEPAD_H