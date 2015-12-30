/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 1st December 2011.
****    Description: basis switches, libraries enabling and preprocessor settings.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_ISLAND_H               // Include guard "CrusIsland.h"
#define CRUS_ISLAND_H

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef  UNICODE
#define  UNICODE
#endif

#if _DEBUG
#define _CRUS_BUG_HUNTING           1   // The debugging, Dr. Watson.
#define _CRUS_MEMORY_CONTROL        1   // Use this to check for memory leaks!
#define _CRUS_DEBUG_CONSOLE         1   // Macros for displaing debug events console.
#endif

// The macro names for commenting out large code blocks.
#define _CRUS_OBSOLETE              0
#define _CRUS_TEMP_DISABLED         0
#define _CRUS_NOT_YET_IMPLEMENTED   0

#ifndef  CRUS_MERGE
#define  CRUS_MERGE(a, b)           _CRUS_MERGE(a, b)
#define _CRUS_MERGE(a, b)           a##b
#endif // CRUS_MERGE

#define  CRUS_WIDEN(str)            CRUS_MERGE(L, str)

#define __WDATE__                   CRUS_WIDEN(__DATE__)
#define __WTIME__                   CRUS_WIDEN(__TIME__)

// :COMPILER: nameless struct/union warning.
#pragma warning(disable: 4201)

#if _CRUS_NOT_YET_IMPLEMENTED
namespace __hidden
{
#define _CRUS_BUILD_DATE                    __DATE__ ", " __TIME__
#define _CRUS_BUILD_VERSION                 "0.1.15.1230"
#define _CRUS_PROJECT                       "Crusoe's Island Engine"
#define _CRUS_WINDOW_CLASS                  "CrusWindowClass"
};
#endif

namespace crus
{
// Unicode names.
namespace names {
    wchar_t const *const kBUILD_DATE = __WDATE__ L", " __WTIME__;
    wchar_t const *const kBUILD_VERSION = L"0.1.15.1230";

    wchar_t const *const kPROJECT = L"Crusoe's Island Engine";
    wchar_t const *const kWINDOW_CLASS = L"CrusWindowClass";
};

// ASCII names.
namespace names_a {
    char const *const kBUILD_DATE = __DATE__ ", " __TIME__;
    char const *const kBUILD_VERSION = "0.1.15.1230";

    char const *const kPROJECT = "Crusoe's Island Engine";
};
};

namespace isle
{
}

namespace app
{
using namespace isle;

void Init();
void Update();
void DrawFrame();
}

namespace __hidden
{
template<size_t x>  struct StaticAssertStruct{};

template<bool x>    struct StaticAssertionFailure;
template<>          struct StaticAssertionFailure<true> {enum {value = 1};};
};

#define CRUS_STATIC_ASSERT(x)\
    typedef __hidden::StaticAssertStruct<\
        sizeof(__hidden::StaticAssertionFailure<x>)>\
            CRUS_MERGE(StaticAssertTypedef_, __COUNTER__);

#include <cstdlib>

#if _CRUS_MEMORY_CONTROL
#   define _CRTDBG_MAP_ALLOC
#   include <crtdbg.h>
#endif // _CRUS_MEMORY_CONTROL

// Are used for unit-tests.
#include "xUnitTest\xUnitTest.h"

// The libraries of general dependencies.
#pragma comment(lib, "UxTheme.lib")

// Excluding default C/C++ libraries.
#if _DEBUG
#   pragma comment(linker, "/NODEFAULTLIB:msvcrt.lib")
#   pragma comment(linker, "/NODEFAULTLIB:msvcprt.lib")
#elif NDEBUG
#   pragma comment(linker, "/NODEFAULTLIB:msvcrtd.lib")
#   pragma comment(linker, "/NODEFAULTLIB:msvcprtd.lib")
#endif

// C runtime library without iostream (/ML, /MD)
#pragma comment(linker, "/NODEFAULTLIB:libc.lib")
#pragma comment(linker, "/NODEFAULTLIB:libcd.lib")
#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")
#pragma comment(linker, "/NODEFAULTLIB:libcmtd.lib")

// Standard C++ library (/ML, /MD)
#pragma comment(linker, "/NODEFAULTLIB:libcp.lib")
#pragma comment(linker, "/NODEFAULTLIB:libcpd.lib")
#pragma comment(linker, "/NODEFAULTLIB:libcpmt.lib")
#pragma comment(linker, "/NODEFAULTLIB:libcpmtd.lib")

// Old iostream library (/ML, /MD, /MDd)
#pragma comment(linker, "/NODEFAULTLIB:libci.lib")
#pragma comment(linker, "/NODEFAULTLIB:libcid.lib")
#pragma comment(linker, "/NODEFAULTLIB:libcimt.lib")
#pragma comment(linker, "/NODEFAULTLIB:libcimtd.lib")
#pragma comment(linker, "/NODEFAULTLIB:msvcirt.lib")
#pragma comment(linker, "/NODEFAULTLIB:msvcirtd.lib")

// /clr /clr:pure
#pragma comment(linker, "/NODEFAULTLIB:msvcmrt.lib")
#pragma comment(linker, "/NODEFAULTLIB:msvcmrtd.lib")
#pragma comment(linker, "/NODEFAULTLIB:msvcurt.lib")
#pragma comment(linker, "/NODEFAULTLIB:msvcurtd.lib")

// :COMPILER: specify a structure without a declarator as members.
#pragma warning(disable: 4201)

// For using in UI Vista-like buttons, edit boxes and etc.
#if UNICODE
#   if defined _M_X64
#       pragma comment(linker, "/manifestdependency:\"type='win32' \
                                name='Microsoft.Windows.Common-Controls' \
                                version='6.0.0.0' processorArchitecture='amd64' \
                                publicKeyToken='6595b64144ccf1df' \
                                language='*'\"")
#   elif defined _M_IA64
#       pragma comment(linker, "/manifestdependency:\"type='win32' \
                                name='Microsoft.Windows.Common-Controls' \
                                version='6.0.0.0' processorArchitecture='ia64' \
                                publicKeyToken='6595b64144ccf1df' \
                                language='*'\"")
#   elif defined _M_IX86
#       pragma comment(linker, "/manifestdependency:\"type='win32' \
                                name='Microsoft.Windows.Common-Controls' \
                                version='6.0.0.0' processorArchitecture='x86' \
                                publicKeyToken='6595b64144ccf1df' \
                                language='*'\"")
#   else
#       pragma comment(linker, "/manifestdependency:\"type='win32' \
                                name='Microsoft.Windows.Common-Controls' \
                                version='6.0.0.0' processorArchitecture='*' \
                                publicKeyToken='6595b64144ccf1df' \
                                language='*'\"")
#   endif
#endif

#endif // CRUS_ISLAND_H