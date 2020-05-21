#pragma once

#include "Device.h"
#include "VulkanResource.h"
#include "vulkan.h"

namespace phx {
template <vk::ImageViewType Type, vk::Format Format, VkImageUsageFlags Usage>
class ImageView : public VulkanResource<vk::UniqueImageView> {
public:
  static constexpr auto type = Type;
  static constexpr auto format = Format;
  static constexpr auto usage = vk::ImageUsageFlags(Usage);

  ImageView(vk::Device device, vk::Image image,
            vk::ImageSubresourceRange range) {
    vk::ImageViewCreateInfo info;
    info.image = image;
    info.viewType = type;
    info.format = format;
    info.subresourceRange = range;
    m_handle = device.createImageViewUnique(info);
  }

private:
};

} // namespace phx
