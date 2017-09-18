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


inline bool Position::operator== (Position const &p)  const
{
    return isle::math::CloseEnough(xyz[0], p.xyz[0])
        && isle::math::CloseEnough(xyz[1], p.xyz[1])
        && isle::math::CloseEnough(xyz[2], p.xyz[2]);
}

inline bool Position::operator!= (Position const &p) const { return !(*this == p); }

inline void Position::ToStream(std::ostream &_stream) const
{
    _stream << xyz[0] << "; " << xyz[1] << "; " << xyz[2];
}

inline Position::operator std::string() const
{
    static std::ostringstream ss;
    ss.str("");

    ss << xyz[0] << "; " << xyz[1] << "; " << xyz[2];

    return ss.good() ? ss.str() : "[undefined]";
}

/*inline std::ostream &operator<< (std::ostream &stream, Position const &p) const
{
    return p.ToStream(stream);
}*/
};