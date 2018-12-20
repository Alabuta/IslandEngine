#pragma once

#include "helpers.hxx"

#include "GAPI/context.hxx"

namespace isle
{
enum class eBUFFER_USAGE : std::uint32_t {
    VERTEX = 0x01,
    INDEX = 0x02,
    TEXTURE = 0x04,
    UNIFORM = 0x08,
    STORAGE = 0x10,
    INDIRECT = 0x20
};

class DeviceBuffer final {
public:

    DeviceBuffer(std::uint32_t handle) noexcept : handle_{handle} { }

    std::uint32_t handle() const noexcept { return handle_; }

private:
    std::shared_ptr<class DeviceMemory> memory_;

    std::uint32_t handle_;

    DeviceBuffer() = delete;
    DeviceBuffer(DeviceBuffer const &) = delete;
    DeviceBuffer(DeviceBuffer &&) = delete;
};
}
