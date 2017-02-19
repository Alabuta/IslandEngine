/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: OpenGL extensions declarations.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_EXTS_H                 // Include guard "CrusExts.h"
#define CRUS_EXTS_H

/*#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B*/

#define GL_GLEXT_PROTOTYPES 1
#define WGL_WGLEXT_PROTOTYPES 1
#define CRUS_USE_GL_EXTENSIONS 0

#include <GL\GLcoreARB.h>
//#include <GL\GLext.h>
#include <GL\wGLext.h>

#include <stack>
#include <mutex>

#include "System\CrusBook.h"

class OpenGLContext final {
public:

    template<typename T>
    auto GetProcedureAddress(acstr _name)
    {
        auto proc = reinterpret_cast<T *const>(wglGetProcAddress(_name));

        if (proc == nullptr
            || proc == reinterpret_cast<void *>(-1) || proc == reinterpret_cast<void *>(+1)
            || proc == reinterpret_cast<void *>(+2) || proc == reinterpret_cast<void *>(+3)) {

            auto hModule = LoadLibraryW(L"OpenGL32.lib");

            if (hModule == nullptr)
                isle::log::Fatal() << "can't load OpenGL32 library.";

            else
                proc = reinterpret_cast<decltype(proc)>(GetProcAddress(hModule, _name));
        }

        if (proc == nullptr)
            isle::log::Fatal() << "can't get procedure address: " << _name;

        return proc;
    }

    static OpenGLContext &ThreadLocal();

    static HDC hMainWndDC();

private:
    static std::mutex mutex_;

    static HDC hMainWndDC_;
    static HGLRC hMainRC_;

    static std::stack<HGLRC> hSharedRCs_;

    explicit OpenGLContext();
    ~OpenGLContext();

    void SetupContext();
    void DeleteContext();
};

inline HDC OpenGLContext::hMainWndDC()
{
    return hMainWndDC_;
}

#endif // CRUS_EXTS_H