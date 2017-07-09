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

    template<class T>
    void ToStream(T &&object)
    {
        using Tt = std::decay_t<decltype(object)>;

        ToStream(object, std::bool_constant<HasToStreamMethod<Tt>::value>());
    }

    void BeginLine(eSEVERITY _note);
    void EndLine();

    static LogStream &inst()
    {
        static LogStream stream;
        return stream;
    }

private:

    std::ostream stream_;
    std::ofstream file_;
    std::ofstream conout_;

    explicit LogStream();
    ~LogStream();

    void InitConsoleWindow();

    template<class T>
    void ToStream(T &&object, std::true_type)
    {
        object.ToStream(stream_);

        if constexpr (kCRUS_DEBUG_CONSOLE)
            object.ToStream(conout_);
    }

    template<class T>
    void ToStream(T &&object, std::false_type)
    {
        stream_ << object;

        if constexpr (kCRUS_DEBUG_CONSOLE)
            conout_ << object;
    }

    template<class T>
    class HasToStreamMethod {
        template<typename U, void(U::*)(std::ostream &) const> struct SFINAE { };

        template<typename U> static char func(SFINAE<U, &U::ToStream>*);
        template<typename U> static int func(...);

    public:
        enum { value = sizeof(func<T>(0)) == sizeof(char) };
    };
};

class Book final {
public:

    explicit Book(eSEVERITY severity);
    ~Book();

    template<class T>
    Book &operator<< (T const &object)
    {
        LogStream::inst().ToStream(object);
        return *this;
    }

private:

    eSEVERITY severity_;
};
};
};

#endif // CRUS_BOOK_H