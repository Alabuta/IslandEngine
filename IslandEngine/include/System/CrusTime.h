/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: engine's time routines file.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_TIME_H                 // Include guard "CrusTime.h"
#define CRUS_TIME_H


#include <chrono>
#include "System\CrusTypes.h"

namespace isle {

// A function execution duration measurement.
template<typename TimeT = std::chrono::milliseconds>
struct measure {
    template<typename F, typename ...Args>
    static auto execution(F func, Args &&... args)
    {
        auto start = std::chrono::system_clock::now();

        func(std::forward<Args>(args)...);

        auto duration = std::chrono::duration_cast<TimeT>(std::chrono::system_clock::now() - start);

        return duration.count();
    }
};

class Time {
public:

    void Restart();
    void Update();

    float delta() const { return delta_ * 0.001f; };
    float elapsed() const { return elapsed_ * 0.001f; };

private:

    std::chrono::time_point<std::chrono::system_clock> appStart_, prevFrame_;
    int64 delta_, elapsed_;
};
}

#endif // CRUS_TIME_H