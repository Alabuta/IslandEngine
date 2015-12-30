/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 22th June 2013.
****    Description: supporting a gamepad xbox 360 controller.
****
****************************************************************************************/
#include "System\CrusSystem.h"
#include "System\CrusWindow.h"

#include "System\CrusInput.h"
#include "System\CrusGamepad.h"

#pragma comment(lib, "XInput.lib")

namespace isle
{
namespace Input
{
Gamepad::Gamepad()
    : num_(0)
{
}

Gamepad::~Gamepad(){};
};
};