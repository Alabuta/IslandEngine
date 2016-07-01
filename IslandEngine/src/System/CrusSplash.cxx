/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 23th July 2009.
****    Description: splash screen are used for hidden application initialization.
****
********************************************************************************************************************************/
#include "System\CrusTypes.h"
#include "System\CrusWindow.h"
#include "System\CrusSplash.h"


namespace __hidden
{
HINSTANCE hInstance;
}

__hidden::CSplash::CSplash(HINSTANCE _hInstance, wcstr _imagePath)
{
    HANDLE const hBitmap = LoadImageW(nullptr, _imagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    BITMAP bm;
    GetObjectW(hBitmap, sizeof(BITMAP), &bm);
    int32 ws[4] = {0, 0, bm.bmWidth, bm.bmHeight};

    hInstance = _hInstance;

    WNDCLASSW const wcs = {
        CS_NOCLOSE,
        (WNDPROC)DefWindowProcW,
        0, 0,
        /*GetModuleHandleW(nullptr)*/hInstance,
        LoadIconW(wcs.hInstance, MAKEINTRESOURCEW(0x4000)),
        nullptr,
        nullptr, nullptr,
        L"CrusSplash"
    };

    if (RegisterClassW(&wcs) == 0ui16) return;

    hWnd_ = CreateWindowW(wcs.lpszClassName, L"", WS_POPUP,
                          ws[0], ws[1], ws[2], ws[3],
                          nullptr, nullptr, wcs.hInstance, nullptr);

    if(hWnd_ == nullptr) this->~CSplash();
    if((hDC_ = GetDC(hWnd_)) == nullptr) this->~CSplash();

    isle::Window::AdjustToWorkArea(*reinterpret_cast<RECT *const>(ws));

    SetWindowPos(hWnd_, nullptr, ws[0], ws[1], ws[2], ws[3], SWP_DRAWFRAME | SWP_NOZORDER);
    ShowWindow(hWnd_, SW_SHOWNORMAL);

    HDC const hSComp = CreateCompatibleDC(hDC_);
    HANDLE const hOldBitmap = SelectObject(hSComp, hBitmap);

    BitBlt(hDC_, 0, 0, ws[2], ws[3], hSComp, 0, 0, SRCCOPY);

    SelectObject(hSComp, hOldBitmap);
    DeleteObject(hBitmap);

    DeleteDC(hSComp);
}

__hidden::CSplash::~CSplash()
{
    if(hDC_ != nullptr && hWnd_ != nullptr){
        ReleaseDC(hWnd_, hDC_);
        hDC_ = nullptr;
    }

    if(hWnd_ != nullptr){
        DestroyWindow(hWnd_);
        hWnd_ = nullptr;
    };

    UnregisterClassW(L"CrusSplash", /*GetModuleHandle(nullptr)*/hInstance);
}