#include <iostream>
#include <ios>

#include <string>
using namespace std::string_literals;

#include "resource.hxx"

namespace
{
auto constexpr getTarget(isle::eOBJECT_TYPE usage)
{
    switch (usage) {
        case isle::eOBJECT_TYPE::TEXTURE_1D: return GL_TEXTURE_1D;
        case isle::eOBJECT_TYPE::TEXTURE_2D: return GL_TEXTURE_2D;
        case isle::eOBJECT_TYPE::TEXTURE_3D: return GL_TEXTURE_3D;
        case isle::eOBJECT_TYPE::TEXTURE_CUBE_MAP: return GL_TEXTURE_CUBE_MAP;
        
        default: return GL_FALSE;
    }
}
}


namespace isle
{
std::shared_ptr<DeviceObject>
ResourceManager::CreateObject(std::size_t size, eOBJECT_TYPE usage) noexcept
{
    std::shared_ptr<DeviceObject> buffer;

    std::uint32_t handle{0u};

    switch (usage) {
        case eOBJECT_TYPE::VERTEX:
        case eOBJECT_TYPE::INDEX:
        case eOBJECT_TYPE::UNIFORM:
        case eOBJECT_TYPE::STORAGE:
            glCreateBuffers(1, &handle);
            glObjectLabel(GL_BUFFER, handle, -1, "[buffer object]");
            break;

        case eOBJECT_TYPE::TEXTURE_1D:
        case eOBJECT_TYPE::TEXTURE_2D:
        case eOBJECT_TYPE::TEXTURE_3D:
        case eOBJECT_TYPE::TEXTURE_CUBE_MAP:
            glCreateTextures(getTarget(usage), 1, &handle);
            glObjectLabel(GL_TEXTURE, handle, -1, "[texture object]");
            break;

        default:
            return { };
    }

    if (auto error = glGetError(); error != GL_NO_ERROR)
        std::cerr << "failed to create buffer: "s << std::hex << error << '\n';

    else {
        buffer = std::make_shared<DeviceObject>(handle);

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
