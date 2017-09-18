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

template<class C, class = void>
struct is_iterable : std::false_type { };

template<class C>
struct is_iterable<C, std::void_t<decltype(cbegin(std::declval<C>())), decltype(cend(std::declval<C>()))>> : std::true_type { };

template<class T>
constexpr bool is_iterable_v = is_iterable<T>::value;

template<class C, typename std::enable_if_t<is_iterable_v<std::decay_t<C>>>...>
std::ostream &operator<< (std::ostream &stream, C &&container)
{
    using T = std::decay_t<C>;

    stream << "[ ";
    std::copy(container.cbegin(), container.cend(), std::ostream_iterator<typename T::value_type>(stream, " "));
    return stream << "]";
}

template<class C, class = void>
struct is_printable : std::false_type { };

template<class C>
struct is_printable<C, std::void_t<decltype(std::ostream << std::declval<C>())>> : std::true_type { };

template<class T>
constexpr bool is_printable_v = is_printable<T>::value;

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

    template<class T, typename std::enable_if_t<is_printable_v<std::decay_t<T>>>...>
    void ToStream(T &&object)
    {
        stream_ << object;
    }

    template<class T, class = void>
    void ToStream(T &&object)
    {
        using Tt = std::decay_t<decltype(object)>;

        ToStream(std::forward<T>(object), std::bool_constant<HasToStreamMethod<Tt>::value>());
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