/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    20th April 2012 / 23th July 2009.
****    Description: splash screen are used for hidden application initialization.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_SPLASH_H               // Include guard "CrusSplash.h"
#define CRUS_SPLASH_H

namespace __covert
{
class CSplash {
public:

    CSplash(wcstr);
    ~CSplash();

    HWND hWnd() const;
    HDC hDC() const;

private:
    HWND hWnd_;
    HDC  hDC_;
};

inline HWND CSplash::hWnd() const
{
    return hWnd_;
}

inline HDC CSplash::hDC() const
{
    return hDC_;
}
};

#endif // CRUS_SPLASH_H