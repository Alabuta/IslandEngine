/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: splash screen are used for hidden application initialization.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_SPLASH_H               // Include guard "CrusSplash.h"
#define CRUS_SPLASH_H

namespace __hidden {
class Splash {
public:

    Splash(HINSTANCE hInstance, wcstr);
    ~Splash();

    HWND hWnd() const;
    HDC hDC() const;

private:
    HWND hWnd_;
    HDC  hDC_;
    HINSTANCE hInstance_;
};

inline HWND Splash::hWnd() const
{
    return hWnd_;
}

inline HDC Splash::hDC() const
{
    return hDC_;
}
};

#endif // CRUS_SPLASH_H