/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 10th March 2010.
****    Description: main application window implementation file.
****
****************************************************************************************/
#include <cstdio>

#include "System\CrusSystem.h"
#include "System\CrusWindow.h"
#include "System\CrusSplash.h"

#include "System\CrusInput.h"
void keyback(WPARAM wParam);

#include "Renderer\CrusRenderer.h"

#include <WindowsX.h>
#include <CommCtrl.h>

#include <VSsym32.h>
#include <DwmAPI.h>

#pragma comment(lib, "DwmAPI.lib")

namespace isle
{
Window::Window() : bWindowed_{false}, bInFocus_{false} {};
Window::~Window() {};

void Window::Create(char const _mode[], int16 _w, int16 _h)
{
    // All of aplication parts and sets.
    System::Setup();

    // Create the splash screen and display it.
    __hidden::CSplash splash(L"..\\..\\Crusoe's Splash.bmp");

    WNDCLASSEXW const wcx = {
        sizeof(WNDCLASSEXW),
        0,
        (WNDPROC)Window::Proc,
        0, 0,
        GetModuleHandleW(nullptr),
        LoadIconW(wcx.hInstance, MAKEINTRESOURCEW(0x4000)),
        LoadCursorW(nullptr, IDC_ARROW),
        CreateSolidBrush(RGB(41, 34, 37)), nullptr,
        crus::names::kWINDOW_CLASS,
        wcx.hIcon
    };

    if(RegisterClassExW(&wcx) == 0ui16)
        Book::AddEvent(NOTE::nALERT, "can't register window class.");

    hWnd_ = CreateWindowExW(0, wcx.lpszClassName, crus::names::kPROJECT,
                            WS_POPUP | WS_SYSMENU, 0, 0, _w, _h,
                            nullptr, nullptr, wcx.hInstance, nullptr);

    if(hWnd_ == nullptr)
        Book::AddEvent(NOTE::nALERT, "can't create window handle.");
    Book::AddEvent(NOTE::nNOTICE, "main window created.");

    // :TODO: it may be better to move to a separate thread?
    Input::Setup();

    // :WARNING: "ChooseMode()" location.
    // The location of this function affects the correctness of the display window.
    // Must be either here or after the "ShowWindow(splash.hWnd(), SW_HIDE);" line.
    ChooseMode(_mode[0]);

    Renderer::inst().SetupContext();

    app::Init();

    Sleep(1000);

    Book::AddEvent(NOTE::nSEPAR);
    Book::AddEvent(NOTE::nNOTICE, "open sesame, here we go!");

#if _CRUS_DEBUG_CONSOLE
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    ShowWindow(splash.hWnd(), SW_HIDE);

    ShowWindow(hWnd_, (_mode[1] == '+' ? SW_MAXIMIZE : SW_SHOW));
#if _CRUS_DEBUG_CONSOLE
    ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL);
#endif

    SetForegroundWindow(hWnd_);
}

void Window::Destroy()
{
    if(hWnd_ != nullptr){
#ifndef _CRUS_DEBUG_CONSOLE
        //AllocConsole();
        ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
        DestroyWindow(hWnd_);
        hWnd_ = nullptr;

		if(!Window::inst().IsWindowed())
			ChangeDisplaySettingsW(nullptr, 0);
    }

    if(UnregisterClassW(crus::names::kWINDOW_CLASS, GetModuleHandleW(nullptr)) == FALSE)
        Book::AddEvent(NOTE::nERROR, "window class still registered.");

    Book::AddEvent(NOTE::nNOTICE, "window destroyed.");
}

/*static*/ void Window::AdjustToWorkArea(RECT &_rc)
{
#if _CRUS_OBSOLETE
    // Shell tray rectangle.
    RECT rcTray;
    GetWindowRect(FindWindowW(L"Shell_TrayWnd", nullptr), &rcTray);

    // Desktop window rectangle.
    RECT rcDesk;
    GetWindowRect(GetDesktopWindow(), &rcDesk);

    // Get the rectangle of the desktop of free from the shell tray.
    rcDesk.right  -= (rcTray.right -= rcTray.left) < rcDesk.right  ? rcTray.right  : 0;
    rcDesk.bottom -= (rcTray.bottom -= rcTray.top) < rcDesk.bottom ? rcTray.bottom : 0;

    // Cause shell tray may be in top, bottom, right or left side,
    // application will adjust window rect.
    if(rcTray.bottom > rcTray.right)
        rcDesk.left = rcTray.left == 0 ? rcTray.right : 0;      // Right or left.
    else rcDesk.top = rcTray.top == 0 ? rcTray.bottom : 0;      // Top or bottom.

    // Adjust rectangle of the window to new desktop rectangle.
    _rc.right  = (_rc.right -= _rc.left) > rcDesk.right  ? rcDesk.right  : _rc.right;
    _rc.bottom = (_rc.bottom -= _rc.top) > rcDesk.bottom ? rcDesk.bottom : _rc.bottom;

    // The left-top frame corner of the window.
    if(_rc.left == 0) _rc.left = ((rcDesk.right - _rc.right) >> 1) + rcDesk.left;
    if(_rc.top == 0) _rc.top = ((rcDesk.bottom - _rc.bottom) >> 1) + rcDesk.top;
#endif

    // Primary desktop work area rectangle.
    RECT rcArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcArea, 0);

    // Adjust rectangle of the window to new desktop rectangle.
    _rc.right  = (_rc.right -= _rc.left) > (rcArea.right -= rcArea.left) ? rcArea.right : _rc.right;
    _rc.bottom = (_rc.bottom -= _rc.top) > (rcArea.bottom -= rcArea.top) ? rcArea.bottom : _rc.bottom;

    // The left-top frame corner of the window.
    if (_rc.left == 0) _rc.left = ((rcArea.right - _rc.right) >> 1) + rcArea.left;
    if (_rc.top == 0) _rc.top = ((rcArea.bottom - _rc.bottom) >> 1) + rcArea.top;
}

/*static*/ __declspec(noinline) Window &Window::inst()
{
    static Window inst;
    return inst;
}

void Window::ChooseMode(char _mode)
{
    RECT rc;

    if(_mode == 'w'){
        GetWindowRect(hWnd_, &rc);
        Window::AdjustToWorkArea(rc);

		SetWindowLongPtrW(hWnd_, GWL_EXSTYLE, WS_EX_WINDOWEDGE);
		SetWindowLongPtrW(hWnd_, GWL_STYLE, WS_TILEDWINDOW ^ (WS_THICKFRAME | WS_CAPTION));

        bWindowed_ = true;
    }

    else {
		// For correct displaying window's small icon in caption.
		ShowWindow(hWnd_, SW_HIDE | SW_SHOWNOACTIVATE);

        Window::ChangeDisplayMode();
        GetWindowRect(GetDesktopWindow(), &rc);

		SetWindowLongPtrW(hWnd_, GWL_EXSTYLE, WS_EX_TOPMOST);
		SetWindowLongPtrW(hWnd_, GWL_STYLE, WS_POPUP);

        bWindowed_ = false;
    }

    uint32 const flags = SWP_DRAWFRAME | SWP_NOZORDER;
    SetWindowPos(hWnd_, nullptr, rc.left, rc.top, rc.right, rc.bottom, flags);
}

void Window::ToggleFullScreen()
{
    static RECT rc = {0, 0, 7680, 4320};
    static bool bZoomed = IsZoomed();

    if(IsWindowed()){
		bZoomed = IsZoomed();

        if(bZoomed)								// If the window zoomed - restore,
            ShowWindow(hWnd_, SW_RESTORE);      // for save correct rectangle.

		// For correctness display window's small icon in caption.
		ShowWindow(hWnd_, SW_HIDE | SW_SHOWNOACTIVATE);

        GetWindowRect(hWnd_, &rc);
        ChooseMode('f');
    }

    else {
        ChangeDisplaySettingsW(nullptr, 0);

        MoveWindow(hWnd_, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
        ChooseMode('w');
    }

    ShowWindow(hWnd_, (bZoomed ? SW_MAXIMIZE : SW_SHOW));
}

/*static*/ void Window::ChangeDisplayMode()
{
    // Desktop window rectangle.
    RECT rcDesk;
    GetWindowRect(GetDesktopWindow(), &rcDesk);

    auto const fsWidth = static_cast<DWORD>(GetSystemMetrics(SM_CXSCREEN));
    auto const fsHeight = static_cast<DWORD>(GetSystemMetrics(SM_CYSCREEN));

    // :TODO: use EnumDisplayDevices().
    DEVMODEW devmode = {
        L"", 0, 0,
        sizeof(DEVMODEW),
        0,
        DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY,
        {0}, 0, 0, 0, 0, 0, L"", 0,
        32, fsWidth, fsHeight,
        {0},
        60,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    if(ChangeDisplaySettingsW(&devmode, 0) != DISP_CHANGE_SUCCESSFUL)
        Book::AddEvent(NOTE::nERROR, "can't change display mode.");
}

LRESULT CALLBACK Window::Proc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
    __assume(_msg < WM_APP + 1);

    switch(_msg){
        case WM_ERASEBKGND:
            return 1L;

        case WM_NCPAINT:
            if(!Window::inst().IsWindowed())
                return 0L;
            break;

        case WM_PAINT:
            ValidateRect(_hWnd, nullptr);
            return 0L;

        case WM_INPUT:
            Input::Process(_wParam, _lParam);
            return 0L;

        case WM_INPUT_DEVICE_CHANGE:
            Input::DeviceChanged(_wParam, _lParam);
            return 0L;

        case WM_NCHITTEST:
            {
                LRESULT result = DefWindowProcW(_hWnd, WM_NCHITTEST, 0, _lParam);

                if (DwmDefWindowProc(_hWnd, WM_NCHITTEST, 0, _lParam, &result) == TRUE)
                    return result;

                if (result >= HTLEFT && result <= HTBOTTOMRIGHT)
                    return HTBORDER;
            }

        case WM_SIZING:
            //GetWindowRect(_hWnd, reinterpret_cast<RECT *const>(_lParam));
            return 1L;

        case WM_SETFOCUS:
            Window::inst().bInFocus_ = true;
            return 0L;

        case WM_KILLFOCUS:
            Window::inst().bInFocus_ = false;
            return 0L;

        case WM_ACTIVATE:
            if(Window::inst().IsWindowed())
                break;

            if(LOWORD(_wParam) == WA_INACTIVE){
                ShowWindow(_hWnd, SW_SHOWMINNOACTIVE);
                ChangeDisplaySettingsW(nullptr, 0);
            }

            else Window::ChangeDisplayMode();
            return 0L;

        case WM_KEYDOWN:
            if(_wParam == VK_F11)
                Window::inst().ToggleFullScreen();

            if (_wParam == VK_ESCAPE)
                System::Destroy();
            //else keyback(_wParam);
            return 0L;

        case WM_MOVE:
            /*WINDOWINFO info;
            GetWindowInfo(Window::inst().hWnd(), &info);
            info.rcClient.left += 5;
            info.rcClient.top += 5;
            info.rcClient.right -= 5;
            info.rcClient.bottom -= 5;

            ClipCursor(&info.rcClient);*/
            return 0L;

        case WM_SIZE:
            Renderer::inst().SetViewport(5, 5, LOWORD(_lParam) - 10, HIWORD(_lParam) - 10);
            return 0L;

        case WM_CLOSE:
            System::Destroy();
            return 0L;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0L;
    }

    return DefWindowProcW(_hWnd, _msg, _wParam, _lParam);
}
};