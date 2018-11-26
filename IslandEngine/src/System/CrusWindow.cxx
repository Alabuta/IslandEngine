/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: main application window implementation file.
****
********************************************************************************************************************************/
#include "System\CrusSystem.h"
#include "System\CrusWindow.h"
#include "System\CrusSplash.h"

#include "Renderer\CrusRender.h"

#include <DwmAPI.h>
#pragma comment(lib, "DwmAPI.lib")

namespace isle {
/*static*/ HWND Window::hMainWnd{nullptr};
/*static*/ std::unordered_map<HWND, Window *> Window::windowsTable;

Window::Window(std::wstring const &_name, HINSTANCE _hInstance, int _w, int _h) : bWindowed_(false), bInFocus_(false)
{
    // All of aplication parts and sets.
    System::Setup();

    // Create the splash screen and display it.
    __hidden::Splash splash(_hInstance, LR"(..\..\Crusoe's Splash.bmp)");

    WNDCLASSEXW const wcx = {
        sizeof(WNDCLASSEXW),
        CS_OWNDC,
        (WNDPROC)Window::ProcessAllWindows,
        0, 0,
        _hInstance,
        LoadIconW(_hInstance, MAKEINTRESOURCEW(0x4000)),
        LoadCursorW(_hInstance, IDC_ARROW),
        CreateSolidBrush(RGB(41, 34, 37)), nullptr,
        crus::names::kMAIN_WINDOW_CLASS,
        wcx.hIcon
    };

    if (RegisterClassExW(&wcx) == 0ui16)
        log::Fatal() << "can't register window class.";

    hWnd_ = CreateWindowExW(0, wcx.lpszClassName, _name.c_str(),
                            WS_POPUP | WS_SYSMENU, 0, 0, _w, _h,
                            nullptr, nullptr, _hInstance, nullptr);

    if (hWnd_ == nullptr)
        log::Fatal() << "can't create window handle.";

    if (hMainWnd == nullptr)
        hMainWnd = hWnd_;

    windowsTable.insert_or_assign(hWnd_, this);

    // :TODO: it may be better to move to a separate thread?
    //Input::Setup();

    // :WARNING: "ChooseMode()" location.
    // The location of this function affects to correctness of the window display.
    // Must be either here or after the "ShowWindow(splash.hWnd(), SW_HIDE);" line.
    ChooseMode('w');

    Render::inst().Init();

    application::Init();

#if _CRUS_DEBUG_CONSOLE
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    ShowWindow(splash.hWnd(), SW_HIDE);

    ShowWindow(hWnd_, ('-' == '+' ? SW_MAXIMIZE : SW_SHOW));
#if _CRUS_DEBUG_CONSOLE
    ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL);
#endif

    SetForegroundWindow(hWnd_);
}

void Window::Destroy()
{
    if (hWnd_ != nullptr) {
        /*if (hWnd_ != nullptr)
            ReleaseDC(hWnd_, hDC_);*/

        DestroyWindow(hWnd_);

        EnumChildWindows(hWnd_, [] (HWND hWnd, [[maybe_unused]] LPARAM lParam) mutable
        {
            Window::windowsTable.erase(windowsTable.find(hWnd));

            return TRUE;
        }, 0);

        windowsTable.erase(windowsTable.find(hWnd_));

        hWnd_ = nullptr;

        if (!IsWindowed())
            ChangeDisplaySettingsW(nullptr, 0);
    }

    if (windowsTable.size() == 0)
        if (UnregisterClassW(crus::names::kMAIN_WINDOW_CLASS, GetModuleHandleW(nullptr)) == FALSE)
            log::Error() << "window class still registered.";
}

/*static*/ void Window::AdjustToWorkArea(RECT &_rc)
{
    // Primary desktop work area rectangle.
    RECT rcArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcArea, 0);

    // Adjust rectangle of the window to new desktop rectangle.
    _rc.right = (_rc.right -= _rc.left) > (rcArea.right -= rcArea.left) ? rcArea.right : _rc.right;
    _rc.bottom = (_rc.bottom -= _rc.top) > (rcArea.bottom -= rcArea.top) ? rcArea.bottom : _rc.bottom;

    // The left-top frame corner of the window.
    if (_rc.left == 0) _rc.left = ((rcArea.right - _rc.right) >> 1) + rcArea.left;
    if (_rc.top == 0) _rc.top = ((rcArea.bottom - _rc.bottom) >> 1) + rcArea.top;
}

/*static*/ Window &Window::main()
{
    //static Window inst;
    return *windowsTable[hMainWnd];
}

void Window::ChooseMode(char _mode)
{
    RECT rc;

    if (_mode == 'w') {
        GetWindowRect(hWnd_, &rc);
        Window::AdjustToWorkArea(rc);

        SetWindowLongPtrW(hWnd_, GWL_EXSTYLE, WS_EX_WINDOWEDGE);
        SetWindowLongPtrW(hWnd_, GWL_STYLE, WS_TILEDWINDOW /*^ (WS_THICKFRAME | WS_CAPTION)*/);

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

    u32 const flags = SWP_DRAWFRAME | SWP_NOZORDER;
    SetWindowPos(hWnd_, nullptr, rc.left, rc.top, rc.right, rc.bottom, flags);
}

void Window::ToggleFullScreen()
{
    static RECT rc = {0, 0, 7680, 4320};
    static bool bZoomed = IsZoomed();

    if (IsWindowed()) {
        bZoomed = IsZoomed();

        if (bZoomed)							// If the window zoomed - restore,
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

    if (ChangeDisplaySettingsW(&devmode, 0) != DISP_CHANGE_SUCCESSFUL)
        log::Error() << "can't change display mode.";
}

LRESULT Window::Process(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
    __assume(_msg < WM_APP + 1);

    switch (_msg) {
        case WM_ERASEBKGND:
            return 1L;

        case WM_NCPAINT:
            if (!IsWindowed())
                return 0L;
            break;

        case WM_PAINT:
            ValidateRect(_hWnd, nullptr);
            return 0L;

        case WM_INPUT:
            inputProcessCallback_(_wParam, _lParam);
            DefWindowProcW(_hWnd, _msg, _wParam, _lParam);
            return 0L;

        case WM_INPUT_DEVICE_CHANGE:
            //Input::DeviceChanged(_wParam, _lParam);
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
            bInFocus_ = true;
            return 0L;

        case WM_KILLFOCUS:
            bInFocus_ = false;
            return 0L;

        case WM_ACTIVATE:
            if (IsWindowed())
                break;

            if (LOWORD(_wParam) == WA_INACTIVE) {
                ShowWindow(_hWnd, SW_SHOWMINNOACTIVE);
                ChangeDisplaySettingsW(nullptr, 0);
            }

            else Window::ChangeDisplayMode();
            return 0L;

        case WM_KEYDOWN:
            if (_wParam == VK_F11)
                ToggleFullScreen();

            if (_wParam == VK_ESCAPE)
                Window::Destroy();

            //else keyback(_wParam);
            return 0L;

        case WM_MOVE:
            /*WINDOWINFO info;
            GetWindowInfo(hWnd(), &info);
            info.rcClient.left += 5;
            info.rcClient.top += 5;
            info.rcClient.right -= 5;
            info.rcClient.bottom -= 5;

            ClipCursor(&info.rcClient);*/
            return 0L;

        case WM_SIZE:
            resizeCallback_(LOWORD(_lParam), HIWORD(_lParam));
            return 0L;

        case WM_CLOSE:
            Window::Destroy();
            return 0L;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0L;
    }

    return DefWindowProcW(_hWnd, _msg, _wParam, _lParam);
}

/*static*/ LRESULT CALLBACK Window::ProcessAllWindows(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
    auto window = windowsTable[_hWnd];

    if (window != nullptr)
        return window->Process(_hWnd, _msg, _wParam, _lParam);

    else
        return DefWindowProcW(_hWnd, _msg, _wParam, _lParam);
}
};