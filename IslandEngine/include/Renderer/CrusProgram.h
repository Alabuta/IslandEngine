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

namespace isle {
class Program {
public:

    enum eIN_OUT_ID {
        nVERTEX = 0, nNORMAL, nTEXCRD, nCOLOR,
        nFRAG_COLOR = 0
    };

    enum eUNIFORM_ID {
        nVIEWPORT = 0, nVIEWPORT_RECT,
        nMAIN_COLOR = 0
    };

    enum eBUFFER_ID {
        nTRANSFORM = 0,
    };

    bool AssignNew(std::initializer_list<std::string> &&names);
    void Destroy();

    void UseThis() const;

    uint32 program() const;

    int32 GetAttributeLoc(std::string_view name) const;
    int32 GetUniformLoc(std::string_view name) const;

private:
    static auto constexpr kINCLUDING_LEVEL{16};
    //static std::unordered_map<std::string, std::string> cachedIncludeFiles;
    thread_local static std::set<std::string> cachedIncludeNames;
    thread_local static std::vector<std::string> cachedIncludeFiles;

    uint32 program_{0};

    uint32 CreateShaderObject(std::vector<std::string> const &includes, std::string_view source, uint32 type);

    bool LinkAndValidateProgram() const;

    static std::string ReadShaderSource(std::string const &parentPath, std::string const &name);
    static std::unordered_map<uint32, std::string> SeparateByStages(std::string const &name, std::string &includes, std::string const &source);
    static void PreprocessIncludes(std::string const &source, std::string_view name, int32 includingLevel = 0);
};

__forceinline uint32 Program::program() const
{
    return program_;
}
};

#endif // CRUS_PROGRAM_H