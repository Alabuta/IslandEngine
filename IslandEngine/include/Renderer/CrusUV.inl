/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****    Description: vertex attributes declaration.
****
********************************************************************************************************************************/

namespace isle {

inline bool UV::operator== (UV const &_uv) const
{
    return math::CloseEnough(u, _uv.u)
        && math::CloseEnough(v, _uv.v);
}

inline bool UV::operator!= (UV const &_v) const
{
    return !(*this == _v);
}

inline UV::operator std::string() const
{
    static std::ostringstream ss;
    ss.str("");

    ss << u << " " << v;

    return ss.good() ? ss.str() : "[undefined]";
}

inline std::ostream &operator<< (std::ostream &_stream, UV const &_uv)
{
    return _stream << _uv.u << " " << _uv.v;
}
};