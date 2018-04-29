/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: settings, such as application system, I/O system and etc.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_SYSTEM_H               // Include guard "CrusSystem.h"
#define CRUS_SYSTEM_H

#include "System\CrusIsland.h"
#include "System\CrusTypes.h"
#include "System\CrusBook.h"
#include "System\CrusTime.h"

namespace isle {
class System {
public:
    static Time time;

    static void Setup();
    static void Destroy();

    static i32 Loop();

protected:

    System() = default;
    ~System() = default;

    static void Update();
};
};

#endif // CRUS_SYSTEM_H