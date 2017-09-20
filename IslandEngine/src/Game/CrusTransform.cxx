#include "Game\CrusTransform.h"

namespace isle {

//__forceinline eCOMPONENT_TYPES Transform::type() const
//{
//    return eCOMPONENT_TYPES::nVIEWPORT;
//}

inline std::ostream &operator<< (std::ostream &_stream, Transform const &tr)
{
    return _stream;
}
};