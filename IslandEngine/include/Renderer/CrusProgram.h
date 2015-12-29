/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    11h December 2015 / 27th June 2010.
****    Description: shader programs loading routines.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_PROGRAM_H              // Include guard "CrusProgram.h"
#define CRUS_PROGRAM_H

#include <vector>
#include <initializer_list>

#define _CRUS_SHADER_DEBUG 1

namespace isle
{
class Program {
public:
    enum eLAYOUT_ID {
        nVERTEX = 0, nFRAG_COLOR = 0, nNORMAL, nTEXCRD
    };

    enum eUNIFORM_ID {
        nMATERIAL = 0, nTRANSFORM, nATLAS
    };

    Program() {};
    ~Program() {};

    bool AssignNew(std::initializer_list<std::string> names, astr options = "", ...);
    void Delete();

#if _CRUS_SHADER_DEBUG
    static void CheckError();
#endif

    void SwitchOn() const;
    static void SwitchOff();

    uint32 GetName() const;

    int32 GetAttributeLoc(astr name) const;
    int32 GetUniformLoc(astr name) const;

    static int32 GetLastAttributeLoc();
    static int32 GetLasUniformLoc();

private:
    static int32 lastAttribute_, lastUniform_;      // Last attribute and uniform locations.
    uint32 program_{0};

#if _CRUS_SHADER_DEBUG
    //std::string name_{"NOT_A_PROGRAM"};
    static bool checked_;
#endif

    bool CreateShader(std::string name, std::vector<std::string> sources, uint32 type);

    bool CheckShader(std::string name, astr type, uint32 shader) const;
    bool CheckProgram(std::string name) const;
};
};

#endif // CRUS_PROGRAM_H