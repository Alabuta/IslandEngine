/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2014 Crusoe's Island LLC.
****
****    Started at 30th November 2009.
****    Description: settings, such as application system, I/O system and etc.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_SYSTEM_H               // Include guard "CrusSystem.h"
#define CRUS_SYSTEM_H

#include "System\CrusIsland.h"
#include "System\CrusTypes.h"
#include "System\CrusBook.h"
#include "System\CrusTime.h"

namespace isle
{
class System {
public:
    static Time time;

    static void Setup();
    static void Destroy();

    static int32 Loop();

protected:

    System();
    ~System();

    static void Update();
};
};

#endif // CRUS_SYSTEM_H