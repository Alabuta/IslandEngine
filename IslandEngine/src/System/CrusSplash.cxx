/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: splash screen are used for hidden application initialization.
****
********************************************************************************************************************************/
#include "System\CrusTypes.h"
#include "System\CrusWindow.h"
#include "System\CrusSplash.h"

__hidden::Splash::Splash(HINSTANCE _hInstance, wcstr _imagePath) : hInstance_(_hInstance)
{
    HANDLE const hBitmap = LoadImageW(_hInstance, _imagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    BITMAP bm;
    GetObjectW(hBitmap, sizeof(BITMAP), &bm);

    RECT rect = {
        0, 0, bm.bmWidth, bm.bmHeight
    };

    WNDCLASSW const wcs = {
        CS_NOCLOSE,
        (WNDPROC)DefWindowProcW,
        0, 0,
        hInstance_,
        LoadIconW(hInstance_, MAKEINTRESOURCEW(0x4000)),
        nullptr,
        nullptr, nullptr,
        L"CrusSplash"
    };

    if (RegisterClassW(&wcs) == 0ui16) return;

    hWnd_ = CreateWindowW(wcs.lpszClassName, L"", WS_POPUP,
                          rect.left, rect.top, rect.right, rect.bottom,
                          nullptr, nullptr, wcs.hInstance, nullptr);

    if (hWnd_ == nullptr)
        this->~Splash();

    if ((hDC_ = GetDC(hWnd_)) == nullptr)
        this->~Splash();

    isle::Window::AdjustToWorkArea(rect);

    SetWindowPos(hWnd_, nullptr, rect.left, rect.top, rect.right, rect.bottom, SWP_DRAWFRAME | SWP_NOZORDER);
    ShowWindow(hWnd_, SW_SHOWNORMAL);

    auto const hSComp = CreateCompatibleDC(hDC_);
    auto const hOldBitmap = SelectObject(hSComp, hBitmap);

    BitBlt(hDC_, 0, 0, rect.right, rect.bottom, hSComp, 0, 0, SRCCOPY);

    SelectObject(hSComp, hOldBitmap);
    DeleteObject(hBitmap);

    DeleteDC(hSComp);
}

__hidden::Splash::~Splash()
{
    if (hDC_ != nullptr && hWnd_ != nullptr) {
        ReleaseDC(hWnd_, hDC_);
        hDC_ = nullptr;
    }

    if (hWnd_ != nullptr) {
        DestroyWindow(hWnd_);
        hWnd_ = nullptr;
    };

    UnregisterClassW(L"CrusSplash", hInstance_);
}