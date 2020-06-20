#pragma once

#include "../VulkanResource.h"
#include "../vulkan.h"
#include <ltl/condition.h>

namespace phx {

template <vk::ImageType type, vk::ImageViewType viewType>
constexpr auto isImageTypeCompatibleWithImageViewType() {
    if constexpr (type == vk::ImageType::e2D) {
        return ltl::bool_v<viewType == vk::ImageViewType::e2D>;
    } else if (type == vk::ImageType::e3D) {
    }
}

template <vk::ImageViewType Type, vk::Format Format, VkImageUsageFlags Usage>
class ImageView : public VulkanResource<vk::UniqueImageView> {
  public:
    static constexpr auto type = Type;
    static constexpr auto format = Format;
    static constexpr auto imageUsage = vk::ImageUsageFlags(Usage);

    ImageView(vk::Device device, vk::Image image, vk::ImageSubresourceRange range) {
        vk::ImageViewCreateInfo info;
        info.image = image;
        info.viewType = type;
        info.format = format;
        info.subresourceRange = range;
        m_handle = device.createImageViewUnique(info);
    }
};

} // namespace phx
