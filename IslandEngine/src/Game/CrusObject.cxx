/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 4th May 2012.
****
********************************************************************************************************************************/

#include "System\CrusSystem.h"
#include "Game\CrusObject.h"

namespace isle {

__forceinline uint32 CrusObject::instanceID() const
{
    return instanceID_;
}

__forceinline std::string const &CrusObject::name() const
{
    return name_;
}

__forceinline bool CrusObject::operator()() const
{
    return instanceID_ > 0 ? true : false;
}
};