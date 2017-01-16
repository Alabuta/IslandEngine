/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
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
    uint32 instanceID{0};
    std::string name{""};

    virtual ~CrusObject() = default;

    explicit operator bool() const;

    virtual std::string ToString() const = 0;

protected:
    explicit CrusObject() = default;

    void MakeValid();
};

inline CrusObject::operator bool() const
{
    return instanceID > 0 ? true : false;
}
};

#endif // CRUS_OBJECT_H