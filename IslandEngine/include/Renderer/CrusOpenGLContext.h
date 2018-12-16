/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: OpenGL context routine.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_OPENGLCONTEXT_H        // Include guard "CrusOpenGLContext.h"
#define CRUS_OPENGLCONTEXT_H

#include <stack>
#include <mutex>

#include <string>
using namespace std::string_literals;

#include <string_view>
using namespace std::string_view_literals;

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

#include "System\CrusBook.h"

namespace isle {
class OpenGLContext final {
public:

    template<typename T>
    auto GetProcedureAddress(std::string_view _name)
    {
        auto proc = reinterpret_cast<T *const>(wglGetProcAddress(std::data(_name)));

        if (proc == nullptr
            || proc == reinterpret_cast<void *>(-1) || proc == reinterpret_cast<void *>(+1)
            || proc == reinterpret_cast<void *>(+2) || proc == reinterpret_cast<void *>(+3)) {

            auto hModule = LoadLibraryW(L"OpenGL32.lib");

            if (hModule == nullptr)
                isle::log::Fatal() << "can't load OpenGL32 library."s;

            else proc = reinterpret_cast<decltype(proc)>(GetProcAddress(hModule, std::data(_name)));
        }

        if (proc == nullptr)
            isle::log::Fatal() << "can't get procedure address: "s << _name;

        return proc;
    }

    static OpenGLContext &ThreadLocal();

    static HDC hMainWndDC();

private:
    static std::mutex mutex_;

    static HDC hMainWndDC_;
    static HGLRC hMainRC_;

    static std::stack<HGLRC> hSharedRCs_;

    HGLRC hRC_;

    explicit OpenGLContext();
    ~OpenGLContext();

    void SetupContext();
    void DeleteContext();
};

inline HDC OpenGLContext::hMainWndDC()
{
    return hMainWndDC_;
}
}

#endif // CRUS_OPENGLCONTEXT_H