/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 4th June 2010.
****    Description: book log.
****
********************************************************************************************************************************/
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

namespace
{
acstr kNOTES[11] = {
    "            --------------------------------------------------\n",
    "            %s\n",
    "          + %s\n",        // - These notes will be displayed after
    " Info     : %s\n",        //   the log buffer overflows,
    " Notice   : %s\n",        //   or after a call to "CBook::Flush()".
    " Debug    : %s\n",        // - Debug event are always displayed.
    " Warning  : %s\n",        // - These kNOTES are used to notification
    " Error    : %s\n",        //   about an error without application break.
    " Alert    : %s\n",        // - After these kNOTES further
    " Critical : %s\n",        //   work of application
    " Emerg    : %s\n"         //   is impossible.
};

#if _CRUS_DEBUG_CONSOLE
#define SET(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t)\
            {{a, b}, {c, d}, {e, f}, {g, h}, {i, j}, {k, l}, {m, n}, {o, p}, {q, r}, {s, t}}

CHAR_INFO const kMARKERS[11][10] = {
    SET(' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, '+', 2),
    SET('I', 4, 'n', 4, 'f', 4, 'o', 4, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ':', 2),
    SET('N', 5, 'o', 5, 't', 5, 'i', 5, 'c', 5, 'e', 5, ' ', 0, ' ', 0, ' ', 0, ':', 2),
    SET('D', 6, 'e', 6, 'b', 6, 'u', 6, 'g', 6, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ':', 2),
    SET('W', 8, 'a', 8, 'r', 8, 'n', 8, 'i', 8, 'n', 8, 'g', 8, ' ', 0, ' ', 0, ':', 2),
    SET('E', 9, 'r', 9, 'r', 9, 'o', 9, 'r', 9, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ':', 2),
    SET('A', 13, 'l', 13, 'e', 13, 'r', 13, 't', 13, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ':', 2),
    SET('C', 12, 'r', 12, 'i', 12, 't', 12, 'i', 12, 'c', 12, 'a', 12, 'l', 12, ' ', 0, ':', 2),
    SET('E', 14, 'm', 14, 'e', 14, 'r', 14, 'g', 14, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ':', 2)
};

#undef SET
#endif

class CBook {
private:
    FILE *sheets_;
    char  line_[1024];

    _locale_t locale_;

    CBook(){};
    ~CBook(){};

    static void DoAdjustConsole();

public:

    void Open();
    void Close();

    void AddEvent(isle::eNOTE note, acstr str, va_list ap);
    void NoteTime(isle::eNOTE note, acstr str);

    static CBook &book();
};

/*static*/ __forceinline CBook &CBook::book()
{
    static CBook book;
    return book;
}

__forceinline void CBook::AddEvent(isle::eNOTE _note, acstr _str, va_list _ap)
{
    if(_str == nullptr || sheets_ == nullptr) return;

    using utype = std::underlying_type<isle::eNOTE>::type;
    auto const note = static_cast<utype>(_note);

    _vsprintf_s_l(line_, sizeof(line_), _str, locale_, _ap);
    _fprintf_s_l(sheets_, kNOTES[note], locale_, line_);

#if _CRUS_DEBUG_CONSOLE
    HANDLE const hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hConsole, &info);

    _printf_s_l(kNOTES[note], locale_, line_);

    if(_note > isle::eNOTE::nEMPTY){
        COORD const pos = {0, 0};
        COORD const rect = {10, 1};

        SMALL_RECT rcDraw = {
            1,  info.dwCursorPosition.Y,
            10, info.dwCursorPosition.Y
        };

        WriteConsoleOutputW(hConsole, kMARKERS[note - 2], rect, pos, &rcDraw);
    }
#endif

    if(_note > isle::eNOTE::nERROR){
#if _CRUS_DEBUG_CONSOLE
        FlushConsoleInputBuffer(hConsole);
#endif
        ::exit(EXIT_FAILURE);
    }
}

/*static*/ void CBook::DoAdjustConsole()
{
#if _CRUS_DEBUG_CONSOLE
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
            0x004000, 0xFFFFFF, 0xABABAB, 0x000000,    // Background, font, eHYPHEN,
            0x368618, 0x368618, 0x88CF8A, 0x000000,    // eINFO, eNOTICE, eDEBUG,
            0x1C1CAC, 0x1C1CAC, 0x000000, 0x000000,    // eWARN, eERROR,
            0x0000FF, 0x0000FF, 0x0000FF, 0xFFFFFF     // eCRITIC, eALERT, eEMERG, font
        }
    };

    HWND const hConsoleWnd = GetConsoleWindow();
    HANDLE const hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

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

#if _CRUS_OBSOLETE
    // Shell tray rectangle.
    RECT rcTray;
    GetWindowRect(FindWindowW(L"Shell_TrayWnd", nullptr), &rcTray);

    // Desktop window rectangle.
    RECT rcDesk;
    GetWindowRect(GetDesktopWindow(), &rcDesk);

    // Get a rectangle of the desktop of free from the shell tray.
    rcDesk.right  -= (rcTray.right -= rcTray.left) < rcDesk.right  ? rcTray.right  : 0;
    rcDesk.bottom -= (rcTray.bottom -= rcTray.top) < rcDesk.bottom ? rcTray.bottom : 0;

    // Cause shell tray may be in top, bottom, right or left side,
    // application will adjust window rect.
    if(rcTray.bottom > rcTray.right)
        rcDesk.left = rcTray.left == 0 ? rcTray.right : 0;      // Right or left.
    else rcDesk.top = rcTray.top == 0 ? rcTray.bottom : 0;      // Top or bottom.
#endif

    // Primary desktop work area rectangle.
    RECT rcArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcArea, 0);

    MoveWindow(hConsoleWnd, rcArea.left, rcArea.top, 736, rcArea.bottom - rcArea.top, TRUE);
#endif // _CRUS_DEBUG_CONSOLE
}

void CBook::Open()
{
#if _CRUS_DEBUG_CONSOLE
    CBook::DoAdjustConsole();
#endif

    locale_ = ::_create_locale(LC_ALL, "English");
    fopen_s(&sheets_, "..\\book.log", "wt");

    if(sheets_ == nullptr || ::ferror(sheets_)){
        perror("Emerg    : can't create log file.\n");
        ::_exit(EXIT_FAILURE);
    }

#if _CRUS_DEBUG_CONSOLE
    {
        wchar_t title[256];
        _swprintf_s_l(title, 256, L"%s at %s (%s)", locale_, 
                      crus::names::kPROJECT, crus::names::kBUILD_DATE,
                      crus::names::kBUILD_VERSION);

        SetConsoleTitleW(title);
    }
#endif

    using utype = std::underlying_type<isle::eNOTE>::type;
    auto const note = static_cast<utype>(isle::eNOTE::nSEPAR);

    _fprintf_s_l(sheets_, "%s at %s (%s).\n%s", locale_,
                 crus::names_a::kPROJECT, crus::names_a::kBUILD_DATE,
                 crus::names_a::kBUILD_VERSION, kNOTES[note]);

    isle::Book::NoteTime(isle::eNOTE::nNOTICE, "started");
}

void CBook::Close()
{
    if(sheets_ == nullptr || ::ferror(sheets_)) return;

    isle::Book::AddEvent(isle::eNOTE::nSEPAR);
    isle::Book::NoteTime(isle::eNOTE::nNOTICE, "shutdown");

    ::fclose(sheets_);
    sheets_ = nullptr;

    if(locale_ ==  nullptr) return;

    ::_free_locale(locale_);
    locale_ =  nullptr;
}
};

namespace isle
{
void Book::Open()
{
    CBook::book().Open();
}

void Book::Close()
{
    CBook::book().Close();
}

void Book::AddEvent(isle::eNOTE _note, acstr _str, ...)
{
    va_list ap;
    va_start(ap, _str);

    CBook::book().AddEvent(_note, _str, ap);
}

void Book::NoteTime(isle::eNOTE _note, acstr _str)
{
    SYSTEMTIME time;
    GetLocalTime(&time);

    Book::AddEvent(_note, "%s at %02d:%02d:%02d.", _str,
                   time.wHour, time.wMinute, time.wSecond);
}
};