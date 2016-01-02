/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 16th September 2012.
****    Description: players routines.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_PLAYER_H               // Include guard "CrusPlayer.h"
#define CRUS_PLAYER_H

#include "System\CrusInput.h"

namespace isle
{
namespace intf
{
class Player {
public:
    Input::Device *controller_;

    Player() : controller_(nullptr) {};
    ~Player() {};

private:
    ;

};
};
};

#endif // CRUS_PLAYER_H