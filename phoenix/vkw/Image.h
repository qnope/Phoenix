#pragma once

#include <ltl/ltl.h>
#include <variant>

#include "ImageView.h"

namespace phx {

struct ColorAttachmentUsageBit {
  static constexpr vk::ImageUsageFlags usage =
      vk::ImageUsageFlagBits::eColorAttachment;
};

template <typename Usage> class Image {
public:
  static constexpr auto usage = Usage::usage;

  Image(vk::Device device, vk::Image image, vk::Format format,
        vk::Extent3D extent, uint32_t mipLevels, uint32_t arrayLayers) noexcept
      : m_device{device}, m_handle{image}, m_format{format}, m_extent{extent},
        m_mipLevels{mipLevels}, m_arrayLayers{arrayLayers} {}

  vk::Format getFormat() const noexcept { return m_format; }
  vk::Extent3D getExtent() const noexcept { return m_extent; }
  uint32_t getMipLevels() const noexcept { return m_mipLevels; }
  uint32_t getArrayLayers() const noexcept { return m_arrayLayers; }

  vk::Image getHandle() const noexcept {
    ltl::overloader visitor{[](vk::Image img) { return img; },
                            [](const vk::UniqueImage &img) { return *img; }};
    return std::visit(visitor, m_handle);
  }

  ImageView<Usage> createImageView(vk::ImageViewType type, vk::Format format,
                                   vk::ImageSubresourceRange range) const
      noexcept {
    return ImageView<Usage>{m_device, getHandle(), type, format, range};
  }

private:
  vk::Device m_device;
  std::variant<vk::Image, vk::UniqueImage> m_handle;
  vk::Format m_format;
  vk::Extent3D m_extent;
  uint32_t m_mipLevels;
  uint32_t m_arrayLayers;
};

} // namespace phx
