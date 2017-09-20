/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: vertex attributes declaration.
****
********************************************************************************************************************************/
#include <string>
#include <sstream>

#include "CrusPosition.h"

namespace isle {


inline bool Position::operator== (Position const &_p)  const
{
    return isle::math::CloseEnough(xyz[0], _p.xyz[0])
        && isle::math::CloseEnough(xyz[1], _p.xyz[1])
        && isle::math::CloseEnough(xyz[2], _p.xyz[2]);
}

inline bool Position::operator!= (Position const &_p) const { return !(*this == _p); }

inline Position::operator std::string() const
{
    static std::ostringstream ss;
    ss.str("");

    ss << xyz[0] << "; " << xyz[1] << "; " << xyz[2];

    return ss.good() ? ss.str() : "[undefined]";
}

inline std::ostream &operator<< (std::ostream &_stream, Position const &_p)
{
    return _stream << _p.xyz[0] << "; " << _p.xyz[1] << "; " << _p.xyz[2];
}
};