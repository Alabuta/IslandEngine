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

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <streambuf>
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
private:
    std::mutex mutex_;

    std::ostream stream_;
    std::ofstream file_;
    std::ofstream conout_;

    acstr kSEVERITIES[5]{
        " Info    :",
        " Debug   :",
        " Warning :",
        " Error   :",
        " Fatal   :"
    };

    explicit LogStream();
    ~LogStream();

    void InitConsoleWindow();

    void WriteToConsole(eSEVERITY _note);

    template<typename T>
    void ToStream(T const &object)
    {
        ToStream(object, std::bool_constant<HasToStreamMethod<T>::value>());
    }

    template<class T>
    void ToStream(T const &object, std::true_type)
    {
        object.ToStream(stream_);
#if _CRUS_DEBUG_CONSOLE
        object.ToStream(conout_);
#endif
    }

    template<typename T>
    void ToStream(T const &object, std::false_type)
    {
        stream_ << object;
#if _CRUS_DEBUG_CONSOLE
        conout_ << object;
#endif
    }

    template<typename T>
    class HasToStreamMethod {
        template<typename U, void(U::*)(std::ostream &) const> struct SFINAE { };

        template<typename U> static char func(SFINAE<U, &U::ToStream>*);
        template<typename U> static int func(...);

    public:
        enum { value = sizeof(func<T>(0)) == sizeof(char) };
    };

    static LogStream &inst()
    {
        static LogStream stream;
        return stream;
    }

    friend class Book;
};

class Book final {
public:

    explicit Book(eSEVERITY severity);
    ~Book();

    template<typename T>
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