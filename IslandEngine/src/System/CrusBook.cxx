/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: logging routines.
****
********************************************************************************************************************************/
#include <iostream>
#include <array>
#include <string_view>

//using namespace std::string_literals;
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

#include "System\CrusIsland.h"
#include "System\CrusTypes.h"
#include "System\CrusBook.h"

#if _CRUS_DEBUG_CONSOLE
#include <UxTheme.h>
#endif

namespace {
#if _CRUS_DEBUG_CONSOLE
std::size_t constexpr kMARKERS_WIDTH = 10;

template<std::size_t A, std::size_t N, std::size_t... I>
std::array<CHAR_INFO, N> constexpr set(std::wstring_view message, std::index_sequence<I...>)
{
    return std::array<CHAR_INFO, N>{ { CHAR_INFO{message[I], A}... }};
}

template<std::size_t A, std::size_t N = kMARKERS_WIDTH>
std::array<CHAR_INFO, N> constexpr set(std::wstring_view message)
{
    return set<A, N>(message, std::make_index_sequence<N>{});
}

std::array<std::array<CHAR_INFO, kMARKERS_WIDTH>, 5> constexpr kMARKERS{{
        set<2>(L"Info     :"sv),
        set<3>(L"Debug    :"sv),
        set<4>(L"Warning  :"sv),
        set<5>(L"Error    :"sv),
        set<6>(L"Fatal    :"sv)
}};

#undef SET
#endif // _CRUS_DEBUG_CONSOLE

std::array<std::string_view, 5> constexpr kSEVERITIES{{
    " Info     :"sv,
    " Debug    :"sv,
    " Warning  :"sv,
    " Error    :"sv,
    " Fatal    :"sv
}};
};

namespace isle {
namespace log {

LogStream::LogStream() : stream_(std::cerr.rdbuf())
{
    std::lock_guard<std::mutex> lock(mutex_);

    file_.open(R"(..\book.log)", std::ios_base::out | std::ios_base::trunc);

    if (!file_.is_open()) {
        std::cerr << "Fatal    : can't create log file." << std::endl;
        ::_exit(EXIT_FAILURE);
    }

    stream_.set_rdbuf(file_.rdbuf());

    if constexpr (kCRUS_DEBUG_CONSOLE)
        InitConsoleWindow();

    stream_ << std::noshowpoint;
    stream_ << crus::names_a::kPROJECT << " at " << crus::names_a::kBUILD_DATE << " (" << crus::names_a::kBUILD_VERSION << ")\n";
    stream_ << "           --------------------------------------------------\n";
}

LogStream::~LogStream()
{
    if (file_.is_open()) {
        file_.flush();
        file_.close();

        stream_.set_rdbuf(std::cerr.rdbuf());
    }

    if constexpr (kCRUS_DEBUG_CONSOLE) {
        conout_.flush();
        conout_.close();
    }

    else {
        // ShowWindow(GetConsoleWindow(), SW_HIDE);
    }
}

#if _CRUS_DEBUG_CONSOLE
void LogStream::InitConsoleWindow()
{
    auto result = AllocConsole();

    conout_.open("conout$", std::ios_base::out | std::ios_base::trunc);

    if (result == 0 || !conout_.is_open()) {
        std::cerr << "Fatal    : can't create console window." << std::endl;
        ::_exit(EXIT_FAILURE);
    }

    CONSOLE_FONT_INFOEX fontInfo = {
        sizeof(CONSOLE_FONT_INFOEX),
        8,
        {8, 16},
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
}
#endif // _CRUS_DEBUG_CONSOLE

void LogStream::BeginLine(eSEVERITY _severity)
{
    auto const index = static_cast<std::underlying_type<eSEVERITY>::type>(_severity);

#if _CRUS_DEBUG_CONSOLE
    auto const hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hConsole, &info);

    SMALL_RECT rcDraw = {
        1,  info.dwCursorPosition.Y,
        kMARKERS_WIDTH, info.dwCursorPosition.Y
    };

    WriteConsoleOutputW(hConsole, std::data(kMARKERS[index]), {kMARKERS_WIDTH, 1}, {0, 0}, &rcDraw);
    SetConsoleCursorPosition(hConsole, {kMARKERS_WIDTH + 2, info.dwCursorPosition.Y});
#endif

    stream_ << kSEVERITIES[index] << ' ';
}

void LogStream::EndLine()
{
    LogStream::inst().stream_ << '\n';

    if constexpr (kCRUS_DEBUG_CONSOLE)
        LogStream::inst().conout_ << std::endl;
}

Book::Book(eSEVERITY _severity) : severity_(_severity)
{
    LogStream::inst().mutex_.lock();

    LogStream::inst().BeginLine(severity_);
}

Book::~Book()
{
    LogStream::inst().EndLine();

    LogStream::inst().mutex_.unlock();

    if (severity_ == eSEVERITY::nFATAL) {
        if constexpr (kCRUS_DEBUG_CONSOLE)
            FlushConsoleInputBuffer(GetStdHandle(STD_OUTPUT_HANDLE));

        auto hMainWnd = FindWindowW(crus::names::kMAIN_WINDOW_CLASS, crus::names::kMAIN_WINDOW_NAME);

        if (hMainWnd != nullptr)
            SendMessage(hMainWnd, WM_CLOSE, 0, 0);

        ::exit(EXIT_FAILURE);
    }
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