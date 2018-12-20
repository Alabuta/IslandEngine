#pragma once

#include <memory>

#include "helpers.hxx"

#include "GAPI/context.hxx"

namespace isle
{
enum class eMEMORY_PROPERTY : std::uint32_t {
    UNSPECIFIED = 0,

    DYNAMIC_STORAGE = 0x01,

    MAP_READ = 0x02,
    MAP_WRITE = 0x04,

    PERSISTENT_READ = 0x08,
    PERSISTENT_WRITE = 0x10,
    
    PERSISTENT_COHERENT_READ = 0x20,
    PERSISTENT_COHERENT_WRITE = 0x40
};

class DeviceMemory final {
public:

    template<class T, typename std::enable_if_t<is_container_v<std::decay_t<T>>>...>
    DeviceMemory(eMEMORY_PROPERTY property, std::size_t size, T &&data) : property_{property}, size_{size}
    {
        glNamedBufferStorage();
    }

private:
    std::size_t size_;

    eMEMORY_PROPERTY property_;

    std::uint32_t handle_;
};

class MemoryManager final {
public:
    ;

    ;

private:
    ;
};
}
