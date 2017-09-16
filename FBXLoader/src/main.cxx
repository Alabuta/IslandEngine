/****************************************************************************************
****
****    Source code of Island Engine.
****	Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: Autodesk FBX file loading routines.
****
****************************************************************************************/

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <iostream>
#include <gsl/gsl>

#include "FBX.h"


int main()
{
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(84);

    std::string const file_name = R"(..\contents\meshes\cube.fbx)";

    FBX fbx;
    fbx.ImportScene(file_name);

    std::cout << "\nDone..." << std::endl;
    std::cin.get();

    return 0;
}