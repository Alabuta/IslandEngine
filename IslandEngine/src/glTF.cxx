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

namespace isle::glTF
{
;
}