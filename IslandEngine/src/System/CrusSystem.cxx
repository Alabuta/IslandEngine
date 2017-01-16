/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: implementation of system's class file.
****
********************************************************************************************************************************/
#include <cstdio>
#include <cstdlib>

#include "System\CrusSystem.h"
#include "System\CrusWindow.h"

#include "System\CrusInput.h"

#include "Renderer\CrusRender.h"
#include "Camera\CrusCamera.h"

namespace isle {
System::System() {};
System::~System() {};

Time System::time;

/*static*/ void System::Setup()
{
    // This method for only start of system and used before splash screen creation.
    HWND const hExistWnd = FindWindowW(crus::names::kWINDOW_CLASS, nullptr);

    if (hExistWnd != nullptr) {
        SetForegroundWindow(hExistWnd);
        ShowWindow(hExistWnd, SW_SHOWNORMAL);

        ::_exit(EXIT_SUCCESS);
    }

    ::atexit(System::Destroy);

#if _CRUS_MEMORY_CONTROL
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF |
        _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    //_CrtSetBreakAlloc(84);
#endif

    //Book::Open();
}

/*static*/ void System::Destroy()
{
    Render::inst().DeleteRC();
    Window::inst().Destroy();

    Input::Destroy();

    log::Info() << "application destroyed.";
    //Book::Close();

    //::_fcloseall();
}

/*static*/ int32 System::Loop()
{
    /*if(_appLoop == nullptr)
        log::Fatal() << "application loop functor are void.");*/

    MSG msg;
    time.Restart();

    // :COMPILER: just to shut up the compiler.
#pragma warning(push, 3)
#pragma warning(disable: 4127)
    while (true) {
#pragma warning(pop)
        while (PeekMessageW(&msg, Window::inst().hWnd(), 0, 0, PM_REMOVE | PM_NOYIELD) != 0) {
            if (msg.message == WM_QUIT)
                return static_cast<int32>(msg.wParam);

            //TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (Window::inst().InFocus()) {
            System::Update();
            Render::inst().DrawFrame();
        }

        // :TODO: it will be necessary to understand.
        else WaitMessage();
    }

    return 0;
}

/*static*/ void System::Update()
{
    time.Update();

    //Camera::inst().Update();
    Input::Release();

    app::Update();

    //Render::inst().Update();
}
};