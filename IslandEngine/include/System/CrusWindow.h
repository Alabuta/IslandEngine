/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: main application window header file.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_WINDOW_H               // Include guard "CrusWindow.h"
#define CRUS_WINDOW_H

#include <unordered_map>

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

    explicit Window(std::wstring const &name, HINSTANCE hInstance, int w, int h);
    void Destroy();

    // Adjust to the sizes of the desktop rectangle.
    static void AdjustToWorkArea(RECT &rc);

    HWND hWnd() const;

    bool InFocus() const;
    bool IsZoomed() const;
    bool IsWindowed() const;

    static Window &main();

    static Window &GetWindowFromHandle(HWND const &hWnd);

private:
    static HWND hMainWnd;
    static std::unordered_map<HWND, Window *> windowsTable;

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

    Window() = delete;
    Window(Window const &) = delete;
    Window(Window &&) = delete;

    void ChooseMode(char mode);

    void ToggleFullScreen();
    static void ChangeDisplayMode();

    LRESULT Process(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK ProcessAllWindows(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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

inline Window &Window::GetWindowFromHandle(HWND const &_hWnd)
{
    return *windowsTable[_hWnd];
}
};

#endif // CRUS_WINDOW_H