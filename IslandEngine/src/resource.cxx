#include <iostream>
#include <ios>

#include <string>
using namespace std::string_literals;

#include "resource.hxx"


namespace isle
{
std::shared_ptr<DeviceBuffer>
ResourceManager::CreateBuffer(std::size_t size, eBUFFER_USAGE usage) noexcept
{
    std::shared_ptr<DeviceBuffer> buffer;

    std::uint32_t handle{0u};
    glCreateBuffers(1, &handle);

    glObjectLabel(GL_BUFFER, handle, -1, "[BO]");

    if (auto error = glGetError(); error != GL_NO_ERROR)
        std::cerr << "failed to create buffer: "s << std::hex << error << '\n';

    else {
        buffer = std::make_shared<DeviceBuffer>(handle);

#if NOT_YET_IMLPEMENTED
        if (memory) {
            if (auto result = vkBindBufferMemory(device_.handle(), *handle, memory->handle(), memory->offset()); result != VK_SUCCESS)
                std::cerr << "failed to bind buffer memory: "s << result << '\n';

            else buffer.reset(
                new VulkanBuffer{memory, *handle},
                [this] (VulkanBuffer *ptr_buffer)
                {
                    ReleaseResource(*ptr_buffer);

                    delete ptr_buffer;
                }
            );
        }
#endif
    }

    return buffer;
}
}
