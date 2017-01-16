/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: logging routines.
****
********************************************************************************************************************************/
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <streambuf>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <clocale>
#include <type_traits>

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

#include "System\CrusIsland.h"
#include "System\CrusTypes.h"
#include "System\CrusBook.h"

#if _CRUS_DEBUG_CONSOLE
#include <UxTheme.h>
#endif


isle::log::LogStream isle::log::Book::helper_;

namespace {
#if _CRUS_DEBUG_CONSOLE
#define SET(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t)\
            {{a, b}, {c, d}, {e, f}, {g, h}, {i, j}, {k, l}, {m, n}, {o, p}, {q, r}, {s, t}}

size_t const kMARKERS_WIDTH = 10;

CHAR_INFO const kMARKERS[][kMARKERS_WIDTH] = {
    SET('I', 2, 'n', 2, 'f', 2, 'o', 2, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ':', 1),
    SET('D', 3, 'e', 3, 'b', 3, 'u', 3, 'g', 3, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ':', 1),
    SET('W', 4, 'a', 4, 'r', 4, 'n', 4, 'i', 4, 'n', 4, 'g', 4, ' ', 0, ' ', 0, ':', 1),
    SET('E', 5, 'r', 5, 'r', 5, 'o', 5, 'r', 5, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ':', 1),
    SET('F', 6, 'a', 6, 't', 6, 'a', 6, 'l', 6, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ':', 1)
};

#undef SET
#endif // _CRUS_DEBUG_CONSOLE
};

namespace isle {
namespace log {

LogStream::LogStream() : stream_(std::cerr.rdbuf())
{
    file_.open("..\\book.log", std::ios_base::out | std::ios_base::trunc);

    if (!file_.is_open()) {
        std::cerr << "Fatal    : can't create log file." << std::endl;
        ::_exit(EXIT_FAILURE);
    }

#if _CRUS_DEBUG_CONSOLE
    stream_.set_rdbuf(conout_.rdbuf());
    InitConsoleWindow();
#else
    stream_.set_rdbuf(file_.rdbuf());
#endif

    stream_ << std::noshowpoint;
    stream_ << crus::names_a::kPROJECT << " at " << crus::names_a::kBUILD_DATE << " (" << crus::names_a::kBUILD_VERSION << ")\n";
    stream_ << "           --------------------------------------------------" << std::endl;
}

LogStream::~LogStream()
{
#if _CRUS_DEBUG_CONSOLE
    if (conout_.is_open()) {
        conout_.flush();
        conout_.close();

        stream_.set_rdbuf(std::cerr.rdbuf());
    }
#else
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

    if (file_.is_open()) {
        file_.flush();
        file_.close();

        stream_.set_rdbuf(std::cerr.rdbuf());
    }
}

void LogStream::InitConsoleWindow()
{
#if _CRUS_DEBUG_CONSOLE
    auto result = AllocConsole();

    conout_.open("conout$", std::ios_base::out | std::ios_base::trunc);

    if (result == 0 || !conout_.is_open()) {
        std::cerr << "Fatal    : can't create console window." << std::endl;
        ::_exit(EXIT_FAILURE);
    }

    CONSOLE_FONT_INFOEX fontInfo = {
        sizeof(CONSOLE_FONT_INFOEX),
        9,
        {8, 18},
        54,
        FW_NORMAL,
        L"Consolas"
    };

    CONSOLE_SCREEN_BUFFER_INFOEX consoleInfo = {
        sizeof(CONSOLE_SCREEN_BUFFER_INFOEX),
        {80, 1024},
        {8, 8},
        15,
        {0, 0, 0, 0},
        {80, 32},
        7,
        FALSE,
        {
            0x004000, 0xFFFFFF,    // Background, font,
            0x368618,              // nINFO, 
            0x88CF8A,              // nDEBUG,
            0x1C1CAC, 0x1C1CAC,    // nWARN, nERROR,
            0x0000FF,              // nFATAL,
            0xFFFFFF,
            0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
            0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF
        }
    };

    auto const hConsoleWnd = GetConsoleWindow();
    auto const hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    {
        WTA_OPTIONS wta = {
            WTNCA_NODRAWICON,
            WTNCA_NODRAWICON
        };

        SetWindowLongPtrW(hConsoleWnd, GWL_STYLE, WS_TILEDWINDOW ^ WS_MAXIMIZEBOX);
        SetWindowThemeAttribute(hConsoleWnd, WTA_NONCLIENT, &wta, sizeof(WTA_OPTIONS));
    }

    SetConsoleScreenBufferInfoEx(hConsole, &consoleInfo);
    SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);

    // Primary desktop work area rectangle.
    RECT rcWorkArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

    MoveWindow(hConsoleWnd, rcWorkArea.left, rcWorkArea.top, 736, rcWorkArea.bottom - rcWorkArea.top, TRUE);

    std::wostringstream consoleWindowTitle;
    consoleWindowTitle << crus::names::kPROJECT << L" at " << crus::names::kBUILD_DATE << L" (" << crus::names::kBUILD_VERSION << L")\n";

    SetWindowTextW(hConsoleWnd, consoleWindowTitle.str().c_str());
#endif // _CRUS_DEBUG_CONSOLE
}

void LogStream::WriteToConsole(eSEVERITY _note) const
{
    auto const note = static_cast<std::underlying_type<eSEVERITY>::type>(_note);

#if _CRUS_DEBUG_CONSOLE
    HANDLE const hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hConsole, &info);


    SMALL_RECT rcDraw = {
        1,  info.dwCursorPosition.Y,
        kMARKERS_WIDTH, info.dwCursorPosition.Y
    };

    WriteConsoleOutputW(hConsole, kMARKERS[note], {kMARKERS_WIDTH, 1}, {0, 0}, &rcDraw);

    SetConsoleCursorPosition(hConsole, {kMARKERS_WIDTH + 2, info.dwCursorPosition.Y});
#endif

    if (_note == eSEVERITY::nFATAL) {
#if _CRUS_DEBUG_CONSOLE
        FlushConsoleInputBuffer(hConsole);
#endif
        ::exit(EXIT_FAILURE);
    }
}

Book::Book(eSEVERITY _severity)
{
#if _CRUS_DEBUG_CONSOLE
    helper_.WriteToConsole(_severity);
#else
    helper_.stream_ << helper_.kSEVERITIES[static_cast<std::underlying_type<eSEVERITY>::type>(_severity)];
#endif
}

Book::~Book()
{
#if _CRUS_DEBUG_CONSOLE
    helper_.stream_ << std::endl;
#else
    helper_.stream_ << '\n';
#endif
}

/*__declspec(noinline)*/ Book Info()
{
    return Book(eSEVERITY::nINFO);
}

/*__declspec(noinline)*/ Book Debug()
{
    return Book(eSEVERITY::nDEBUG);
}

/*__declspec(noinline)*/ Book Warning()
{
    return Book(eSEVERITY::nWARNING);
}

/*__declspec(noinline)*/ Book Error()
{
    return Book(eSEVERITY::nERROR);
}

/*__declspec(noinline)*/ Book Fatal()
{
    return Book(eSEVERITY::nFATAL);
}
};
};