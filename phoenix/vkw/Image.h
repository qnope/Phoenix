#pragma once

#include <ltl/ltl.h>
#include <variant>

#include "ImageView.h"

namespace phx {

template <vk::ImageType Type, vk::Format Format, VkImageUsageFlags Usage>
class Image {
public:
  static constexpr auto type = Type;
  static constexpr auto usage = vk::ImageUsageFlags(Usage);
  static constexpr auto format = Format;

  Image(vk::Device device, vk::Image image, vk::Extent3D extent,
        uint32_t mipLevels, uint32_t arrayLayers) noexcept
      : m_device{device}, m_handle{image}, m_extent{extent},
        m_mipLevels{mipLevels}, m_arrayLayers{arrayLayers} {}

  vk::Extent3D getExtent() const noexcept { return m_extent; }
  uint32_t getMipLevels() const noexcept { return m_mipLevels; }
  uint32_t getArrayLayers() const noexcept { return m_arrayLayers; }

  vk::Image getHandle() const noexcept { return m_handle; }

  template <vk::ImageViewType ImageViewType>
  ImageView<ImageViewType, Format, Usage>
  createImageView(vk::ImageSubresourceRange range) const noexcept {
    static_assert(isImageTypeCompatibleWithImageViewType(Type, ImageViewType),
                  "ImageViewType must be compatible with ImageType");
    return ImageView<ImageViewType, Format, Usage>{m_device, getHandle(),
                                                   range};
  }

private:
  vk::Device m_device;
  vk::Image m_handle;
  vk::Extent3D m_extent;
  uint32_t m_mipLevels;
  uint32_t m_arrayLayers;
};

} // namespace phx
