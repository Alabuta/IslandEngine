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

#define _CRUS_SHADER_DEBUG 1

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

    Program() {};
    ~Program() {};

    bool AssignNew(std::initializer_list<std::string> names);
    void Destroy();

#if _CRUS_SHADER_DEBUG
    static void CheckError();
#endif

    void SwitchOn() const;
    static void SwitchOff();

    uint32 program() const;

    int32 GetAttributeLoc(astr name) const;
    int32 GetUniformLoc(astr name) const;

    static int32 GetLastAttributeLoc();
    static int32 GetLasUniformLoc();

private:
    static auto const kINCLUDING_LEVEL{16};
    static int32 lastAttribute_, lastUniform_;      // Last attribute and uniform locations.
    uint32 program_{0};

    struct ShaderInfo {
        std::string type;
        uint32 shader;
    };

#if _CRUS_SHADER_DEBUG
    //std::string name_{"NOT_A_PROGRAM"};
    static bool checked_;
#endif

    std::string PreprocessIncludes(std::string const &source, std::string const &name, int32 includingLevel = 0) const;

    bool CreateShader(std::string const &name, std::string source, uint32 type);

    bool CheckShader(std::string name, ShaderInfo const &shaderInfo) const;
    bool CheckProgram(std::string name) const;
};

__forceinline void Program::SwitchOn() const
{
    glUseProgram(program_);

#if _CRUS_OBSOLETE
    if (glGetError() != GL_NO_ERROR) {
        Book::AddEvent(eNOTE::nERROR, "invalid program number: %d (%s)", program_, name_.c_str());
    }
#endif
}

__forceinline /*static*/ void Program::SwitchOff()
{
    glUseProgram(0);
}

__forceinline uint32 Program::program() const
{
    return program_;
}

__forceinline int32 Program::GetAttributeLoc(astr _name) const
{
    lastAttribute_ = glGetAttribLocation(program_, _name);

#if _CRUS_SHADER_DEBUG
    if (lastAttribute_ < 0 && !checked_)
        Book::AddEvent(eNOTE::nWARN, "attribute \"%s\" unexist or are not used.", _name);
#endif

    return lastAttribute_;
}

__forceinline int32 Program::GetUniformLoc(astr _name) const
{
    lastUniform_ = glGetUniformLocation(program_, _name);

#if _CRUS_SHADER_DEBUG
    if (lastUniform_ < 0 && !checked_)
        Book::AddEvent(eNOTE::nWARN, "uniform \"%s\" unexist or are not used.", _name);
#endif

    return lastUniform_;
}

__forceinline /*static*/ int32 Program::GetLastAttributeLoc()
{
    return lastAttribute_;
}

__forceinline /*static*/ int32 Program::GetLasUniformLoc()
{
    return lastUniform_;
}
};

#endif // CRUS_PROGRAM_H