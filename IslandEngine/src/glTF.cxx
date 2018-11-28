#include <string>
#include <string_view>

using namespace std::string_literals;
using namespace std::string_view_literals;

#ifdef _MSC_VER
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif

#ifdef _MSC_VER
#pragma warning(push, 3)
#pragma warning(disable: 4127)
#include "nlohmann/json.hpp"
#pragma warning(pop)
#else
#include "nlohmann/json.hpp"
#endif

#include "glTF.hxx"

namespace
{
enum class GL {
    BYTE = 0x1400, UNSIGNED_BYTE,
    SHORT, UNSIGNED_SHORT,
    INT, UNSIGNED_INT,
    FLOAT
};

//auto constexpr kARRAY_BUFFER         = 0x8892;
//auto constexpr kELEMENT_ARRAY_BUFFER = 0x8893;
}

namespace isle::glTF
{
bool load(std::string_view name, vertex_buffer_t &vertices, index_buffer_t &indices)
{
    return false;
}
;
}