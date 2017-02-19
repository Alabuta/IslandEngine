/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: engine's time routines file.
****
********************************************************************************************************************************/
#include "System\CrusTime.h"

namespace isle {

void Time::Restart()
{
    appStart_ = std::chrono::system_clock::now();
    prevFrame_ = std::chrono::system_clock::now();

    elapsed_ = 0LL;
    delta_ = 0LL;
}

void Time::Update()
{
    elapsed_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - appStart_).count();

    delta_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - prevFrame_).count();
    prevFrame_ = std::chrono::system_clock::now();
}
}