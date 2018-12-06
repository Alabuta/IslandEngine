/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: shader programs loading routines.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_PROGRAM_H              // Include guard "CrusProgram.h"
#define CRUS_PROGRAM_H

#include <vector>
#include <string_view>
#include <set>
#include <initializer_list>

#include "System\CrusSystem.h"
#include "vertex_format.hxx"

namespace isle {
class Program {
public:

    template<typename... Ts>
    constexpr bool AssignNew(std::initializer_list<std::string> &&names, Ts &&... options)
    {
        std::ostringstream stream;
        UnpackOptionsToStream(stream, std::forward<Ts>(options)...);

        return AssignNew(std::move(names), stream.good() ? stream.str() : "");
    }

    bool AssignNew(std::initializer_list<std::string> &&names, std::string options = "");
    void Destroy();

    void bind() const;

    u32 handle() const;

    i32 GetAttributeLoc(std::string_view name) const;
    i32 GetUniformLoc(std::string_view name) const;

private:
    static auto constexpr kINCLUDING_LEVEL{16};
    //static std::unordered_map<std::string, std::string> cachedIncludeFiles;
    thread_local static std::set<std::string> cachedIncludeNames;
    thread_local static std::vector<std::string> cachedIncludeFiles;

    u32 program_{0};

    u32 CreateShaderObject(std::vector<std::string> const &includes, std::string_view source, u32 type, std::string options);

    bool LinkAndValidateProgram() const;

    static std::string ReadShaderSource(std::string const &parentPath, std::string const &name);
    static std::unordered_map<u32, std::string> SeparateByStages(std::string const &name, std::string &includes, std::string const &source);
    static void PreprocessIncludes(std::string const &source, i32 includingLevel = 0);

    template<typename T, typename S>
    constexpr std::ostream &UnpackOptionsToStream(std::ostream &stream, T &&first, S &&second)
    {
        return stream << "#define " << std::forward<T>(first) << '\t' << std::forward<S>(second) << '\n';
    }

    template<typename T, typename S, typename ... Ts>
    constexpr std::ostream &UnpackOptionsToStream(std::ostream &stream, T &&first, S &&second, Ts &&...options)
    {
        stream << "#define " << std::forward<T>(first) << '\t' << std::forward<S>(second) << '\n';
        return UnpackOptionsToStream(stream, std::forward<Ts>(options)...);
    }
};

inline u32 Program::handle() const
{
    return program_;
}
};

#endif // CRUS_PROGRAM_H