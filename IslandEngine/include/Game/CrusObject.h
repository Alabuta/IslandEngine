/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 4th May 2012.
****    Description: base class for all objects the engine references.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_OBJECT_H               // Include guard "CrusObject.h"
#define CRUS_OBJECT_H

#include <string>
#include "System\CrusTypes.h"

namespace isle {
class CrusObject {
public:

    virtual ~CrusObject() = default;

    uint32 instanceID() const;
    std::string const &name() const;

    bool operator()() const;

protected:
    CrusObject() = default;

private:
    uint32 instanceID_{0};
    std::string name_{""};
};
};

#endif // CRUS_OBJECT_H