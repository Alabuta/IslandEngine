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

    virtual ~CrusObject() = default;

    // Object existing checking method.
    explicit operator bool() const;

    /*bool operator== (CrusObject const &object) const;
    bool operator!= (CrusObject const &object) const;*/

    std::string name() const;

protected:
    uint32 instanceID{0};
    std::string name_{""};

    explicit CrusObject() = default;

    void MakeValid();
};

inline CrusObject::operator bool() const
{
    return instanceID > 0 ? true : false;
}

inline std::string CrusObject::name() const
{
    return name_;
}
};

#endif // CRUS_OBJECT_H