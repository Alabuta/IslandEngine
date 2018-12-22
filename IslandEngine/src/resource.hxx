#pragma once

#include <memory>

#include "helpers.hxx"

#include "GAPI/context.hxx"

#include "deviceObject.hxx"

namespace isle
{
class ResourceManager final {
public:

#if NOT_YET_IMLPEMENTED
    [[nodiscard]] std::shared_ptr<VulkanImage>
    CreateImage(VkFormat format, std::uint16_t width, std::uint16_t height, std::uint32_t mipLevels,
                VkSampleCountFlagBits samplesCount, VkImageTiling tiling, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);

    [[nodiscard]] std::optional<VulkanImageView>
    CreateImageView(VulkanImage const &image, VkImageViewType type, VkImageAspectFlags aspectFlags) noexcept;

    [[nodiscard]] std::shared_ptr<VulkanSampler>
    CreateImageSampler(std::uint32_t mipLevels) noexcept;
#endif

    [[nodiscard]]
    std::shared_ptr<class DeviceObject> CreateObject(std::size_t size, eOBJECT_TYPE usage) noexcept;

private:

    template<class T, std::enable_if_t<is_one_of_v<std::decay_t<T>, class DeviceObject>>...>
    void ReleaseResource(T &&resource) noexcept;

    ResourceManager(ResourceManager const &) = delete;
    ResourceManager(ResourceManager &&) = delete;
};
}
