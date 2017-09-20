/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: logging interfaces.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_BOOK_H                 // Include guard "CrusBook.h"
#define CRUS_BOOK_H

#if _DEBUG
#define _CRUS_DEBUG_CONSOLE         1   // Macros for displaing debug events console.
#endif

#if _CRUS_DEBUG_CONSOLE
constexpr auto kCRUS_DEBUG_CONSOLE = true;
#else
constexpr auto kCRUS_DEBUG_CONSOLE = false;
#endif

#include <sstream>
#include <fstream>
#include <utility>
#include <mutex>

#include "System\CrusTypes.h"

namespace isle {
namespace log {

class Book;

Book Info();
Book Debug();
Book Warning();
Book Error();
Book Fatal();

enum class eSEVERITY : size_t {
    nINFO = 0,
    nDEBUG,
    nWARNING, nERROR,
    nFATAL
};

class LogStream final {
public:
    std::mutex mutex_;

    void BeginLine(eSEVERITY _note);
    void EndLine();

    template<class T>
    std::ostream &ToStream(T &&object)
    {
        stream_ << object;

        if constexpr (kCRUS_DEBUG_CONSOLE)
            conout_ << object;

        return stream_;
    }

    static LogStream &inst()
    {
        static LogStream stream;
        return stream;
    }

private:

    std::ostream stream_;
    std::ofstream conout_, file_;

    explicit LogStream();
    ~LogStream();

    void InitConsoleWindow();
};

class Book final {
public:

    explicit Book(eSEVERITY severity);
    ~Book();

    template<class T>
    Book &operator<< (T &&object)
    {
        LogStream::inst().ToStream(std::forward<T>(object));
        return *this;
    }

private:

    eSEVERITY severity_;
};
};
};

#endif // CRUS_BOOK_H