/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: basis switches, libraries enabling and preprocessor settings.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_ISLAND_H               // Include guard "CrusIsland.h"
#define CRUS_ISLAND_H

#define CRUS_USE_SSE_MATH 1

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

namespace crus {
// Unicode names.
namespace names {
wchar_t const *const kBUILD_DATE = __WDATE__ L", " __WTIME__;
wchar_t const *const kBUILD_VERSION = L"0.1.17.0116";

wchar_t const *const kPROJECT = L"Crusoe's Island Engine";

wchar_t const *const kMAIN_WINDOW_CLASS = L"CrusWindowClass";
wchar_t const *const kMAIN_WINDOW_NAME = L"Crusoe's Island Engine";
};

// ASCII names.
namespace names_a {
char const *const kBUILD_DATE = __DATE__ ", " __TIME__;
char const *const kBUILD_VERSION = "0.1.17.0116";

char const *const kPROJECT = "Crusoe's Island Engine";

char const *const kMAIN_WINDOW_CLASS = "CrusWindowClass";
char const *const kMAIN_WINDOW_NAME = "Crusoe's Island Engine";
};
};

namespace isle {
}

namespace app {
using namespace isle;

void Init();
void Update();
void DrawFrame();
}

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