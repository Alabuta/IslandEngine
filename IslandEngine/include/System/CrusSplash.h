/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 23th July 2009.
****    Description: splash screen are used for hidden application initialization.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_SPLASH_H               // Include guard "CrusSplash.h"
#define CRUS_SPLASH_H

namespace __hidden
{
class CSplash {
public:

    CSplash(HINSTANCE hInstance, wcstr);
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