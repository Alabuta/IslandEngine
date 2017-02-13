/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: base class for all objects the engine references.
****
********************************************************************************************************************************/
#include "System\CrusTypes.h"
#include "Game\CrusObject.h"

namespace isle {
__declspec(noinline) void CrusObject::MakeValid()
{
    static auto totalNumberOfInstancedObjects = 0u;

    instanceID = ++totalNumberOfInstancedObjects;
}
};