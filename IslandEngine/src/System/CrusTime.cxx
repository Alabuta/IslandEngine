/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2014 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****	Description: engine's time routines file.
****
****************************************************************************************/
#include "System\CrusTime.h"

namespace isle
{
Time::Time()
{
    Restart();
}

void Time::Restart()
{
    delta_ = 0.0f;
    frames_ = 1;
    counter_ = 1;

    DWORD_PTR oldMask = SetThreadAffinityMask(GetCurrentThread(), 0);

    QueryPerformanceFrequency(&freq_);
    QueryPerformanceCounter(&time_[1]);
    QueryPerformanceCounter(&time_[2]);

    SetThreadAffinityMask(GetCurrentThread(), oldMask);

    memset(buffer, 0, sizeof(buffer));
}

void Time::Update()
{
    DWORD_PTR oldMask = SetThreadAffinityMask(GetCurrentThread(), 0);

    QueryPerformanceCounter(&time_[0]);

    SetThreadAffinityMask(GetCurrentThread(), oldMask);

    static float elapsed = 0.0f;
    elapsed = (time_[0].QuadPart - time_[1].QuadPart) / float(freq_.QuadPart);

    time_[1].QuadPart = time_[0].QuadPart;

    if (fabsf(elapsed - delta_) < 1.0f) {
        memmove(&buffer[1], buffer, sizeof(float) * (kBufferSize - 1));
        buffer[0] = elapsed;
        if (counter_ < kBufferSize) ++counter_;
    } delta_ = 0.0f;

    for (uint8 i = 0; i < counter_; ++i) delta_ += buffer[i];
    delta_ /= counter_;

    elapsed_ = (time_[0].QuadPart - time_[2].QuadPart) / float(freq_.QuadPart);
}
}