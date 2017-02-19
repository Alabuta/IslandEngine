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
#include <initializer_list>

#include "System\CrusSystem.h"

namespace isle {
class Program {
public:

    enum eIN_OUT_ID {
        nVERTEX = 0, nNORMAL, nTEXCRD,
        nFRAG_COLOR = 0
    };

    enum eUNIFORM_ID {
        nVIEWPORT = 0,
        nMAIN_COLOR = 0
    };

    enum eBUFFER_ID {
        nTRANSFORM = 0,
    };

    bool AssignNew(std::initializer_list<char const *> &&names);
    void Destroy();

    void UseThis() const;

    uint32 program() const;

    int32 GetAttributeLoc(astr name) const;
    int32 GetUniformLoc(astr name) const;

private:
    static auto constexpr kINCLUDING_LEVEL{16};
    uint32 program_{0};

    bool CreateShader(std::string const &source, uint32 type);
    bool CompileShader(std::pair<uint32, astr> const &shaderInfo) const;

    bool LinkAndValidateProgram() const;

    std::string PreprocessIncludes(std::string const &source, std::string const &name, int32 includingLevel = 0) const;
};

__forceinline uint32 Program::program() const
{
    return program_;
}
};

#endif // CRUS_PROGRAM_H