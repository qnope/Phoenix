#pragma once

#include "Device.h"
#include "VulkanResource.h"
#include "vulkan.hpp"

namespace phx {
template <std::size_t Usage>
class ImageView : public VulkanResource<vk::UniqueImageView> {
public:
  static constexpr std::size_t usage = Usage;

  ImageView(vk::Device device, vk::Image image, vk::ImageViewType type,
            vk::Format format, vk::ImageSubresourceRange range) {
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
