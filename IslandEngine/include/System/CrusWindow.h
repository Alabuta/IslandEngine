/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 10th March 2010.
****    Description: main application window header file.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_WINDOW_H               // Include guard "CrusWindow.h"
#define CRUS_WINDOW_H

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef  UNICODE
#define  UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "System\CrusTypes.h"

namespace isle {
class Window {
public:

    // Args: mode, width and height in pixels.
    void Create(HINSTANCE hInstance, char const mode[], int16 w, int16 h);
    void Destroy();

    // Adjust to the sizes of the desktop rectangle.
    static void AdjustToWorkArea(RECT &rc);

    HWND hWnd() const;

    bool InFocus() const;
    bool IsZoomed() const;
    bool IsWindowed() const;

    static Window &inst();

private:
    HWND hWnd_{nullptr};

    // :COMPILER: nameless struct/union warning.
#pragma warning(push, 3)
#pragma warning(disable: 4201)
    struct {
        bool bWindowed_ : 1;
        bool bInFocus_ : 1;
        bool _reserved_ : 6;
    };
#pragma warning(pop)

    Window();
    ~Window() = default;

    void ChooseMode(char mode);

    void ToggleFullScreen();
    static void ChangeDisplayMode();

    static LRESULT CALLBACK Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

inline HWND Window::hWnd() const
{
    return hWnd_;
}

inline bool Window::InFocus() const
{
    return bInFocus_;
}

inline bool Window::IsZoomed() const
{
    return ::IsZoomed(hWnd_) == TRUE;
}

inline bool Window::IsWindowed() const
{
    return bWindowed_;
}
};

#endif // CRUS_WINDOW_H