#pragma once

#include "helpers.hxx"

#include "GAPI/context.hxx"

// #include "resource.hxx"

namespace isle
{
enum class eBUFFER_USAGE : std::uint32_t {
    VERTEX = 1,
    INDEX,
    UNIFORM,
    STORAGE,
    INDIRECT,
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D,
    TEXTURE_CUBE_MAP
};

class DeviceBuffer final {
public:

    DeviceBuffer(std::uint32_t handle) noexcept : handle_{handle} { }

    std::uint32_t handle() const noexcept { return handle_; }

private:
    // std::shared_ptr<class DeviceMemory> memory_;

    std::uint32_t handle_;

    DeviceBuffer() = delete;
    DeviceBuffer(DeviceBuffer const &) = delete;
    DeviceBuffer(DeviceBuffer &&) = delete;
};
}
